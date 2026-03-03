// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)

/* global XR8:readonly, sumerian:readonly */

import {create8wCanvas} from './canvas'

const EngineKind = {
  XR: 0,
  FACE: 1,
}

function Quaternion(w, x, y, z) {
  this.w = w
  this.x = x
  this.y = y
  this.z = z

  this.toRotationMatrix = function (out) {
    const wx = w * x
    const wy = w * y
    const wz = w * z
    const xx = x * x
    const xy = x * y
    const xz = x * z
    const yy = y * y
    const yz = y * z
    const zz = z * z

    const mm00 = (1.0 - 2.0 * (yy + zz))
    const mm01 = (2.0000000 * (xy - wz))
    const mm02 = (2.0000000 * (xz + wy))

    const mm10 = (2.0000000 * (xy + wz))
    const mm11 = (1.0 - 2.0 * (xx + zz))
    const mm12 = (2.0000000 * (yz - wx))

    const mm20 = (2.0000000 * (xz - wy))
    const mm21 = (2.0000000 * (yz + wx))
    const mm22 = (1.0 - 2.0 * (xx + yy))

    const rmat = [[mm00, mm01, mm02, 0.0],  // Row 0
      [mm10, mm11, mm12, 0.0],  // Row 1
      [mm20, mm21, mm22, 0.0],  // Row 2
      [0.0, 0.0, 0.0, 1.0]]  // Row 3

    for (let i = 0; i < 4; ++i) {
      for (let j = 0; j < 4; ++j) {
        out[i + 4 * j] = rmat[i][j]
      }
    }
  }
}

