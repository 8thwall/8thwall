import WEBMODELVIEW from '@nia/c8/model/web/web-model-view-wasm'

import {writeStringToEmscriptenHeap, writeArrayToEmscriptenHeap} from '@nia/c8/ems/ems'
// TODO: Import from @nia/c8/xrapi/xrapi-types
// import type {
//   DomHighResTimeStamp, XrFrame, XrFrameCallback, XrReferenceSpace, XrSession,
// } from '@nia/c8/xrapi/xrapi-types'

import {createMouseToTouchTranslator} from '@nia/c8/browser/mouse-to-touch-translater'

import {ModelManager} from './model-manager'
import type {ModelSrc} from './model-manager-types'

declare let XRWebGLLayer: any  // TODO: Use browser type

// TODO: Use imported types.
type DomHighResTimeStamp = number
type XrFrame = any
type XrFrameCallback = (time: DomHighResTimeStamp, frame: XrFrame) => void
type XrReferenceSpace = {}
type XrSession = any

type FrameRequestCallback = (time: DomHighResTimeStamp) => void

const {xr} = navigator as any
let session_: XrSession | null = null
let refSpace_: XrReferenceSpace | null = null

interface Ticker {
  stop: () => void
}

const startTick = (cb: XrFrameCallback | FrameRequestCallback, session?: XrSession): Ticker => {
  let stopped_ = false
  let lastAnimationFrame_: number | null = null
  const animate = () => {
    if (stopped_) {
      return
    }
    if (session) {
      lastAnimationFrame_ = session.requestAnimationFrame((time, frame) => {
        (cb as XrFrameCallback)(time, frame)
        animate()
      })
    } else {
      lastAnimationFrame_ = window.requestAnimationFrame((time) => {
        (cb as FrameRequestCallback)(time)
        animate()
      })
    }
  }
  const stop = () => {
    if (lastAnimationFrame_) {
      if (session) {
        session.cancelAnimationFrame(lastAnimationFrame_)
      } else {
        window.cancelAnimationFrame(lastAnimationFrame_)
      }
    }
    stopped_ = true
  }

  animate()

  return {
    stop,
  }
}

let ticker_: Ticker | null = null

interface ViewRange {
  min: number
  start: number
  max: number
}

interface OrbitParams {
  center: Number[]
  radius: ViewRange
  yaw: ViewRange
  pitch: ViewRange
}

interface ViewParams {
  orbit: OrbitParams
  space: number
  sortTexture: boolean
  multiTexture: boolean
}

interface Box3 {
  min: number[]
  max: number[]
}

enum CropShape {
  BOX = 0,
  CYLINDER = 1,
}

interface ModelEdits {
  crop: Box3
  rotationDegrees: number
  shape: CropShape
}

enum CropBoxSide {
  TOP = 0,
  LEFT = 1,
  RIGHT = 2,
  FRONT = 3,
}

const preventDefault = (e) => {
  e.preventDefault()
  e.stopPropagation()
}

