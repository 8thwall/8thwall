// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

// Entry point for jsxr package
import type {XrccModule} from 'reality/app/xr/js/src/types/xrcc'

import {XrDeviceFactory, setIncompatibilityOverride} from './js-device'

import {checkGLError} from './gl-renderer'

import {contextWrapper} from './context-wrapper'

import {create8wCanvas} from './canvas'
import {OffscreenCanvasAvailable, hasValidOffscreenCanvas} from './offscreen-canvas'
import {XrPermissionsFactory} from './permissions'
import {XrConfigFactory} from './config'
import {createCallbackManager} from './callback-manager'
import {SetManager} from './set-manager'
import {isDocumentVisible, getVisibilityEventName} from './document-visibility'
import {SessionManagerDesktop3D} from './session-manager-desktop3d'
import {SessionManagerGetUserMedia} from './session-manager-getusermedia'
import type {
  ExtendedUpdateInput, PipelineState, StepResult, RunConfig, SessionManagerDefinition,
  SessionManagerInstance, ModuleName, FrameworkHandle, ModuleEvent, EventHandlers,
  EventHandlerRegistration, EventName, EventHandler, RenderContext,
  EmscriptenWebGLContextAttributes,
} from './types/pipeline'
import type {CallbackName, InputForCallbackName, WithoutFramework} from './types/callbacks'
import type {Module} from './types/module'
import {writeStringToEmscriptenHeap} from './write-emscripten'
import {createContinuousLifecycle} from './lifecycle'
import {createRunLoop} from './loop'

// TODO(christoph): Fill in proper typing
type TodoType = any

declare const OffscreenCanvas: any

/* eslint-disable @typescript-eslint/no-use-before-define */