const xrWebSystem = (runConfig, engineKind) => {
  const state = {
    cameraEntity: null,
    cameraFeedRenderer: null,
    captureSize: {w: 300, h: 150},
    displaySize: {w: 300, h: 150},
    facing: {qw: 0, qx: 0, qy: 0, qz: 0},
    firstFrame: true,
    origin: {x: 0, y: 1, z: 0},
    realityDidInitialize: false,
    videoViewport: {offsetX: 0, offsetY: 0, width: 0, height: 0},
    engineKind,
  }

  const setupXRCanvas = () => {
    const arview = document.createElement('div')
    arview.id = 'arview'
    arview.setAttribute('style', 'position:absolute; top: 0; left: 0; bottom: 0; right: 0; z-index: -1;')
    const arcanvas = create8wCanvas('overlayView3d')
    arcanvas.id = 'overlayView3d'
    arview.appendChild(arcanvas)
    document.body.appendChild(arview)
  }

  const updateSumerian = (rot, pos, intrinsics) => {
    const q = new Quaternion(rot.w, rot.x, rot.y, rot.z)

    const extrinsic = []
    q.toRotationMatrix(extrinsic)

    extrinsic[0 + 3 * 4] = pos.x
    extrinsic[1 + 3 * 4] = pos.y
    extrinsic[2 + 3 * 4] = pos.z

    const extrinsicMat = new window.sumerian.Matrix4(extrinsic)
    const projectionMatrix = new window.sumerian.Matrix4(intrinsics)
    const posVec = new sumerian.Vector3()
    const rotMat = new sumerian.Matrix3()
    extrinsicMat.getRotation(rotMat)
    extrinsicMat.getTranslation(posVec)

    if (state.cameraEntity) {
      state.cameraEntity.transformComponent.setTranslation(posVec)
      state.cameraEntity.transformComponent.setRotationMatrix(rotMat)
      state.cameraEntity.transformComponent.sync()
      state.cameraEntity.cameraComponent.updateCamera(state.cameraEntity.transformComponent.transform)
      state.cameraEntity.cameraComponent.camera.setProjectionMatrixPerspective(projectionMatrix)
    }
  }

  const recenterCallback = (params) => {
    if (params && (params.origin || params.rotation)) {
      const {origin} = params
      const facing = params.rotation
      if (origin) {
        state.origin = {x: origin.x, y: origin.y, z: origin.z}
      }
      if (facing) {
        state.facing = {
          w: facing.qw || facing.w,
          x: facing.qx || facing.x,
          y: facing.qy || facing.y,
          z: facing.qz || facing.z,
        }
      }

      if (state.engineKind === EngineKind.XR) {
        XR8.XrController.updateCameraProjectionMatrix({
          origin: state.origin,
          facing: state.facing,
        })
      } else {
        XR8.FaceController.configure({
          coordinates: {
            origin: {
              position: state.origin,
              rotation: state.facing,
            },
          },
        })
      }
    }

    if (state.engineKind === EngineKind.XR) {
      XR8.XrController.recenter()
    }
  }

  const onScreenshotRequested = (onScreenshotReady) => {
    const sumerianCanvas = document.getElementById('sumerian')
    XR8.CanvasScreenshot.setForegroundCanvas(sumerianCanvas)
    XR8.CanvasScreenshot.takeScreenshot().then(
      (data) => {
        window.sumerian.SystemBus.emit('screenshotready', data)

        // Deprecated API asked for a callback to be provided at screenshot request time.
        if (onScreenshotReady) {
          onScreenshotReady(data)
        }
      },
      (error) => {
        window.sumerian.SystemBus.emit('screenshoterror', error)

        // Apps on deprecated API are unaware of error API. Send them an empty data buffer.
        if (onScreenshotReady) {
          onScreenshotReady('')
        }
      }
    )
  }

  const setupSumerianXR = () => {
    setupXRCanvas()

    const realityEventEmitter = (event) => {
      if (event.name.startsWith('reality.')) {
        window.sumerian.SystemBus.emit(`xr${event.name.substring('reality.'.length)}`, event.detail)
      }
      if (event.name.startsWith('facecontroller.')) {
        window.sumerian.SystemBus.emit(`xr${event.name.substring('facecontroller.'.length)}`, event.detail)
      }
    }

    const canvas = document.getElementById('overlayView3d')
    XR8.addCameraPipelineModule(XR8.CanvasScreenshot.pipelineModule())

    if (state.engineKind === EngineKind.XR) {
      XR8.addCameraPipelineModule(XR8.XrController.pipelineModule())
    } else {
      XR8.addCameraPipelineModule(XR8.FaceController.pipelineModule())
    }

    XR8.addCameraPipelineModule({
      name: 'sumerianjs',
      onUpdate: ({frameStartResult, processCpuResult}) => {
        const sumerianCanvas = document.getElementById('sumerian')
        if (
          sumerianCanvas && sumerianCanvas.width && sumerianCanvas.height &&
          (canvas.width != sumerianCanvas.width || canvas.height != sumerianCanvas.height)) {
          canvas.width = sumerianCanvas.width
          canvas.height = sumerianCanvas.height
        }

        const {reality, facecontroller} = processCpuResult
        if (!reality && !facecontroller) {
          return
        }

        if (state.firstFrame) {
          state.firstFrame = false
          window.sumerian.SystemBus.emit('xrready')
        }

        const {
          textureWidth: videoWidth,
          textureHeight: videoHeight,
        } = frameStartResult

        maybeUpdateCameraParams({videoWidth, videoHeight, canvasWidth: canvas.width, canvasHeight: canvas.height})

        const {rotation, position, intrinsics} = reality || facecontroller
        updateSumerian(rotation, position, intrinsics)

        const realityTexture =
        (reality && reality.realityTexture) || (facecontroller && facecontroller.cameraFeedTexture)

        if (realityTexture) {
          state.cameraFeedRenderer.render({
            renderTexture: realityTexture,
            viewport: state.videoViewport,
          })
        }
      },
      onStart: onXRStart,
      onCameraStatusChange: (data) => {
        window.sumerian.SystemBus.emit('camerastatuschange', data)
      },
      onException: (e) => {
        window.sumerian.SystemBus.emit('xrerror', {
          compatibility: XR8.getCompatibility(),
          error: e,
          isDeviceBrowserSupported: XR8.XrDevice.isDeviceBrowserCompatible(runConfig),
        })
      },
      listeners: [
        {event: 'reality.imageloading', process: realityEventEmitter},
        {event: 'reality.imagescanning', process: realityEventEmitter},
        {event: 'reality.imagefound', process: realityEventEmitter},
        {event: 'reality.imageupdated', process: realityEventEmitter},
        {event: 'reality.imagelost', process: realityEventEmitter},
        {event: 'reality.trackingstatus', process: realityEventEmitter},
        {event: 'facecontroller.faceloading', process: realityEventEmitter},
        {event: 'facecontroller.facescanning', process: realityEventEmitter},
        {event: 'facecontroller.facefound', process: realityEventEmitter},
        {event: 'facecontroller.faceupdated', process: realityEventEmitter},
        {event: 'facecontroller.facelost', process: realityEventEmitter},
      ],
    })

    window.sumerian.SystemBus.addListener('recenter', recenterCallback)
    window.sumerian.SystemBus.addListener('screenshotrequest', onScreenshotRequested)

    /**
     * Deprecated after 10.1 in favor for 'screenshotrequest', which is consistent with
     * our screenshot API on A-Frame.
     */
    window.sumerian.SystemBus.addListener('requestscreenshot', onScreenshotRequested)
  }

  const onXRStart = ({videoWidth, videoHeight, canvasWidth, canvasHeight}) => {
    state.realityDidInitialize = true
    if (state.cameraEntity) {
      const {camera} = state.cameraEntity.cameraComponent
      state.origin.x = camera.translation.x
      state.origin.y = camera.translation.y
      state.origin.z = camera.translation.z
    }
    updateCameraParams({videoWidth, videoHeight, canvasWidth, canvasHeight})
    state.facing = {w: 1, x: 0, y: 0, z: 0}

    if (state.engineKind === EngineKind.XR) {
      XR8.XrController.updateCameraProjectionMatrix({
        cam: {
          pixelRectWidth: canvasWidth,
          pixelRectHeight: canvasHeight,
          nearClipPlane: 0.01,
          farClipPlane: 1000,
        },
        origin: state.origin,
        facing: state.facing,
      })
    } else {
      XR8.FaceController.configure({
        coordinates: {
          origin: {
            position: state.origin,
            rotation: state.facing,
          },
        },
      })
    }
  }

  const maybeUpdateCameraParams = ({videoWidth, videoHeight, canvasWidth, canvasHeight}) => {
    if (state.captureSize.w == videoWidth &&
      state.captureSize.h == videoHeight &&
      state.displaySize.w == canvasWidth &&
      state.displaySize.h == canvasHeight) {
      return
    }

    updateCameraParams({canvasWidth, canvasHeight, videoWidth, videoHeight})
  }

  // TODO(alvin): This is a straight copy of what's in xr-aframe.js. Maybe we can move this to a utility class?
  const updateCameraParams = ({canvasWidth, canvasHeight, videoWidth, videoHeight}) => {
    state.displaySize.w = canvasWidth
    state.displaySize.h = canvasHeight
    state.captureSize.w = videoWidth
    state.captureSize.h = videoHeight

    state.videoViewport =
      XR8.GlTextureRenderer.fillTextureViewport(videoWidth, videoHeight, canvasWidth, canvasHeight)
  }

  /**
   * Sumerian ships their System class as ES6, which prevents our transpiled ES5 from extending
   * this class directly. Instead, we will create a new object from the Sumerian System prototype,
   * then override the desired functions from the new object's prototype directly.
   */
  const XRWebSystem = function () {
    Object.assign(this, new window.sumerian.System('XRWebSystem', ['CameraComponent']))
  }

  XRWebSystem.prototype = Object.create(window.sumerian.System.prototype)

  XRWebSystem.prototype.setup = function (world) {
    setupSumerianXR()
    const canvas = document.getElementById('overlayView3d')
    const GLctx = canvas.getContext('webgl')
    state.cameraFeedRenderer = new XR8.GlTextureRenderer.create({GLctx})
    if (world._state == window.sumerian.World.STATE_RUNNING) {
      // This system may be added to the world after it is already running. If so,
      // manually call start().
      this.start()
    }
  }

  XRWebSystem.prototype.inserted = function (entity) {
    state.cameraEntity = entity
  }

  XRWebSystem.prototype.start = function () {
    // eslint-disable-next-line prefer-object-spread
    XR8.run(Object.assign({canvas: document.getElementById('overlayView3d')}, runConfig))
  }

  XRWebSystem.prototype.resume = function () {
    XR8.resume()
  }

  XRWebSystem.prototype.stop = function () {
    XR8.stop()
  }

  XRWebSystem.prototype.pause = function () {
    XR8.pause()
  }

  XRWebSystem.prototype.cleanup = function () {
    window.sumerian.SystemBus.removeListener('recenter', recenterCallback)
    window.sumerian.SystemBus.removeListener('screenshotrequest', onScreenshotRequested)

    /**
     * Deprecated after 10.1 in favor for 'screenshotrequest', which is consistent with
     * our screenshot API on A-Frame.
     */
    window.sumerian.SystemBus.removeListener('requestscreenshot', onScreenshotRequested)
  }

  XRWebSystem.prototype.onPreRender = function () {
    if (!XR8.XrDevice.isDeviceBrowserCompatible(runConfig)) {
      return
    }

    if (state.realityDidInitialize) {
      XR8.runPreRender(Date.now())
    }
  }

  XRWebSystem.prototype.onPostRender = function () {
    if (!state.realityDidInitialize) {
      return
    }
    XR8.runPostRender()
  }

  return new XRWebSystem()
}

