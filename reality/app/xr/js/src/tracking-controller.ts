// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

// @dep(//reality/app/xr/js:xr-tracking-wasm)

/* global XR8:readonly */

/* eslint-disable import/no-unresolved */
import * as capnp from 'capnp-ts'

// The following two imports resolve because of the NODE_PATH variabled set in build.js
/* eslint-disable import/no-unresolved */
import XRTRACKINGCC from 'reality/app/xr/js/xr-tracking-wasm'

import {Message} from 'capnp-ts'
import {DeviceInfo} from 'reality/engine/api/device/info.capnp'
import {
  CurvyGeometry,
  ImageTargetMetadata,
  DetectedImage,
  ImageTargetNames,
  RealityResponse,
  XRConfiguration,
  CoordinateSystemConfiguration,
  XrQueryRequest,
  XrQueryResponse,
  XrHitTestResult,
  ImageTargetTypeMsg,
} from 'reality/engine/api/reality.capnp'
import {XrTrackingState} from 'reality/engine/api/response/pose.capnp'

/* eslint-enable import/no-unresolved */

import {
  ImageTargetType,
  TargetMetadata,
  ImageTargetData,
  ProcessedImageTargetData,
  DetectedImageTarget,
} from '@repo/reality/shared/engine/image-targets'

import type {XrccModule} from '@repo/reality/app/xr/js/src/types/xrcc'
import type {XrTrackingccModule} from '@repo/reality/app/xr/js/src/types/xrtrackingcc'

// For window._c8
// @dep(//reality/app/xr/js/src/types:xrmodule)

import {getQuaternion} from './quaternion'
import {GlTextureRenderer} from './gl-renderer'

import {XrDeviceFactory} from './js-device'
import {XrPermissionsFactory} from './permissions'
import {XrConfigFactory} from './config'
import {Sensors} from './sensors'
import {singleton} from './factory'
import {writeStringToEmscriptenHeap} from './write-emscripten'
import {CompressionFormat} from './types/recorder'
import type {FrameStartResult, FrameworkHandle, RenderContext} from './types/pipeline'
import type * as Callbacks from './types/callbacks'
import type {XrPermission} from './types/common'
import type {CameraProjectionMatrix} from './types/api'

const loadRemoteImageUrl = (url: string, fragment: DocumentFragment) => (
  new Promise<HTMLImageElement>((resolve, reject) => {
    const img = document.createElement('img')
    img.onload = () => resolve(img)
    img.onerror = reject
    img.setAttribute('crossorigin', 'anonymous')
    img.setAttribute('src', url)
    fragment.appendChild(img)
  })
)

const getImageTargetTypeFromXrhomeString = (typeString: ImageTargetData['type']) => {
  switch (typeString) {
    case ImageTargetType.PLANAR.xrhomeName:
      return ImageTargetType.PLANAR
    case ImageTargetType.CYLINDER.xrhomeName:
      return ImageTargetType.CYLINDER
    case ImageTargetType.CONICAL.xrhomeName:
      return ImageTargetType.CONICAL
    default:
      return ImageTargetType.UNSPECIFIED
  }
}

const getImageTargetApiTypeFromMsg = (imageTargetMsg: DetectedImage) => {
  switch (imageTargetMsg.getType()) {
    case ImageTargetTypeMsg.CURVY:
      return Math.abs(imageTargetMsg.getCurvyGeometry().getTopRadius() -
        imageTargetMsg.getCurvyGeometry().getBottomRadius()) > 0.001
        ? ImageTargetType.CONICAL.apiName
        : ImageTargetType.CYLINDER.apiName
    case ImageTargetTypeMsg.PLANAR:
      return ImageTargetType.PLANAR.apiName
    default:
      return ImageTargetType.UNSPECIFIED.apiName
  }
}

const metadataFromImageTarget = (it: ImageTargetData) => {
  // Take user metadata as a string initially.
  let {metadata} = it
  if (it.userMetadataIsJson) {
    try {
      // If user metadata can parse as json, parse it.
      metadata = JSON.parse(metadata)
    } catch (e) {
      // pass
    }
  }
  return metadata
}

const extractTargetMetadata = (it: ImageTargetData): ProcessedImageTargetData => {
  const type = getImageTargetTypeFromXrhomeString(it.type)
  const targetMetadata: ProcessedImageTargetData = {
    name: it.name,
    type: type.apiName,
    metadata: metadataFromImageTarget(it),
    properties: null,
  }

  let xrMetadata: TargetMetadata = {}
  if (type === ImageTargetType.PLANAR || type === ImageTargetType.UNSPECIFIED) {
    // TODO(paris) Prevent additional fields from being added to this
    xrMetadata.moveable = it.moveable
    xrMetadata.physicalWidthInMeters = it.physicalWidthInMeters
  }
  xrMetadata = {
    ...xrMetadata,
    ...it.properties,
  }

  targetMetadata.properties = xrMetadata
  return targetMetadata
}

const initFakeRealityTexture = (GLctx: RenderContext) => {
  const blackPixel = new Uint8Array([45, 46, 67, 255])

  const restoreTex = GLctx.getParameter(GLctx.TEXTURE_BINDING_2D)

  const texture = GLctx.createTexture()
  GLctx.bindTexture(GLctx.TEXTURE_2D, texture)
  GLctx.texImage2D(
    GLctx.TEXTURE_2D,
    0,
    GLctx.RGBA,
    1 /* width */,
    1 /* height */,
    0,
    GLctx.RGBA,
    GLctx.UNSIGNED_BYTE,
    blackPixel
  )

  GLctx.bindTexture(GLctx.TEXTURE_2D, restoreTex)

  return texture
}

const createAuthTokenSubscription = (sccPromise_: Promise<XrTrackingccModule>) => {
  let handle_
  // NOTE(christoph): We use pendingLoad_ to skip calling into the engine if the
  // subscription has been cancelled, or a new value was received, while the xrccPromise
  // was pending.
  let pendingLoad_ = 0

  const setXrSessionTokenInEngine = (xrSessionToken: string) => {
    const load = ++pendingLoad_
    sccPromise_.then((scc) => {
      if (load !== pendingLoad_) {
        return
      }
      const xrSessionTokenPtr = writeStringToEmscriptenHeap(scc, xrSessionToken)
      scc._c8EmAsm_setXrSessionToken(xrSessionTokenPtr)
      scc._free(xrSessionTokenPtr)
    })
  }

  const start = () => {
    if (!handle_) {
      handle_ = XR8.Platform.registerAuthorizationTokenCallback(setXrSessionTokenInEngine)
    }
  }

  const stop = () => {
    pendingLoad_++
    if (handle_) {
      XR8.Platform.unregisterAuthorizationTokenCallback(handle_)
      handle_ = null
    }
  }

  return {
    start,
    stop,
  }
}

interface TrackingControllerConfig {
  enableLighting?: boolean
  imageTargets?: string[]
  imageTargetData?: ImageTargetData[]
  leftHandedAxes?: boolean
  mirroredDisplay?: boolean
  enableJpgEncoding?: boolean
  enableDataRecorder?: boolean

  // These are not documented
  mapSrcUrl?: string
  // Deprecated
  enableFeatureSet: boolean
}

