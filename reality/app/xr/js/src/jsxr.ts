// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

// Entry point for jsxr package
/* eslint-disable import/no-unresolved */
// @dep(//reality/app/xr/js:xr-js-asm)
import XRCC from 'reality/app/xr/js/xr-js-asm'
import {IS_SIMD, VERSION} from 'reality/app/xr/js/build-constants'
import {objectUrl} from 'reality/app/xr/js/xr-js-asm.blob-worker'
/* eslint-enable import/no-unresolved */

import type {XrccModule} from 'reality/app/xr/js/src/types/xrcc'

import {JSDevice, XrDeviceFactory, getCompatibility} from './js-device'
import {CanvasScreenshotFactory} from './canvas-screenshot'
import {CameraLoop} from './camera-loop'
import {CameraPixelArray} from './camera-providers'
import {LayersControllerFactory} from './layers-controller'
import {FullWindowCanvasFactory} from './full-window-canvas'
import {GLRenderer, GlTextureRenderer} from './gl-renderer'
import {Sumerian} from './xr-sumerian'
import {ThreejsFactory} from './threejs-renderer'
import {CloudStudioThreejsFactory} from './cloudstudio-threejs-renderer'
import {AFrameFactory, registerAFrameXrComponent, registerAFrameXrPrimitive} from './xr-aframe'
import {BabylonjsFactory} from './xr-babylonjs'
import {PlayCanvasFactory} from './xr-playcanvas'
import {XrPermissionsFactory} from './permissions'
import {XrConfigFactory} from './config'
import {setRenameDeprecation, setRemovalDeprecation, setNoopDeprecation} from './deprecate'
import {WorldPointsRendererModule} from './worldpoint-renderer'
import {MediaRecorderFactory} from './media-recorder'
import {YuvPixelsArray} from './yuv-pixels-array'
import {
  UaResult,
  getDeviceInfoBytes, getFixedUAResultWithClientHints,
} from './devices/compatibility'
import {getRendererAsync} from './devices/override'

import {loadSpecificChunk} from './chunk-loader'
import type {ChunkName} from './types/api'