//  world:            The sumerianRunner world.
//  config            [Optional] Configuration parameters to pass to XR8.run:
//  {
//   webgl2:          [Optional] If true, use WebGL2 if available, otherwise fallback to WebGL1. If
//                               false, always use WebGL1.
//                               Default false.
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
// }
const addWebSystem = (world, config, engineKind) => {
  if (!window.sumerian) {
    throw new Error('window.sumerian does not exist but is required for addXRWebSystem')
  } else if (!world) {
    throw new Error('Sumerian world not provided')
  }

  // eslint-disable-next-line prefer-object-spread
  const runConfig = Object.assign({verbose: false, webgl2: false, ownRunLoop: false}, config)

  const system = xrWebSystem(runConfig, engineKind)

  // Errors with valid input, but incorrect environment should trigger error event
  // instead of throwing the error.
  if (!XR8.XrDevice.isDeviceBrowserCompatible(runConfig)) {
    // We don't expect the camera to run due to lack of compatibility, but we still need to try so
    // that error handling can propagate to pipeline modules.
    system.start()
    // TODO(nb): throw an error from engine.js onStart() / onVideoStatusChange() if incompatible.
    // We are throwing an error here because the setupSumerianXR has not yet been called to add
    // the pipeline module that adds error handling.
    window.sumerian.SystemBus.emit('xrerror', {
      compatibility: XR8.getCompatibility(),
      error: new Error('Device and browser combination is not supported.'),
      isDeviceBrowserSupported: XR8.XrDevice.isDeviceBrowserCompatible(runConfig),
    })
    return
  }

  const entities = world.getEntities()

  // The scene has likely already loaded itself and attached all entities to default systems.
  // Let's attach the camera to our system.
  if (entities) {
    entities.forEach((entity) => {
      system._insertEntityIfInterested(entity)
    })
  }

  world.setSystem(system)

  // When setting up sumerian with Amplify, the canvas does not seem to fit to the size of the
  // screen unless the position is changed. Let's see if this element exists, then update that.
  const sumerianCanvasContainer = document.getElementById('sumerian-canvas-inner-container')
  if (sumerianCanvasContainer) {
    sumerianCanvasContainer.style.position = 'static'
  }
}