const create = ({canvas}) => {
  const gl = canvas?.getContext('webgl2', {antialias: false})

  let modelView
  let stopped_ = false
  const defaultManagerConfig_ = {
    coordinateSpace: ModelManager.CoordinateSystem.RUF,
    splatResortRadians: 0.18,  // 10 degrees.
    splatResortMeters: 0.2,    // 7.5 inches.
    sortTexture: false,
    multiTexture: false,
    bakeSkyboxMeters: 95,      // 95m.
    pointPruneDistance: 0,     // No skybox pruning.
    pointFrustumLimit: 1.7,    // 120 degree FOV.
    pointSizeLimit: 1e-4,      // 0.3mm @ 1m.
  }
  let onLoadedCb_ = () => {}

  const mouseToTouchTranslator = createMouseToTouchTranslator()

  const modulePromise = WEBMODELVIEW({}).then((module) => {
    modelView = module
  })

  const onloaded = ({data}) => {
    const dataPtr = writeArrayToEmscriptenHeap(modelView, data)
    modelView._c8EmAsm_setModel(dataPtr, data.byteLength)
    modelView._free(dataPtr)
    onLoadedCb_()
  }

  const onupdated = ({data}) => {
    const dataPtr = writeArrayToEmscriptenHeap(modelView, data)
    modelView._c8EmAsm_updateModel(dataPtr, data.byteLength)
    modelView._free(dataPtr)
  }

  let modelManager
  let clientWidth_ = 0
  let clientHeight_ = 0

  const needsResize = () => (
    canvas.clientWidth !== clientWidth_ || canvas.clientHeight !== clientHeight_
  )

  const resize = () => {
    const MAX_RES = 1920
    clientHeight_ = canvas.clientHeight
    clientWidth_ = canvas.clientWidth
    const targetWidth = clientWidth_ * window.devicePixelRatio
    const targetHeight = clientHeight_ * window.devicePixelRatio
    const downsampleX = Math.min(1, MAX_RES / targetWidth)
    const downsampleY = Math.min(1, MAX_RES / targetHeight)
    const downsample = Math.min(downsampleX, downsampleY)

    const newWidth = targetWidth * downsample
    const newHeight = targetHeight * downsample

    canvas.width = newWidth
    canvas.height = newHeight

    modelView._c8EmAsm_setResolution(newWidth, newHeight)
  }

  const tick = (frameTimeMillis, xrFrame: XrFrame) => {
    if (stopped_) {
      return
    }
    if (needsResize()) {
      resize()
    }
    if (xrFrame) {
      const pose = xrFrame.getViewerPose(refSpace_)
      if (!pose) {
        return
      }

      const {baseLayer} = xrFrame.session.renderState

      if (!baseLayer?.framebuffer) {
        return
      }

      if (baseLayer && baseLayer.framebuffer) {
        if (!baseLayer.framebuffer.name) {
          const id = modelView.GL.getNewId(modelView.GL.framebuffers)
          baseLayer.framebuffer.name = id
          modelView.GL.framebuffers[id] = baseLayer.framebuffer
        }

        const frame = {
          framebuffer: {
            width: baseLayer.framebufferWidth,
            height: baseLayer.framebufferHeight,
            id: baseLayer.framebuffer.name,
          },
          pose: {
            position: pose.transform.position,
            orientation: pose.transform.orientation,
          },
          views: pose.views.map((view) => {
            const vp = baseLayer.getViewport(view)
            return {
              viewport: {x: vp.x, y: vp.y, width: vp.width, height: vp.height},
              position: view.transform.position,
              orientation: view.transform.orientation,
              projectionMatrix: [...view.projectionMatrix],
            }
          }),
        }
        const frameJson = writeStringToEmscriptenHeap(modelView, JSON.stringify(frame))
        modelView._c8EmAsm_handleWebXrFrame(frameJson)
        modelView._free(frameJson)
      }
    }
    modelView._c8EmAsm_render(frameTimeMillis)
    const renderState = JSON.parse((window as any)._modelView.renderState)
    delete (window as any)._modelView
    if (renderState.updatedCamera) {
      modelManager.updateView(renderState)
    }
  }

  const emitGestureEvent = (e) => {
    preventDefault(e)
    const touches = e.touches || e.detail.touches
    // TODO: Android has something like:
    //     int countMod = a == MotionEvent.ACTION_UP || a == MotionEvent.ACTION_POINTER_UP ? -1 : 0;
    // Do we need that here?
    const touchPosition = t => ({
      x: t.clientX,
      y: t.clientY,
      rawX: t.clientX,
      rawY: t.clientY,
    })
    const touchEvent = {
      timeMillis: Date.now(),
      count: touches.length,
      pos: [...touches].map(touchPosition),
      screenWidth: window.innerWidth,
      screenHeight: window.innerHeight,
      viewWidth: canvas.clientWidth,
      viewHeight: canvas.clientHeight,
    }

    const touchJson = writeStringToEmscriptenHeap(modelView, JSON.stringify(touchEvent))
    modelView._c8EmAsm_handleTouchEvent(touchJson)
    modelView._free(touchJson)
  }

  const configure = (config: Partial<ViewParams>) => {
    const newConfig = {...defaultManagerConfig_, ...config}
    modelManager.configure(newConfig)

    const configJson = writeStringToEmscriptenHeap(modelView, JSON.stringify(newConfig))
    modelView._c8EmAsm_configure(configJson)
    modelView._free(configJson)
  }

  const onLoad = () => {
    modelManager = ModelManager.create()

    modelManager.onloaded(onloaded)
    modelManager.onupdated(onupdated)

    if (!gl) {
      throw new Error('[WebModelView] WebGL2 required but not supported')
    }
    // synchronize our Emscripten GL to this context
    const attributes = gl.getContextAttributes() || {}
    if (gl.PIXEL_PACK_BUFFER) {
      (attributes as any).majorVersion = 2
    }

    // Reset the old string cache in case there was a different context before.
    modelView.GL.stringCache = {}
    modelView.GL.makeContextCurrent(modelView.GL.registerContext(gl, attributes))

    configure({})

    mouseToTouchTranslator.attach({canvas})

    canvas.addEventListener('touchstart', emitGestureEvent)
    canvas.addEventListener('touchend', emitGestureEvent)
    canvas.addEventListener('touchmove', emitGestureEvent)

    resize()

    ticker_ = startTick(tick)
  }

  const startup = modulePromise.then(() => onLoad())

  const loadFiles = (fileList: FileList) => startup.then(() => modelManager.loadFiles(fileList))
  const loadBytes = (filename: string, bytes: Uint8Array) => (
    startup.then(() => modelManager.loadBytes(filename, bytes))
  )
  const loadModel = (srcs: ModelSrc[]) => startup.then(() => modelManager.loadModel(srcs))

  const stop = () => {
    stopped_ = true
  }

  const setOnLoaded = (onLoadedCb) => { onLoadedCb_ = onLoadedCb }

  const unload = () => {
    stop()
    mouseToTouchTranslator.detach()
    canvas.removeEventListener('touchstart', emitGestureEvent)
    canvas.removeEventListener('touchend', emitGestureEvent)
    canvas.removeEventListener('touchmove', emitGestureEvent)
  }

  const startCropMode = (edits: Partial<ModelEdits>) => {
    const editsJson = writeStringToEmscriptenHeap(modelView, JSON.stringify(edits))
    modelView._c8EmAsm_startCropMode(editsJson)
    modelView._free(editsJson)
  }

  const finishCropMode = (): ModelEdits => {
    modelView._c8EmAsm_finishCropMode()
    const editState = JSON.parse((window as any)._modelView.editState) as ModelEdits
    delete (window as any)._modelView
    return editState
  }

  const updateCropViewpoint = (side: CropBoxSide) => {
    const json = writeStringToEmscriptenHeap(modelView, JSON.stringify({side}))
    modelView._c8EmAsm_updateCropViewpoint(json)
    modelView._free(json)
  }

  const updateCropMode = (edits: Partial<ModelEdits>) => {
    const editsJson = writeStringToEmscriptenHeap(modelView, JSON.stringify(edits))
    modelView._c8EmAsm_updateCropMode(editsJson)
    modelView._free(editsJson)
  }

  const supportedXrSessionTypes = async (): Promise<string[]> => {
    if (!xr) {
      return []
    }

    const [arSupported, vrSupported] = await Promise.all([
      xr.isSessionSupported('immersive-ar'),
      xr.isSessionSupported('immersive-vr'),
    ])

    const types: string[] = []
    if (arSupported) {
      types.push('immersive-ar')
    }
    if (vrSupported) {
      types.push('immersive-vr')
    }

    return types
  }

  const startXr = async (sessionType: string): Promise<void> => {
    if (session_) {
      throw new Error('XR session already started')
    }
    // Start the session.
    session_ = await xr.requestSession(sessionType, {requiredFeatures: ['local-floor']})

    // Set the reference space, but don't wait for it to be ready.
    session_.requestReferenceSpace('local-floor').then((refSpace) => { refSpace_ = refSpace })

    // Start animating with the session's raf.
    ticker_?.stop()
    ticker_ = startTick(tick, session_)
    modelView._c8EmAsm_startWebXr()

    // TODO:
    // xrImmersiveRefSpace.addEventListener('reset', (evt) => {
    //     if (evt.transform) {
    //       // AR experiences typically should stay grounded to the real world.
    //       // If there's a known origin shift, compensate for it here.
    //       xrImmersiveRefSpace = xrImmersiveRefSpace.getOffsetReferenceSpace(evt.transform);
    //     }
    //   })

    session_.addEventListener('end', () => {
      ticker_?.stop()
      ticker_ = startTick(tick)
      modelView._c8EmAsm_finishWebXr()
      session_ = null
      refSpace_ = null
    })

    // Wait for the context to be XR compatible and then attach it to the session.
    await (gl as any).makeXRCompatible()

    const baseLayer = new XRWebGLLayer(session_, gl)
    baseLayer.fixedFoveation = 1  // Speed up rendering on headsets that support it.

    session_.updateRenderState({baseLayer})
  }

  return {
    configure: (config: Partial<ViewParams>) => modulePromise.then(() => configure(config)),
    loadBytes,
    loadFiles,
    loadModel,
    onloaded: setOnLoaded,
    stop,
    unload,
    // Crop
    startCropMode,
    updateCropMode,
    updateCropViewpoint,
    finishCropMode,
    // XR
    supportedXrSessionTypes,
    startXr,
  }
}

const CoordinateSpace = {
  UNSPECIFIED: 0,
  RUF: 1,
  RUB: 2,
}

const WebModelView = {
  create,
  CoordinateSpace,
  CropBoxSide,
  CropShape,
}

export {
  WebModelView,
  CoordinateSpace,
  CropBoxSide,
  CropShape,
  ViewParams,
  ModelEdits,
}