const loadJsxr = async () => {
// Early initialization for emscripten module.
  let isXrccInitialized = false

  const runConfig = {
    verbose: false,  // undocumented on purpose
    render: true,  // undocumented. Is this still used?
    webgl2: true,
    ownRunLoop: true,
    allowFront: false,  // undocumented, unused, deprecated for cameraConfig
    cameraConfig: null,
    glContextConfig: null,
    allowedDevices: null,
    sessionConfiguration: {},
  }

  const log = isError => (s) => {
    if (!runConfig.verbose || /\[tam\]/.test(s) || /trust_region_minimizer/.test(s)) {
      return
    }

    /* eslint-disable no-console */
    if (isError) {
      console.error(s)
    } else {
      console.log(s)
    }
    /* eslint-enable no-console */
  }

  // initXrccDevice is called from within xrccPromise after xrcc is initialized,
  // so xrcc is guaranteed to contain module functions by the time this is run.
  const initXrccDevice = (xrcc: XrccModule, ua: UaResult, extraRendererInfo: string) => {
    // Calculate device info.
    const deviceInfoBuffer = getDeviceInfoBytes(ua, {extraRendererInfo})
    const deviceInfoPtr = xrcc._malloc(deviceInfoBuffer.byteLength)
    xrcc.writeArrayToMemory(new Uint8Array(deviceInfoBuffer), deviceInfoPtr)

    xrcc._c8EmAsm_initDevice()

    xrcc._c8EmAsm_getDeviceModelInfo(
      deviceInfoPtr,
      deviceInfoBuffer.byteLength
    )

    xrcc._free(deviceInfoPtr)
  }

  let loop: ReturnType<typeof CameraLoop> = null
  const onError = (e) => {
    if (loop) {
      loop.onError(e)
    } else {
      console.error(e)  // eslint-disable-line no-console
    }
  }

  const locateFile = (path: string, prefix: string): string => {
  // We cannot use a worker init script that is elsewhere. Redirect to our blobified worker URL.
  // For a singleFile=0 build with pthreads, the list of files we got locateFile on are
  //  * xr-js-asm.wasm
  //  * xr-js-asm.worker.js
    if (path === 'xr-js-asm.worker.js') {
      return objectUrl
    }
    return prefix + path
  }

  const getXrcc = async (): Promise<XrccModule> => {
  // Start getting the UA data while we work on Wasm module compilation
    const [ua, module, extraRendererInfo] = await Promise.all([
      getFixedUAResultWithClientHints(),
      XRCC({
        thisProgram: '8thWallXR',
        print: log(false),
        printErr: log(true),
        onAbort: onError,
        onError,
        /* eslint-enable max-len */
        locateFile,
      }),
      getRendererAsync(),
    ])
    initXrccDevice(module, ua, extraRendererInfo)

    isXrccInitialized = true
    return module
  }

  const xrccPromise = window.WebAssembly
    ? getXrcc()
    : Promise.resolve({} as XrccModule)

  loop = CameraLoop(xrccPromise)

  // Open the camera and start the run loop.
  // Inputs:
  // {
  //   canvas:          The HTML Canvas that the camera feed will be drawn to.
  //   webgl2:          [Optional] If true, use WebGL2 if available, otherwise fallback to WebGL1.
  //                               If false, always use WebGL1.
  //                               Default true.
  //   ownRunLoop:      [Optional] If true, XR should use it's own run loop. If false, you will
  //                               provide your own run loop and be responsible for calling
  //                               runPreRender and runPostRender yourself [Advanced Users only].
  //                               Default true.
  //   cameraConfig:    [Optional] Desired camera to use. Supported values for direction are
  //                               XR8.XrConfig.camera().BACK or XR8.XrConfig.camera().FRONT.
  //                               Default: {direction: XR8.XrConfig.camera().BACK}
  //   glContextConfig: [Optional] The attributes to configure the WebGL canvas context.
  //                               Default: null.
  //   allowedDevices:  [Optional] Specify the class of devices that the pipeline should run on. If
  //                               the current device is not in that class, running will fail prior
  //                               to opening the camera. If allowedDevices is
  //                               XR8.XrConfig.device().ANY, always open the camera. Note that
  //                               world tracking should only be used with
  //                               XR8.XrConfig.device().MOBILE.
  //                               Default: XR8.XrConfig.device().MOBILE.
  //   sessionConfiguration: {     Configure options related to varying types of sessions.
  //                               Default: {}, all fields are optional
  //     disableXrTablet:          Disable the tablet visible in immersive sessions
  //     xrTabletStartsMinimized:   The tablet will start minimized. Default: false
  //     defaultEnvironment: {
  //       disabled:               Disable the default "void space" background. Default: false
  //       floorColor:             Set the floor color.
  //       floorScale:             Shrink or grow the floor texture. Default: 1
  //       floorTexture:           Specify an alternative texture URL for the tiled floor
  //       fogIntensity:           Increase or decrease fog density. Default: 1
  //       skyBottomColor:         Set the color of the sky at the horizon.
  //       skyTopColor:            Set the color of the sky directly above the user.
  //       skyGradientStrength:    Control how sharply the sky gradient transitions. Default: 1
  //   }
  // }
  const run = ({
    canvas,
    verbose = false,  // undocumented on purpose
    render = true,  // undocumented. Is this still used?
    webgl2 = true,
    ownRunLoop = true,
    allowFront = false,  // undocumented, unused, deprecated for cameraConfig
    cameraConfig = null,
    glContextConfig = null,
    allowedDevices = null,
    sessionConfiguration = {},
  }) => {
    try {
      if (!window.WebAssembly) {
        throw new Error('WebAssembly is not supported by this device\'s browser')
      }

      Object.assign(runConfig, {
        verbose,
        render,
        webgl2,
        ownRunLoop,
        allowFront,
        cameraConfig,
        glContextConfig,
        allowedDevices,
        paused: false,
        sessionConfiguration,
      })

      loop.run(canvas, runConfig)
    } catch (e) {
      loop.onError(e)
    }
  }

  const jsxr = {
    run,
    async loadChunk(chunkName: ChunkName) {
      await loadSpecificChunk(this, runConfig,
        loop.xrccWithContextPromise, chunkName, log, onError)
    },
    version: () => VERSION,
    featureFlags: () => {
    // A purposefully un-documented method for determining what features are present on this build.
      const flags = []
      if (IS_SIMD) {
        flags.push('s')
      }
      return flags
    },

    // Core functionality pipeline modules.
    CanvasScreenshot: CanvasScreenshotFactory(),  // detachable
    MediaRecorder: MediaRecorderFactory(),        // detachable
    YuvPixelsArray,                               // detachable
    CameraPixelArray,                             // detachable
    GlTextureRenderer,                            // detachable
    FaceController: null,                         // detachable
    LayersController: LayersControllerFactory(runConfig,
      loop.xrccWithContextPromise),               // detachable
    XrController: null,                           // detachable

    // Rendering system pipeline modules.
    AFrame: AFrameFactory(loop.xrccWithContextPromise),  // detachable
    Babylonjs: BabylonjsFactory(),                       // detachable
    PlayCanvas: PlayCanvasFactory(),                     // not detachable
    Sumerian,                                            // not detachable
    Threejs: ThreejsFactory(),                           // detachable
    // detachable
    CloudStudioThreejs: CloudStudioThreejsFactory(),

    // Static utilities that are no modules.
    XrConfig: XrConfigFactory(),            // no pipeline
    XrDevice: XrDeviceFactory(),            // no pipeline
    XrPermissions: XrPermissionsFactory(),  // no pipeline

    // Check the status of web assembly loading.
    initialize: () => xrccPromise.then(() => {}),
    isInitialized: () => isXrccInitialized,

    // Methods to drive the pipeline.
    pause: loop.pause,
    resume: loop.resume,
    isPaused: loop.isPaused,
    stop: loop.stop,
    // TODO(christoph): Re-enable after adding support to the pipeline modules
    reconfigureSession: loop.reconfigureSession,
    addCameraPipelineModule: loop.addCameraPipelineModule,
    addCameraPipelineModules: loop.addCameraPipelineModules,
    removeCameraPipelineModule: loop.removeCameraPipelineModule,
    removeCameraPipelineModules: loop.removeCameraPipelineModules,
    clearCameraPipelineModules: loop.clearCameraPipelineModules,
    runPreRender: loop.runPreRender,
    runRender: loop.runRender,
    runPostRender: loop.runPostRender,
    requiredPermissions: loop.requiredPermissions,
    drawTexForComputeTex: loop.drawTexForComputeTex,
  }

  // If AFrame javascript is loaded before us, automatically register the component. This allows
  // it to be embedded directly into the html body.  If we can't load aframe before jsxr, then
  // the register method is provided as an explicit call.
  const registerAFrame = () => {
    registerAFrameXrComponent()
    registerAFrameXrPrimitive()
  }
  if (window.AFRAME) {
    registerAFrame()
  }

  setRenameDeprecation(jsxr, 'GLRenderer', 'XR8.GlTextureRenderer.pipelineModule', GLRenderer)
  setRenameDeprecation(jsxr, 'ThreejsRenderer', 'XR8.Threejs', jsxr.Threejs.ThreejsRenderer)
  setRenameDeprecation(
    jsxr, 'canvasScreenshot', 'XR8.CanvasScreenshot', jsxr.CanvasScreenshot.canvasScreenshot
  )

  setRenameDeprecation(jsxr, 'CameraPixelArrayModule', 'XR8.CameraPixelArray.pipelineModule',
    jsxr.CameraPixelArray.pipelineModule)

  setRenameDeprecation(jsxr, 'getGLctxParameters', 'XR8.GlTextureRenderer.getGLctxParameters',
    jsxr.GlTextureRenderer.getGLctxParameters)

  setRenameDeprecation(jsxr, 'setGLctxParameters', 'XR8.GlTextureRenderer.setGLctxParameters',
    jsxr.GlTextureRenderer.setGLctxParameters)

  setRenameDeprecation(jsxr, 'isDeviceBrowserSupported', 'XR8.XrDevice.isDeviceBrowserCompatible',
    jsxr.XrDevice.isDeviceBrowserCompatible)

  setRenameDeprecation(
    jsxr, 'FullWindowCanvas', 'XRExtras.FullWindowCanvas', FullWindowCanvasFactory(), 'R13.1'
  )

  setRemovalDeprecation(jsxr, 'getCompatibility', getCompatibility, 'XR8.XrDevice')
  setRemovalDeprecation(jsxr, 'JSDevice', JSDevice, 'XR8.XrDevice')
  setRemovalDeprecation(jsxr, 'WorldPointsRendererModule', WorldPointsRendererModule)
  setRemovalDeprecation(jsxr, 'trace8', loop.trace8)

  setRemovalDeprecation(jsxr, 'registerAFrameXrComponent', registerAFrame,
    'AFRAME.registerComponent(\'xrweb\', XR8.AFrame.xrwebComponent())')

  setNoopDeprecation(jsxr, 'onInitScene')
  setNoopDeprecation(jsxr, 'exportTraces')
  setNoopDeprecation(jsxr, 'displayTraces')
  setNoopDeprecation(jsxr, 'logSummary')
  setNoopDeprecation(jsxr, 'execute')

  return jsxr
}

export {
  loadJsxr,
}