// All publicly exported values should be inserted here.
const Sumerian = {

  // Sumerian XR Events
  // These are the events you can listen for in your web application if you call addXRWebSystem()
  //
  // Event: xrready
  // Description: This event is emitted when 8th Wall Web has initialized and at least one frame has
  //   been successfully processed. This is the recommended time at which any loading elements should
  //   be hidden.
  // Example:
  //    window.sumerian.SystemBus.addListener('xrready', () => {
  //      // Hide loading UI
  //    })
  //
  //
  // Event: xrerror
  // Description: This event is emitted when an error has occured when initializing 8th Wall Web.
  //   This is the recommended time at which any error messages should be displayed. The XrDevice
  //   API can help with determining what type of error messaging should be displayed.
  // Example:
  //    window.sumerian.SystemBus.addListener('xrerror', (data) => {
  //      if (XR8.XrDevice.isDeviceBrowserCompatible) {
  //        // Browser is compatible. Print the exception for more information.
  //        console.log(data.error)
  //        return
  //      }
  //
  //      // Browser is not compatible. Check the reasons why it may not be.
  //      for (let reason of XR8.XrDevice.incompatibleReasons()) {
  //        // Handle each XR8.XrDevice.IncompatibleReason
  //      }
  //    })
  //
  //
  // Event: camerastatuschange
  // Description: This event is emitted when the status of the camera changes. See `onCameraStatusChange`
  //   from XR8.addCameraPipelineModule for more information on the possible status.
  // Example:
  //   var handleCameraStatusChange = function handleCameraStatusChange(data) {
  //     console.log('status change', data.status);
  //
  //     switch (data.status) {
  //       case 'requesting':
  //         // Do something
  //         break;
  //
  //       case 'hasStream':
  //         // Do something
  //         break;
  //
  //       case 'failed':
  //         // Do something
  //         break;
  //     }
  //   };
  //  window.sumerian.SystemBus.addListener('camerastatuschange', handleCameraStatusChange)
  //
  //
  // Event: screenshotready
  // Description: This event is emitted in response to the `screenshotrequest` event being
  // being completed. The JPEG compressed image of the AFrame canvas will be provided.
  // Example:
  //    window.sumerian.SystemBus.addListener('screenshotready', (data) => {
  //      // screenshotPreview is an <img> HTML element
  //      const image = document.getElementById('screenshotPreview')
  //      image.src = 'data:image/jpeg;base64,' + data
  //    })
  //
  //
  // Event: screenshoterror
  // Description: This event is emitted in response to the `screenshotrequest` event resulting
  // in an error.
  // Example:
  //    window.sumerian.SystemBus.addListener('screenshoterror', (data) => {
  //       console.log(event.detail)
  //       // Handle screenshot error.
  //    })
  //
  //
  // Sumerian Face Effects Events
  // These are the events you can listen for in your web application if you call
  // addFaceEffectsWebSystem()
  //
  // Event: xrfaceloading
  // Description: Fires when loading begins for additional face AR resources.
  // Example:
  //    window.sumerian.SystemBus.addListener(
  //      'xrfaceloading',
  //      ({maxDetections, pointsPerDetection, indices, uvs}) => {
  //    })
  //
  //
  // Event: xrfacescanning
  // Description: Fires when all face AR resources have been loaded and scanning has begun.
  // Example:
  //    window.sumerian.SystemBus.addListener(
  //      'xrfacescanning',
  //      ({maxDetections, pointsPerDetection, indices, uvs}) => {
  //    })
  //
  //
  // Event: xrfacefound
  // Description: Fires when a face first found.
  // Example:
  //    window.sumerian.SystemBus.addListener(
  //      'xrfacefound',
  //      ({id, transform, attachmentPoints, vertices, normals}) => {
  //    })
  //
  //
  // Event: xrfaceupdated
  // Description: Fires when a face is subsequently found.
  // Example:
  //    window.sumerian.SystemBus.addListener(
  //      'xrfaceupdated',
  //      ({id, transform, attachmentPoints, vertices, normals}) => {
  //    })
  //
  // Event: xrfacelost
  // Description: Fires when a face is no longer being tracked.
  // Example:
  //    window.sumerian.SystemBus.addListener(
  //      'xrfacelost',
  //      ({id}) => {
  //    },}
  //
  //
  // Sumerian Event Listeners
  // These are the events you can emit from your web application.
  //
  // Event: screenshotrequest
  // Description: Emits a request to the engine to capture a screenshot of the Sumerian canvas.
  //   The engine will emit a `screenshotready` event with the JPEG compressed image,
  //   or `screenshoterror` if an error has occured.
  // Example:
  //    window.sumerian.SystemBus.emit('screenshotrequest')
  //
  //
  // Event: recenter
  // Description: Recenters the camera feed to its origin. If a new origin is provided as an
  //   argument, the camera's origin will be reset to that, then it will recenter.
  // Arguments (Optional):
  //  {
  //    // The location of the new origin.
  //    origin: {x, y, z},
  //    // A quaternion representing direction the camera should face at the origin.
  //    facing: {w, x, y, z}
  //  }
  // Examples:
  //    window.sumerian.SystemBus.emit('recenter')
  //  OR
  //    window.sumerian.SystemBus.emit('recenter', {
  //       origin: {x: 1, y: 4, z: 0},
  //       facing: {w: 0.9856, x:0, y:0.169, z:0}
  //    })

  // Adds a custom Sumerian System to the provided Sumerian world. If the given world is
  // already running (i.e. in a {World#STATE_RUNNING} state), this system will start itself.
  // Otherwise, it will wait for the world to start before running. When starting, this system
  // will attach to the camera in the scene, modify it's position, and render the camera feed
  // to the background. The given Sumerian world must only contain one camera.
  //
  // Arguments:
  //    world - the Sumerian world that corresponds to the loaded scene
  //    config            [Optional] Configuration parameters to pass to XR8.run:
  //    {
  //     webgl2:          [Optional] If true, use WebGL2 if available, otherwise fallback to WebGL1.
  //                                 If false, always use WebGL1.
  //                                 Default false.
  //     ownRunLoop:      [Optional] If true, XR should use it's own run loop. If false, you will
  //                                 provide your own run loop and be responsible for calling
  //                                 runPreRender and runPostRender yourself [Advanced Users only].
  //                                 Default true.
  //     cameraConfig:    [Optional] Desired camera to use. Supported values for direction are
  //                                 XR8.XrConfig.camera().BACK or XR8.XrConfig.camera().FRONT.
  //                               Default: {direction: XR8.XrConfig.camera().BACK}
  //     glContextConfig: [Optional] The attributes to configure the WebGL canvas context.
  //                                 Default: null.
  //     allowedDevices:  [Optional] Specify the class of devices that the pipeline should run on.
  //                                 If the current device is not in that class, running will fail
  //                                 prior to opening the camera. If allowedDevices is
  //                                 XR8.XrConfig.device().ANY, always open the camera. Note that
  //                                 world tracking should only be used with
  //                                 XR8.XrConfig.device().MOBILE.
  //                                 Default: XR8.XrConfig.device().MOBILE.
  //    }
  addXRWebSystem: (world, config) => addWebSystem(world, config, EngineKind.XR),
  addFaceEffectsWebSystem: (world, config) => addWebSystem(world, config, EngineKind.FACE),
}

export {
  Sumerian,
}
