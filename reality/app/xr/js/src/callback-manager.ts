// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Christoph Bartschat (christoph@8thwall.com)
//
// Manages lists of module callbacks
//
// Example usage:
//   const myCallbackManager = createCallbackManager()
//   myCallbackManager.addModule(myModule)
//   myCallbackManager.getCallbacks('onCallbackName').forEach(cb => ...)
//   myCallbackManager.getNamedCallbacks('onCallbackName').forEach({cb, name} => ...)

import type {CallbackName, CallbackNameToHandler} from './types/callbacks'
import type {Module} from './types/module'
import type {ModuleName} from './types/pipeline'

const CALLBACK_NAMES: Readonly<CallbackName[]> = [
  // onBeforeRun is called immediately after XR8.onRun is called. If any promises are returned, XR
  // will wait on all promises before continuing.
  // Inputs:
  // {
  //   config: The configuration parameters that were passed to XR8.run()
  // }
  'onBeforeRun',
  // onRunConfigure is called after onBeforeRun.
  // It is the preferred way for components to record the current configuration of the pipeline.
  // Inputs:
  // {
  //   config: The configuration parameters passed to XR8.run() or XR8.reconfigureSession()
  // }
  'onRunConfigure',
  // onBeforeSessionInitialize is called before initializing each session manager.
  // If the module throws from the callback, the session manager will be rejected, skipping the
  // initialization and continuing to the next session manger.
  // Inputs:
  // {
  //   config: The configuration parameters that were passed to XR8.run()
  //   sessionAttributes: An object containing arbitrary flags returned by the pending session
  //     manager, but by convention should include:
  //   {
  //     cameraLinkedToViewer: The camera position is aligned to the user's head or eyes.
  //     controlsCamera: The renderer's camera position is controlled by the session manager.
  //     fillsCameraTexture: The session manager will include cameraTexture in frameStartResult.
  //     providesRunLoop: The session manager cannot accept an external run loop.
  //     supportsHtmlEmbedded: HTML can be embedded within a 3D scene.
  //     supportsHtmlOverlay: HTML can be displayed directly on top of 3D content.
  //     usesMediaDevices: Uses the MediaDevices device api to open a MediaStream.
  //     usesWebXr: Uses the WebXR device api to open a WebXR session.
  //   }
  //  }
  'onBeforeSessionInitialize',
  // onStart is called when XR starts.
  // Called with an object containing information such as canvas, WebGL context, and video size.
  // Inputs:
  // {
  //   canvas: The canvas that backs GPU processing and user display.
  //   GLctx: The drawing canvas's WebGLRenderingContext or WebGL2RenderingContext.
  //   computeCtx: The compute canvas's WebGLRenderingContext or WebGL2RenderingContext.
  //   isWebgl2: True if GLCtx is a WebGL2RenderingContext.
  //   orientation: The rotation of the ui from portrait, in degrees (-90, 0, 90, 180).
  //   videoWidth: The height of the camera feed, in pixels.
  //   videoHeight: The height of the camera feed, in pixels.
  //   canvasWidth: The width of the GLctx canvas, in pixels.
  //   canvasHeight: The height of the GLctx canvas, in pixels.
  //   config: The configuration parameters that were passed to XR8.run()
  // }
  'onStart',
  // onProcessGpu is called to start GPU processing.
  // Called with {framework, frameStartResult}
  // Inputs:
  // framework: {
  //   dispatchEvent(eventName, detail): Emits a named event with the supplied detail.
  // }
  // frameStartResult: {
  //    cameraTexture: The drawing canvas's WebGLTexture containing camera feed data.
  //    computeTexture: The compute canvas's WebGLTexture containing camera feed data.
  //    GLctx:  The drawing canvas's WebGLRenderingContext or WebGL2RenderingContext.
  //    computeCtx:  The compute canvas's WebGLRenderingContext or WebGL2RenderingContext.
  //    textureWidth: The width (in pixels) of the camera feed texture.
  //    textureHeight: The height (in pixels) of the camera feed texture.
  //    orientation: The rotation of the ui from portrait, in degrees (-90, 0, 90, 180).
  //    videoTime: The timestamp of this video frame in seconds.
  //    frameTime: The timestamp when this frame was read in seconds.
  //    repeatFrame: True if the camera feed has not updated since the last call.
  //  }
  'onProcessGpu',
  // onProcessCpu is called to read results of GPU processing and return usable data.
  // Called with {framework, frameStartResult, processGpuResult}.  Data returned by modules in
  // onProcessGpu will be present as processGpu.modulename where the name is given by
  // module.name = "modulename".
  // Inputs:
  // {
  //   framework: The framework bindings for this module for dispatching events.
  //   frameStartResult: The data that was provided at the beginning of a frame.
  //   processGpuResult: Data returned by all installed modules during onProcessGpu.
  // }
  'onProcessCpu',
  // onUpdate is called to update the scene before render.
  // Called with {framework, frameStartResult, processGpuResult, processCpuResult}. Data returned
  // by modules in onProcessGpu and onProcessCpu will be present as processGpu.modulename and
  // processCpu.modulename where the name is given by module.name = "modulename".
  // Inputs:
  // {
  //   framework: The framework bindings for this module for dispatching events.
  //   frameStartResult: The data that was provided at the beginning of a frame.
  //   processGpuResult: Data returned by all installed modules during onProcessGpu.
  //   processCpuResult: Data returned by all installed modules during onProcessCpu.
  // }
  'onUpdate',
  // onRender is called after onUpdate. This is the time for the rendering engine to issue any
  // WebGL drawing commands. If an application is providing its own run loop and is relying on
  // runPreRender and runPostRender, this method is not called and all rendering must be
  // coordinated by the external run loop.
  'onRender',
  // onPaused is called when the camera loop is paused by calling XR8.pause().
  'onPaused',
  // onResume is called when the camera loop is paused by calling XR8.resume().
  'onResume',
  // onException is called when an error occurs in XR.
  // Called with the error object.
  'onException',
  // onDeviceOrientationChange is called when the device changes landscape/portrait orientation.
  // Called with new orientation of the device.
  // {
  //   GLctx: The drawing canvas's WebGLRenderingContext or WebGL2RenderingContext.
  //   computeCtx: The compute canvas's WebGLRenderingContext or WebGL2RenderingContext.
  //   videoWidth: the width of the camera feed, in pixels.
  //   videoHeight: the height of the camera feed, in pixels.
  //   orientation: the rotation of the ui from portrait, in degrees (-90, 0, 90, 180).
  // }
  'onDeviceOrientationChange',
  // onCanvasSizeChange is called when the canvas changes size.
  // Called with dimensions of video and canvas.
  // Inputs:
  // {
  //   GLctx: The drawing canvas's WebGLRenderingContext or WebGL2RenderingContext.
  //   computeCtx: The compute canvas's WebGLRenderingContext or WebGL2RenderingContext.
  //   videoWidth: the width of the camera feed, in pixels.
  //   videoHeight: the height of the camera feed, in pixels.
  //   canvasWidth: the width of the GLctx canvas, in pixels.
  //   canvasHeight: the height of the GLctx canvas, in pixels.
  // }
  'onCanvasSizeChange',
  // onVideoSizeChange is called when the canvas changes size.
  // Called with dimensions of video and canvas as well as device orientation.
  // Inputs:
  // {
  //   GLctx: The drawing canvas's WebGLRenderingContext or WebGL2RenderingContext.
  //   computeCtx: The compute canvas's WebGLRenderingContext or WebGL2RenderingContext.
  //   videoWidth: the width of the camera feed, in pixels.
  //   videoHeight: the height of the camera feed, in pixels.
  //   canvasWidth: the width of the GLctx canvas, in pixels.
  //   canvasHeight: the height of the GLctx canvas, in pixels.
  //   orientation: the rotation of the ui from portrait, in degrees (-90, 0, 90, 180).
  // }
  'onVideoSizeChange',
  // onCameraStatusChange is called when a change occurs during the camera permissions request.
  // Called with the status, and, if applicable, a reference to the newly available data. The
  // typical status flow will be requesting -> hasStream -> hasVideo.
  //
  // * In 'requesting', the browser is opening the camera, and if applicable, checking the user
  //   permissions. In this state, it is appropriate to display a prompt to the user to accept
  //   camera permissions.
  // * Once the user permissions are granted and the camera is successfully opened, the status
  //   switches to 'hasStream' and any user prompts regarding permissions can be dismissed.
  // * Once camera frame data starts to be available for processing, the status switches to
  //   'hasVideo', and the camera feed can begin displaying.
  //
  // If the camera feed fails to open, the status is 'failed'. In this case it's possible that the
  // user has denied permissions, and so helping them to re-enable permissions is advisable.
  // The 'reason' field might contain more information to specialize your message.
  //
  // Inputs:
  // {
  //   status: one of ['requesting', 'hasStream', hasVideo', 'failed']
  //   reason: one of ['UNSPECIFIED', 'NO_CAMERA', 'DENY_CAMERA', 'NO_MICROPHONE', 'DENY_MICROPHONE']
  //   stream: [Optional] The MediaStream associated with the camera feed, if status is hasStream.
  //   video: [Optional] The video dom element displaying the stream, if status is hasVideo.
  //   sessionType: [Optional] The type of immersive-xr session that was started, if the session is
  //                an XRSession (https://developer.mozilla.org/en-US/docs/Web/API/XRSession).
  //   config: The configuration parameters that were passed to XR8.run(), if status is 'requesting'
  // }
  'onCameraStatusChange',
  // onAppResourcesLoaded is called when we have received the resources attached to an app from
  // the server.
  //
  // Inputs:
  // {
  //   framework: The framework bindings for this module for dispatching events.
  //   version: The engine version, e.g. 10.1.7.520.
  //   imageTargets: [Optional] An array of image targets with the fields
  //       {imagePath, metadata, name}.
  // }
  'onAppResourcesLoaded',
  // onAttach is called before the first time a module receives frame updates. It is called
  // on modules that were added either before or after the pipeline is running. It includes
  // all the most recent data available from
  //
  // * onStart
  // * onDeviceOrientationChange
  // * onCanvasSizeChange
  // * onVideoSizeChange
  // * onCameraStatusChange
  // * onAppResourcesLoaded
  //
  // Inputs:
  // {
  //   framework: The framework bindings for this module for dispatching events.
  //   canvas: The canvas that backs GPU processing and user display.
  //   GLctx: The drawing canvas's WebGLRenderingContext or WebGL2RenderingContext.
  //   computeCtx: The compute canvas's WebGLRenderingContext or WebGL2RenderingContext.
  //   isWebgl2: True if GLCtx is a WebGL2RenderingContext.
  //   orientation: The rotation of the ui from portrait, in degrees (-90, 0, 90, 180).
  //   videoWidth: The height of the camera feed, in pixels.
  //   videoHeight: The height of the camera feed, in pixels.
  //   canvasWidth: The width of the GLctx canvas, in pixels.
  //   canvasHeight: The height of the GLctx canvas, in pixels.
  //   status: one of ['requesting', 'hasStream', hasVideo', 'failed']
  //   stream: The MediaStream associated with the camera feed.
  //   video: The video dom element displaying the stream.
  //   version: [Optional] The engine version, e.g. 10.1.7.520, if app resources are loaded.
  //   imageTargets: [Optional] An array of image targets with the fields
  //       {imagePath, metadata, name} if an app has image targets and if app resources are loaded.
  //   config: The configuration parameters that were passed to XR8.run()
  //   sessionAttributes: An object containing arbitrary flags returned by the current session
  //     manager, but by convention should include:
  //   sessionType: [Optional] The type of immersive-xr session that was started, if the session is
  //                an XRSession (https://developer.mozilla.org/en-US/docs/Web/API/XRSession).
  //   {
  //     cameraLinkedToViewer: The camera position is aligned to the user's head or eyes.
  //     controlsCamera: The renderer's camera position is controlled by the session manager.
  //     fillsCameraTexture: The session manager will include cameraTexture in frameStartResult.
  //     providesRunLoop: The session manager cannot accept an external run loop.
  //     supportsHtmlEmbedded: HTML can be embedded within a 3D scene.
  //     supportsHtmlOverlay: HTML can be displayed directly on top of 3D content.
  //     usesMediaDevices: Uses the MediaDevices device api to open a MediaStream.
  //     usesWebXr: Uses the WebXR device api to open a WebXR session.
  //   }
  // }
  'onAttach',
  // onSessionAttach is like onAttach but re-runs when a new session starts or reconfigures
  'onSessionAttach',
  // onDetach is called after the last time a module receives frame updates. This is either after
  // stop is called, or after the module is manually removed from the pipeline.
  //
  // Inputs:
  // {
  //   framework: The framework bindings for this module for dispatching events.
  // }
  'onDetach',
  // onSessionDetach is like onDetach but re-runs when a session gets reconfigured
  'onSessionDetach',
  // onRemove is called just before a module will be removed.
  //
  // Inputs:
  // {
  //   framework: The framework bindings for this module for dispatching events.
  // }
  'onRemove',
  // A module can provide an alternative implementation of a camera runtime session. This is
  // responsible for starting the sensor runtime stack and providing frame data at the start of
  // each frame. Session managers should provide the following functionality:
  // {
  //   initialize({drawCanvas, config, runOnCameraStatusChange}): Returns a promise that resolves
  //     if the session manager can handle the requested config, or rejects if the session manager
  //     cannot handle the requested config. During `startSession` and `resume`,
  //     runOnCameraStatusChange({status: 'currentStatus', ...}) should be called to inform other
  //     pipeline modules of the session startup status.
  //   obtainPermissions(): Returns a promise that resolves when the user has provided necessary
  //     permissions for the session, or rejects if permissions cannot be obtained.
  //   start(): Returns a promise that resolves when the session is started and the session manager
  //     is ready to provide per-frame data.
  //   pause(): Pause the session and halt updates.
  //   resume(): Returns a promise that resolves when the session is resumed.
  //   hasNewFrame(): Returns {repeatFrame: true/false, videoSize: {width: w, height: h}} with info
  //     about the next frame that will be returned on the next call to `frameStartResult`. If the
  //     underlying device has not produced new data since the last call to frameStartResult,
  //     newFrame should be false. It's recommended to provide non-null, non-zero values for
  //     videoSize since some existing flows may assume that there is always a camera texture with
  //     a non-trivial size.
  //   frameStartResult({repeatFrame, drawCtx, computeCtx, drawTexture, computeTexture}): If the
  //     session manager is providing camera texture data, it should fill both drawTexture and
  //     computeTexture with the frame data. The session manager should then return any data that
  //     will be provided to all pipeline modules in the `frameStartResult` object.
  //   stop(): Stop the current session.
  //   getSessionAttributes(): Returns an object containing arbitrary flags returned by the current
  //     session manager, but by convention should include:
  //       {
  //         cameraLinkedToViewer: The camera position is aligned to the user's head or eyes.
  //         controlsCamera: The renderer's camera position is controlled by the session manager.
  //         fillsCameraTexture: The session manager will include cameraTexture in frameStartResult.
  //         providesRunLoop: The session manager cannot accept an external run loop.
  //         supportsHtmlEmbedded: HTML can be embedded within a 3D scene.
  //         supportsHtmlOverlay: HTML can be displayed directly on top of 3D content.
  //         usesMediaDevices: Uses the MediaDevices device api to open a MediaStream.
  //         usesWebXr: Uses the WebXR device api to open a WebXR session.
  //       }
  // }
  'sessionManager',
]