function CameraLoop(rawXrccPromise: Promise<XrccModule>) {
  let runConfig: RunConfig = null
  let xrcc: XrccModule = null

  let xrccWithContextResolver: TodoType = null
  const xrccWithContextPromise = new Promise<XrccModule>((resolve) => {
    xrccWithContextResolver = resolve
  })

  // Run state
  let paused = false
  let isEngaged = false
  let isResuming = false
  const pipelineState: PipelineState = {}
  const attachState: TodoType = {}

  // Session manager
  let sessionManager_: SessionManagerInstance = null

  // Video state
  const lastVideoSize = {width: -1, height: -1}
  const nextVideoSize = {width: 0, height: 0}
  let timestamp: number
  let lastFrameTime: number

  // drawCanvas is used for the user's WebGL engine like THREE.js.  We also manipulate this canvas
  // when drawing the camera feed to it.
  let drawCanvas: HTMLCanvasElement
  let drawCtx: RenderContext
  const lastDrawCanvasSize = {width: -1, height: -1}
  // computeCanvas is an offscreen canvas that runs our Emscripten computer vision algorithms.
  // It is not added to the dom.  We have two different compute canvases because if the user
  // switches between WebGl1 and WebGl2 during a running application, then it the computeCanvas will
  // stay with its original context version.  This way, even if they switch multiple times, they
  // will only switch between the same two webgl contexts.
  let computeCanvas: HTMLCanvasElement | typeof OffscreenCanvas
  let computeCtx: RenderContext

  // Module state
  const modules = new Set<string>()
  let events: ModuleEvent[] = []
  const eventHandlers: EventHandlers = {}
  const requiredPermissionsManager = new SetManager([XrPermissionsFactory().permissions().CAMERA])
  const callbackManager = createCallbackManager()
  let isDispatchingEvents_ = false

  const attachLifecycle = createContinuousLifecycle(
    (filter) => {
      const cb = callbackManager.getNamedCallbacks('onAttach')
      runCallbacks(cb.filter(e => filter(e.name)), attachState)
    },
    (filter) => {
      const cb = callbackManager.getNamedCallbacks('onDetach')
      runCallbacks(cb.filter(e => filter(e.name)), {})
    }
  )

  const sessionAttachLifecycle = createContinuousLifecycle(
    (filter) => {
      const cb = callbackManager.getNamedCallbacks('onSessionAttach')
      runCallbacks(cb.filter(e => filter(e.name)), attachState)
    },
    (filter) => {
      const cb = callbackManager.getNamedCallbacks('onSessionDetach')
      runCallbacks(cb.filter(e => filter(e.name)), {})
    }
  )

  const verboseLog = (...args) => {
    if (runConfig && runConfig.verbose) {
      console.log(...args)  // eslint-disable-line no-console
    }
  }

  const verboseError = (...args) => {
    if (runConfig && runConfig.verbose) {
      console.error(...args)  // eslint-disable-line no-console
    }
  }

  // a map used to keep the camera feed in sync between the draw and compute canvas.  We produce
  // the computer vision data for a camera feed frame a few frames after it was originally captured.
  // This cache gets populated in readCameraToTexture, and has at most the number of frames that
  // our engine's RingBuffer stores. Therefore, once we produce the CV data, we are able to retrieve
  // the correct drawCtx frame given the computeCtx buffer's name.
  let drawTexMap = {}
  const drawTexForComputeTex = (computeTex) => {
    let t = drawTexMap[computeTex.name]
    if (!t) {
      // we don't have a corresponding draw texture for the compute texture, so we are going to
      // create a new texture in the drawContext.

      // Make a copy of the current texture bound to TEXTURE_BINDING_2D, which we will re-set to
      // TEXTURE_BINDING_2D at the end
      const restoreTex = drawCtx.getParameter(drawCtx.TEXTURE_BINDING_2D)

      // create a corresponding drawCtx texture object for the computeCtx texture object
      t = drawCtx.createTexture()
      // give them the same name so the name can be the key for drawTexMap
      t.name = computeTex.name
      t.drawCtx = drawCtx
      drawTexMap[t.name] = t

      // We bind the texture TEXTURE_2D.  This is necessary so we can set OpengL wrapping and
      // filtering commands onto the texture buffer using commands like texParameteri(). We're not
      // actually setting the texture's contents here.  We really should only be hitting this path
      // when called from readCameraToTexture(), which is where we will be filling the texture
      // with the contents of the camera feed.
      drawCtx.bindTexture(drawCtx.TEXTURE_2D, t)
      drawCtx.hint(drawCtx.GENERATE_MIPMAP_HINT, drawCtx.FASTEST)
      drawCtx.texParameteri(drawCtx.TEXTURE_2D, drawCtx.TEXTURE_WRAP_S, drawCtx.CLAMP_TO_EDGE)
      drawCtx.texParameteri(drawCtx.TEXTURE_2D, drawCtx.TEXTURE_WRAP_T, drawCtx.CLAMP_TO_EDGE)
      drawCtx.texParameteri(drawCtx.TEXTURE_2D, drawCtx.TEXTURE_MAG_FILTER, drawCtx.LINEAR)
      drawCtx.texParameteri(drawCtx.TEXTURE_2D, drawCtx.TEXTURE_MIN_FILTER, drawCtx.LINEAR)

      if (runConfig.verbose) {
        checkGLError({GLctx: drawCtx, msg: 'CameraLoop.drawTexForComputeTex'})
      }

      // Reset to the previously bound TEXTURE_2D. That way, we don't disturb the user's scene.
      // We do a similar caching and restoring of TEXTURE_2D in readCameraToTexture().
      drawCtx.bindTexture(drawCtx.TEXTURE_2D, restoreTex)
    }
    return t
  }

  const isWebgl2 = gl => !!gl.PIXEL_PACK_BUFFER

  const onError = (e) => {
    const onExceptionCallbacks = callbackManager.getCallbacks('onException')
    if (onExceptionCallbacks.length) {
      verboseError('[XR] Error:', e)
      runRawCallbacks(onExceptionCallbacks, e)
    } else {
      console.error('[XR] Error:', e)  // eslint-disable-line no-console
    }
  }

  const canPause = () => (runConfig && isEngaged && (!paused || isResuming))

  // Gets a user-visible subset of the config struct.
  const getUserVisibleConfig = () => {
    const {
      webgl2, ownRunLoop, cameraConfig, glContextConfig, allowedDevices, verbose,
      sessionConfiguration, sessionInitBehavior,
    } = runConfig
    const visibleConfig: RunConfig = {
      allowedDevices,
      cameraConfig: {...cameraConfig},
      glContextConfig: {...glContextConfig},
      ownRunLoop,
      webgl2,
      sessionConfiguration: {...sessionConfiguration},
      sessionInitBehavior,
    }

    // only expose 'verbose' if it is explicitly set
    if (verbose) {
      visibleConfig.verbose = true
    }

    return visibleConfig
  }

  const isCompatibleMobile = () => (
    XrDeviceFactory().isDeviceBrowserCompatible({allowedDevices: XrConfigFactory().device().MOBILE})
  )

  const windowOrientation = () => {
    if (!isCompatibleMobile()) {
      // Desktop browsers have a landscape mode sensor but report a portrait orientation. We need
      // to correct for that here.
      return 90
    }

    // NOTE(dat): On iOS 16.4, window.screen.orientation is now implemented. However,
    // window.screen.orientation.angle is different than Android or Firefox
    // https://bugs.webkit.org/show_bug.cgi?id=254863
    // It's as if the primary orientation angle is CW instead of CCW.
    if (window.screen.orientation) {
      const isIos = XrDeviceFactory().deviceEstimate().os === 'iOS'
      // NOTE(paris): The logic below to compute angle doesn't fully work for iOS because
      // `window.screen.orientation.angle` is not updated when the `'orientationchange'` event is
      // fired. This results in the camera feed becoming warped and the returned value not matching
      // the old `window.orientation` value. So here we just return the old `window.orientation`
      // value which is still available and correct.
      // TODO(someone): The correct fix is likely to only use `window.screen.orientation` when
      // `ScreenOrientation.onchange` is called. If so it indicates that there is an iOS bug where
      // `'orientationchange'` is not the same as `ScreenOrientation.onchange`.
      if (isIos && window.orientation != null) {
        return window.orientation
      }
      const angle = isIos ? 360 - window.screen.orientation.angle : window.screen.orientation.angle
      return ((angle + 90) % 360) - 90  // 270 -> -90
    }

    // This device does not support the new API, fallback to the deprecated API.
    return window.orientation
  }

  const pushTraceGen = (tag) => {
    let tagPtr: number
    return () => {
      if (!tagPtr) {
        tagPtr = writeStringToEmscriptenHeap(xrcc, tag)
      }
      xrcc._c8EmAsm_pushTrace(tagPtr)
    }
    // WARNING: free is not called; this means when a method is wrapped as a traced method, it
    // creates a string that lasts the lifetime of the app. In practice, this means that trace8
    // should never be used with functions that are declared inline.
  }

  const popTrace = () => {
    xrcc._c8EmAsm_popTrace()
  }

  const flushTrace = () => {
    xrcc._c8EmAsm_flushTrace()
  }

  const trace8 = (tag, cb) => {
    const pushTrace = pushTraceGen(tag)
    return (...args) => {
      pushTrace()
      try {
        return cb(...args)
      } finally {
        popTrace()
      }
    }
  }

  const shouldSkipFrame = () => {
    if (timestamp === lastFrameTime) {
      return true
    }

    if (paused) {
      return true
    }

    if (!isEngaged) {
      return true
    }

    return false
  }

  const frameworkForModule = (moduleName: ModuleName): FrameworkHandle => ({
    dispatchEvent: (eventName: EventName, detail: unknown) => {
      events.push({name: `${moduleName}.${eventName}`, detail})
    },
  })

  const readCameraToTexture = trace8('read-camera-to-texture', () => {
    let cameraTexture = window._c8.readyTexture
    const {repeatFrame, videoSize} = sessionManager_.hasNewFrame()
    nextVideoSize.width = videoSize.width
    nextVideoSize.height = videoSize.height

    // The video can be paused or have 0 dimensions if the user does something like navigate away
    // from the page and then come back on Safari, which would resume the page instead of reloading.
    // In that case, just return the previous frame by keeping repeatFrame true.
    if (!repeatFrame || !cameraTexture) {
      xrcc._c8EmAsm_getReadyInPipeline(nextVideoSize.width, nextVideoSize.height)
      cameraTexture = window._c8.readyTexture
      xrcc._c8EmAsm_markTextureFilledInPipeline()
    }

    // also load the camera feed into the draw context.  The cameraTexture has a name, so we get
    // the corresponding drawCtx buffer from the drawTexMap cache.  Then we re-use the drawCtx
    // buffer and populate it with the camera feed frame.
    const drawTexture = drawTexForComputeTex(cameraTexture)

    pipelineState.processGpuInput = {
      frameStartResult: Object.assign(  // eslint-disable-line prefer-object-spread
        {
          cameraTexture: drawTexture,
          computeTexture: cameraTexture,
          GLctx: drawCtx,
          computeCtx,
          orientation: windowOrientation(),
          repeatFrame,
          frameTime: window.performance.now() / 1000,
        },
        sessionManager_.frameStartResult(
          {repeatFrame, drawCtx, computeCtx, drawTexture, computeTexture: cameraTexture}
        )
      ),
    }
  })

  const copySubFields = <T extends Record<string, Record<string, unknown>>>(data: T): T => (
    (Object.keys(data) as (keyof T)[]).reduce((copy, key) => {
      copy[key] = {...data[key]}
      return copy
    }, {} as T)
  )

  const runCallbacks = (callbacks, data): StepResult => {
    const finalResult = callbacks.reduce((result, {cb, name}) => {
      const input = name ? ({framework: frameworkForModule(name), ...data}) : data
      const callbackResult = cb(input)
      if (name) {
        result[name] = callbackResult
      }
      return result
    }, {})
    dispatchFrameworkEvents()
    return finalResult
  }

  const runRawCallbacks = (callbacks, data?: TodoType) => callbacks.forEach(cb => cb(data))

  const maybeRunCanvasSizeChange = () => {
    if (nextVideoSize.width === 0 || nextVideoSize.height === 0 || drawCanvas.width === 0 ||
        drawCanvas.height === 0) {
      return
    }
    if (lastDrawCanvasSize.width !== -1 && lastDrawCanvasSize.height !== -1 &&
        (lastDrawCanvasSize.width !== drawCanvas.width ||
          lastDrawCanvasSize.height !== drawCanvas.height)) {
      runOnCanvasSizeChange({
        GLctx: drawCtx,
        computeCtx,
        videoWidth: nextVideoSize.width,
        videoHeight: nextVideoSize.height,
        canvasWidth: drawCanvas.width,
        canvasHeight: drawCanvas.height,
      })
    }
    lastDrawCanvasSize.width = drawCanvas.width
    lastDrawCanvasSize.height = drawCanvas.height
  }

  const maybeRunOnVideoSizeChange = () => {
    if (nextVideoSize.width === 0 || nextVideoSize.height === 0 || drawCanvas.width === 0 ||
        drawCanvas.height === 0) {
      return
    }
    if (lastVideoSize.width !== -1 && lastVideoSize.height !== -1 &&
        (lastVideoSize.width !== nextVideoSize.width ||
          lastVideoSize.height !== nextVideoSize.height)) {
      runOnVideoSizeChange({
        GLctx: drawCtx,
        computeCtx,
        videoWidth: nextVideoSize.width,
        videoHeight: nextVideoSize.height,
        canvasWidth: drawCanvas.width,
        canvasHeight: drawCanvas.height,
        orientation: windowOrientation(),
      })
    }
    lastVideoSize.width = nextVideoSize.width
    lastVideoSize.height = nextVideoSize.height
  }

  const runOnProcessGpu = trace8('process-gpu', () => {
    const processGpuInput = copySubFields(pipelineState.processGpuInput)
    const processGpuResult =
      runCallbacks(callbackManager.getNamedCallbacks('onProcessGpu'), pipelineState.processGpuInput)
    pipelineState.processCpuInput = {processGpuResult, ...processGpuInput}
  })

  const runOnProcessCpu = trace8('process-cpu', () => {
    if (!pipelineState.processCpuInput) {
      return
    }
    const processCpuInput = copySubFields(pipelineState.processCpuInput)

    // Duplicate field to support new and deprecated names
    processCpuInput.cameraTextureReadyResult = processCpuInput.processGpuResult

    const processCpuResult =
      runCallbacks(callbackManager.getNamedCallbacks('onProcessCpu'), processCpuInput)

    pipelineState.updateInput = {processCpuResult, ...processCpuInput}

    const {repeatFrame} = pipelineState.processCpuInput.frameStartResult
    if (!repeatFrame) {
      xrcc._c8EmAsm_markStageFinishedInPipeline()
    }
  })

  const runOnUpdate = trace8('update', () => {
    if (!pipelineState.updateInput) {
      return
    }

    const updateInput: ExtendedUpdateInput = Object.assign(
      copySubFields(pipelineState.updateInput), {
        fps: 1000 / (timestamp - (lastFrameTime || 0)),
        GLctx: pipelineState.updateInput.frameStartResult.GLctx,
        cameraTexture: pipelineState.updateInput.frameStartResult.cameraTexture,
        video: null,

        // Duplicate field to support new and deprecated names
        cameraTextureReadyResult: pipelineState.updateInput.processGpuResult,
        heapDataReadyResult: pipelineState.updateInput.processCpuResult,
      }
    )

    const updateResults = runCallbacks(callbackManager.getNamedCallbacks('onUpdate'), updateInput)

    if (pipelineState.onFrameFinished) {
      const {cameraTexture} = pipelineState.onFrameFinished.frameStartResult
      const {repeatFrame} = pipelineState.processCpuInput.frameStartResult
      // We can only release this frame if the next frame in the pipeline is not a repeat of this
      // frame.
      if (!repeatFrame) {
        if (pipelineState.heldForRelease && pipelineState.heldForRelease.length) {
          xrcc._c8EmAsm_releaseInPipeline(pipelineState.heldForRelease.shift())
          pipelineState.heldForRelease = undefined
        } else {
          xrcc._c8EmAsm_releaseInPipeline(cameraTexture.name)
        }
      } else {
        if (!pipelineState.heldForRelease) {
          pipelineState.heldForRelease = []
        }
        pipelineState.heldForRelease.push(cameraTexture.name)
      }
    }
    pipelineState.onFrameFinished = {updateResults, ...updateInput}
  })

  const runRenderCallbacks = () => {
    runRawCallbacks(callbackManager.getCallbacks('onRender'))
  }

  const runOnBeforeSessionInitialize = (sessionAttributes) => {
    const callbacks = callbackManager.getCallbacks('onBeforeSessionInitialize')
    const data = {sessionAttributes, config: getUserVisibleConfig()}
    Object.assign(attachState, data)
    runRawCallbacks(callbacks, data)
  }

  const runPreRenderInternal = trace8('pre-render', () => {
    try {
      maybeRunOnVideoSizeChange()
      maybeRunCanvasSizeChange()
      runOnProcessCpu()
      runOnUpdate()
      readCameraToTexture()
    } catch (e) {
      onError(e)
    }
  })

  const pushTraceRaf = pushTraceGen('request-animation-frame')
  const runPreRender = (timestamp_) => {
    flushTrace()
    pushTraceRaf()  // popped in runPostRender
    runOnAnimationFrameStart(timestamp_)
    if (shouldSkipFrame()) {
      return
    }
    runPreRenderInternal()
  }

  const runRender = trace8('render', () => {
    if (shouldSkipFrame()) {
      return
    }

    try {
      runRenderCallbacks()
    } catch (e) {
      onError(e)
    }
  })

  const runPostRenderInternal = trace8('post-render', () => {
    try {
      runOnProcessGpu()
      runOnAnimationFrameEnd()
    } catch (e) {
      onError(e)
    }
  })

  const runPostRender = () => {
    try {
      if (shouldSkipFrame()) {
        return
      }
      runPostRenderInternal()
      maybeFixCameraStall()
    } finally {
      popTrace()  // popped from runPreRender
    }
  }

  const runOnAnimationFrameStart = (timestamp_) => {
    timestamp = timestamp_
  }

  const runOnAnimationFrameEnd = () => {
    lastFrameTime = timestamp
  }

  // Called once every animation frame
  const runLoop = createRunLoop((timestamp_: number) => {
    runPreRender(timestamp_)
    if (!shouldSkipFrame()) {
      runRender()
    }
    runPostRender()
  })

  const refreshRunLoop = () => {
    const shouldBeLooping = (
      isEngaged &&
      !paused &&
      runConfig && runConfig.ownRunLoop &&
      sessionManager_ &&
      !sessionManager_.getSessionAttributes().providesRunLoop
    )

    if (shouldBeLooping) {
      runLoop.start()
    } else {
      runLoop.stop()
    }
  }

  const CONTEXT_OVERRIDES = {
    // alpha: true,
    // antialias: true,
    // depth: true,
    // failIfMajorPerformanceCaveat: false,
    // powerPreference: "default",
    // premultipliedAlpha: true,
    preserveDrawingBuffer: true,  // default: false
    // stencil: false,
    // desynchronized: false,
    // xrCompatible: false,
  }

  const createOffscreenCanvas = (): HTMLCanvasElement | typeof OffscreenCanvas => {
    const hasOffscreen = hasValidOffscreenCanvas()
    let newCanvas = null
    try {
      if (hasOffscreen === OffscreenCanvasAvailable.Webgl1 ||
        hasOffscreen === OffscreenCanvasAvailable.Webgl2) {
        newCanvas = new OffscreenCanvas(0, 0)
        verboseLog('[XR] Creating an OffscreenCanvas')
        return newCanvas
      }
    } catch {
      // do nothing
    }
    // If we can't use the offscreen canvas, fallback to a normal one.
    verboseLog('[XR] Fallback. Creating a compute canvas in DOM.')
    return create8wCanvas('compute')
  }

  const createContext = () => {
    let isWebGl2 = false
    const glContextConfig = {...CONTEXT_OVERRIDES, ...runConfig.glContextConfig || {}}

    // Get webgl2 if it is preferred by the user and available. Otherwise, fall back to webgl1.
    if (runConfig.webgl2) {
      drawCtx = drawCanvas.getContext('webgl2', glContextConfig) as WebGL2RenderingContext
      isWebGl2 = drawCtx && isWebgl2(drawCtx)
    }
    if (!drawCtx) {
      drawCtx = drawCanvas.getContext('webgl', glContextConfig) as WebGLRenderingContext
    }

    // If the user passed in a canvas with a pre-defined webgl2 context, but forgot to specify in
    // the run config {webgl2: true}, then we still would not have a drawCtx at this point.
    if (!drawCtx) {
      drawCtx = drawCanvas.getContext('webgl2', glContextConfig) as WebGL2RenderingContext
      isWebGl2 = drawCtx && isWebgl2(drawCtx)
    }

    // We need to make sure that we don't keep creating compute canvases every time the user
    // starts and stops the experience. Otherwise, we'll hit the max WebGLContext limit.
    if (!computeCanvas) {
      computeCanvas = createOffscreenCanvas()
    }

    computeCtx = computeCanvas.getContext((isWebGl2 ? 'webgl2' : 'webgl'), glContextConfig)

    // Restore compute canvas and context if context is lost
    if (!computeCtx || computeCtx.isContextLost()) {
      computeCanvas = createOffscreenCanvas()
      computeCtx = computeCanvas.getContext((isWebGl2 ? 'webgl2' : 'webgl'), glContextConfig)
    }

    verboseLog('[XR] Using WebGL', (isWebGl2) ? '2' : '1')
    try {
      verboseLog('[XR] Using OffscreenCanvas', computeCanvas instanceof OffscreenCanvas)
    } catch {
      // Older version of browsers (Safari) will crash when trying to access OffscreenCanvas
    }

    contextWrapper(drawCtx, runConfig.verbose, true, isWebGl2)
    contextWrapper(computeCtx, runConfig.verbose, true, isWebGl2)

    // synchronize our Emscripten GL to this context
    const attributes: EmscriptenWebGLContextAttributes = computeCtx.getContextAttributes()
    if (!attributes) {
      // The context has been lost
      throw new Error('[XR] Context has been lost before call to getContextAttributes().')
    }

    if (isWebGl2) {
      attributes.majorVersion = 2
    } else {
      attributes.majorVersion = 1
    }

    // This turns of Emscripten loading all available extensions for the platform.  It also prevents
    // Emscripten from wrapping WebGL1 with WebGL2 methods.  This is done by us in contextWrapper,
    // and we don't want Emscripten wrapping our already wrapped methods.
    attributes.enableExtensionsByDefault = false

    const resgisterGlWithModule = () => {
      // If the user stops and then restarts the camera loop, we will be passing in the same
      // computeCtx as before.
      const glHandle = xrcc.GL.registerContext(computeCtx, attributes)
      // Reset the old string cache in case there was a different context before.
      xrcc.GL.stringCache = {}
      xrcc.GL.makeContextCurrent(glHandle)
    }

    if (xrcc) {
      // If this code has run before, we don't need to wait on the initialization promise. This will
      // reset gl state on xrcc immediately.
      resgisterGlWithModule()
    } else {
      // Block until web assembly is initialized before setting the gl context.
      rawXrccPromise.then((xrcc_) => {
        xrcc = xrcc_
        resgisterGlWithModule()
        xrccWithContextResolver(xrcc)
      })
    }
    return {isWebGl2}
  }

  // Called whenever there is an update to the browsers visibility
  const onVisibilityChange = (isVisible) => {
    if (isVisible) {
      // The page is visible once again.
      // If the user exits the experience without pausing or stopping, then the video is
      // automatically paused without CameraLoop() knowing that anything has changed. If the user
      // comes back, the camera feed will be stalled. We should simply resume the camera feed in
      // this case.
      if (isEngaged && !paused && !isResuming && sessionManager_.resumeIfAutoPaused) {
        sessionManager_.resumeIfAutoPaused()  // undocumented
      }
    }
  }

  // Whenever the browser's visibility changes, this method gets the browser specific keywords
  // for the visibility change and then calls onVisibilityChange
  const onBrowserVisibilityUpdate = () => {
    onVisibilityChange(isDocumentVisible())
  }

  const initializeCameraPipeline = () => {
    const canvasWidth = drawCanvas.width
    const canvasHeight = drawCanvas.height

    xrcc._c8EmAsm_initCameraRuntime(
      nextVideoSize.width,
      nextVideoSize.height,
      canvasWidth,
      canvasHeight,
      isWebgl2(computeCtx)
    )
  }

  const handleFirstFrame = () => {
    const {videoSize} = sessionManager_.hasNewFrame()
    nextVideoSize.width = videoSize.width
    nextVideoSize.height = videoSize.height

    // Initialize pipeline for video feed.
    initializeCameraPipeline()

    lastDrawCanvasSize.width = drawCanvas.width
    lastDrawCanvasSize.height = drawCanvas.height

    // Wait until loopCtx is initialized before adding orientation change listeners.
    window.addEventListener('orientationchange', orientationChangeCallback)
    document.addEventListener(getVisibilityEventName(), onBrowserVisibilityUpdate)

    runOnStart({
      canvas: drawCanvas,
      GLctx: drawCtx,
      computeCtx,
      isWebgl2: isWebgl2(computeCtx),
      orientation: windowOrientation(),
      videoWidth: nextVideoSize.width,
      videoHeight: nextVideoSize.height,
      canvasWidth: drawCanvas.width,
      canvasHeight: drawCanvas.height,
      rotation: windowOrientation(),
      config: getUserVisibleConfig(),
    })

    maybeRunOnVideoSizeChange()
    maybeRunCanvasSizeChange()

    // Set isEngaged so that any pipeline modules added during attach will have onAttach called.
    isEngaged = true
    attachLifecycle.attach()
    sessionAttachLifecycle.attach()
    runOnAppResourcesLoaded()

    refreshRunLoop()
  }

  // The Emscripten GL state should be fully reset when the pipeline is stopped.
  // Otherwise the GL.counter and all tables will keep growing every time the pipeline is started.
  // There is an open issue at https://github.com/emscripten-core/emscripten/issues/14595
  const resetEmscriptenGLState = (gl: any) => {
    if (!gl) {
      return
    }

    gl.counter = 1
    gl.buffers = []
    gl.contexts = []
    gl.framebuffers = []
    gl.programs = []
    gl.queries = []
    gl.renderbuffers = []
    if (gl.samplers !== undefined) {
      gl.samplers = []
    }
    gl.shaders = []
    gl.textures = []
    gl.uniforms = []
    gl.vaos = []
  }

  const stop = () => {
    if (sessionManager_) {
      sessionManager_.stop()
    }

    sessionAttachLifecycle.detach()
    attachLifecycle.detach()

    if (xrcc) {
      xrcc._c8EmAsm_resetOnPipelineStop()
      // delete the gl texture objects and clear our cache
      Object.keys(drawTexMap).map(key => (drawCtx.deleteTexture(drawTexMap[key])))
      drawTexMap = {}
    }

    isEngaged = false
    paused = false
    refreshRunLoop()

    if (drawCtx) {
      // API to undo context wrapper functions.
      drawCtx.disable(drawCtx.NO_ERROR)
      drawCtx = null
    }
    if (computeCtx) {
      computeCtx.disable(computeCtx.NO_ERROR)
      computeCtx = null
    }

    if (xrcc) {
      resetEmscriptenGLState(xrcc.GL)
    }

    window.removeEventListener('orientationchange', orientationChangeCallback)
    document.removeEventListener(getVisibilityEventName(), onBrowserVisibilityUpdate)

    // Clear pipeline state.
    Object.keys(pipelineState).forEach(k => delete pipelineState[k])
    Object.keys(attachState).forEach(k => delete attachState[k])

    isDispatchingEvents_ = false
    events = []
  }

  const produceSessionManagers = (): SessionManagerDefinition[] => {
    const moduleSessionManagers = callbackManager.getNamedCallbacks('sessionManager')
    return [
      moduleSessionManagers,
      {name: 'getusermedia', cb: () => SessionManagerGetUserMedia(xrccWithContextPromise)},
      {name: 'desktop3d', cb: () => SessionManagerDesktop3D()},
    ].flat()
  }

  type InitializeSessionResult = {
    initialized: boolean
    cancelled?: boolean
  }

  const tryInitializeSession = async (
    manager: SessionManagerDefinition
  ): Promise<InitializeSessionResult> => {
    const {name, cb: sessionManagerFactory} = manager
    sessionManager_ = sessionManagerFactory()
    verboseLog(`[XR] Starting session with manager '${name}'`)

    const sessionAttributes = sessionManager_.getSessionAttributes()

    // If the caller has provided a custom run loop (i.e. the camera-loop does not own the)
    // run-loop), it will likely interact poorly with one from a session manager. In this case
    // disable session managers that provide run loops.
    if (sessionAttributes.providesRunLoop && !runConfig.ownRunLoop) {
      verboseLog(
        `[XR] Skipping sessionManager ${name} because external runner also has a loop`
      )
      return {initialized: false}
    }

    try {
      runOnBeforeSessionInitialize(sessionAttributes)
    } catch (err) {
      verboseLog(`[XR] sessionManager ${name} rejected from onBeforeSessionInitialize:`, err)
      return {initialized: false}
    }

    const onSessionInitError = (err) => {
      if (runConfig.sessionInitBehavior === 'fallback') {
        // Don't bubble up the error on init if init is allowed to fallback
        return
      }
      onError(err)
    }

    try {
      await sessionManager_.initialize({
        drawCanvas,
        config: getUserVisibleConfig(),
        runOnCameraStatusChange,
        onError: onSessionInitError,  // undocumented
      })
    } catch (err) {
      verboseError(`[XR] Session manager '${name}' initialization failed`, err)
      return {initialized: false}
    }

    try {
      // NOTE(dat): permissions-helpers only work with motion, orientation, and gps.
      // The next call won't include camera session permission.
      await sessionManager_.obtainPermissions(requiredPermissionsManager)
    } catch (e) {
      if (runConfig.sessionInitBehavior === 'fallback') {
        // Pass on this session and go to the next one
        verboseError(`[XR] Session manager '${name}' obtain permissions failed`, JSON.stringify(e))
        return {initialized: false}
      }

      // Don't allow permission to fail. Consider this a failed run.
      throw e
    }

    const cancelled = await sessionManager_.start()
    if (runConfig.sessionInitBehavior === 'fallback' && cancelled) {
      return {initialized: false}
    }

    return {initialized: true, cancelled: !!cancelled}
  }

  // Returns true if the session initialized but was cancelled partway.
  const initializeValidSession = async (sessionManagers: SessionManagerDefinition[]) => {
    for (let i = 0; i < sessionManagers.length; i++) {
      // eslint-disable-next-line no-await-in-loop
      const result = await tryInitializeSession(sessionManagers[i])
      if (result.initialized) {
        return result.cancelled
      }
    }

    // TODO(pawel) https://github.com/8thwall/code8/pull/20314#pullrequestreview-779162268
    // Consider how to have incompatibility override not apply in onException.
    setIncompatibilityOverride(true)
    throw new Error('No valid session manager to handle this session.')
  }

  const runInternal = async (canvas_: HTMLCanvasElement, runConfig_: RunConfig) => {
    // NOTE: jsxr-run returns early prior to calling this if getUserMedia or WebAssembly are not
    // present in the browser.
    runConfig = runConfig_
    drawCanvas = canvas_
    try {
      createContext()
      const sessionManagers = produceSessionManagers()
      await runOnBeforeRun()

      runOnRunConfigure({config: getUserVisibleConfig()})

      const cancelled = await initializeValidSession(sessionManagers)
      if (!cancelled) {
        await xrccWithContextPromise
        handleFirstFrame()
      }
    } catch (err) {
      onError(err)
    } finally {
      setIncompatibilityOverride(false)
    }
  }

  const run = (canvas_: HTMLCanvasElement, runConfig_: RunConfig) => {
    // NOTE(christoph): XR8.run() historically has not returned a promise
    runInternal(canvas_, runConfig_)
  }

  const pause = () => {
    if (!canPause()) {
      // eslint-disable-next-line no-console
      console.warn('[XR] Pause cannot be called at this time.')
      return
    }
    if (sessionManager_) {
      sessionManager_.pause()
    }
    paused = true
    refreshRunLoop()
    runConfig.paused = true
    if (isResuming) {
      isResuming = false
    } else {
      try {
        runRawCallbacks(callbackManager.getCallbacks('onPaused'))
      } catch (e) {
        onError(e)
      }
    }
  }

  const canResume = () => (isEngaged && paused && !isResuming)

  const resume = () => {
    if (!canResume()) {
      // eslint-disable-next-line no-console
      console.warn('[XR] Resume cannot be called at this time.')
      return
    }

    isResuming = true

    const onResumeComplete = (cancelled) => {
      if (cancelled) {
        return cancelled
      }
      isResuming = false
      paused = false
      runConfig.paused = false
      refreshRunLoop()

      try {
        runRawCallbacks(callbackManager.getCallbacks('onResume'))
      } catch (e) {
        onError(e)
      }

      return cancelled
    }

    // Once we have reset the stream, mark the resume process as complete
    sessionManager_.resume().then(onResumeComplete)
  }

  const maybeFixCameraStall = () => {
    // Set by engine.cc
    const c8 = window._c8
    if (!c8 || !c8.isAllZeroes || !c8.wasEverNonZero) {
      return
    }

    // If we can't pause, we can't fix
    if (!canPause()) {
      return
    }

    pause()
    resume()
  }

  const generateStateHandlerWithCatch = <T extends CallbackName>(callbackName: T) => (
    (data: InputForCallbackName<T>) => {
      if (data && attachState !== data) {
        Object.assign(attachState, data)
      }
      try {
        runRawCallbacks(callbackManager.getCallbacks(callbackName), data)
      } catch (e) {
        onError(e)
      }
    }
  )

  const generateNamedStateHandlerWithCatch = <T extends CallbackName>(callbackName: T) => (
    (data?: WithoutFramework<InputForCallbackName<T>>, moduleName?: string) => {
      if (data && attachState !== data) {
        Object.assign(attachState, data)
      }
      try {
        const callbacksToRun = moduleName
          ? callbackManager.getNamedCallbacks(callbackName).filter(({name}) => moduleName === name)
          : callbackManager.getNamedCallbacks(callbackName)
        if (callbacksToRun.length) {
          runCallbacks(callbacksToRun, data)
        }
      } catch (e) {
        onError(e)
      }
    }
  )

  // runOnBeforeRun has some interesting properties which make it not suitable for
  // generateStateHandlerWithCatch:
  //   * Its callers may return a promise, and all promises must finish before the pipeline
  //     proceeds.
  //   * Errors here should be propagated so that subsequent calls can be blocked.
  const runOnBeforeRun = () => (
    Promise.all(callbackManager.getCallbacks('onBeforeRun')
      .map(cb => cb({config: getUserVisibleConfig()})))
  )

  const runOnRunConfigure = generateStateHandlerWithCatch('onRunConfigure')
  const runOnStart = generateStateHandlerWithCatch('onStart')
  const runOnDeviceOrientationChange = generateStateHandlerWithCatch('onDeviceOrientationChange')
  const runOnCanvasSizeChange = generateStateHandlerWithCatch('onCanvasSizeChange')
  const runOnVideoSizeChange = generateStateHandlerWithCatch('onVideoSizeChange')
  const runOnCameraStatusChange = generateStateHandlerWithCatch('onCameraStatusChange')
  const runOnAppResourcesLoaded = generateNamedStateHandlerWithCatch('onAppResourcesLoaded')
  const runOnRemove = generateNamedStateHandlerWithCatch('onRemove')

  const orientationChangeCallback = () => runOnDeviceOrientationChange({
    GLctx: drawCtx,
    computeCtx,
    videoWidth: nextVideoSize.width,
    videoHeight: nextVideoSize.height,
    orientation: windowOrientation(),
  })

  const isPaused = () => paused

  // 8th Wall camera applications are built using a camera pipeline module framework. Applications
  // install modules which then control the behavior of the application at runtime. A module
  // object must have a .name string which is unique within the application, and then should provide
  // one or more of the camera lifecycle methods.
  //
  // Here's an example camera pipeline module for managing camera permissions:
  //
  // XR8.addCameraPipelineModule({
  //   name: 'camerastartupmodule',
  //   onCameraStatusChange: ({status}) {
  //     if (status == 'requesting') {
  //       myApplication.showCameraPermissionsPrompt()
  //     } else if (status == 'hasStream') {
  //       myApplication.dismissCameraPermissionsPrompt()
  //     } else if (status == 'hasVideo') {
  //       myApplication.startMainApplictation()
  //     } else if (status == 'failed') {
  //       myApplication.promptUserToChangeBrowserSettings()
  //     }
  //   },
  // })
  //
  // During the main runtime of an application, each camera frame goes through an
  //
  // onProcessGpu -> onProcessCpu -> onUpdate
  //
  // cycle. Camera modules that implement onProcessGpu or onProcessCpu can provide data to
  // subsequent stages of the pipeline. This is done by the module's name. For example, a QR code
  // scanning application could be built like this:
  //
  // // Install a module which gets the camera feed as a UInt8Array.
  // XR8.addCameraPipelineModule(
  //   XR8.CameraPixelArray.pipelineModule({luminance: true, width: 240, height: 320}))
  //
  // // Install a module that draws the camera feed to the canvas.
  // XR8.addCameraPipelineModule(XR8.GlTextureRenderer.pipelineModule())
  //
  // // Create our custom application logic for scanning and displaying QR codes.
  // XR8.addCameraPipelineModule({
  //   name: 'qrscan',
  //   onProcessCpu: ({onProcessGpuResult}) => {
  //     // CameraPixelArray.pipelineModule() returned these in onProcessGpu.
  //     const { pixels, rows, cols, rowBytes } = onProcesGpuResult.camerapixelarray
  //     const { wasFound, url, corners } = findQrCode(pixels, rows, cols, rowBytes)
  //     return { wasFound, url, corners }
  //   },
  //   onUpdate: ({onProcessCpuResult}) => {
  //     // These were returned by this module ('qrscan') in onProcessCpu
  //     const {wasFound, url, corners } = onProcessCpuResult.qrscan
  //     if (wasFound) {
  //       showUrlAndCorners(url, corners)
  //     }
  //   },
  // })
  const addCameraPipelineModule = (module: Module) => {
    if (!module.name || typeof (module.name) !== 'string' || module.name === '') {
      throw new Error('module.name must be a non-empty string')
    }

    if (modules.has(module.name)) {
      // eslint-disable-next-line no-console
      console.warn(`[XR] Camera Pipeline Module named ${module.name} was already added; skipping.`)
      return
    }

    // If pipeline is engaged that means it is already running. Because this module did not have a
    // chance to reject the session when XR8.run() was called, give it a chance to reject being
    // added to the current session.
    if (isEngaged && module.onBeforeSessionInitialize) {
      try {
        // attachState already contains sessionAttributes.
        module.onBeforeSessionInitialize(attachState)
      } catch (err) {
        // eslint-disable-next-line no-console
        console.error(`[xr] Failed to add module ${module.name} to pipeline: ${err.toString()}`)
        return
      }
    }

    modules.add(module.name)

    const tracename = module.name.toLowerCase().replace(/\//g, '-')
    const traceWrapper: TodoType = cb => trace8(tracename, cb)
    callbackManager.addModule(module, traceWrapper)

    // listeners is an array of event listeners that handle events dispatched to the framework.
    // Each listener in the array should have the fields:
    //  {
    //    event [string]: the name of the event, in the form ${moduleName}.${eventName}
    //    process [ ({name, detail}) => {} ]: A function that processes the detail of the event.
    //  }
    //
    // For example:
    //  const logEvent = ({name, detail}) => {
    //    console.log(`Handling event ${name}, got detail, ${JSON.stringify(detail)}`)
    //  }
    //
    //  XR8.addCameraPipelineModule({
    //    name: 'eventlogger',
    //    listeners: [
    //      {event: 'reality.imagefound', process: logEvent},
    //      {event: 'reality.imageupdated', process: logEvent},
    //      {event: 'reality.imagelost', process: logEvent},
    //    ],
    //  })
    if (module.listeners) {
      module.listeners.forEach((l) => {
        addEventHandler({eventName: l.event, moduleName: module.name, process: l.process})
      })
    }

    // Modules can indicate what browser capabilities they require that may need permissions
    // requests. These can be used by the framework to request appropriate permissions if absent,
    // or to create components that request the appropriate permissions before running XR.
    requiredPermissionsManager.add(module.name, module.requiredPermissions)

    attachLifecycle.add(module.name)
    sessionAttachLifecycle.add(module.name)
  }

  // Removes a module from the camera pipeline.
  //
  // Input
  //   moduleName: The string name string of a module
  const removeCameraPipelineModule = (moduleName) => {
    if (!modules.has(moduleName)) {
      // eslint-disable-next-line no-console
      console.warn(`[XR] Module "${moduleName}" not found in Camera Pipeline; skipping.`)
      return
    }

    sessionAttachLifecycle.remove(moduleName)
    attachLifecycle.remove(moduleName)

    runOnRemove(undefined, moduleName)

    requiredPermissionsManager.remove(moduleName)

    Object.keys(eventHandlers).forEach(
      (eventName) => {
        eventHandlers[eventName] = eventHandlers[eventName].filter(h => h.moduleName !== moduleName)
      }
    )

    callbackManager.removeModule(moduleName)

    modules.delete(moduleName)
  }

  // Remove multiple camera pipeline modules. This is a convenience method that calls
  // removeCameraPipelineModule in order on each element of the input array.
  //
  // Input
  //   moduleNames: An array of objects with a name property, or a name strings of modules
  const removeCameraPipelineModules = (moduleNames: Array<ModuleName | {name: ModuleName}>) => {
    moduleNames.forEach((moduleName) => {
      removeCameraPipelineModule(typeof moduleName === 'string' ? moduleName : moduleName.name)
    })
  }

  // Remove all camera pipeline modules from the camera loop
  const clearCameraPipelineModules = () => removeCameraPipelineModules(Array.from(modules))

  const dispatchFrameworkEvents = () => {
    if (isDispatchingEvents_) {
      return
    }
    isDispatchingEvents_ = true
    const handleOneEvent = (h: EventHandler, event: unknown) => {
      try {
        h.process(event)
      } catch (e) {
        onError(e)
      }
    }

    // NOTE(christoph): We only dispatch events present before we started dispatching.
    const eventsToDispatch = events
    events = []
    eventsToDispatch.forEach((event) => {
      const handlers = eventHandlers[event.name]
      if (handlers) {
        handlers.forEach(h => handleOneEvent(h, event))
      }
    })

    isDispatchingEvents_ = false
  }

  const addEventHandler = ({eventName, moduleName, process}: EventHandlerRegistration) => {
    let handlers = eventHandlers[eventName]
    if (!handlers) {
      handlers = []
      eventHandlers[eventName] = handlers
    }
    handlers.push({moduleName, process})
  }

  // Add multiple camera pipeline modules. This is a convenience method that calls
  // addCameraPipelineModule in order on each element of the input array.
  //
  // Input
  //   modules: An array of camera pipeline modules.
  const addCameraPipelineModules = (newModules: Module[]) => (
    newModules.forEach(m => addCameraPipelineModule(m))
  )

  const requiredPermissions = () => requiredPermissionsManager.getCopy()

  const reconfigureSession = async (newRunConfig: RunConfig) => {
    if (!isEngaged) {
      throw new Error('[XR8] Cannot reinitialize session at this time.')
    }
    // TODO(christoph): Should we allow reconfigure during pause?
    if (paused) {
      throw new Error('[XR8] Cannot reinitialize session while paused.')
    }

    sessionAttachLifecycle.detach()

    // Since attach is meant to last the duration of a run, we pause callbacks until we set up
    // the next session, but we don't detach the modules.
    attachLifecycle.pause()

    isEngaged = false
    refreshRunLoop()

    sessionManager_.stop()
    sessionManager_ = null

    // TODO(christoph): Merge camera config and such? Or should this be a full replacement?
    Object.assign(runConfig, newRunConfig)

    runOnRunConfigure({config: getUserVisibleConfig()})

    try {
      const sessionManagers = produceSessionManagers()
      const cancelled = await initializeValidSession(sessionManagers)
      if (!cancelled) {
        attachLifecycle.resume()
        sessionAttachLifecycle.attach()
        isEngaged = true
        refreshRunLoop()
      }
    } catch (err) {
      onError(err)
    } finally {
      setIncompatibilityOverride(false)
    }
  }

  return {
    run,
    stop,
    reconfigureSession,
    addCameraPipelineModule,
    addCameraPipelineModules,
    removeCameraPipelineModule,
    removeCameraPipelineModules,
    clearCameraPipelineModules,
    pause,
    resume,
    isPaused,
    onError,
    runPreRender,
    runRender,
    runPostRender,
    trace8,
    requiredPermissions,
    drawTexForComputeTex,
    xrccWithContextPromise,

    // Deprecated
    onInitScene: () => {},  // Deprecated in 9.1
  }
}

export {
  CameraLoop,
}
