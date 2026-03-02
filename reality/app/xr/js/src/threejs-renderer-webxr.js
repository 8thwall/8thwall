// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Akul Santhosh (akulsanthosh@nianticlabs.com)

// Editor project using the session manager for QR marker based AR: 8th.io/j69fx

import {
  showResumeImmersivePrompt,
  showImmersivePrompt,
  hideImmersivePrompt,
} from './permissions-helper'

const ThreejsWebXRFactory = () => {
  let scene3 = null
  let framework_ = null
  let engaged = false

  const pipelineModule = () => {
    if (!window.THREE) {
      throw new Error('window.THREE does not exist but is required by the ThreeJS pipeline module')
    }

    // Creates the threejs scene, camera and renderer
    const createScene = (canvas, context) => {
      if (scene3) {
        return
      }

      const canvasWidth = canvas.width
      const canvasHeight = canvas.height

      const scene = new window.THREE.Scene()
      const camera = new window.THREE.PerspectiveCamera(
        60.0, /* initial field of view; will get set based on device info later. */
        canvasWidth / canvasHeight,
        0.01,
        1000.0
      )
      scene.add(camera)

      const renderer = new window.THREE.WebGLRenderer({
        canvas,
        context,
        alpha: true,
        antialias: true,
      })
      renderer.autoClear = false
      renderer.setSize(canvasWidth, canvasHeight)

      const cameraTexture = new window.THREE.Texture()

      scene3 = {scene, camera, renderer, cameraTexture}
    }

    const engage = ({
      canvas, GLctx,
    }) => {
      // engage is called in onStart and onAttach (in case threejs renderer was late added).
      // If engage was called from onStart, we don't need to call the rest of it again.
      if (engaged) {
        return
      }

      createScene(canvas, GLctx)
      engaged = true
    }

    // A session manager to handle WebXR lifecycle:
    // 1) Check if WebXR AR is supported in device
    // 2) Obtain permissions from user
    // 3) Request a new session and sets the reference space
    // 4) Update render state base layer and starts the animation loop
    // 5) Handle end of session.

    // The session manager will use the Raw Camera Access (RCA) API will
    // obtain the raw camera texture, calculate the intrinsic and extrinsic
    // of the camera and dispatch an event "imagecaptured" when a new texture
    // is ready.
    const SessionManagerRCA = () => {
      let drawCanvas_ = null
      let session_ = null
      let sessionType_ = null
      let glBinding_ = null
      let texInfo_ = null
      let intrinsic = null
      let lastFrameTime_ = -1
      let frameTime_ = 0
      let frame_ = null

      const initialize = ({drawCanvas}) => {
        drawCanvas_ = drawCanvas
        return Promise.resolve()
      }

      const onSessionEnded = () => {
        session_.removeEventListener('end', onSessionEnded)
        session_ = null
        scene3.renderer.setAnimationLoop(null)
        window.XR8.pause()
        showResumeImmersivePrompt(sessionType_).then(() => window.XR8.resume())  // auto resume.
      }

      const getCameraIntrinsic = (projectionMatrix, viewport) => {
        const p = projectionMatrix

        // Principal point in pixels (typically at or near the center of the viewport)
        const u0 = (1 - p[8]) * (viewport.width / 2) + viewport.x
        const v0 = (1 - p[9]) * (viewport.height / 2) + viewport.y

        // Focal lengths in pixels (these are equal for square pixels)
        const ax = (viewport.width / 2) * p[0]
        const ay = (viewport.height / 2) * p[5]

        // Skew factor in pixels (nonzero for rhomboid pixels)
        const gamma = (viewport.width / 2) * p[4]

        // Intrinsic Matrix
        // K = [[ax, gamma, u0, 0],
        //      [0,  ay,    v0, 0],
        //      [0,  0,     1,  0],
        //      [0,  0,     0,  1]]

        return {
          fh: ax,
          fv: ay,
          cx: u0,
          cy: v0,
          gamma,
        }
      }

      const frameStartResult = () => {
        if (!texInfo_) {
          return {}
        }
        return {
          cameraTexture: texInfo_.tex,
          computeTexture: texInfo_.tex,
          timestamp: frameTime_,
          cameraPose: texInfo_.pose,
          renderingFov: texInfo_.intrinsic,
          XRframe: frame_,
          projection: texInfo_.projectionMatrix,
          textureWidth: texInfo_.width,
          textureHeight: texInfo_.height,
        }
      }

      const runXR = (time, frame) => {
        const {renderer} = scene3

        lastFrameTime_ = frameTime_
        frameTime_ = time

        const referenceSpace = renderer.xr.getReferenceSpace()
        if (!referenceSpace) {
          return
        }

        // getViewerPose() is a XRFrame method which describes the viewer's pose
        // (position and orientation) relative to the specified reference space
        const pose = frame.getViewerPose(referenceSpace)
        if (!pose) {
          return
        }

        // A view corresponds to a display used by an XR device to present imagery to the user.
        // A view can be used to retrieve its projection matrix, transform and physical
        // output properties. getCameraImage() is a XRWebGLBinding method which provides the raw
        // camera texture
        const viewWithCamera = pose.views.find(v => glBinding_.getCameraImage(v.camera))
        if (!viewWithCamera) {
          return
        }

        const {transform, projectionMatrix} = viewWithCamera
        const {baseLayer} = session_.renderState
        const viewport = baseLayer.getViewport(viewWithCamera)
        const {width, height} = viewport

        if (!intrinsic) {
          intrinsic = getCameraIntrinsic(projectionMatrix, viewport)
        }

        texInfo_ = {
          tex: glBinding_.getCameraImage(viewWithCamera.camera),
          width,
          height,
          intrinsic,
          pose: transform.matrix,
          projectionMatrix,
        }

        frame_ = frame

        if (framework_) {
          framework_.dispatchEvent('imagecaptured', frameStartResult())
        }
      }

      const render = (time) => {
        if (!window.XR8.isInitialized()) {
          return
        }
        const {camera, renderer, scene} = scene3
        lastFrameTime_ = frameTime_
        frameTime_ = time
        window.XR8.runPreRender(time)
        window.XR8.runRender()
        renderer.clearDepth()
        renderer.render(scene, camera)
        window.XR8.runPostRender()
      }

      const onSessionStarted = async () => {
        const {renderer} = scene3
        const gl = renderer.getContext()
        await gl.makeXRCompatible()
        glBinding_ = new XRWebGLBinding(session_, gl)

        renderer.setAnimationLoop((time, frame) => {
          if (frame) {
            runXR(time, frame)
          }
          render(time)
        })
      }

      const startWebXRSession = () => {
        sessionType_ = 'immersive-ar'
        const sessionInit = {
          requiredFeatures: ['camera-access', 'local', 'hit-test'],
          optionalFeatures: ['dom-overlay'],
          // Dom overlay can be used to visualize intermediate steps.
          domOverlay: {
            root: document.getElementById('xr-overlay'),
          },
        }
        return navigator.xr.requestSession(sessionType_, sessionInit)
          .then((session) => {
            session_ = session
            createScene(drawCanvas_)
            session_.addEventListener('end', onSessionEnded)
            const {renderer} = scene3
            renderer.autoClear = true
            renderer.xr.enabled = true
            renderer.xr.setReferenceSpaceType(sessionInit.requiredFeatures[1])
            return renderer.xr.setSession(session)
          })
          .then(() => {
            onSessionStarted()
          })
      }

      // We obtain permissions by starting a session.
      const obtainPermissions = () => startWebXRSession().catch(
        () => showImmersivePrompt(sessionType_).then(
          () => startWebXRSession()
        )
      )

      const stop = () => {
        hideImmersivePrompt()
        if (session_) {
          session_.end()
        }
      }

      const startSession = () => {
        if (session_) {
          return Promise.resolve()
        }
        return obtainPermissions()
      }

      const pause = () => {
        // TODO(akuls) Implement pause functionality
      }

      const resume = () => {
        hideImmersivePrompt()
        return obtainPermissions()
      }

      const hasNewFrame = () => ({
        repeatFrame: frameTime_ <= 0 || frameTime_ === lastFrameTime_,
        videoSize: {
          width: 1,
          height: 1,
        },
      })

      const providesRunLoop = () => true

      const getSessionAttributes = () => ({
        cameraLinkedToViewer: false,
        controlsCamera: true,
        fillsCameraTexture: true,
        providesRunLoop: true,
        supportsHtmlEmbedded: true,
        supportsHtmlOverlay: false,
        usesMediaDevices: false,
        usesWebXr: true,
      })

      return {
        initialize,
        obtainPermissions,
        stop,
        start: startSession,
        pause,
        resume,
        hasNewFrame,
        frameStartResult,
        providesRunLoop,
        getSessionAttributes,
      }
    }

    const onAttach = (args) => {
      framework_ = args.framework
      engage(args)
    }

    const onStart = (args) => {
      engage(args)
    }

    return {
      name: 'threejsrendererwebxr',
      onAttach,
      onStart,
      sessionManager: SessionManagerRCA,
    }
  }

  const xrScene = () => scene3

  return {
    pipelineModule,
    xrScene,
  }
}

export {
  ThreejsWebXRFactory,
}