// If a wrapper function is provided to addModule, only the callbacks listed here will have their
// functions wrapped.
const WRAPPED_CALLBACK = {
  onProcessGpu: true,
  onProcessCpu: true,
  onUpdate: true,
  onRender: true,
} as const

const DEPRECATED_NAME_MAP: Partial<Record<CallbackName, string[]>> = {
  onProcessGpu: ['onCameraTextureReady'],
  onProcessCpu: ['onHeapDataReady'],
  onUpdate: ['onUpdateWithResults'],
}

type Wrapper = <T>(cb: T) => T

type NamedCallback<N extends CallbackName> = {name: ModuleName, cb: CallbackNameToHandler[N]}

type WrappedNamedCallback<N extends CallbackName> = {
  deactivate: () => void
  ncb: NamedCallback<N>
}

type NamedCallbackMap = Partial<{
  [K in CallbackName]: Array<WrappedNamedCallback<K>>
}>

const createCallbackManager = () => {
  const namedCallbackMap_: NamedCallbackMap = {}

  const getModuleCallback = <T extends CallbackName>(
    module: Module, callbackName: T
  ): CallbackNameToHandler[typeof callbackName] => {
    // @ts-ignore
    let callback: CallbackNameToHandler[typeof callbackName] | undefined = module[callbackName]
    if (callback) {
      return callback
    }

    const deprecatedNameList = DEPRECATED_NAME_MAP[callbackName]
    if (!deprecatedNameList) {
      return null
    }

    for (let i = 0; i < deprecatedNameList.length; i++) {
      callback = module[deprecatedNameList[i]]
      if (callback) {
        // eslint-disable-next-line no-console
        console.warn(
          `[XR] Module '${module.name}' uses deprecated callback name. ` +
          `'${deprecatedNameList[i]}' has been renamed to '${callbackName}'. See docs.8thwall.com.`
        )
        return callback
      }
    }

    return null
  }

  const genNamedCallback = <T extends CallbackName>(
    name: ModuleName, cb: CallbackNameToHandler[T]
  ) => {
    let active = true
    return {
      ncb: {
        name,
        cb: (...args: any[]) => (active ? cb(...args as [any]) : undefined),
      },
      deactivate: () => { active = false },
    } as WrappedNamedCallback<T>
  }

  const addModuleCallback = <T extends CallbackName>(
    module: Module, callbackName: T, wrapper: Wrapper
  ) => {
    let callback = getModuleCallback(module, callbackName)
    if (!callback) {
      return
    }

    let namedCallbackList = namedCallbackMap_[callbackName]

    if (!namedCallbackList) {
      namedCallbackList = []
      namedCallbackMap_[callbackName] = namedCallbackList
    }

    if (wrapper) {
      callback = wrapper(callback)
    }

    namedCallbackList.push(genNamedCallback(module.name, callback))
  }

  // Adds module callbacks to callback lists
  const addModule = (module: Module, wrapper?: Wrapper) => CALLBACK_NAMES.forEach(callbackName => (
    addModuleCallback(module, callbackName, WRAPPED_CALLBACK[callbackName] && wrapper)
  ))

  // Gets list of relevant callbacks in the form {cb, name}
  const getNamedCallbacks = <T extends CallbackName>(callbackName: T): NamedCallback<T>[] => {
    if (!namedCallbackMap_[callbackName]) {
      namedCallbackMap_[callbackName] = []
    }
    return namedCallbackMap_[callbackName].map(({ncb}) => ncb)
  }

  // Gets list of relevant callback functions
  const getCallbacks = <T extends CallbackName>(callbackName: T) => (
    getNamedCallbacks(callbackName).map(e => e.cb)
  )

  const removeModuleCallback = (moduleName: ModuleName, callbackName: CallbackName) => {
    const namedCallbackList = namedCallbackMap_[callbackName]
    if (!namedCallbackList) {
      return
    }
    const callbackIndex = namedCallbackList.findIndex(e => e.ncb.name === moduleName)
    if (callbackIndex === -1) {
      return
    }
    namedCallbackList[callbackIndex].deactivate()
    namedCallbackList.splice(callbackIndex, 1)
  }

  // Removes module callbacks from callback lists
  // Takes the module name as an argument
  const removeModule = (moduleName: ModuleName) => {
    CALLBACK_NAMES.forEach(callbackName => removeModuleCallback(moduleName, callbackName))
  }

  return {
    addModule,
    getCallbacks,
    getNamedCallbacks,
    removeModule,
  }
}

export {
  createCallbackManager,
}