const XrControllerFactory = singleton((
  runConfig,
  xrccPromise: Promise<XrccModule>,
  logCallback,
  errorCallback
) => {
  // XRCC is expected to be loaded before handling the first camera frame.
  let xrcc_: XrccModule = null
  xrccPromise.then((xrcc) => {
    xrcc_ = xrcc
  })

  let computeHandle_: number = 0  // handle for Compute Context

  let toCheckAllZeros_: number | null = null  // check this flag against window._c8.checkAllZeroes

  // Must resolve the returned promise to ensure that this module CC gets loaded before accessing
  // the constructed controller instance.
  let scc_: XrTrackingccModule = null  // local slam cc
  const resolveXrTrackingCc = async (): Promise<XrTrackingccModule> => {
    if (!scc_) {
      const scc = await XRTRACKINGCC({
        print: logCallback(false),
        printErr: logCallback(true),
        onAbort: errorCallback,
        onError: errorCallback,
      })
      scc_ = scc
    }
    return scc_
  }
  const xrTrackingPromise_ = resolveXrTrackingCc()

  const sensors = Sensors(xrTrackingPromise_)

  const enableCamera = true
  const enableSurfaces = false
  const enableVerticalSurfaces = false
  const enableCameraAutofocus = true

  let isSimulator_ = false

  let fakeSimulatorExtrinsic_ = {position: {x: 0, y: 0, z: 0}, rotation: {w: 1, x: 0, y: 0, z: 0}}

  let lastDevicePose_ = {timeStamp: 0, alpha: 0, beta: 0, gamma: 0}
  let lastGeolocation_ = {latitude: 0, longitude: 0, accuracy: 0}

  // Whether we will try and estimate the metric scale.
  let enableLighting_ = false

  let disableWorldTracking_ = false
  let leftHandedAxes_ = false
  let mirroredDisplay_ = false
  // Image target data set in config.
  let configuredImageTargetData_: ImageTargetData[] = null
  // Names of image targets loaded in the engine.
  let currentTargetUrls_: string[] = []
  // When true, dispatch an event so other modules can know about these changes
  let notifyOthersOfConfigChange_ = false

  // The current tracking status.
  let trackingStatus_ = 'UNSPECIFIED'
  let trackingReason_ = 'UNSPECIFIED'

  let cameraProcessingDisabled_ = false

  let scale_ = 1.0
  let origin_ = {x: 0, y: 2, z: 0}
  let facing_ = {w: 1, x: 0, y: 0, z: 0}
  let recenterOrigin_ = {x: 0, y: 2, z: 0}
  let recenterFacing_ = {w: 1, x: 0, y: 0, z: 0}
  const cam_ = {pixelRectWidth: 0, pixelRectHeight: 0, nearClipPlane: 0.01, farClipPlane: 1000.0}
  let mapSrcUrl_ = ''

  // Currently detected Image Targets. Key is the Image Target name, value is the object.
  const detectedImageMap_: Record<string, DetectedImageTarget> = {}
  const imagesToLoad_: number[] = []
  let targetMetadata_: ProcessedImageTargetData[] = []

  const authTokenSubscription = createAuthTokenSubscription(xrTrackingPromise_)

  let imgId_ = 0
  let imageBatchLoadId_ = 0
  const imageTargetCache_: Record<string, ImageTargetData> = {}

  let appResourcesLoaded_ = false
  let framework_: FrameworkHandle = null
  let drawCtx = null
  let fakeRealityTexture_  // Fake a reality texture in headset mode so the loading screen goes away

  let intrinsics_ = null

  let enableDataRecorder_ = false
  let isDataRecording_ = false
  // scratch space for getting YUV data from yuv-pixels-array into WASM
  let yuvBufferInitialized_ = false
  let yuvBuffer_: number  // offset into WASM heap for yuvBuffer
  let yuvBufferArray_: Uint8Array
  const initializeYuvBuffer = (size: number) => {
    yuvBuffer_ = scc_._malloc(size)
    yuvBufferInitialized_ = true
  }

  const collectedLogChunks_: {message: ArrayBuffer}[] = []

  // Data recorder needs to be enabled before it can be started
  const startDatarecorder = () => {
    if (!enableDataRecorder_) {
      // eslint-disable-next-line no-console
      console.warn('[XR] Data Recorder not enabled. Enable it first before starting.')
      return
    }
    isDataRecording_ = true
    scc_._c8EmAsm_setIsDataRecording(true)
  }

  const stopDatarecorder = (): Blob | undefined => {
    if (enableDataRecorder_) {
      isDataRecording_ = false
      scc_._c8EmAsm_setIsDataRecording(false)
      let combinedLogBlob: Blob | undefined
      if (collectedLogChunks_.length > 0) {
        // NOTE(jason): Reinitialize buffer. For some reason yuvBufferArray_.set(pixels) gets stuck
        // after once video cycle on simulator without this.
        yuvBufferInitialized_ = false
        combinedLogBlob = new Blob(collectedLogChunks_.map(c => c.message))

        collectedLogChunks_.length = 0
      }
      return combinedLogBlob
    }
    return undefined
  }

  const getIsDataRecording = () => isDataRecording_

  const maybeDispatchConfigChange = (framework: FrameworkHandle, recenter?: boolean) => {
    if ((!notifyOthersOfConfigChange_ && !recenter) || !framework) {
      return
    }

    framework.dispatchEvent('cameraconfigured', {
      mirroredDisplay: mirroredDisplay_,
      origin: {
        position: origin_,
        rotation: facing_,
      },
      recenter: !!recenter,
    })
    notifyOthersOfConfigChange_ = false
  }

  const updateDeviceExtrinsic = ({position, rotation}: typeof fakeSimulatorExtrinsic_) => {
    fakeSimulatorExtrinsic_ = {position, rotation}
  }

  const updateDeviceOrientation = ({timeStamp, alpha, beta, gamma}: typeof lastDevicePose_) => {
    lastDevicePose_ = {timeStamp, alpha, beta, gamma}
  }

  const updateDeviceGeolocation = ({coords}: {coords: typeof lastGeolocation_}) => {
    lastGeolocation_ = {
      latitude: coords.latitude, longitude: coords.longitude, accuracy: coords.accuracy,
    }
  }

  type WindowEventListener = (event: Event & any) => void

  const addWindowListener = (
    name: string,
    callback: WindowEventListener,
    useCapture = true
  ) => {
    window.addEventListener(name, callback, useCapture)
  }

  const removeWindowListener = (name: string,
    callback: WindowEventListener, useCapture = true) => {
    window.removeEventListener(name, callback, useCapture)
  }

  const removeMotionOrientationListeners = () => {
    removeWindowListener('deviceorientation', updateDeviceOrientation)
    removeWindowListener('devicemotion', sensors.updateDeviceMotion)
  }

  const addMotionOrientationListeners = () => {
    addWindowListener('deviceorientation', updateDeviceOrientation)
    addWindowListener('devicemotion', sensors.updateDeviceMotion)
  }

  const forwardCustomMessage = (event: any) => {
    if (event.data.deviceOrientation8w) {
      const customDeviceOrientationEvent = new CustomEvent(
        'deviceorientation8w',
        {
          detail: event.data.deviceOrientation8w,
        }
      )
      window.dispatchEvent(customDeviceOrientationEvent)
    }
    if (event.data.deviceMotion8w) {
      const customDeviceMotionEvent = new CustomEvent(
        'devicemotion8w',
        {
          detail: event.data.deviceMotion8w,
        }
      )
      window.dispatchEvent(customDeviceMotionEvent)
    }
    if (event.data.gps8w) {
      const customGpsEvent = new CustomEvent(
        'gps8w',
        {
          detail: event.data.gps8w,
        }
      )
      window.dispatchEvent(customGpsEvent)
    }
  }

  const onProcessGpu: Callbacks.ProcessGpuHandler = ({framework, frameStartResult}) => {
    const {
      cameraTexture,
      textureWidth,
      textureHeight,
      orientation,
      repeatFrame,
      videoTime,
      frameTime,
    } = frameStartResult
    if (repeatFrame || cameraProcessingDisabled_) {
      return
    }
    maybeDispatchConfigChange(framework)
    // TODO(dat): Plump lastDevicePose_.timeStamp to cc
    const {alpha, beta, gamma} = lastDevicePose_
    const devicePose = getQuaternion(alpha, beta, gamma)
    const timeNanos = window.performance.now() * 1000 * 1000

    // TODO(paris): This data could just be passed in with _c8EmAsm_stageFrame.
    sensors.setEventQueue()

    scc_._c8EmAsm_stageFrame(
      cameraTexture.name,
      textureWidth,
      textureHeight,
      orientation,
      alpha,
      beta,
      gamma,
      devicePose.w,
      devicePose.x,
      devicePose.y,
      devicePose.z,
      videoTime,
      frameTime,
      timeNanos,
      lastGeolocation_.latitude,
      lastGeolocation_.longitude,
      lastGeolocation_.accuracy
    )
  }

  // Returns:
  // {
  //   rotation: {w, x, y, z} The orientation (quaternion) of the camera in the scene.
  //   position: {x, y, z} The position of the camera in the scene.
  //   intrinsics: a 16 dimensional column-major 4x4 projection matrix that gives the scene
  //     camera the same field of view as the rendered camera feed.
  //   realityTexture: The WebGLTexture containing camera feed data.
  //
  //   trackingStatus: One of "UNSPECIFIED", "NOT_AVAILABLE, "LIMITED" or "NORMAL".
  //   trackingReason: One of "UNSPECIFIED", "INITIALIZING", "RELOCALIZING",
  //                          "TOO_MUCH_MOTION", "NOT_ENOUGH_TEXTURE".
  //
  //   worldPoints: [{id, confidence, position: {x, y, z}}] An array of detected points in the
  //     world at their location in the scene. Only filled if XrController is configured to return
  //     world points and trackingReason != INITIALIZING.
  // }
  //
  // Dispatches Events:
  //
  // imageloading: Fires when detection image loading begins.
  //    detail: {
  //      name: The image's name.
  //      type: The target type: "FLAT", "CYLINDRICAL", "CONICAL".
  //      metadata: User metadata.
  //    }
  // imagescanning: Fires when all detection images have been loaded and scanning has begun.
  //    detail: {
  //      name: The image's name.
  //      type: The target type: "FLAT", "CYLINDRICAL", "CONICAL".
  //      metadata: User metadata.
  //
  //      If type=FLAT:
  //      geometry: {
  //        scaledWidth: The width of the image in the scene, when multiplied by scale.
  //        scaledHeight: The height of the image in the scene, when multiplied by scale.
  //      }
  //
  //      Else if type=CYLINDRICAL or type=CONICAL:
  //      geometry: {
  //        height: Height of the curved target.
  //        radiusTop: Radius of the curved target at the top.
  //        radiusBottom: Radius of the curved target at the bottom.
  //        arcStartRadians: Starting angle in radians.
  //        arcLengthRadians: Central angle in radians.
  //      }
  // imagefound: Fires when an image target is first found.
  // imageupdated: Fires when an image target changes position, rotation or scale.
  // imagelost: Fires when an image target is no longer being tracked.
  //    detail: {
  //      id: A numerical id of the located image.  // TODO(nb): removeme.
  //      name: The image's name.
  //      type: The target type: "FLAT", "CYLINDRICAL", "CONICAL".
  //      position {x, y, z}: The 3d position of the located image.
  //      rotation {w, x, y, z}: The 3d local orientation of the located image.
  //      scale: A scale factor that should be applied to object attached to this image.
  //
  //      If type=FLAT:
  //      scaledWidth: The width of the image in the scene, when multiplied by scale.
  //      scaledHeight: The height of the image in the scene, when multiplied by scale.
  //
  //      Else if type=CYLINDRICAL or type=CONICAL:
  //      height: Height of the curved target.
  //      radiusTop: Radius of the curved target at the top.
  //      radiusBottom: Radius of the curved target at the bottom.
  //      arcStartRadians: Starting angle in radians.
  //      arcLengthRadians: Central angle in radians.
  //    }
  //
  // trackingstatus: Fires when the tracking status or tracking reason is updated.
  //    detail: {
  //      status: One of "UNSPECIFIED", "NOT_AVAILABLE, "LIMITED" or "NORMAL".
  //      reason: One of "UNSPECIFIED", "INITIALIZING", "RELOCALIZING",
  //                     "TOO_MUCH_MOTION", "NOT_ENOUGH_TEXTURE".
  //    }
  const onProcessCpu: Callbacks.ProcessCpuHandler = ({
    framework,
    frameStartResult,
    processGpuResult,
  }: {
    framework: FrameworkHandle
    frameStartResult: FrameStartResult
    processGpuResult: any
  }) => {
    // TODO(christoph): webxr: is there a better thing to check here?
    const {repeatFrame} = frameStartResult

    if (cameraProcessingDisabled_) {
      fakeRealityTexture_ = fakeRealityTexture_ || initFakeRealityTexture(drawCtx)
      // TODO(christoph): Do we need to include other fake position/rotation/etc, now that we're
      // returning an object here?
      return {realityTexture: fakeRealityTexture_}
    }

    if (runConfig.paused) {
      return null
    }

    maybeDispatchConfigChange(framework)

    if (isDataRecording_) {
      const {yuvpixelsarray} = processGpuResult
      if (!yuvpixelsarray || !yuvpixelsarray.pixels) return null
      const {rows, cols, rowBytes, pixels} = yuvpixelsarray
      if (pixels && pixels.length > 0) {
        if (!yuvBufferInitialized_) {
          initializeYuvBuffer(rows * rowBytes)
        }
        if (!repeatFrame) {
          // Copy the YUV data into the yuvBufferArray_.
          // Need to get new view of array buffer since the buffer becomes detached when wasm
          // linear memory grows, but the buffer pointer still stays valid
          yuvBufferArray_ = new Uint8Array(scc_.HEAPU8.buffer, yuvBuffer_, rows * rowBytes)
          yuvBufferArray_.set(pixels)
          scc_._c8EmAsm_prepareYuvBuffer(rows, cols, rowBytes, yuvBuffer_)
        }
      }
    }

    if (!repeatFrame) {
      scc_._c8EmAsm_processStagedFrame()
    }

    if (!window._c8.realityTexture || !window._c8.reality) {
      return null
    }

    const byteBuffer =
      scc_.HEAPU8.subarray(window._c8.reality, window._c8.reality + window._c8.realitySize)

    const message = new capnp.Message(byteBuffer, false).getRoot(RealityResponse)
    const camera = message.getXRResponse().getCamera()
    intrinsics_ = camera.getIntrinsic().getMatrix44f().toArray()

    const extrinsic = camera.getExtrinsic()
    const xrq = extrinsic.getRotation()
    const xrp = extrinsic.getPosition()
    const rotation = {x: xrq.getX(), y: xrq.getY(), z: xrq.getZ(), w: xrq.getW()}
    const position = {x: xrp.getX(), y: xrp.getY(), z: xrp.getZ()}

    let lighting
    if (enableLighting_) {
      const lightingMessage = message.getXRResponse().getLighting().getGlobal()
      lighting = {
        exposure: lightingMessage.getExposure(),
        temperature: lightingMessage.getTemperature(),
      }
    }

    const cameraTrackingStateReason = camera.getTrackingState().getReason()
    const trackingReason = XrTrackingState.XrTrackingStatusReason[cameraTrackingStateReason]

    let worldPoints: Array<{ id: number; confidence: number; position: {
      x: number; y: number; z: number } }> = []
    // TODO(nb): rNext -- this goes to the returns docs.
    //   surfaces: [{id, width, height, type: {UNSPECIFIED|HORIZONTAL_PLANE|VERTICAL_PLANE},
    //     vertices: [[x,y,z]], position: [x, y, z]}]
    //     An array of surfaces in the world at their location in the scene.  Only filled if
    //     XrController is configured to return surfaces and trackingReason != INITIALIZING.
    //  let surfaces = []
    if (trackingReason !== 'INITIALIZING') {
      const points = message.getFeatureSet().getPoints()
      worldPoints = Array.from({length: points.getLength()}, (_, i) => {
        const pt = points.get(i)  // { id, confidence, position }
        const pos = pt.getPosition()
        return {
          id: pt.getId().toNumber(),
          confidence: pt.getConfidence(),
          position: {
            x: pos.getX(),
            y: pos.getY(),
            z: pos.getZ(),
          },
        }
      })

      /*
      // TODO(nb): rNext
      const surfaceSet = message.getXRResponse().getSurfaces().getSet();
      surfaces = Array.from({length: surfaceSet.getSurfaces().getLength()}, (_, i) => {
        const surface = surfaceSet.getSurfaces().get(i)
        const id = surface.getId().getEventTimeMicros().toNumber()
        const nVertices = surface.getVerticesEndIndex() - surface.getVerticesBeginIndex()
        const vertices = Array.from({length: nVertices}, (_, i) => {
          const vv = surfaceSet.getVertices().get(surface.getVerticesBeginIndex() + i)
          return {x: vv.getX(), y: vv.getY(), z: vv.getZ()}
        })

        const getMidAndWidth = i => {
          if (getMidAndWidth[i]) {
            return getMidAndWidth[i]
          }
          const v = vertices.map(v => v[i])
          const max = Math.max(...v)
          const min = Math.min(...v)
          const rv = { wid: max - min, mid: (max + min) / 2.0 }
          getMidAndWidth[i] = rv
          return rv
        }
        const wid = i => getMidAndWidth(i).wid
        const position = {
          x: getMidAndWidth(0).mid,
          y: getMidAndWidth(1).mid,
          z: getMidAndWidth(2).mid
        }

        const type = Surface.SurfaceType[surface.getSurfaceType()]
        const width = type === 'HORIZONTAL_PLANE' ? wid(0) : Math.max(wid(0), wid(2))
        const height = type === 'HORIZONTAL_PLANE' ? wid(2) : wid(1)

        return { id, type, vertices, position, width, height }
      })
      */
    }  // trackingReason !== 'INITIALIZING'

    // Get detected images from c8 engine
    const detectedImages: DetectedImageTarget[] = message.getXRResponse().getDetection()
      .getImages().reduce((o: DetectedImageTarget[], msg: DetectedImage) => {
        const storedTargetMetadata = targetMetadata_.find(it => it.name === msg.getName())
        // A target that was just unloaded may still fire tracking events until it is fully
        // unloaded.
        if (storedTargetMetadata != null) {
          const image: DetectedImageTarget = {
            name: storedTargetMetadata.name,
            metadata: storedTargetMetadata.metadata,
            position: {
              x: msg.getPlace().getPosition().getX(),
              y: msg.getPlace().getPosition().getY(),
              z: msg.getPlace().getPosition().getZ(),
            },
            rotation: {
              x: msg.getPlace().getRotation().getX(),
              y: msg.getPlace().getRotation().getY(),
              z: msg.getPlace().getRotation().getZ(),
              w: msg.getPlace().getRotation().getW(),
            },
            scale: Math.max(msg.getWidthInMeters(), msg.getHeightInMeters()) || 1.0,
            type: getImageTargetApiTypeFromMsg(msg),
            properties: {
              width: storedTargetMetadata.properties.width,
              height: storedTargetMetadata.properties.height,
              top: storedTargetMetadata.properties.top,
              left: storedTargetMetadata.properties.left,
              originalWidth: storedTargetMetadata.properties.originalWidth,
              originalHeight: storedTargetMetadata.properties.originalHeight,
              isRotated: storedTargetMetadata.properties.isRotated,
              staticOrientation: storedTargetMetadata.properties.staticOrientation,
            },
          }

          Object.assign(image, storedTargetMetadata.geometry)
          o.push(image)
        }
        return o
      }, [])

    const seenNames = new Set()
    detectedImages.forEach((img) => {
      seenNames.add(img.name)

      const found = detectedImageMap_[img.name]
      if (!found) {
        detectedImageMap_[img.name] = img
        framework.dispatchEvent('imagefound', img)
        return
      }

      if (JSON.stringify(found) !== JSON.stringify(img)) {
        detectedImageMap_[img.name] = img
        framework.dispatchEvent('imageupdated', img)
      }
    })

    Object.keys(detectedImageMap_).forEach((k) => {
      if (!seenNames.has(k)) {
        const found = {...detectedImageMap_[k]}
        framework.dispatchEvent('imagelost', found)
        delete detectedImageMap_[k]
      }
    })

    const trackingStatus = XrTrackingState.XrTrackingStatus[camera.getTrackingState().getStatus()]

    // eslint-disable-next-line no-constant-condition
    if (trackingStatus !== trackingStatus_ || trackingReason !== trackingReason_) {
      // We have a new tracking status/reason which we will emit as a framework event.
      trackingStatus_ = trackingStatus
      trackingReason_ = trackingReason
      framework.dispatchEvent('trackingstatus', {status: trackingStatus, reason: trackingReason})
    }
    if (isDataRecording_ && !repeatFrame) {
      const recordBuffer = scc_.HEAPU8.subarray(window._c8.logRecord,
        window._c8.logRecord + window._c8.logRecordSize)
      const messageArrayBuf = new capnp.Message(recordBuffer, false).toArrayBuffer()
      collectedLogChunks_.push({message: messageArrayBuf})
    }
    return {
      rotation,
      position,
      intrinsics: intrinsics_,
      realityTexture: XR8.drawTexForComputeTex(window._c8.realityTexture),
      trackingStatus,
      trackingReason,
      // TODO(nb): rNext - trackingScale: camera.getTrackingState().getScale(),
      worldPoints,
      // TODO(nb): rNext - surfaces,
      lighting,
      detectedImages,

      // Deprecated in 9.1
      points: {
        getLength: () => 0,
        get: (i) => { throw new Error(`get(${i}) exceeds length 0`) },
      },
      meanRGB: 0,
    }
  }

  const readImageDetectionTarget = (
    computeCtx: RenderContext,
    img: HTMLImageElement,
    imgId: number,
    imageTarget: ImageTargetData,
    framework: FrameworkHandle,
    batchLoadId: number
  ) => {
    if (batchLoadId !== imageBatchLoadId_) {
      // If a new batch of loading has started, end the current load and don't load this imgId again
      const index = imagesToLoad_.indexOf(imgId)
      if (index > -1) {
        imagesToLoad_.splice(index, 1)
      }
      return
    }

    if (imagesToLoad_[0] !== imgId) {
      // Wait until imgId is first in queue to load
      window.requestAnimationFrame(
        () => readImageDetectionTarget(computeCtx, img, imgId, imageTarget, framework, batchLoadId)
      )
      return
    }
    const iw = img.naturalWidth
    const ih = img.naturalHeight
    const message = new capnp.Message()
    const imageTargetMetadata = message.initRoot(ImageTargetMetadata)
    imageTargetMetadata.setName(imageTarget.name)
    imageTargetMetadata.setImageWidth(iw)
    imageTargetMetadata.setImageHeight(ih)

    const {properties} = imageTarget

    imageTargetMetadata.setOriginalImageWidth(properties.originalWidth)
    imageTargetMetadata.setOriginalImageHeight(properties.originalHeight)
    imageTargetMetadata.setCropOriginalImageX(properties.left)
    imageTargetMetadata.setCropOriginalImageY(properties.top)
    imageTargetMetadata.setCropOriginalImageWidth(properties.width)
    imageTargetMetadata.setCropOriginalImageHeight(properties.height)
    imageTargetMetadata.setIsRotated(properties.isRotated === true ||
      // @ts-expect-error
      properties.isRotated === 'true')

    if (properties.staticOrientation) {
      imageTargetMetadata.setIsStaticTarget(true)
      imageTargetMetadata.setRollAngle(properties.staticOrientation.rollAngle)
      imageTargetMetadata.setPitchAngle(properties.staticOrientation.pitchAngle)
    } else {
      imageTargetMetadata.setIsStaticTarget(false)
    }

    if (imageTarget.type === ImageTargetType.CYLINDER.xrhomeName ||
      imageTarget.type === ImageTargetType.CONICAL.xrhomeName) {
      imageTargetMetadata.setType(ImageTargetTypeMsg.CURVY)
      const curvyGeometry = imageTargetMetadata.getCurvyGeometry()
      try {
        curvyGeometry.setCurvyCircumferenceTop(properties.cylinderCircumferenceTop)
        curvyGeometry.setCurvyCircumferenceBottom(properties.cylinderCircumferenceBottom)
        curvyGeometry.setCurvySideLength(properties.cylinderSideLength)
        curvyGeometry.setTargetCircumferenceTop(properties.targetCircumferenceTop)
      } catch (e) {
        // eslint-disable-next-line no-console
        console.error(`[XR] Error when parsing xrMetadata as JSON: ${e.message}`)
        imagesToLoad_.shift()
        if (framework && imagesToLoad_.length === 0) {
          framework.dispatchEvent('imagescanning', {imageTargets: targetMetadata_})
        }
        return
      }
    } else if (imageTarget.type === ImageTargetType.PLANAR.xrhomeName) {
      imageTargetMetadata.setType(ImageTargetTypeMsg.PLANAR)
    } else {
      imageTargetMetadata.setType(ImageTargetTypeMsg.UNSPECIFIED)
    }

    const buffer = message.toArrayBuffer()
    const ptr = scc_._malloc(buffer.byteLength)
    scc_.writeArrayToMemory(new Uint8Array(buffer), ptr)
    scc_._c8EmAsm_addNewDetectionImageLoader(ptr, buffer.byteLength)
    scc_._free(ptr)

    computeCtx.bindTexture(computeCtx.TEXTURE_2D, window.omni8.imageTargetTexture)
    computeCtx.texImage2D(
      computeCtx.TEXTURE_2D,
      0,
      computeCtx.RGBA,
      computeCtx.RGBA,
      computeCtx.UNSIGNED_BYTE,
      img
    )

    window.requestAnimationFrame(() => {
      if (batchLoadId !== imageBatchLoadId_) {
        // If a new batch of loading has started, end the current load and don't load this
        // imgId again.
        imagesToLoad_.shift()

        // We have already added the loader, so we need to signal that we will not use it.
        scc_._c8EmAsm_cancelProcessNewDetectionImageTexture()
        return
      }

      scc_._c8EmAsm_processNewDetectionImageTexture()
      currentTargetUrls_.push(imageTarget.imagePath)
      imagesToLoad_.shift()

      const itMetadata = targetMetadata_.find(o => o.name === imageTarget.name)
      if (imageTarget.type === ImageTargetType.CYLINDER.xrhomeName ||
        imageTarget.type === ImageTargetType.CONICAL.xrhomeName) {
        const byteBuffer = scc_.HEAPU8.subarray(
          window._c8.lastProcessedCurvyGeometry,
          window._c8.lastProcessedCurvyGeometry + window._c8.lastProcessedCurvyGeometrySize
        )
        const curvyGeometryMessage = new capnp.Message(byteBuffer, false).getRoot(CurvyGeometry)

        itMetadata.geometry = {
          height: curvyGeometryMessage.getHeight(),
          radiusTop: curvyGeometryMessage.getTopRadius(),
          radiusBottom: curvyGeometryMessage.getBottomRadius(),
          arcLengthRadians: curvyGeometryMessage.getArcLengthRadians(),
          arcStartRadians: curvyGeometryMessage.getArcStartRadians(),
        }
      } else {
        itMetadata.geometry = {
          scaledWidth: properties.isRotated ? 1.0 : (properties.width / properties.height),
          scaledHeight: properties.isRotated ? (properties.width / properties.height) : 1.0,
        }
      }

      if (framework && imagesToLoad_.length === 0) {
        framework.dispatchEvent('imagescanning', {imageTargets: targetMetadata_})
      }
    })
  }

  let loadedComputeCtx = null

  const registerComputeCtx = (computeCtx: RenderContext) => {
    if (computeHandle_ !== 0 || !computeCtx) {
      return
    }
    if (scc_) {
      computeHandle_ = scc_.GL.registerContext(computeCtx, computeCtx.getContextAttributes())
      scc_.GL.makeContextCurrent(computeHandle_)
    }
    if (scc_ && xrcc_) {
      // Share the previously allocated textures with the global xrcc GL compute context
      scc_.GL.textures = xrcc_.GL.textures
    }
  }

  const onStart: Callbacks.StartHandler = ({computeCtx, canvasWidth, canvasHeight}) => {
    maybeDispatchConfigChange(framework_)

    registerComputeCtx(computeCtx)

    if (!cam_.pixelRectWidth) {
      cam_.pixelRectWidth = canvasWidth
    }
    if (!cam_.pixelRectHeight) {
      cam_.pixelRectHeight = canvasHeight
    }
  }

  const onSessionAttach: Callbacks.AttachHandler = ({
    videoWidth,
    videoHeight,
    canvasWidth,
    canvasHeight,
    rotation,
    computeCtx,
    framework,
    GLctx,
    sessionAttributes,
  }) => {
    registerComputeCtx(computeCtx)

    if (toCheckAllZeros_ === null) {
      toCheckAllZeros_ = window._c8.checkAllZeroes
      if (scc_) {
        scc_._c8EmAsm_setCheckAllZeroes(window._c8.checkAllZeroes)
      }
    }

    cameraProcessingDisabled_ = !sessionAttributes.fillsCameraTexture
    framework_ = framework
    notifyOthersOfConfigChange_ = true
    maybeDispatchConfigChange(framework_)
    if (!cam_.pixelRectWidth) {
      cam_.pixelRectWidth = canvasWidth
    }
    if (!cam_.pixelRectHeight) {
      cam_.pixelRectHeight = canvasHeight
    }

    // init device info
    const deviceEst = XrDeviceFactory().deviceEstimate()
    const message = new Message()
    const deviceEnv = message.initRoot(DeviceInfo)
    deviceEnv.setManufacturer(deviceEst.manufacturer)
    deviceEnv.setModel(deviceEst.model)
    deviceEnv.setOs(deviceEst.os)
    deviceEnv.setOsVersion(deviceEst.osVersion)
    const buffer = message.toArrayBuffer()
    const ptr = scc_._malloc(buffer.byteLength)
    scc_.writeArrayToMemory(new Uint8Array(buffer), ptr)
    scc_._c8EmAsm_initDeviceInfo(ptr, buffer.byteLength)
    scc_._free(ptr)

    // TODO(paris): Re-enable pixel buffer on iOS once iOS 15 + pixel buffer bug is resolved.
    scc_._c8EmAsm_setDisablePixelBuffer(XrDeviceFactory().deviceEstimate().os === 'iOS')
    scc_._c8EmAsm_engineInit(videoWidth, videoHeight, rotation)

    // Kept to be used when app resources is available
    loadedComputeCtx = computeCtx

    drawCtx = GLctx

    GlTextureRenderer.configure({mirroredDisplay: !!mirroredDisplay_})
    GlTextureRenderer.setTextureProvider(
      ({processCpuResult: {reality}}) => (reality ? reality.realityTexture : null)
    )

    isSimulator_ = sessionAttributes.isSimulator === true
    if (isSimulator_) {
      removeMotionOrientationListeners()
    }
  }

  const onSessionDetach: Callbacks.DetachHandler = () => {
    cameraProcessingDisabled_ = false

    if (scc_) {
      scc_._c8EmAsm_engineCleanup()
      scc_.GL.textures = null
      computeHandle_ = 0
      scc_.GL.makeContextCurrent(computeHandle_)
    }

    addMotionOrientationListeners()

    authTokenSubscription.stop()

    lastDevicePose_.alpha = 0
    lastDevicePose_.beta = 0
    lastDevicePose_.gamma = 0

    lastGeolocation_.latitude = 0
    lastGeolocation_.longitude = 0
    lastGeolocation_.accuracy = 0

    if (fakeRealityTexture_) {
      drawCtx.deleteTexture(fakeRealityTexture_)
      fakeRealityTexture_ = null
    }
    drawCtx = null
    scale_ = 1.0
    origin_.x = 0
    origin_.y = 2
    origin_.z = 0
    facing_.w = 1
    facing_.x = 0
    facing_.y = 0
    facing_.z = 0
    cam_.pixelRectWidth = 0
    cam_.pixelRectHeight = 0
    cam_.nearClipPlane = 0.01
    cam_.farClipPlane = 1000.0
    yuvBufferInitialized_ = false
    scc_._free(yuvBuffer_)
    yuvBuffer_ = null
    yuvBufferArray_ = null
    mapSrcUrl_ = ''
    trackingStatus_ = 'UNSPECIFIED'
    trackingReason_ = 'UNSPECIFIED'
    Object.keys(detectedImageMap_).forEach(k => delete detectedImageMap_[k])

    imgId_ = 0
    imageBatchLoadId_++  // Increment imageBatchLoadId_ to halt loading of current batch.
    Object.keys(imageTargetCache_).forEach(k => delete imageTargetCache_[k])
    appResourcesLoaded_ = false
    framework_ = null
    GlTextureRenderer.setTextureProvider(null)
    GlTextureRenderer.configure({mirroredDisplay: false})

    // We have set the EngineData singleton inside slamcontroller.cc to nullptr.
    // Therefore each _c8 value that references the singleton should be set to null
    window._c8.isAllZeroes = null
    window._c8.wasEverNonZero = null
    window._c8.queryResponse = null
    window._c8.queryResponseSize = null
    window._c8.reality = null
    window._c8.realitySize = null
    window._c8.realityTexture = null
    window._c8.lastProcessedCurvyGeometry = null
    window._c8.lastProcessedCurvyGeometrySize = null
    window._c8.logRecord = null
    window._c8.logRecordSize = null
  }

  const onDetach: Callbacks.DetachHandler = () => {
    enableLighting_ = false
    disableWorldTracking_ = false
    leftHandedAxes_ = false
    mirroredDisplay_ = false

    imagesToLoad_.length = 0
    targetMetadata_.length = 0
    currentTargetUrls_.length = 0
  }

  const unloadImageTargets = (targetNamesToUnload: string[]) => {
    if (!targetNamesToUnload || !targetNamesToUnload.length) {
      return
    }

    // Unload targets from engine.
    const message = new capnp.Message()
    const imageTargetNames = message.initRoot(ImageTargetNames)
    imageTargetNames.initNames(targetNamesToUnload.length)
    targetNamesToUnload.forEach((n, i) => {
      imageTargetNames.getNames().set(i, n)
    })

    const buffer = message.toArrayBuffer()
    const ptr = scc_._malloc(buffer.byteLength)
    scc_.writeArrayToMemory(new Uint8Array(buffer), ptr)
    scc_._c8EmAsm_unloadDetectionImages(ptr, buffer.byteLength)
    scc_._free(ptr)
  }

  const setActiveImageTargets = (
    imageTargets: ImageTargetData[],
    framework: FrameworkHandle
  ) => {
    // Existing loads' imageBatchLoadId_ will now differ, so they will stop.
    const batchLoadId = ++imageBatchLoadId_

    // We will load targets that are in imageTargets but not in currentTargetUrls.
    const targetsToLoad = imageTargets.filter(
      x => !currentTargetUrls_.some(obj => obj === x.imagePath)
    )

    // We unload targets that are in currentTargetUrls_ but not in imageTargets.
    const targetUrlsToUnload =
      currentTargetUrls_.filter(obj => !imageTargets.some(obj2 => obj === obj2.imagePath))
    unloadImageTargets(targetUrlsToUnload)

    // Existing image targets that have been loaded into the engine.
    const existingTargets = imageTargets.filter(
      obj => !targetsToLoad.some(obj2 => obj.imagePath === obj2.imagePath)
    )

    const existingTargetNames = existingTargets.map(a => a.name)
    const existingTargetUrls = existingTargets.map(a => a.imagePath)
    // Only existing targets for now, we'll append new targets once they are loaded in the engine.
    // Do so in case these newly added targets never are added into the engine because
    // setActiveImageTargets() is called again.
    currentTargetUrls_ = existingTargetUrls

    if (!targetsToLoad.length) {
      return
    }

    // Only keep existing targets that have been loaded into the engine, plus new targets
    targetMetadata_ = targetMetadata_.filter(it => existingTargetNames.includes(it.name))
    targetMetadata_.push(...targetsToLoad.map(extractTargetMetadata))
    framework.dispatchEvent('imageloading', {imageTargets: targetMetadata_})

    const fragment = document.createDocumentFragment()
    targetsToLoad.forEach((it) => {
      const id = imgId_++
      imagesToLoad_.push(id)
      loadRemoteImageUrl(it.imagePath, fragment)
        .then((img) => {
          readImageDetectionTarget(loadedComputeCtx, img, id, it, framework, batchLoadId)
        })
    })
  }

  const addTargetToCache = (target: ImageTargetData) => {
    imageTargetCache_[target.name] = target
  }

  const onAppResourcesLoaded: Callbacks.AppResourcesLoadedHandler = ({imageTargets, framework}) => {
    if (appResourcesLoaded_) {
      return
    }
    maybeDispatchConfigChange(framework)
    appResourcesLoaded_ = true
    framework_ = framework

    if (imageTargets) {
      imageTargets.forEach(addTargetToCache)
    }

    if (configuredImageTargetData_) {
      setActiveImageTargets(configuredImageTargetData_, framework)
    }
  }

  type XrConfigurationArgs = {
    cam?: {
      pixelRectWidth: number
      pixelRectHeight: number
      nearClipPlane: number
      farClipPlane: number
    }
    origin?: { x: number, y: number, z: number }
    facing?: { w: number, x: number, y: number, z: number }
    scale?: number
    mapSrcUrl?: string
  }

  const configureXr = ({cam, origin, facing, scale, mapSrcUrl}: XrConfigurationArgs) => {
    const _configureXr = (msg: capnp.Message) => {
      const configBuffer = msg.toArrayBuffer()
      const configPtr = scc_._malloc(configBuffer.byteLength)
      scc_.writeArrayToMemory(new Uint8Array(configBuffer), configPtr)
      scc_._c8EmAsm_configureXr(configPtr, configBuffer.byteLength)
      scc_._free(configPtr)
      notifyOthersOfConfigChange_ = true
    }

    // Set compute mask and camera config.
    if (cam) {
      const message = new capnp.Message()
      const config = message.initRoot(XRConfiguration)

      const configMask = config.getMask()
      configMask.setLighting(enableLighting_)
      configMask.setCamera(enableCamera)
      configMask.setSurfaces(enableSurfaces)
      configMask.setVerticalSurfaces(enableVerticalSurfaces)
      configMask.setFeatureSet(false)
      configMask.setEstimateScale(false)
      config.getCameraConfiguration().setAutofocus(enableCameraAutofocus)

      const graphicsIntrinsicsConfig = config.getGraphicsIntrinsics()
      graphicsIntrinsicsConfig.setTextureWidth(cam.pixelRectWidth)
      graphicsIntrinsicsConfig.setTextureHeight(cam.pixelRectHeight)
      graphicsIntrinsicsConfig.setNearClip(cam.nearClipPlane)
      graphicsIntrinsicsConfig.setFarClip(cam.farClipPlane)
      graphicsIntrinsicsConfig.setDigitalZoomHorizontal(1.0)
      graphicsIntrinsicsConfig.setDigitalZoomVertical(1.0)

      _configureXr(message)
    }

    // Set coordinate system.
    if (facing && origin && scale) {
      const message = new capnp.Message()
      const config = message.initRoot(XRConfiguration)

      const coords = config.getCoordinateConfiguration()

      coords.setAxes(leftHandedAxes_
        ? CoordinateSystemConfiguration.CoordinateAxes.X_RIGHT_YUP_ZFORWARD
        : CoordinateSystemConfiguration.CoordinateAxes.X_LEFT_YUP_ZFORWARD)

      coords.setMirroredDisplay(mirroredDisplay_)
      GlTextureRenderer.configure({mirroredDisplay: !!mirroredDisplay_})
      const r = coords.getOrigin().getRotation()
      r.setW(facing.w)
      r.setX(facing.x)
      r.setY(facing.y)
      r.setZ(facing.z)
      const p = coords.getOrigin().getPosition()
      p.setX(origin.x)
      p.setY(origin.y)
      p.setZ(origin.z)
      coords.setScale(scale)

      _configureXr(message)
    }

    if (mapSrcUrl) {
      const message = new capnp.Message()
      const config = message.initRoot(XRConfiguration)
      config.setMapSrcUrl(mapSrcUrl)
      _configureXr(message)
    }
  }

  // Configures what processing is performed by XrController (may have performance implications).
  //
  // Inputs:
  // {
  //   enableLighting:       [Optional] If true, return an estimate of lighting information.
  //   imageTargets:         [Deprecated] List of names of the image target to detect. If an empty
  //                         array is passed, no image targets will be detected.
  //   imageTargetData:      [Optional] List of image target data to detect as an option to use
  //                         instead of gettign images through the API.
  //   leftHandedAxes:       [Optional] If true, use left-handed coordinates.
  //   mirroredDisplay:      [Optional] If true, flip left and right in the output.
  //   enableJpgEncoding:    [Optional] If true, encode datarecorder image frames as JPG, otherwise
  //                         leave it as raw data.
  //   enableDataRecorder:   [Optional] If true, use all default values for coordinate config.
  const configure = (args: TrackingControllerConfig) => {
    if (!args) {
      return
    }
    if (args.enableLighting !== undefined) {
      enableLighting_ = !!args.enableLighting
    }
    if (args.leftHandedAxes !== undefined) {
      leftHandedAxes_ = !!args.leftHandedAxes
    }
    if (args.mirroredDisplay !== undefined) {
      notifyOthersOfConfigChange_ =
        notifyOthersOfConfigChange_ || (mirroredDisplay_ !== !!args.mirroredDisplay)
      mirroredDisplay_ = !!args.mirroredDisplay
    }

    if (args.imageTargetData !== undefined) {
      if (!Array.isArray(args.imageTargetData)) {
        throw new Error('[XR] Expected array for arg: imageTargetData')
      }

      if (cameraProcessingDisabled_) {
        // eslint-disable-next-line no-console
        console.warn('[XR] Image Targets are not supported in the current session.')
      } else {
        configuredImageTargetData_ = args.imageTargetData

        if (appResourcesLoaded_) {
          setActiveImageTargets(configuredImageTargetData_, framework_)
        }
      }
    } else if (args.imageTargets !== undefined) {
      // eslint-disable-next-line no-console
      console.warn('[XR] imageTargets is deprecated, please use imageTargetData instead.')
    }

    if (args.mapSrcUrl !== undefined) {
      mapSrcUrl_ = args.mapSrcUrl
    }

    if (args.enableJpgEncoding !== undefined) {
      scc_._c8EmAsm_setCompressedFormat(
        args.enableJpgEncoding
          ? CompressionFormat.JPG_RGBA
          : CompressionFormat.UNSPECIFIED
      )
    }

    if (args.enableDataRecorder !== undefined) {
      enableDataRecorder_ = !!args.enableDataRecorder
      scc_._c8EmAsm_setEnableDataRecorder(!!args.enableDataRecorder)
    }

    configureXr({
      cam: cam_,
      origin: origin_,
      facing: facing_,
      scale: scale_,
      mapSrcUrl: mapSrcUrl_,
    })
  }

  // Reset the scene's display geometry and the camera's starting position in the scene. The display
  // geometry is needed to properly overlay the position of objects in the virtual scene on top of
  // their corresponding position in the camera image. The starting position specifies where the
  // camera will be placed and facing at the start of a session.
  //
  // Inputs:
  // {
  //   cam [Optional] {
  //     pixelRectWidth: The width of the canvas that displays the camera feed.
  //     pixelRectHeight: The height of the canvas that displays the camera feed.
  //     nearClipPlane: The closest distance to the camera at which scene objects are visible.
  //     farClipPlane: The farthest distance to the camera at which scene objects are visible.
  //   }
  //   origin {x, y, z}: [Optional] The starting position of the camera in the scene.
  //   facing {w, x, y, z}: [Optional] The starting direction (quaternion) of the camera in the
  //     scene.
  //   updateRecenterPoint boolean: [Optional] Chanage the point we will recenter to
  // }
  const updateCameraProjectionMatrix = ({
    cam,
    origin,
    facing,
    updateRecenterPoint = true,
  }: CameraProjectionMatrix) => {
    notifyOthersOfConfigChange_ = true
    if (cam) {
      Object.assign(cam_, cam)
    }
    if (origin) {
      origin_ = {x: origin.x, y: origin.y, z: origin.z}
      if (updateRecenterPoint) {
        recenterOrigin_ = {...origin_}
      }
    }
    if (facing) {
      facing_ = {w: facing.w, x: facing.x, y: facing.y, z: facing.z}
      if (updateRecenterPoint) {
        recenterFacing_ = {...facing_}
      }
    }

    // Set the scale to the height of the camera origin if we are in 'responsive' mode.
    scale_ = origin_.y

    // Only configure cam if it was provided as an arg.
    configureXr({cam: cam_, origin: origin_, facing: facing_, scale: scale_, mapSrcUrl: mapSrcUrl_})
  }

  const onDeviceOrientationChange: Callbacks.DeviceOrientationChangeHandler = ({
    videoWidth, videoHeight, orientation,
  }) => {
    maybeDispatchConfigChange(framework_)
    scc_._c8EmAsm_engineOrientationChange(videoWidth, videoHeight, orientation)
    configureXr({cam: cam_, origin: origin_, facing: facing_, scale: scale_, mapSrcUrl: mapSrcUrl_})
  }

  const onCanvasSizeChange: Callbacks.CanvasSizeChangeHandler = ({canvasWidth, canvasHeight}) => {
    maybeDispatchConfigChange(framework_)
    cam_.pixelRectWidth = canvasWidth
    cam_.pixelRectHeight = canvasHeight
    configureXr({cam: cam_})
  }

  const onVideoSizeChange: Callbacks.VideoSizeChangeHandler = ({
    videoWidth, videoHeight, orientation,
  }) => {
    maybeDispatchConfigChange(framework_)
    scc_._c8EmAsm_engineVideoSizeChange(videoWidth, videoHeight, orientation)
  }

  const onCameraStatusChange: Callbacks.CameraStatusChangeHandler = ({status, cameraConfig}) => {
    if (status !== 'requesting') {
      return
    }
    maybeDispatchConfigChange(framework_)
    const {direction} = cameraConfig
    if (direction !== XrConfigFactory().camera().BACK && !disableWorldTracking_) {
      throw Object.assign(new Error('[XR] World tracking is only supported on the back camera'), {
        type: 'configuration',
        source: 'reality',
        err: 'slam-front-camera-unsupported',
      })
    }
  }

  const requiredPermissions = () => {
    const permissions = XrPermissionsFactory().permissions()
    const pl: XrPermission[] = [permissions.CAMERA]
    if (!disableWorldTracking_ && !cameraProcessingDisabled_) {
      pl.push(permissions.DEVICE_ORIENTATION)
      pl.push(permissions.DEVICE_MOTION)
    }
    return pl
  }

  const onBeforeSessionInitialize: Callbacks.BeforeSessionInitializeHandler = ({
    sessionAttributes,
  }) => {
    const hasImageTargets = configuredImageTargetData_ && configuredImageTargetData_.length
    cameraProcessingDisabled_ = !sessionAttributes.fillsCameraTexture

    if (!sessionAttributes.fillsCameraTexture && hasImageTargets) {
      throw new Error('[XR] Image Targets require a session that can provide a camera texture')
    }
    if (sessionAttributes.controlsCamera && disableWorldTracking_) {
      throw new Error('[XR] Disabled world tracking requires a session without camera control')
    }
    // SLAM is only compatible with GetUserMediaSessionManager on mobile devices. To test this, we
    // simulate a user config to restrict to mobile and see if the test passes. This means that the
    // user is on a mobile device.
    if (!disableWorldTracking_) {
      const mobileOnlyRunConfig = {...runConfig, allowedDevices: XrConfigFactory().device().MOBILE}
      if (sessionAttributes.fillsCameraTexture &&
        !XrDeviceFactory().isDeviceBrowserCompatible(mobileOnlyRunConfig)) {
        throw new Error(
          '[XR] Reality with camera on non-mobile devices requires disableWorldTracking'
        )
      }
    }
  }

  // Creates a camera pipeline module that, when installed, receives callbacks on when
  // the camera has started, camera proessing events, and other state changes. These are used
  // to calculate the camera's position.
  //
  // Dispatched events:
  //
  // cameraconfigured: Fires when certain camera config has changed
  //    detail: {
  //      mirroredDisplay: whether we are operating with a mirrored display
  //    }
  const pipelineModule = () => ({
    name: 'reality',
    onAppResourcesLoaded,
    onBeforeSessionInitialize,
    onStart,
    onSessionAttach,
    onSessionDetach,
    onDetach,
    onProcessGpu,
    onProcessCpu,
    onDeviceOrientationChange,
    onCanvasSizeChange,
    onVideoSizeChange,
    onCameraStatusChange,
    requiredPermissions,
  })

  // Repositions the camera to the origin / facing direction specified by
  // updateCameraProjectionMatrix and restart tracking.
  const recenter = () => {
    // TODO (Tri) fix this recenter origin not applying to XR session => 3D session
    origin_ = {...recenterOrigin_}
    facing_ = {...recenterFacing_}
    if (scc_) {
      scc_._c8EmAsm_recenter()
    }
    maybeDispatchConfigChange(framework_, true)
  }

  const hitTestTypeEnum = {
    'UNSPECIFIED': 'UNSPECIFIED',
    'FEATURE_POINT': 'FEATURE_POINT',
    'ESTIMATED_SURFACE': 'ESTIMATED_SURFACE',
    'DETECTED_SURFACE': 'DETECTED_SURFACE',
  } as const

  // Estimate the 3D position of a point on the camera feed. X and Y are specified
  // as numbers between 0 and 1, where (0, 0) is the upper left corner and (1, 1) is the lower right
  // corner of the camera feed as rendered in the camera that was specified by
  // updateCameraProjectionMatrix.  Multiple 3d position estimates may be returned for a single
  // hit test based on the source of data being used to estimate the position. The data source
  // that was used to estimate the position is indicated by the hitTest.type.
  type HitTestType = Array<keyof typeof hitTestTypeEnum>
  const hitTest = (x: number, y: number, includedTypes: HitTestType = []) => {
    if (!scc_) {
      return []
    }
    const message = new capnp.Message()
    const ht = message.initRoot(XrQueryRequest).getHitTest()
    ht.setX(x)
    ht.setY(y)
    if (includedTypes && includedTypes.length) {
      ht.initIncludedTypes(includedTypes.length)
      includedTypes.forEach((t, i) => {
        switch (t) {
          case hitTestTypeEnum.FEATURE_POINT:
            ht.getIncludedTypes().set(i, XrHitTestResult.ResultType.FEATURE_POINT)
            break
          case hitTestTypeEnum.ESTIMATED_SURFACE:
            ht.getIncludedTypes().set(i, XrHitTestResult.ResultType.ESTIMATED_SURFACE)
            break
          case hitTestTypeEnum.DETECTED_SURFACE:
            ht.getIncludedTypes().set(i, XrHitTestResult.ResultType.DETECTED_SURFACE)
            break
          default:
            // pass
            break
        }
      })
    }

    const buffer = message.toArrayBuffer()
    const ptr = scc_._malloc(buffer.byteLength)
    scc_.writeArrayToMemory(new Uint8Array(buffer), ptr)
    scc_._c8EmAsm_query(ptr, buffer.byteLength)
    scc_._free(ptr)

    const {queryResponse: qr, queryResponseSize: qrSize} = window._c8
    const byteBuffer = scc_.HEAPU8.subarray(qr, qr + qrSize)

    const response = new capnp.Message(byteBuffer, false).getRoot(XrQueryResponse)
    return response.getHitTest().getHits().map(hit => ({
      type: (() => {
        switch (hit.getType()) {
          case XrHitTestResult.ResultType.FEATURE_POINT:
            return hitTest.type.FEATURE_POINT
          case XrHitTestResult.ResultType.ESTIMATED_SURFACE:
            return hitTest.type.ESTIMATED_SURFACE
          case XrHitTestResult.ResultType.DETECTED_SURFACE:
            return hitTest.type.DETECTED_SURFACE
          default:
            return hitTest.type.UNSPECIFIED
        }
      })(),

      position: {
        x: hit.getPlace().getPosition().getX(),
        y: hit.getPlace().getPosition().getY(),
        z: hit.getPlace().getPosition().getZ(),
      },

      rotation: {
        x: hit.getPlace().getRotation().getX(),
        y: hit.getPlace().getRotation().getY(),
        z: hit.getPlace().getRotation().getZ(),
        w: hit.getPlace().getRotation().getW(),
      },

      distance: hit.getDistance(),

    }))
  }
  hitTest.type = hitTestTypeEnum

  // TODO(nb): make device updates stop on pause.
  addWindowListener('message', forwardCustomMessage, false)
  addWindowListener('deviceorientation8w', ({detail}) => updateDeviceOrientation(detail))
  addWindowListener('devicemotion8w', ({detail}) => sensors.updateDeviceMotion(detail))
  addWindowListener('extrinsic8w', ({detail}) => updateDeviceExtrinsic(detail))
  addWindowListener('gps8w', ({detail}) => updateDeviceGeolocation({coords: detail}))
  addMotionOrientationListeners()

  const getIntrinsic = () => intrinsics_

  return xrTrackingPromise_.then(() => ({
    // Creates a camera pipeline module that, when installed, receives callbacks on when
    // the camera has started, camera proessing events, and other state changes. These are used
    // to calculate the camera's position.
    pipelineModule,

    // Configures what processing is performed by XrController (may have performance implications).
    configure,

    // Repositions the camera to the origin / facing direction specified by
    // updateCameraProjectionMatrix and restart tracking.
    recenter,

    // Estimate the 3D position of a point on the camera feed. X and Y are specified
    // as numbers between 0 and 1, where (0, 0) is the upper left corner and (1, 1) is the lower
    // right corner of the camera feed as rendered in the camera that was specified by
    // updateCameraProjectionMatrix.  Multiple 3d position estimates may be returned for a single
    // hit test based on the source of data being used to estimate the position. The data source
    // that was used to estimate the position is indicated by the hitTest.type.
    hitTest,

    // Reset the scene's display geometry and the camera's starting position in the scene. The
    // display geometry is needed to properly overlay the position of objects in the virtual scene
    // on top of their corresponding position in the camera image. The starting position specifies
    // where the camera will be placed and facing at the start of a session.
    updateCameraProjectionMatrix,

    // Get the camera intrinsic parameters.
    getIntrinsic,

    // Get state of the data recorder
    getIsDataRecording,

    // Start or stop the data recorder
    startDatarecorder,

    stopDatarecorder,

    // Deprecated at 9.1
    xrController: () => ({
      cameraPipelineModule: pipelineModule,
      configure,
      recenter,
      updateCameraProjectionMatrix,
      updateCameraOrigin: updateCameraProjectionMatrix,
    }),
  }))
})

export {
  XrControllerFactory,
}

export type {
  TrackingControllerConfig,
}
