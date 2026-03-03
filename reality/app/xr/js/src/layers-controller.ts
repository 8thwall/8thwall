// A pipeline for returning semantic layers.

/* global XR8:readonly */

// There are a main thread and a web worker running in this layers-controller.
// Main thread does 2 things for every frame while interacting with the web worker
// whenever it's ready:
//
// For every frame:
//  1. GPU resizer to downsize camera textures
//     so that the downsized images can be used for semantics inference.
//     see 'onProcessGpu' on downsizing the texture to w144x192h RGBA8888 images
//     and reading downsized images into CPU
//  2. GPU to render semantics results based on an internal cubemap texture from the engine.
//     then read the result pixels int CPU.
//     see 'onProcessGpu' on rendering the semantics results based on device poses.
//     and see 'onProcessCpu' on getting the semantics results from engine to CPU.
//
// Main thread interacts with the web worker :
//  1. When the web worker is ready, invoke the web worker with the latest frame
//     and the device pose associated with this frame for inferencing.
//  2. When the web worker finishes processing its frame, get the inference results
//     from the web worker, along with the corresponding device pose,
//     call the engine to update the internal semantics cubemap.
//  3. repeat step 1
//
// The web worker's job is to call the inference engine to get the semantics results.
//

// The following imports resolve because of the NODE_PATH variable set in build.js
/* eslint-disable import/no-unresolved */
import {Message} from 'capnp-ts'
import {CameraEnvironment} from 'reality/engine/api/base/camera-environments.capnp'

import {
  SemanticsCameraTransformMsg,
  SemanticsInferenceResult,
  SemanticsResponse,
} from 'reality/engine/api/semantics.capnp'

import type {XrccModule} from 'reality/app/xr/js/src/types/xrcc'

import {Quaternion, getQuaternion} from './quaternion'
import {GlTextureRenderer} from './gl-renderer'
import {XrPermissionsFactory} from './permissions'
import {
  scaleFactorSameAspectRatioCrop,
  toImageArrayLetterbox,
  toImageArrayRotateCW90,
  toImageArrayRotateCCW90,
} from './pixel-transforms'

import type {Position, Coordinates, ImageData2D} from './types/common'
import type {EnvironmentUpdate} from './types/environment-update'
import {
  SUPPORTED_LAYERS,
  DEFAULT_INVERT_LAYER_MASK,
  DEFAULT_EDGE_SMOOTHNESS,
  LayerName,
  LayerOptions,
  LayersControllerOptions,
  LayersDebugOptions,
} from './types/layers'
import {getOrigin, createSemanticsOptionsMsg} from './semantics/options'
import {singleton} from './factory'
import type {
  FrameworkHandle, onAttachInput, onCameraStatusChangeInput, ProcessCpuInput, ProcessGpuInput,
  RunConfig, SessionAttributes,
} from './types/pipeline'
import {ResourceUrls} from './resources'

interface LayersController {
  // Gets a pipeline module that controls semantic layers.
  //
  // Dispatches Events:
  //
  // layerloading: Fires on this method when we are about to load extra data required
  //    detail: {}
  // layerscanning: Fires when extra data has been loaded and the layers starting to be scanned.
  //                 One event is dispatched per layer being scanned.
  //    detail: {
  //      name: string name of the layer which we are scanning
  //    }
  // layerfound: Fires the first time a layer has been found
  //    detail: {
  //      name: string name of the layer which has been found
  //      percentage: percentage of pixels that are sky
  //    }
  pipelineModule(): LayersControllerPipelineModule

  // Configures the pipeline module that controls semantic layers.
  configure(args: LayersControllerArgs): void

  // Repositions the camera to the origin / facing direction.
  recenter(): void

  // Get the configured layers.
  getLayerNames(): readonly string[]
}

interface LayersControllerPipelineModule {
  name: string
  onAttach(args: any): void
  onBeforeSessionInitialize(args: any): void
  onCameraStatusChange(args: any): void
  onCanvasSizeChange(args: any): void
  onDeviceOrientationChange(args: any): void
  onDetach(args: any): void
  onProcessCpu(args: any): LayersControllerProcessCpuOutput | null
  onProcessGpu(args: any): LayersControllerProcessGpuOutput | null
  onStart(args: any): void
  onVideoSizeChange(args: any): void
  requiredPermissions(): Array<any>
}

/**
 * Configuration for LayersController.
 *
 * @member nearClip the closest distance to the camera at which scene objects are visible.
 * @member farClip the farthest distance to the camera at which scene objects are visible.
 * @member coordinates camera configuration.
 * @member layers the layers. To remove a layer pass null as the layer value. To reset a
 * @member debug options if debug output is requested
 * layer option to its default value pass null for that option value. Only valid layer is 'sky'.
 */
interface LayersControllerArgs {
  nearClip?: number
  farClip?: number
  // The origin's default position y is 2.
  coordinates?: Coordinates
  layers?: Record<LayerName, LayerOptions | null>
  debug?: LayersDebugOptions
}

interface LayerDebugOutputs {
  inputMaskImg?: ImageData2D
}

interface LayerOutput {
  // The texture mask.
  // - The r, g, b channels indicate our confidence of whether the layer is present at this pixel.
  //   0.0 indicates the layer is not present and 1.0 indicates it is present. Note that this value
  //   will be flipped if invertLayerMask has been set to true.
  // - The alpha channel will be 0.0 if this pixel is still unknown, else 1.0 if known.
  texture?: WebGLTexture
  textureWidth: number
  textureHeight: number
  // percentage of pixel that is classified as sky. Value in the range of [0, 1]
  percentage: number
  // debug info if requested
  debug?: LayerDebugOutputs
}

interface LayersControllerProcessCpuOutput {
  // The position of the camera in the scene.
  position: Position
  // The orientation (quaternion) of the camera in the scene.
  rotation: Quaternion
  // A 16 dimensional column-major 4x4 projection matrix that gives the scene camera the same field
  // of view as the rendered camera feed.
  intrinsics: number[]
  // A WebGLTexture containing camera feed data.
  cameraFeedTexture: WebGLTexture
  // The layers.
  layers?: Record<LayerName, LayerOutput>
}

interface InvokeData {
  rotation: Quaternion
  pixels: Uint8Array
  rows: number
  cols: number
  rowBytes: number
}

interface LayersControllerProcessCpuInput extends ProcessCpuInput {
  processGpuResult: {
    layerscontroller: LayersControllerProcessGpuOutput
  }
}

interface LayersControllerProcessGpuOutput {
  // Pre-processed data to be used for inference.
  invokeData: InvokeData
}

interface DeviceOrientation8wEvent extends Event {
  detail: DeviceOrientationEvent
}

const IDENTITY_POS: Position = {x: 0.0, y: 0.0, z: 0.0}

// Always supply the semantics model with data in portrait mode in U8[1, 3, 256, 144] tensor shape
const SEMANTICS_MODEL_INPUT_WIDTH: number = 144
const SEMANTICS_MODEL_INPUT_HEIGHT: number = 256

const LayersControllerFactory = singleton((
  runConfig: RunConfig, xrccPromise: Promise<XrccModule>
): LayersController => {
  let init_: Promise<any> = null
  let xrcc_: XrccModule = null
  let framework_ = null

  let glCtx_ = null  // Rendering Context
  let glHandle_ = null  // handle for Rendering Context
  let computeHandle_ = null  // handle for Compute Context

  let hasVA_: boolean = true  // flag for whether GL context has vertex array or not
  let VAext_ = null  // Vertex Array extension if GL context does not support VA natively

  let cameraResizeRenderer_ = null  // GPU resizer to down size camera texture
  let cameraResizedTex_ = null  // result texture from GPU resizer, owned by cameraResizeRenderer_
  // The rotation of the camera for the texture stored in cameraResizedTex_.
  let cameraResizedTexRotation_: Quaternion | null = null

  // Directly render to semanticsTex_ within rendering context
  let semanticsTex_ = null             // output texture from sky cubemap, owned by this
  let semanticsTexWidth_: number = 0   // semantics texture width
  let semanticsTexHeight_: number = 0  // semantics texture height

  // semantics-worker related variables
  let worker_: Worker | null = null    // Semantics worker.
  let data_ = null                     // Semantics data.
  let percentage_: number = 0          // The percent of the camera frame which is sky.
  let frame_: number = 0               // Check for the same frame.
  let isInvokeReady_: boolean = false  // Check if semantics classifier is ready to perform invoke.
  let configurationChanged_: boolean = false
  let attached_: boolean = false

  let lastDeviceRotation_ = {timeStamp: 0, alpha: 0, beta: 0, gamma: 0}

  // Parameters for sky texture post-processing
  let haveSeenSky_: boolean = false  // Whether we have ever found the sky.

  const env_ = {
    videoWidth: 0,
    videoHeight: 0,
    canvasWidth: 0,
    canvasHeight: 0,
    modelInputHeight: 0,
    modelInputWidth: 0,
    videoResizeHeight: 0,
    videoResizeWidth: 0,
    orientation: 0,
    deviceEstimate: null,
  }

  // Options configured with configure().
  const options_: LayersControllerOptions = {
    nearClip: null,
    farClip: null,
    coordinates: null,
    layers: {},
  }

  // Options configured with configure().
  const debugOptions_: LayersDebugOptions = {
    inputMask: false,
  }

  // semantics inference results
  const debugSemanticsMask_: ImageData2D = {
    rows: 0,
    cols: 0,
    rowBytes: 0,
    pixels: undefined,
  }

  // Image capture dimensions.
  const src_ = {
    videoWidth: 0,
    videoHeight: 0,
  }

  const dest_ = {
    width: 0,
    height: 0,
  }

  const bindVertexArray = v => (hasVA_ ? glCtx_.bindVertexArray(v) : VAext_.bindVertexArrayOES(v))
  const vertexArrayBinding = () => glCtx_.getParameter(
    hasVA_ ? glCtx_.VERTEX_ARRAY_BINDING : VAext_.VERTEX_ARRAY_BINDING_OES
  )

  const deleteSemanticsTexture = () => {
    if (!semanticsTex_ || !glCtx_) {
      return
    }

    if (xrcc_ && semanticsTex_.name) {
      xrcc_.GL.textures[semanticsTex_.name] = null
    }
    glCtx_.deleteTexture(semanticsTex_)
    semanticsTex_ = null
    semanticsTexWidth_ = 0
    semanticsTexHeight_ = 0
  }

  // Calculates the appropriate dimensions to be letterboxed into a 144x256 image.
  const calculateOutputSize = (videoWidth: number, videoHeight: number) => {
    const aspectRatio = videoHeight / videoWidth

    if (aspectRatio > 1) {  // Portrait view.
      env_.modelInputHeight = 256
      env_.modelInputWidth = 144
    } else {  // Landscape view.
      env_.modelInputHeight = 144
      env_.modelInputWidth = 256
    }

    const scaleFactor = scaleFactorSameAspectRatioCrop(
      videoWidth, videoHeight, env_.modelInputWidth, env_.modelInputHeight
    )
    env_.videoResizeWidth = scaleFactor * videoWidth
    env_.videoResizeHeight = scaleFactor * videoHeight
    return {width: env_.modelInputWidth, height: env_.modelInputHeight}
  }

  // Updates the size for the renderer.
  const updateResizerDimensions = (videoWidth: number, videoHeight: number) => {
    if (!attached_) {
      return
    }

    if (videoWidth === src_.videoWidth && videoHeight === src_.videoHeight) {
      return
    }

    src_.videoHeight = videoHeight
    src_.videoWidth = videoWidth

    const dsize = calculateOutputSize(videoWidth, videoHeight)

    if (!dsize.width || !dsize.height) {
      return
    }

    if (cameraResizeRenderer_ && dsize.width === dest_.width && dsize.height === dest_.height) {
      return
    }

    dest_.width = dsize.width
    dest_.height = dsize.height

    cameraResizeRenderer_ = GlTextureRenderer.create({
      GLctx: glCtx_,
      toTexture: {width: dest_.width, height: dest_.height},
      flipY: true,
    })
  }

  const updateCameraPoseMsg = (
    camera: SemanticsCameraTransformMsg,
    position: Position,
    rotation: Quaternion
  ) => {
    camera.getPosition().setX(position.x)
    camera.getPosition().setY(position.y)
    camera.getPosition().setZ(position.z)
    camera.getDevicePose().setW(rotation.w)
    camera.getDevicePose().setX(rotation.x)
    camera.getDevicePose().setY(rotation.y)
    camera.getDevicePose().setZ(rotation.z)
  }

  const updateDebugSemanticsResult = (
    semResult: ArrayBuffer,
    semWidth: number,
    semHeight: number
  ) => {
    if (debugSemanticsMask_.cols !== semWidth ||
        debugSemanticsMask_.rows !== semHeight ||
        !debugSemanticsMask_.pixels) {
      debugSemanticsMask_.cols = semWidth
      debugSemanticsMask_.rows = semHeight
      debugSemanticsMask_.rowBytes = 4 * semWidth
      debugSemanticsMask_.pixels = new Uint8Array(4 * semWidth * semHeight)
    }

    const result = new DataView(semResult)
    const dataPtr = debugSemanticsMask_.pixels

    // converting from [0,1] float to [0,255] gray
    for (let r = 0; r < semHeight; ++r) {
      let offset = 4 * r * semWidth
      for (let c = 0; c < semWidth; ++c) {
        const color = result.getFloat32(offset, true) * 255

        dataPtr[offset + 0] = color  // R
        dataPtr[offset + 1] = color  // G
        dataPtr[offset + 2] = color  // B
        dataPtr[offset + 3] = 255    // A

        offset += 4
      }
    }
  }

  const updateCubemap = (
    semResult: ArrayBuffer,
    semWidth: number,
    semHeight: number,
    position: Position,
    rotation: Quaternion
  ) => {
    const message = new Message()
    const infMsg = message.initRoot(SemanticsInferenceResult)
    updateCameraPoseMsg(infMsg.getCamera(), position, rotation)

    // to RGBA8888 image without letterbox
    infMsg.getImage().setRows(semHeight)
    infMsg.getImage().setCols(semWidth)
    infMsg.getImage().setBytesPerRow(4 * semWidth)
    infMsg.getImage().initUInt8PixelData(4 * semWidth * semHeight)
    const dataPtr = infMsg.getImage().getUInt8PixelData().toUint8Array()

    const result = new DataView(semResult)

    // converting from [0,1] float to [0,255] gray
    for (let r = 0; r < semHeight; ++r) {
      let offset = 4 * r * semWidth
      for (let c = 0; c < semWidth; ++c) {
        const color = result.getFloat32(offset, true) * 255

        dataPtr[offset + 0] = color  // R
        dataPtr[offset + 1] = color  // G
        dataPtr[offset + 2] = color  // B
        dataPtr[offset + 3] = 255    // A

        offset += 4
      }
    }

    xrcc_.GL.makeContextCurrent(glHandle_)

    const restoreViewport = glCtx_.getParameter(glCtx_.VIEWPORT)
    const restoreDepth = glCtx_.isEnabled(glCtx_.DEPTH_TEST)
    const restoreFrontFace = glCtx_.getParameter(glCtx_.FRONT_FACE)
    const restoreVA = vertexArrayBinding()
    const restoreArrayBuffer = glCtx_.getParameter(glCtx_.ARRAY_BUFFER_BINDING)
    const restoreIb = glCtx_.getParameter(glCtx_.ELEMENT_ARRAY_BUFFER_BINDING)
    const restoreShader = glCtx_.getParameter(glCtx_.CURRENT_PROGRAM)
    const restoreFb = glCtx_.getParameter(glCtx_.FRAMEBUFFER_BINDING)
    const restoreRb = glCtx_.getParameter(glCtx_.RENDERBUFFER_BINDING)
    const restoreTexCube = glCtx_.getParameter(glCtx_.TEXTURE_BINDING_CUBE_MAP)
    const restoreActive = glCtx_.getParameter(glCtx_.ACTIVE_TEXTURE)
    const restoreTex2D = glCtx_.getParameter(glCtx_.TEXTURE_BINDING_2D)
    // Get the textures bind with the texture units that we are overwriting
    glCtx_.activeTexture(glCtx_.TEXTURE0)
    const restoreTex0 = glCtx_.getParameter(glCtx_.TEXTURE_BINDING_2D)
    glCtx_.activeTexture(glCtx_.TEXTURE1)
    const restoreTex1 = glCtx_.getParameter(glCtx_.TEXTURE_BINDING_2D)

    const restoreUnpackFlipY = glCtx_.getParameter(glCtx_.UNPACK_FLIP_Y_WEBGL)
    if (restoreUnpackFlipY) {
      glCtx_.pixelStorei(glCtx_.UNPACK_FLIP_Y_WEBGL, false)
    }

    const buffer = message.toArrayBuffer()
    const ptr = xrcc_._malloc(buffer.byteLength)
    xrcc_.writeArrayToMemory(new Uint8Array(buffer), ptr)
    xrcc_._c8EmAsm_semanticsControllerRenderToCubemap(ptr, buffer.byteLength)
    xrcc_._free(ptr)

    glCtx_.viewport(
      restoreViewport[0], restoreViewport[1], restoreViewport[2], restoreViewport[3]
    )
    bindVertexArray(restoreVA)
    glCtx_.frontFace(restoreFrontFace)
    glCtx_.bindBuffer(glCtx_.ARRAY_BUFFER, restoreArrayBuffer)
    glCtx_.bindBuffer(glCtx_.ELEMENT_ARRAY_BUFFER, restoreIb)
    glCtx_.useProgram(restoreShader)
    glCtx_.bindFramebuffer(glCtx_.FRAMEBUFFER, restoreFb)
    glCtx_.bindRenderbuffer(glCtx_.RENDERBUFFER, restoreRb)
    glCtx_.bindTexture(glCtx_.TEXTURE_CUBE_MAP, restoreTexCube)
    glCtx_.activeTexture(glCtx_.TEXTURE0)
    glCtx_.bindTexture(glCtx_.TEXTURE_2D, restoreTex0)
    glCtx_.activeTexture(glCtx_.TEXTURE1)
    glCtx_.bindTexture(glCtx_.TEXTURE_2D, restoreTex1)
    glCtx_.activeTexture(restoreActive)
    glCtx_.bindTexture(glCtx_.TEXTURE_2D, restoreTex2D)
    if (restoreDepth) {
      glCtx_.enable(glCtx_.DEPTH_TEST)
    }
    if (restoreUnpackFlipY) {
      glCtx_.pixelStorei(glCtx_.UNPACK_FLIP_Y_WEBGL, restoreUnpackFlipY)
    }

    xrcc_.GL.makeContextCurrent(computeHandle_)
  }

  const maybeDispatchConfigChange = (framework: FrameworkHandle, recenter?: boolean) => {
    if ((!configurationChanged_ && !recenter) || !framework) {
      return
    }

    const origin = getOrigin(options_?.coordinates)
    framework.dispatchEvent('cameraconfigured', {
      mirroredDisplay: !!options_?.coordinates?.mirroredDisplay,
      origin,
      recenter: !!recenter,
    })
    configurationChanged_ = false
  }

  const updateEnvironment = ({
    videoWidth,
    videoHeight,
    canvasWidth,
    canvasHeight,
    orientation,
    deviceEstimate,
  }: EnvironmentUpdate) => {
    if (videoWidth != null) {
      env_.videoWidth = videoWidth
    }
    if (videoHeight != null) {
      env_.videoHeight = videoHeight
    }
    if (canvasWidth != null) {
      env_.canvasWidth = canvasWidth
    }
    if (canvasHeight != null) {
      env_.canvasHeight = canvasHeight
    }
    if (orientation != null) {
      env_.orientation = orientation
    }
    if (deviceEstimate != null) {
      env_.deviceEstimate = deviceEstimate
    }

    const message = new Message()
    const env = message.initRoot(CameraEnvironment)
    env.setCameraWidth(env_.videoWidth)
    env.setCameraHeight(env_.videoHeight)
    env.setDisplayWidth(env_.canvasWidth)
    env.setDisplayHeight(env_.canvasHeight)
    env.setOrientation(env_.orientation)
    if (env_.deviceEstimate) {
      env.getDeviceEstimate().setManufacturer(env_.deviceEstimate.manufacturer)
      env.getDeviceEstimate().setModel(env_.deviceEstimate.model)
      env.getDeviceEstimate().setOs(env_.deviceEstimate.os)
      env.getDeviceEstimate().setOsVersion(env_.deviceEstimate.osVersion)
    }

    updateResizerDimensions(videoWidth, videoHeight)

    if (xrcc_ && glCtx_ && glHandle_) {
      xrcc_.GL.makeContextCurrent(glHandle_)

      const restoreVA = vertexArrayBinding()
      const restoreArrayBuffer = glCtx_.getParameter(glCtx_.ARRAY_BUFFER_BINDING)
      const restoreIb = glCtx_.getParameter(glCtx_.ELEMENT_ARRAY_BUFFER_BINDING)
      const restoreShader = glCtx_.getParameter(glCtx_.CURRENT_PROGRAM)
      const restoreFb = glCtx_.getParameter(glCtx_.FRAMEBUFFER_BINDING)
      const restoreRb = glCtx_.getParameter(glCtx_.RENDERBUFFER_BINDING)
      const restoreTex2D = glCtx_.getParameter(glCtx_.TEXTURE_BINDING_2D)
      const restoreTexCube = glCtx_.getParameter(glCtx_.TEXTURE_BINDING_CUBE_MAP)
      const restoreUnpackFlipY = glCtx_.getParameter(glCtx_.UNPACK_FLIP_Y_WEBGL)
      if (restoreUnpackFlipY) {
        glCtx_.pixelStorei(glCtx_.UNPACK_FLIP_Y_WEBGL, false)
      }

      xrcc_._c8EmAsm_initSemanticsRenderer()

      if (!semanticsTex_ ||
        semanticsTexWidth_ !== env_.videoResizeWidth ||
        semanticsTexHeight_ !== env_.videoResizeHeight) {
        deleteSemanticsTexture()

        const texId = xrcc_.GL.getNewId(xrcc_.GL.textures)
        semanticsTex_ = glCtx_.createTexture()
        glCtx_.bindTexture(glCtx_.TEXTURE_2D, semanticsTex_)
        glCtx_.texParameteri(glCtx_.TEXTURE_2D, glCtx_.TEXTURE_WRAP_S, glCtx_.CLAMP_TO_EDGE)
        glCtx_.texParameteri(glCtx_.TEXTURE_2D, glCtx_.TEXTURE_WRAP_T, glCtx_.CLAMP_TO_EDGE)
        glCtx_.texParameteri(glCtx_.TEXTURE_2D, glCtx_.TEXTURE_MIN_FILTER, glCtx_.LINEAR)
        glCtx_.texParameteri(glCtx_.TEXTURE_2D, glCtx_.TEXTURE_MAG_FILTER, glCtx_.LINEAR)

        // Assign a name and pass the texture to GL.textures array.
        // Emscripten uses this array for managing WebGL and OpenGL textures internally.
        semanticsTex_.name = texId
        xrcc_.GL.textures[texId] = semanticsTex_

        semanticsTexWidth_ = env_.videoResizeWidth
        semanticsTexHeight_ = env_.videoResizeHeight

        glCtx_.texImage2D(glCtx_.TEXTURE_2D,
          0,
          glCtx_.RGBA,
          semanticsTexWidth_,
          semanticsTexHeight_,
          0,
          glCtx_.RGBA,
          glCtx_.UNSIGNED_BYTE,
          null)
      }

      if (env_.deviceEstimate) {
        const buffer = message.toArrayBuffer()
        const ptr = xrcc_._malloc(buffer.byteLength)
        xrcc_.writeArrayToMemory(new Uint8Array(buffer), ptr)
        xrcc_._c8EmAsm_updateSemanticsCameraEnvironment(ptr, buffer.byteLength)
        xrcc_._free(ptr)
      }

      const msg = createSemanticsOptionsMsg(
        env_.videoResizeWidth, env_.videoResizeHeight, haveSeenSky_, options_
      )
      const buf = msg.toArrayBuffer()
      const optr = xrcc_._malloc(buf.byteLength)
      xrcc_.writeArrayToMemory(new Uint8Array(buf), optr)
      xrcc_._c8EmAsm_updateSemanticsOptions(optr, buf.byteLength)
      xrcc_._free(optr)

      // restore GL states
      bindVertexArray(restoreVA)
      glCtx_.bindBuffer(glCtx_.ARRAY_BUFFER, restoreArrayBuffer)
      glCtx_.bindBuffer(glCtx_.ELEMENT_ARRAY_BUFFER, restoreIb)
      glCtx_.useProgram(restoreShader)
      glCtx_.bindFramebuffer(glCtx_.FRAMEBUFFER, restoreFb)
      glCtx_.bindRenderbuffer(glCtx_.RENDERBUFFER, restoreRb)
      glCtx_.bindTexture(glCtx_.TEXTURE_2D, restoreTex2D)
      glCtx_.bindTexture(glCtx_.TEXTURE_CUBE_MAP, restoreTexCube)
      if (restoreUnpackFlipY) {
        glCtx_.pixelStorei(glCtx_.UNPACK_FLIP_Y_WEBGL, restoreUnpackFlipY)
      }

      xrcc_.GL.makeContextCurrent(computeHandle_)
    }
  }

  const initializeWebworker = (): Promise<any> => {
    if (worker_) {
      return Promise.resolve(worker_)
    }
    if (!window.Worker) {
      // eslint-disable-next-line no-console
      console.warn('[XR] Web Workers are not available, so semantic segmentation is not available.')
      return Promise.reject(new Error('Web Workers are not available'))
    }

    return new Promise((resolve) => {
      // to use your local version of semantics-worker.js, you can update the importScripts link to
      // https://<your_ip>:8888/reality/app/xr/js/semantics-worker.js.
      // To update the file on CDN, either run:
      //   eslint-disable-next-line max-len
      //   1) bash ./reality/app/xr/js/src/semantics-worker-upload.sh
      //   2) The same build but upload with prod8, ex: https://github.com/8thwall/prod8/pull/374

      const url = ResourceUrls.resolveSemanticsWorker()
      const blob = new Blob([`
        importScripts(${JSON.stringify(url)})
        SEMANTICSMODULE().then(() => self.initWorker())`],
      {type: 'application/javascript'})
      worker_ = new Worker(URL.createObjectURL(blob))
      worker_.onmessage = (e) => {
        switch (e.data.type) {
          case 'loaded': {
            // When the worker loads the layers controller cc module.
            resolve(worker_)
            worker_!.postMessage({
              type: 'start',
              config: {baseUrl: ResourceUrls.getBaseUrl()},
            })
            break
          }
          case 'started': {
            // When the worker loads the semantics classifier model.
            isInvokeReady_ = true
            // NOTE(dat): It's possible that if the WebWorker is really fast that framework_ is
            //            not yet available.
            // TODO(dat): buffer this event and invoke here or onAttach
            framework_?.dispatchEvent('layerscanning', {name: 'sky'})
            break
          }
          case 'invoke': {
            // When the model finishes performing invoke.
            const {data, position, rotation, percentage} = e.data.output
            data_ = data
            percentage_ = percentage

            if (data_ && debugOptions_.inputMask) {
              // update the semantics results for debug output
              updateDebugSemanticsResult(data_,
                SEMANTICS_MODEL_INPUT_WIDTH,
                SEMANTICS_MODEL_INPUT_HEIGHT)
            }

            if (percentage_ > 0.05 && !haveSeenSky_) {
              haveSeenSky_ = true
              framework_?.dispatchEvent('layerfound', {name: 'sky', percentage})
              xrccPromise.then((xrcc) => {
                xrcc.GL.makeContextCurrent(glHandle_)

                const restoreFb = glCtx_.getParameter(glCtx_.FRAMEBUFFER_BINDING)
                const restoreRb = glCtx_.getParameter(glCtx_.RENDERBUFFER_BINDING)
                const restoreTex2D = glCtx_.getParameter(glCtx_.TEXTURE_BINDING_2D)

                const msg = createSemanticsOptionsMsg(
                  env_.videoResizeWidth, env_.videoResizeHeight, haveSeenSky_, options_
                )
                const buf = msg.toArrayBuffer()
                const optr = xrcc._malloc(buf.byteLength)
                xrcc.writeArrayToMemory(new Uint8Array(buf), optr)
                xrcc._c8EmAsm_updateSemanticsOptions(optr, buf.byteLength)
                xrcc._free(optr)

                glCtx_.bindFramebuffer(glCtx_.FRAMEBUFFER, restoreFb)
                glCtx_.bindRenderbuffer(glCtx_.RENDERBUFFER, restoreRb)
                glCtx_.bindTexture(glCtx_.TEXTURE_2D, restoreTex2D)

                xrcc.GL.makeContextCurrent(computeHandle_)
              })
            }
            isInvokeReady_ = true

            if (xrcc_ && data_ && rotation) {
              updateCubemap(data_,
                SEMANTICS_MODEL_INPUT_WIDTH,
                SEMANTICS_MODEL_INPUT_HEIGHT,
                position, rotation)
            }

            if (frame_ === 0) {
              frame_ = 1
            } else {
              frame_ = 0
            }
            break
          }
          default:
            break
        }
      }
    })
  }

  const addWindowListener = (
    name: string,
    callback: (e: Event) => void,
    useCapture = true
  ) => {
    window.addEventListener(name, callback, useCapture)
  }

  const removeWindowListener = (
    name: string,
    callback: (e: Event) => void,
    useCapture = true
  ) => {
    window.removeEventListener(name, callback, useCapture)
  }

  const updateDeviceOrientation = (e: DeviceOrientationEvent) => {
    const {timeStamp, alpha, beta, gamma} = e
    lastDeviceRotation_ = {timeStamp, alpha, beta, gamma}
  }

  const updateDeviceOrientation8w = (
    {detail}: DeviceOrientation8wEvent
  ) => updateDeviceOrientation(detail)

  const initializeLayersController = (): Promise<any> => {
    if (!init_) {
      init_ = Promise.all([
        xrccPromise.then((xrcc) => { xrcc_ = xrcc }),
      ])
    }
    return init_
  }

  const pipelineModule = (): LayersControllerPipelineModule => {
    // Start loading semantics model if it has't been loaded already.
    initializeLayersController()

    const onAttach = ({
      GLctx,
      computeCtx,
      framework,
      canvasWidth,
      canvasHeight,
      videoWidth,
      videoHeight,
      orientation,
    }: onAttachInput) => {
      glCtx_ = GLctx
      if (xrcc_) {
        glHandle_ = xrcc_.GL.registerContext(GLctx, GLctx.getContextAttributes())
        hasVA_ = !!GLctx.bindVertexArray
        VAext_ = hasVA_ ? null : GLctx.getExtension('OES_vertex_array_object')
        computeHandle_ = xrcc_.GL.registerContext(computeCtx, computeCtx.getContextAttributes())
      }

      framework_ = framework
      attached_ = true
      maybeDispatchConfigChange(framework_)

      addWindowListener('deviceorientation', updateDeviceOrientation)
      addWindowListener('deviceorientation8w', updateDeviceOrientation8w)

      xrccPromise
        .then(() => framework.dispatchEvent('layerloading', {}))
        .then(() => initializeLayersController())

      updateEnvironment({
        canvasWidth,
        canvasHeight,
        videoWidth,
        videoHeight,
        orientation,
        deviceEstimate: XR8.XrDevice.deviceEstimate(),
      })

      GlTextureRenderer.setTextureProvider(
        ({processCpuResult: {layerscontroller}}) => (
          layerscontroller ? layerscontroller.cameraFeedTexture : null)
      )
    }

    const onStart = ({
      canvasWidth,
      canvasHeight,
      videoWidth,
      videoHeight,
      orientation,
    }: EnvironmentUpdate) => {
      cameraResizeRenderer_ = null
      cameraResizedTex_ = null  // owned by renderer
      cameraResizedTexRotation_ = null
      glCtx_ = null
      attached_ = false
      src_.videoWidth = 0
      src_.videoHeight = 0
      dest_.width = 0
      dest_.height = 0
      GlTextureRenderer.setTextureProvider(null)

      if (cameraResizeRenderer_) {
        cameraResizeRenderer_.destroy()
      }
      maybeDispatchConfigChange(framework_)
      updateEnvironment({
        canvasWidth,
        canvasHeight,
        videoWidth,
        videoHeight,
        orientation,
        deviceEstimate: XR8.XrDevice.deviceEstimate(),
      })

      initializeWebworker()
    }

    const onDetach = ({framework}: {framework: FrameworkHandle}) => {
      maybeDispatchConfigChange(framework)
      worker_!.postMessage({type: 'cleanup'})
      worker_!.terminate()
      worker_ = null

      deleteSemanticsTexture()

      XR8.GlTextureRenderer.setTextureProvider(null)
      XR8.GlTextureRenderer.configure({mirroredDisplay: false})

      if (xrcc_) {
        xrcc_.GL.makeContextCurrent(glHandle_)

        const restoreVA = vertexArrayBinding()
        const restoreArrayBuffer = glCtx_.getParameter(glCtx_.ARRAY_BUFFER_BINDING)
        const restoreIb = glCtx_.getParameter(glCtx_.ELEMENT_ARRAY_BUFFER_BINDING)
        const restoreShader = glCtx_.getParameter(glCtx_.CURRENT_PROGRAM)
        const restoreFb = glCtx_.getParameter(glCtx_.FRAMEBUFFER_BINDING)
        const restoreRb = glCtx_.getParameter(glCtx_.RENDERBUFFER_BINDING)
        const restoreTex2D = glCtx_.getParameter(glCtx_.TEXTURE_BINDING_2D)
        const restoreTexCube = glCtx_.getParameter(glCtx_.TEXTURE_BINDING_CUBE_MAP)

        xrcc_._c8EmAsm_semanticsControllerCleanup()

        bindVertexArray(restoreVA)
        glCtx_.bindBuffer(glCtx_.ARRAY_BUFFER, restoreArrayBuffer)
        glCtx_.bindBuffer(glCtx_.ELEMENT_ARRAY_BUFFER, restoreIb)
        glCtx_.useProgram(restoreShader)
        glCtx_.bindFramebuffer(glCtx_.FRAMEBUFFER, restoreFb)
        glCtx_.bindRenderbuffer(glCtx_.RENDERBUFFER, restoreRb)
        glCtx_.bindTexture(glCtx_.TEXTURE_2D, restoreTex2D)
        glCtx_.bindTexture(glCtx_.TEXTURE_CUBE_MAP, restoreTexCube)

        xrcc_.GL.makeContextCurrent(computeHandle_)
      }

      // cleanup GL contexts
      glCtx_ = null
      glHandle_ = null
      computeHandle_ = null
      hasVA_ = true
      VAext_ = null

      removeWindowListener('deviceorientation', updateDeviceOrientation)
      removeWindowListener('deviceorientation8w', updateDeviceOrientation8w)

      // we have set the values of the LayersControllerData singleton inside
      // layers-controller.cc to nullptr when calling semanticsControllerCleanup.
      // We need to unreference those values in JS
      // or else we will be pointing to an already deleted object.
      window._c8.response = null
      window._c8.responseSize = null
      window._c8.responseTexture = null
      framework_ = null
    }

    const onProcessGpu = ({framework, frameStartResult}: ProcessGpuInput) => {
      maybeDispatchConfigChange(framework)

      // Copied from camerapixelarray, to obtain the resized image to be processed by CPU.
      const {repeatFrame, textureWidth, textureHeight, cameraTexture} = frameStartResult
      if (!cameraTexture || repeatFrame) {
        return null
      }

      updateResizerDimensions(textureWidth, textureHeight)

      if (!cameraResizeRenderer_ || !lastDeviceRotation_) {
        return null
      }

      let retval: LayersControllerProcessGpuOutput | null = null

      // Check if we have cameraResizedTex_ available to pass to processCpu(). We get this from a
      // previous frame because we use cameraResizeRenderer_ to render this to a smaller texture.
      if (cameraResizedTex_ && cameraResizedTexRotation_) {
        const restoreFb = glCtx_.getParameter(glCtx_.FRAMEBUFFER_BINDING)
        glCtx_.bindFramebuffer(glCtx_.FRAMEBUFFER, cameraResizeRenderer_.framebuffer())

        const cols = dest_.width
        const rows = dest_.height
        const pixels = new Uint8Array(cols * rows * 4)
        const rowBytes = cols * 4

        glCtx_.readPixels(
          0,
          0,
          cols,
          rows,
          glCtx_.RGBA,
          glCtx_.UNSIGNED_BYTE,
          pixels
        )

        glCtx_.bindFramebuffer(glCtx_.FRAMEBUFFER, restoreFb)

        retval = {
          invokeData: {rotation: cameraResizedTexRotation_!, pixels, rows, cols, rowBytes},
        }
      }

      // Get the device rotation for the current frame.
      const {alpha, beta, gamma} = lastDeviceRotation_
      const deviceRotation = getQuaternion(alpha, beta, gamma)

      // render center part of the video texture into the viewport
      const offX = (env_.videoResizeWidth - env_.modelInputWidth) / 2
      const offY = (env_.videoResizeHeight - env_.modelInputHeight) / 2
      const viewport = {
        offsetX: -offX,
        offsetY: -offY,
        width: env_.videoResizeWidth,
        height: env_.videoResizeHeight,
      }
      cameraResizedTex_ = cameraResizeRenderer_.render({renderTexture: cameraTexture, viewport})
      cameraResizedTexRotation_ = deviceRotation

      // Stage the current frame (raw camera texture and rotation) for later processing.
      if (xrcc_) {
        const message = new Message()
        const cam = message.initRoot(SemanticsCameraTransformMsg)
        cam.setCameraTexture(cameraTexture.name)
        updateCameraPoseMsg(cam, IDENTITY_POS, deviceRotation)

        const buffer = message.toArrayBuffer()
        const ptr = xrcc_._malloc(buffer.byteLength)
        xrcc_.writeArrayToMemory(new Uint8Array(buffer), ptr)
        xrcc_._c8EmAsm_stageLayersControllerFrame(ptr, buffer.byteLength)
      }

      return retval
    }

    const onProcessCpu = ({
      framework,
      frameStartResult,
      processGpuResult,
    }: LayersControllerProcessCpuInput): LayersControllerProcessCpuOutput | null => {
      if (runConfig.paused || !processGpuResult) {
        return null
      }

      maybeDispatchConfigChange(framework)
      const {repeatFrame} = frameStartResult

      if (!repeatFrame) {
        if (!window._c8) {
          window._c8 = {}
        }

        if (glCtx_ && semanticsTex_ && semanticsTex_.name) {
          xrcc_.GL.makeContextCurrent(glHandle_)

          // c8EmAsm_processStagedLayersControllerFrame() renders the cubemap into semanticsTex_.
          // Save gl state before this and restore it after.
          const restoreViewport = glCtx_.getParameter(glCtx_.VIEWPORT)
          const restoreVA = vertexArrayBinding()
          const restoreArrayBuffer = glCtx_.getParameter(glCtx_.ARRAY_BUFFER_BINDING)
          const restoreIb = glCtx_.getParameter(glCtx_.ELEMENT_ARRAY_BUFFER_BINDING)
          const restoreShader = glCtx_.getParameter(glCtx_.CURRENT_PROGRAM)
          const restoreFb = glCtx_.getParameter(glCtx_.FRAMEBUFFER_BINDING)
          const restoreRb = glCtx_.getParameter(glCtx_.RENDERBUFFER_BINDING)
          const restoreTex2D = glCtx_.getParameter(glCtx_.TEXTURE_BINDING_2D)
          const restoreTexCube = glCtx_.getParameter(glCtx_.TEXTURE_BINDING_CUBE_MAP)
          const restoreFrontFace = glCtx_.getParameter(glCtx_.FRONT_FACE)
          const restoreActive = glCtx_.getParameter(glCtx_.ACTIVE_TEXTURE)
          // Get the textures bind with the texture units that we are overwriting
          glCtx_.activeTexture(glCtx_.TEXTURE0)
          const restoreTex0 = glCtx_.getParameter(glCtx_.TEXTURE_BINDING_2D)
          glCtx_.activeTexture(glCtx_.TEXTURE1)
          const restoreTex1 = glCtx_.getParameter(glCtx_.TEXTURE_BINDING_2D)

          xrcc_._c8EmAsm_processStagedLayersControllerFrame(
            semanticsTex_.name, semanticsTexWidth_, semanticsTexHeight_
          )

          glCtx_.viewport(
            restoreViewport[0], restoreViewport[1], restoreViewport[2], restoreViewport[3]
          )
          glCtx_.frontFace(restoreFrontFace)
          bindVertexArray(restoreVA)
          glCtx_.bindBuffer(glCtx_.ARRAY_BUFFER, restoreArrayBuffer)
          glCtx_.bindBuffer(glCtx_.ELEMENT_ARRAY_BUFFER, restoreIb)
          glCtx_.useProgram(restoreShader)
          glCtx_.bindFramebuffer(glCtx_.FRAMEBUFFER, restoreFb)
          glCtx_.bindRenderbuffer(glCtx_.RENDERBUFFER, restoreRb)
          glCtx_.bindTexture(glCtx_.TEXTURE_CUBE_MAP, restoreTexCube)
          glCtx_.activeTexture(glCtx_.TEXTURE1)
          glCtx_.bindTexture(glCtx_.TEXTURE_2D, restoreTex1)
          glCtx_.activeTexture(glCtx_.TEXTURE0)
          glCtx_.bindTexture(glCtx_.TEXTURE_2D, restoreTex0)

          glCtx_.activeTexture(restoreActive)
          glCtx_.bindTexture(glCtx_.TEXTURE_2D, restoreTex2D)

          xrcc_.GL.makeContextCurrent(computeHandle_)
        }

        // Skips frames if webworker is still running invoke.
        if (isInvokeReady_) {
          // Classifier cannot perform another invoke if it is already invoking.
          isInvokeReady_ = false

          // Resizing the image for semantics classifer.
          // Currently works only for portrait. It will display for landscape as well, but it'll
          // be cropped to fit a portrait view.
          const resizedPixels = new Uint8Array(
            4 * SEMANTICS_MODEL_INPUT_WIDTH * SEMANTICS_MODEL_INPUT_HEIGHT
          )
          resizedPixels.fill(0)
          if (env_.orientation === 90) {
            toImageArrayRotateCW90(
              processGpuResult.layerscontroller.invokeData.pixels,
              processGpuResult.layerscontroller.invokeData.cols,
              processGpuResult.layerscontroller.invokeData.rows,
              SEMANTICS_MODEL_INPUT_WIDTH,
              SEMANTICS_MODEL_INPUT_HEIGHT,
              resizedPixels
            )
          } else if (env_.orientation === -90) {
            toImageArrayRotateCCW90(
              processGpuResult.layerscontroller.invokeData.pixels,
              processGpuResult.layerscontroller.invokeData.cols,
              processGpuResult.layerscontroller.invokeData.rows,
              SEMANTICS_MODEL_INPUT_WIDTH,
              SEMANTICS_MODEL_INPUT_HEIGHT,
              resizedPixels
            )
          } else {
            toImageArrayLetterbox(
              processGpuResult.layerscontroller.invokeData.pixels,
              processGpuResult.layerscontroller.invokeData.cols,
              processGpuResult.layerscontroller.invokeData.rows,
              SEMANTICS_MODEL_INPUT_WIDTH,
              SEMANTICS_MODEL_INPUT_HEIGHT,
              resizedPixels
            )
          }

          worker_!.postMessage({
            type: 'invoke',
            img: resizedPixels.buffer,
            position: IDENTITY_POS,
            rotation: processGpuResult.layerscontroller.invokeData.rotation,
          },
          [resizedPixels.buffer])
        }
      }

      if (!window._c8.response || !window._c8.responseTexture) {
        return null
      }

      // Read semantics results
      const byteBuffer =
        xrcc_.HEAPU8.subarray(window._c8.response, window._c8.response + window._c8.responseSize)
      const message = new Message(byteBuffer, false).getRoot(SemanticsResponse)

      const camera = message.getCamera()
      const intrinsics = camera.getIntrinsic().getMatrix44f().toArray()
      const extrinsic = camera.getExtrinsic()
      const xrq = extrinsic.getRotation()
      const xrp = extrinsic.getPosition()
      const rotation = {x: xrq.getX(), y: xrq.getY(), z: xrq.getZ(), w: xrq.getW()}
      const position = {x: xrp.getX(), y: xrp.getY(), z: xrp.getZ()}

      const skyOutput: LayerOutput = {
        texture: semanticsTex_,
        textureWidth: semanticsTexWidth_,
        textureHeight: semanticsTexHeight_,
        percentage: percentage_,
      }

      // Extract debug output if requested.
      if (debugOptions_.inputMask) {
        skyOutput.debug = {}
        if (debugSemanticsMask_.pixels) {
          Object.assign(skyOutput.debug, {
            inputMaskImg: {
              rows: debugSemanticsMask_.rows,
              cols: debugSemanticsMask_.cols,
              rowBytes: debugSemanticsMask_.rowBytes,
              pixels: debugSemanticsMask_.pixels,
            },
          })
        }
      }

      return {
        position,
        rotation,
        intrinsics,
        cameraFeedTexture: XR8.drawTexForComputeTex(window._c8.responseTexture),
        layers: {
          'sky': skyOutput,
        },
      }
    }

    const onCanvasSizeChange = ({
      canvasWidth,
      canvasHeight,
      videoWidth,
      videoHeight,
    }: EnvironmentUpdate) => {
      maybeDispatchConfigChange(framework_)
      updateEnvironment({canvasWidth, canvasHeight, videoWidth, videoHeight})
    }

    const onVideoSizeChange = ({
      canvasWidth,
      canvasHeight,
      videoWidth,
      videoHeight,
      orientation,
    }: EnvironmentUpdate) => {
      maybeDispatchConfigChange(framework_)
      updateEnvironment({canvasWidth, canvasHeight, videoWidth, videoHeight, orientation})
    }

    const onDeviceOrientationChange = ({
      videoWidth,
      videoHeight,
      orientation,
    }: EnvironmentUpdate) => {
      maybeDispatchConfigChange(framework_)
      updateEnvironment({videoWidth, videoHeight, orientation})
    }

    const onCameraStatusChange = ({status, video}: onCameraStatusChangeInput) => {
      if (status !== 'hasVideo' || !video) {
        return
      }
      maybeDispatchConfigChange(framework_)
      updateEnvironment({
        videoWidth: video.videoWidth,
        videoHeight: video.videoHeight,
      })
    }

    const requiredPermissions = () => [
      XrPermissionsFactory().permissions().CAMERA,
      XrPermissionsFactory().permissions().DEVICE_MOTION,
      XrPermissionsFactory().permissions().DEVICE_ORIENTATION]

    const onBeforeSessionInitialize = ({
      sessionAttributes,
    }: {sessionAttributes: SessionAttributes}) => {
      if (!sessionAttributes.fillsCameraTexture) {
        throw new Error(
          '[XR] LayersController requires a session that can provide a camera texture'
        )
      }
    }

    return {
      name: 'layerscontroller',
      onAttach,
      onBeforeSessionInitialize,
      onCameraStatusChange,
      onCanvasSizeChange,
      onDeviceOrientationChange,
      onDetach,
      onProcessCpu,
      onProcessGpu,
      onStart,
      onVideoSizeChange,
      requiredPermissions,
    }
  }

  // Update `options_` state with new partial configuration.
  const optionsReducer = (args: LayersControllerArgs) => {
    if (!args) {
      return
    }

    if (args.nearClip !== undefined) {
      options_.nearClip = args.nearClip
    }
    if (args.farClip !== undefined) {
      options_.farClip = args.farClip
    }

    // Update coordinate configuration piece by piece to allow different components to be set at
    // different times.
    if (args.coordinates === null) {
      options_.coordinates = null
    } else if (args.coordinates !== undefined) {
      if (!options_.coordinates) {
        options_.coordinates = {origin: {}}
      }
      const {origin, axes, mirroredDisplay} = args.coordinates
      if (origin !== undefined) {
        configurationChanged_ = true
        if (!origin) {
          options_.coordinates.origin = {}
        }
        if (origin?.position) {
          options_.coordinates.origin!.position =
            {x: origin.position.x, y: origin.position.y, z: origin.position.z}
        }
        if (origin?.rotation) {
          options_.coordinates.origin!.rotation = {
            w: origin.rotation.w, x: origin.rotation.x, y: origin.rotation.y, z: origin.rotation.z,
          }
        }
      }
      if (axes !== undefined) {
        options_.coordinates.axes = axes
      }
      if (mirroredDisplay !== undefined) {
        configurationChanged_ = configurationChanged_ ||
          (!!mirroredDisplay !== options_.coordinates.mirroredDisplay)
        options_.coordinates.mirroredDisplay = !!mirroredDisplay
      }
    }

    if (args.layers === null) {
      options_.layers = {}
    } else if (args.layers !== undefined) {
      Object.keys(args.layers).forEach((layer) => {
        if (!SUPPORTED_LAYERS.includes(layer)) {
          throw new Error(
            `[XR] Invalid layer '${layer}'. Supported layers are: ${SUPPORTED_LAYERS
              .join(', ')}.`
          )
        }
      })

      Object.entries(args.layers).forEach(([layerName, layerOptions]) => {
        // configure({layers: {sky: null}}) will delete the sky layer.
        if (layerOptions === null) {
          delete options_.layers[layerName]
          return
        }
        // configure({layers: {sky: undefined}}) is a no-op.
        if (!layerOptions) {
          return
        }

        // If we don't yet have this layer, start out with default values.
        if (!(layerName in options_.layers)) {
          options_.layers[layerName] = {
            invertLayerMask: DEFAULT_INVERT_LAYER_MASK,
            edgeSmoothness: DEFAULT_EDGE_SMOOTHNESS,
          }
        }

        // Then update the layer piece by piece.
        // - configure({layers: {sky: {foo: null}}}) will reset foo to its default value.
        // - configure({layers: {sky: {foo: undefined}}}) is a no-op.
        if (layerOptions.invertLayerMask === null) {
          options_.layers[layerName].invertLayerMask = DEFAULT_INVERT_LAYER_MASK
        } else if (layerOptions.invertLayerMask !== undefined) {
          if (typeof (layerOptions.invertLayerMask) !== 'boolean') {
            throw new Error(
              `[XR] Layer 'invertLayerMask' should have type 'boolean' but has type '
              ${typeof (layerOptions.invertLayerMask)}'`
            )
          }
          options_.layers[layerName].invertLayerMask = layerOptions.invertLayerMask
        }
        if (layerOptions.edgeSmoothness === null) {
          options_.layers[layerName].edgeSmoothness = DEFAULT_EDGE_SMOOTHNESS
        } else if (layerOptions.edgeSmoothness !== undefined) {
          if (typeof (layerOptions.edgeSmoothness) !== 'number') {
            throw new Error(
              `[XR] Layer 'edgeSmoothness' should have type 'number' but has type '
              ${typeof (layerOptions.edgeSmoothness)}'`
            )
          }
          options_.layers[layerName].edgeSmoothness = layerOptions.edgeSmoothness
        }
      })
    }

    if (args.debug === null) {
      debugOptions_.inputMask = false
    } else if (args.debug !== undefined) {
      debugOptions_.inputMask = args.debug.inputMask
    }
  }

  /**
   * Configures LayersController.
   * @param args the LayersController configuration.
   */
  const configure = (args: LayersControllerArgs) => {
    if (!args) {
      return
    }

    optionsReducer(args)

    xrccPromise.then((xrcc) => {
      if (env_.videoResizeWidth > 0 && env_.videoResizeHeight > 0 && glCtx_) {
        xrcc.GL.makeContextCurrent(glHandle_)

        const restoreFb = glCtx_.getParameter(glCtx_.FRAMEBUFFER_BINDING)
        const restoreRb = glCtx_.getParameter(glCtx_.RENDERBUFFER_BINDING)
        const restoreTex2D = glCtx_.getParameter(glCtx_.TEXTURE_BINDING_2D)

        const msg = createSemanticsOptionsMsg(
          env_.videoResizeWidth, env_.videoResizeHeight, haveSeenSky_, options_
        )
        const buf = msg.toArrayBuffer()
        const optr = xrcc._malloc(buf.byteLength)
        xrcc.writeArrayToMemory(new Uint8Array(buf), optr)
        xrcc._c8EmAsm_updateSemanticsOptions(optr, buf.byteLength)
        xrcc._free(optr)

        glCtx_.bindFramebuffer(glCtx_.FRAMEBUFFER, restoreFb)
        glCtx_.bindRenderbuffer(glCtx_.RENDERBUFFER, restoreRb)
        glCtx_.bindTexture(glCtx_.TEXTURE_2D, restoreTex2D)

        xrcc.GL.makeContextCurrent(computeHandle_)
      }

      // By updating the texture renderer inside the promise we keep the camera feed in sync with
      // the layers controller pipeline output.
      GlTextureRenderer.configure({
        mirroredDisplay: !!(options_.coordinates && options_.coordinates.mirroredDisplay),
      })
    })
  }

  const getLayerNames = (): readonly string[] => Object.keys(options_.layers)

  const recenter = () => {
    if (xrcc_) {
      xrcc_._c8EmAsm_semanticsControllerRecenter()
    }
    maybeDispatchConfigChange(framework_, true)
  }

  return {
    pipelineModule,
    configure,
    recenter,
    getLayerNames,
  }
})

export {
  LayersControllerFactory,
}

export type {
  LayersController,
}
