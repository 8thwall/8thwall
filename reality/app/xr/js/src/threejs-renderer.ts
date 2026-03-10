// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)
//
// ThreeJS based renderer
/* global XR8:readonly, THREE:readonly */

import {
  showResumeImmersivePrompt,
  showImmersivePrompt,
  hideImmersivePrompt,
} from './permissions-helper'

import {XrInputManagerFactory} from './webxrcontrollersthreejs/xr-input-manager'
import {addVoidSpace} from './webxrcontrollersthreejs/void-space'
import {desktopCameraManager} from './webxrcontrollersthreejs/desktop-camera-manager'
import {
  rawGeoToThreejsBufferGeometry,
} from './geometry-conversions'
import {LayerName, SUPPORTED_LAYERS} from './types/layers'
import {getSupportedHeadsetSessionType} from './webxr-compatibility'
import {XrDeviceFactory} from './js-device'
import type {DefaultEnvironmentConfiguration, RunConfig} from './types/pipeline'
import type {AttachInput} from './types/callbacks'
import {hasGltfLoader} from './threejs-support'

declare let navigator: any

interface ThreejsRendererArgs {
  renderCameraTexture?: boolean
  // The layers to create new threejs scenes for.
  layerScenes?: LayerName[]
}

interface ThreejsRendererOptions {
  renderCameraTexture: boolean
  layerNames: LayerName[]
}

interface LayerScene {
  scene: any
  camera: any
}

interface Scene3 {
  // The Threejs renderer.
  renderer: any
  // The Threejs main camera.
  camera: any
  // The Threejs scene.
  scene: any
  // The Threejs texture with camera feed cropped to canvas' size
  cameraTexture: any
  // Layer name -> LayerScene.
  layerScenes?: Record<string, LayerScene>
}

function ThreejsFactory() {
  const onInitSceneCallbacks = []
  const options_: ThreejsRendererOptions = {
    renderCameraTexture: true,
    layerNames: [],
  }
  let scene3_: Scene3 | null = null
  let engaged_ = false
  let framework_ = null

  let mirroredDisplay_ = false
  let cameraFeedRenderer_ = null
  // Opts used to create a renderer
  let rendererCreateOpt_: Record<any, any> = {}
  let canvasWidth_ = null
  let canvasHeight_ = null
  let videoWidth_ = null
  let videoHeight_ = null
  let texProps = null
  let GLctx_ = null
  let needsRecenter_ = false

  let initialSceneCameraPosition_ = null
  let xrInputManager_ = null

  // Pipeline module session state
  let shouldManageCamera_ = false
  let useRealityPosition_ = false
  let addedVoidSpace_: ReturnType<typeof addVoidSpace> | null = null
  let checkedEnvironment_ = false
  let hasXrSession_ = false
  let defaultEnvironmentConfiguration_: DefaultEnvironmentConfiguration | null = null
  let runConfig_: RunConfig | null = null
  let rendersOpaque_ = false
  let shouldClearColor_ = false

  // Layer name -> WebGLRenderTarget.
  let layerRenderTargets_: Record<string, any> = {}

  // A pipeline module that interfaces with the threejs environment and lifecyle. The threejs scene
  // can be queried using Threejs.xrScene() after Threejs.pipelineModule()'s onStart method is
  // called. Setup can be done in another pipeline module's onStart method by referring to
  // Threejs.xrScene() as long as XR8.addCameraPipelineModule is called on the second module *after*
  // calling XR8.addCameraPipelineModule(Threejs.pipelineModule()).
  //
  // * onStart, a threejs renderer and scene are created and configured to draw over a camera feed.
  // * onUpdate, the threejs camera is driven with the phone's motion.
  // * onRender, the renderer's render() method is invoked.
  //
  // Note that this module does not actually draw the camera feed to the canvas, GlTextureRenderer
  // does that. To add a camera feed in the background, install the
  // GlTextureRenderer.pipelineModule() before installing this module (so that it is rendered before
  // the scene is drawn).
  //
  // Typical usage:
  //
  //  // Add XrController.pipelineModule(), which enables 6DoF camera motion estimation.
  //  XR8.addCameraPipelineModule(XR8.XrController.pipelineModule())
  //
  //  // Add a GlTextureRenderer which draws the camera feed to the canvas.
  //  XR8.addCameraPipelineModule(XR8.GlTextureRenderer.pipelineModule())
  //
  //  // Add Threejs.pipelineModule() which creates a threejs scene, camera, and renderer, and
  //  // drives the scene camera based on 6DoF camera motion.
  //  XR8.addCameraPipelineModule(XR8.Threejs.pipelineModule())
  //
  //  // Add custom logic to the camera loop. This is done with camera pipeline modules that provide
  //  // logic for key lifecycle moments for processing each camera frame. In this case, we'll be
  //  // adding onStart logic for scene initialization, and onUpdate logic for scene updates.
  //  XR8.addCameraPipelineModule({
  //    // Camera pipeline modules need a name. It can be whatever you want but must be unique
  //    // within your app.
  //    name: 'myawesomeapp',
  //
  //    // onStart is called once when the camera feed begins. In this case, we need to wait for the
  //    // XR8.Threejs scene to be ready before we can access it to add content.
  //    onStart: ({canvasWidth, canvasHeight}) => {
  //      // Get the 3js scene. This was created by XR8.Threejs.pipelineModule().onStart(). The
  //      // reason we can access it here now is because 'myawesomeapp' was installed after
  //      // XR8.Threejs.pipelineModule().
  //      const {scene, camera, renderer, cameraTexture} = XR8.Threejs.xrScene()
  //
  //      // Add some objects to the scene and set the starting camera position.
  //      initXrScene({scene, camera, renderer, cameraTexture})
  //
  //      // Sync the xr controller's 6DoF position and camera parameters with our scene.
  //      XR8.XrController.updateCameraProjectionMatrix({
  //        origin: camera.position,
  //        facing: camera.quaternion,
  //      })
  //    },
  //
  //    // onUpdate is called once per camera loop prior to render. Any 3js geometry scene would
  //    // typically happen here.
  //    onUpdate: () => {
  //      // Update the position of objects in the scene, etc.
  //      updateScene(XR8.XrController.xrScene())
  //    },
  //  })
  function pipelineModule() {
    if (!window.THREE) {
      throw new Error('window.THREE does not exist but is required by the ThreeJS pipeline module')
    }

    const desktopCameraManager_ = desktopCameraManager()

    const updateCamera = (camera, position, rotation, intrinsics) => {
      for (let i = 0; i < 16; i++) {
        camera.projectionMatrix.elements[i] = intrinsics[i]
      }

      // Fix for broken raycasting in r103 and higher. Related to:
      //   https://github.com/mrdoob/three.js/pull/15996
      // Note: camera.projectionMatrixInverse wasn't introduced until r96 so check before setting
      // the inverse
      if (camera.projectionMatrixInverse) {
        if (camera.projectionMatrixInverse.invert) {
          // THREE 123 preferred version
          camera.projectionMatrixInverse.copy(camera.projectionMatrix).invert()
        } else {
          // Backwards compatible version
          camera.projectionMatrixInverse.getInverse(camera.projectionMatrix)
        }
      }

      if (rotation) {
        camera.setRotationFromQuaternion(rotation)
      }
      if (position) {
        camera.position.set(position.x, position.y, position.z)
      }
    }

    // A method which GlTextureRenderer will call and use in rendering. Returns a list where each
    // element contains two WebGLTexture's:
    // 1) foregroundTexture: The render target texture which we've rendered the LayerScene to.
    // 2) foregroundMaskTexture: Semantics results. Used for alpha blending of foregroundTexture.
    const layerTextures = ({processCpuResult}) => {
      if (options_.layerNames.length === 0 || !processCpuResult.layerscontroller ||
      !processCpuResult.layerscontroller.layers) {
        return []
      }

      // If we are using XrController then update the scene using its extrinsic.
      const {rotation, position, intrinsics} =
        processCpuResult.reality || processCpuResult.layerscontroller

      const {renderer, layerScenes} = scene3_!

      // Here we 1) update the layer scene camera 2) render the layer scene to a texture 3) return
      // the layer scene texture as well as the alpha mask for that layer. Note that we need to do
      // #2 here rather than in onRender() or onUpdate() because we need the ouput availabe in this
      // function, which is called by GlTextureRenderer's onUpdate(). And GlTextureRenderer's
      // onUpdate() can be called before ThreejsRenderer's onUpdate().
      const foregroundTexturesAndMasks = options_.layerNames.map((layerName) => {
        // 1) Update the camera in the layer scene.
        updateCamera(layerScenes![layerName].camera, position, rotation, intrinsics)

        // 2) Render the layer scene to a texture
        if (layerRenderTargets_[layerName].width !== canvasWidth_ ||
          layerRenderTargets_[layerName].height !== canvasHeight_) {
          // We change the size here instead of in updateSize() because setSize() will call
          // this.dispose() which frees the texture. We only want to do this right before drawing
          // a new texture. Example:
          // - https://r105.threejsfundamentals.org/threejs/lessons/threejs-rendertargets.html
          layerRenderTargets_[layerName].setSize(canvasWidth_, canvasHeight_)
        }

        // Update the encoding in case it has changed.
        if (layerRenderTargets_[layerName].texture.colorSpace !== renderer.outputColorSpace) {
          layerRenderTargets_[layerName].texture.colorSpace = renderer.outputColorSpace
        }
        renderer.setRenderTarget(layerRenderTargets_[layerName])
        renderer.clear()
        renderer.render(layerScenes![layerName].scene, layerScenes![layerName].camera)
        renderer.setRenderTarget(null)

        // 3) Return the layer scene texture as well as the alpha mask for that layer.
        // Get a handle to the WebGLTexture from the WebGLRenderTarget's Texture. See:
        // - https://stackoverflow.com/a/44176225/4979029
        // - https://stackoverflow.com/a/51097136/4979029
        const layerRenderTargetTexture =
          scene3_.renderer.properties.get(layerRenderTargets_[layerName].texture)
        return {
          foregroundTexture: layerRenderTargetTexture.__webglTexture,
          foregroundMaskTexture: processCpuResult.layerscontroller.layers[layerName].texture,
          foregroundTextureFlipY: true,
          foregroundMaskTextureFlipY: false,
        }
      })
      return foregroundTexturesAndMasks
    }

    // Update size for reality scene texture.
    const updateSize = ({videoWidth, videoHeight, canvasWidth, canvasHeight, GLctx}) => {
      // scale texture output width, height to never be bigger than videoWidth, videoHeight
      let width = canvasWidth
      let height = canvasHeight
      if (width > videoWidth) {
        const scale = videoWidth / width
        width *= scale
        height *= scale
      }
      if (height > videoHeight) {
        const scale = videoHeight / height
        width *= scale
        height *= scale
      }
      if (!cameraFeedRenderer_ ||
        rendererCreateOpt_.width !== width ||
        rendererCreateOpt_.height_ !== height ||
        rendererCreateOpt_.mirroredDisplay !== mirroredDisplay_
      ) {
        if (cameraFeedRenderer_) {
          cameraFeedRenderer_.destroy()
        }
        cameraFeedRenderer_ = window.XR8.GlTextureRenderer.create({
          GLctx,
          toTexture: {width, height},
          flipY: false,
          mirroredDisplay: mirroredDisplay_,
        })
        rendererCreateOpt_ = {
          width,
          height,
          mirroredDisplay: mirroredDisplay_,
        }
      }

      // Save original passed in data
      canvasWidth_ = canvasWidth
      canvasHeight_ = canvasHeight
      videoWidth_ = videoWidth
      videoHeight_ = videoHeight
      GLctx_ = GLctx

      // For desktop 3d sessions we need to manually update the projection matrix on canvas resize.
      // Here we infer that controlsCamera sessions will do this logic themselves, and camera
      // driven experiences (slam / face effects) will also, so what's left are things like
      // desktop 3d that don't have an external source for this info.
      // TODO: consider whether there's something better or more explicit here than
      // fillsCameraTexture.
      if (scene3_ && shouldManageCamera_) {
        const {camera} = scene3_
        camera.aspect = canvasWidth_ / canvasHeight_
        camera.updateProjectionMatrix()
      }
    }

    const createScene = (canvas, context) => {
      if (scene3_) {
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
        alpha: false,
        antialias: true,
      })
      renderer.autoClear = false
      renderer.setSize(canvasWidth, canvasHeight)

      const cameraTexture = new window.THREE.Texture()

      scene3_ = {scene, camera, renderer, cameraTexture}

      if (options_.layerNames) {
        const layerScenes: Record<string, LayerScene> = {}
        options_.layerNames.forEach((layerName) => {
          // Create a scene for each layer.
          const layerScene = new window.THREE.Scene()
          layerScene.name = layerName

          const layerCamera = camera.clone()
          layerScene.add(layerCamera)

          layerScenes[layerName] = {'scene': layerScene, 'camera': layerCamera}

          // Create a render target which we will render the layer scene to.
          layerRenderTargets_[layerName] = new THREE.WebGLRenderTarget(canvasWidth, canvasHeight, {
            depthBuffer: true,
            colorSpace: renderer.outputColorSpace,
          })
        })

        // TODO(paris): Add comment about how to use this to pipelineModule() docstring.
        scene3_ = {...scene3_, layerScenes}

        window.XR8.GlTextureRenderer.setForegroundTextureProvider(layerTextures)
      }

      if (onInitSceneCallbacks.length) {
        onInitSceneCallbacks.forEach(cb => cb(scene3_))
      }
    }

    const engage = ({
      framework, canvas, videoWidth, videoHeight, canvasWidth, canvasHeight, GLctx,
    }) => {
      // engage is called in onStart and onAttach (in case threejs renderer was late added). Session
      // is only available in onAttach, so we always need to grab a handle to it if available, even
      // if engage was already called from onStart.
      framework_ = framework

      // If engage was called from onStart, we don't need to call the rest of it again.
      if (engaged_) {
        return
      }
      updateSize({videoWidth, videoHeight, canvasWidth, canvasHeight, GLctx})
      createScene(canvas, GLctx)
    }

    const processCameraConfigured = (event) => {
      const {camera} = scene3_
      if (event.detail.origin) {
        const {position, rotation} = event.detail.origin
        if (rotation) {
          camera.setRotationFromQuaternion(rotation)
        }
        if (position) {
          camera.position.set(position.x, position.y, position.z)
        }
        camera.updateMatrix()
        initialSceneCameraPosition_ = camera.matrix.clone()
        if (scene3_) {
          if (addedVoidSpace_) {
            addedVoidSpace_.rescale(initialSceneCameraPosition_, scene3_.camera)
          }
          if (desktopCameraManager_) {
            desktopCameraManager_.recenter(initialSceneCameraPosition_, scene3_.camera)
          }
        }
      }

      const {mirroredDisplay, recenter} = event.detail
      needsRecenter_ = needsRecenter_ || !!recenter
      if (mirroredDisplay === mirroredDisplay_ || mirroredDisplay === undefined || !GLctx_) {
        return
      }

      mirroredDisplay_ = mirroredDisplay
      updateSize({
        videoWidth: videoWidth_,
        videoHeight: videoHeight_,
        canvasWidth: canvasWidth_,
        canvasHeight: canvasHeight_,
        GLctx: GLctx_,
      })
    }

    const realityEventEmitter = (event) => {
      if (event.name === 'reality.meshfound') {
        const {position, rotation, geometry, id} = event.detail
        framework_.dispatchEvent(
          'meshfound',
          {position, rotation, bufferGeometry: rawGeoToThreejsBufferGeometry(geometry), id}
        )
      } else if (event.name.startsWith('reality.')) {
        framework_.dispatchEvent(event.name.substring('reality.'.length), event.detail)
      }
    }

    const HeadsetSessionManager = () => {
      const originalRAF_ = window.requestAnimationFrame
      const originalCAF_ = window.cancelAnimationFrame
      let cameraLoadId_ = 0  // Monotonic counter to handle cancelling camera feed initialization.
      let runOnCameraStatusChange_ = null
      let paused_ = false
      let isResuming_ = false
      let sessionType_ = ''
      let session_ = null
      let sessionInit_ = null
      let canvas_ = null
      let frame_ = null
      let lastFrameTime_ = -1
      let frameTime_ = 0
      let gltfLoaderPromise_ = Promise.resolve()
      let sessionConfiguration_
      const pendingAnimations_ = new Map()  // id Number to callback

      let sceneScale_ = 0
      const originTransform_ = new window.THREE.Matrix4()
      const r3 = parseFloat(window.THREE.REVISION)

      let didCameraLoopStop_ = false
      let didCameraLoopPause_ = false

      const immersiveSessionType = async () => {
        // Some devices (e.g. Magic Leap 2) depends on deviceInfo extended info
        const deviceInfo = await XrDeviceFactory().deviceInfo()
        const browserName = deviceInfo.browser.name
        const sessionType = await getSupportedHeadsetSessionType(browserName, deviceInfo.model)

        if (sessionType && browserName === 'Edge' && r3 < 125) {
          // eslint-disable-next-line no-console
          console.warn(`Detected THREE revision ${r3}, HoloLens requires revision 125 or later.`)
          return ''
        }

        return sessionType
      }

      const updateOrigin = (obj) => {
        if (!(obj && sceneScale_)) {
          return
        }
        // Offset the object's transform by the origin and scene scale.
        obj.matrix.premultiply(originTransform_)
        // Update the object's position, etc. to reflect the new matrix.
        obj.matrix.decompose(obj.position, obj.quaternion, obj.scale)
        // Recompute obj.matrixWorld and obj.matrixWorldInverse
        obj.updateMatrixWorld(true)
        // For webxr cameras, threejs updates their projection matrix but not its inverse.
        if (obj.projectionMatrix) {
          obj.projectionMatrixInverse.copy(obj.projectionMatrix).invert()
        }
      }

      const cloneCamera = (fromCam, toCam) => {
        toCam.matrix.copy(fromCam.matrix)
        toCam.matrix.decompose(toCam.position, toCam.quaternion, toCam.scale)
        toCam.updateMatrixWorld(true)
        toCam.projectionMatrix.copy(fromCam.projectionMatrix)
        toCam.projectionMatrixInverse.copy(fromCam.projectionMatrixInverse)
      }

      const groundRotation = (quat) => {
        const e = new window.THREE.Euler(0, 0, 0, 'YXZ')
        e.setFromQuaternion(quat)
        return new window.THREE.Quaternion().setFromEuler(new window.THREE.Euler(0, e.y, 0, 'YXZ'))
      }

      const recenter = (currentXrExtrinsic) => {
        // Get the current XR transform parameters.
        const xrTransform = new window.THREE.Matrix4().fromArray(currentXrExtrinsic)
        const xrPos = new window.THREE.Vector3()
        const xrRot = new window.THREE.Quaternion()
        const unusedScale = new window.THREE.Vector3()
        xrTransform.decompose(xrPos, xrRot, unusedScale)

        // Find the parameters of the desired initial scene camera.
        const initPos = new window.THREE.Vector3()
        const initRot = new window.THREE.Quaternion()
        initialSceneCameraPosition_.decompose(initPos, initRot, unusedScale)

        // Find the scale needed to transform between the current XR height and the desired scene
        // height.
        sceneScale_ = initPos.y / xrPos.y

        // Get the ground rotation needed to go from the current XR camera to the desired scene
        // camera.
        const rotY = groundRotation(initRot).multiply(groundRotation(xrRot).invert())

        // Translate xr camera to desired scene x/z with height 0.
        xrPos.applyQuaternion(rotY)
        initPos.setX(initPos.x - xrPos.x * sceneScale_)
        initPos.setY(0)
        initPos.setZ(initPos.z - xrPos.z * sceneScale_)

        // Put it all together.
        originTransform_.compose(
          initPos, rotY, new window.THREE.Vector3(sceneScale_, sceneScale_, sceneScale_)
        )

        if (session_) {  // xrInputManager_ attach state is set on session_ start.
          xrInputManager_.setOriginTransform(originTransform_, sceneScale_)
        }

        needsRecenter_ = false
      }

      const fixScale = (frame) => {
        if (!frame) {
          return
        }
        const {renderer, camera} = scene3_
        const referenceSpace = renderer.xr.getReferenceSpace()
        const pose = frame.getViewerPose(referenceSpace)

        if (!pose) {
          return
        }

        const {views} = pose

        if (!views.length) {
          return
        }

        views.forEach((view, idx) => {
          if (!sceneScale_ || needsRecenter_) {
            recenter(view.transform.matrix)
          }

          if (!sceneScale_) {
            return
          }

          updateOrigin(renderer.xr.getCamera(camera).cameras[idx])
        })

        if (!sceneScale_) {
          return
        }

        const mainXrCamera = renderer.xr.getCamera(camera)
        updateOrigin(mainXrCamera)
        cloneCamera(mainXrCamera, scene3_.camera)
      }

      const render = (time, frame) => {
        if (!window.XR8.isInitialized()) {
          return
        }
        // TODO(paris): Do we need to also render layersTarget_ here for responsive immersive?
        frame_ = frame
        lastFrameTime_ = frameTime_
        frameTime_ = time
        fixScale(frame)
        if (xrInputManager_) {
          xrInputManager_.update(frame)
        }
        window.XR8.runPreRender(time)
        window.XR8.runRender()
        window.XR8.runPostRender()
      }

      const animate = () => {
        scene3_.renderer.setAnimationLoop(null)
        scene3_.renderer.xr.setAnimationLoop(render)
      }

      const onSessionStarted = () => {
        const {scene, renderer, camera} = scene3_
        if (!isResuming_) {
          sceneScale_ = 0
        }

        const initControllers = () => {
          xrInputManager_.attach({
            renderer,
            canvas: canvas_,
            scene,
            camera,
            pauseSession: () => {
              if (!session_) {
                return
              }
              session_.end()  // Auto-resume on session end.
            },
            sessionConfiguration: sessionConfiguration_,
          })
          xrInputManager_.setOriginTransform(originTransform_, sceneScale_)
        }

        if (hasGltfLoader()) {
          initControllers()
        } else {
          gltfLoaderPromise_.then(initControllers)
        }

        window.requestAnimationFrame = (callback: Function) => {
          const id = session_.requestAnimationFrame((time, frame) => {
            pendingAnimations_.delete(id)
            callback(time, frame)
          })
          pendingAnimations_.set(id, callback)
          return id
        }

        window.cancelAnimationFrame = (id) => {
          // If this is a session RAF, cancel on the session. Otherwise cancel on the window.
          if (pendingAnimations_.delete(id)) {
            session_.cancelAnimationFrame(id)
          } else {
            originalCAF_(id)
          }
        }

        // pendingAnimations_ can have elements in it from a previous session which we want to queue
        // up on the new session.
        const callbacks = Array.from(pendingAnimations_.values())
        pendingAnimations_.clear()
        callbacks.forEach(callback => window.requestAnimationFrame(callback))

        isResuming_ = false
        animate()
      }

      const onSessionEnded = () => {
        if (xrInputManager_) {
          xrInputManager_.detach()
        }
        session_.removeEventListener('end', onSessionEnded)
        session_ = null
        window.requestAnimationFrame = originalRAF_
        window.cancelAnimationFrame = originalCAF_
        scene3_.renderer.setAnimationLoop(null)
        // If the user didn't call XR8.pause() or XR8.stop(), it's a system stop which should pause
        // the camera runtime and try to auto-resume.
        if (!didCameraLoopPause_ && !didCameraLoopStop_) {
          window.XR8.pause()
          showResumeImmersivePrompt(sessionType_).then(() => XR8.resume())  // auto resume.
        }
        didCameraLoopPause_ = false
        didCameraLoopStop_ = false
      }

      const initialize = ({config, drawCanvas, runOnCameraStatusChange}) => {
        if (config.allowedDevices &&
          config.allowedDevices !== XR8.XrConfig.device().MOBILE_AND_HEADSETS &&
            config.allowedDevices !== XR8.XrConfig.device().ANY) {
          return Promise.reject(new Error('user requested config doesn\'t allow headsets.'))
        }
        canvas_ = drawCanvas
        runOnCameraStatusChange_ = runOnCameraStatusChange
        return immersiveSessionType().then((sessionType) => {
          sessionType_ = sessionType
          if (!sessionType_) {
            throw (new Error('threejs headset session only supports headsets.'))
          }

          // We know now that we want to start an immersive session. Create the input manager so we
          // can start doing any needed prework.
          xrInputManager_ = XrInputManagerFactory()

          sessionConfiguration_ = config.sessionConfiguration
          if (!sessionConfiguration_.disableXrTablet) {
            xrInputManager_.initializeTablet()
          }
          // Wait until after we know that this will be an immersive session to try to add the
          // GLTFLoader if it's missing. Ideally users would add this in their head.html.
          if (!hasGltfLoader()) {
            gltfLoaderPromise_ = new Promise((resolve, reject) => {
              const v = THREE.REVISION
              // eslint-disable-next-line no-console
              console.warn(`GLTFLoader for THREE is required on headsets. Loading r${v} now.`)
              const scriptEl = document.createElement('script')
              scriptEl.onload = () => resolve()
              scriptEl.src =
                `https://cdn.rawgit.com/mrdoob/three.js/r${v}/examples/js/loaders/GLTFLoader.js`
              scriptEl.onerror = () => reject(new Error(`Failed loading GLTFLoader ${v}.`))
              document.head.appendChild(scriptEl)
            })
          }
        })
      }

      // TODO (tri) remove sessionType check here once Vision Pro fix their spec
      const getSessionRendersOpaque = (session, sessionType) => (
        session.environmentBlendMode === 'opaque' ||
          (session.environmentBlendMode === undefined && sessionType === 'immersive-vr')
      )

      const startHeadsetSession = (loadId) => {
        if (loadId !== cameraLoadId_) {
          return Promise.resolve(true)
        }

        didCameraLoopPause_ = false
        didCameraLoopStop_ = false

        sessionInit_ = {
          requiredFeatures: ['local-floor'],
          optionalFeatures: ['hand-tracking', 'dom-overlay', 'plane-detection'],
        }

        createScene(canvas_, null)  // TODO(nb): specify context.

        runOnCameraStatusChange_(
          {status: 'requestingWebXr', sessionType: sessionType_, sessionInit: sessionInit_}
        )
        return navigator.xr.requestSession(sessionType_, sessionInit_)
          .then((session) => {
            session_ = session
            runOnCameraStatusChange_({
              status: 'hasSessionWebXr',
              sessionType: sessionType_,
              sessionInit: sessionInit_,
              session: session_,
              rendersOpaque: getSessionRendersOpaque(session_, sessionType_),
            })
            const {renderer, camera} = scene3_
            renderer.autoClear = true
            session_.addEventListener('end', onSessionEnded)
            renderer.xr.enabled = true
            renderer.xr.setReferenceSpaceType(sessionInit_.requiredFeatures[0])
            camera.updateMatrix()
            initialSceneCameraPosition_ = camera.matrix.clone()
            return renderer.xr.setSession(session)
          })
          .then(() => {
            runOnCameraStatusChange_({
              status: 'hasContextWebXr',
              sessionType: sessionType_,
              sessionInit: sessionInit_,
              session: session_,
              rendersOpaque: getSessionRendersOpaque(session_, sessionType_),
            })
            onSessionStarted()
          })
      }

      // We obtain headset permissions by starting a session.
      const obtainPermissions = () => startHeadsetSession(cameraLoadId_).catch(
        () => showImmersivePrompt(sessionType_).then(
          () => startHeadsetSession(cameraLoadId_)
        )
      )

      const stop = () => {
        const {camera} = scene3_
        // Increment cameraLoadId to ensure camera loads are cancelled
        ++cameraLoadId_

        hideImmersivePrompt()
        didCameraLoopStop_ = true
        if (session_) {
          session_.end()
        }

        initialSceneCameraPosition_.decompose(camera.position, camera.quaternion, camera.scale)
        camera.updateMatrix()

        sceneScale_ = 0
        sessionType_ = ''
        xrInputManager_ = null
        pendingAnimations_.clear()
      }

      // We obtain headset permissions by starting a session.
      const startSession = () => (session_ ? Promise.resolve() : obtainPermissions())

      const pause = () => {
        // Stop the session.
        paused_ = true
        ++cameraLoadId_
        didCameraLoopPause_ = true
        if (session_) {
          session_.end()  // Auto-resume on session end.
        }
      }

      const resume = () => {
        paused_ = false
        isResuming_ = true
        hideImmersivePrompt()
        return obtainPermissions()
      }

      const hasNewFrame = () => (
        {
          repeatFrame: paused_ || frameTime_ <= 0 || frameTime_ === lastFrameTime_,
          videoSize: {
            width: 1,
            height: 1,
          },
        }
      )

      const frameStartResult = () => ({
        textureWidth: 1,
        textureHeight: 1,
        videoTime: frameTime_,
        session: session_,
        frame: frame_,
      })

      const resumeIfAutoPaused = () => {
        // Doesn't apply
      }

      const getSessionAttributes = () => ({
        cameraLinkedToViewer: true,
        controlsCamera: true,
        fillsCameraTexture: false,
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
        resumeIfAutoPaused,
        hasNewFrame,
        frameStartResult,
        providesRunLoop: () => true,
        getSessionAttributes,
      }
    }

    // This is a workaround for https://bugs.webkit.org/show_bug.cgi?id=237230
    const device = window.XR8.XrDevice.deviceEstimate()
    // NOTE(christoph): We're not checking for "Safari" since this affects Chrome on iOS as well.
    const needsPrerenderFinish = device.os === 'iOS' && parseFloat(device.osVersion) >= 15.4

    return {
      name: 'threejsrenderer',
      onBeforeSessionInitialize: ({sessionAttributes}) => {
        if (!sessionAttributes.fillsCameraTexture) {
          // Reject non-camera sessions (i.e. headset, desktop) if three.js version < 123
          const r3 = parseFloat(window.THREE.REVISION)
          if (r3 < 123) {
            throw new Error(`Non-camera Session requires three.js r125 or later but found r${r3}`)
          }
        }
      },
      onBeforeRun: () => { scene3_ = null },
      onStart: args => engage(args),
      onAttach: args => engage(args),
      onSessionAttach: (input: AttachInput) => {
        const {sessionAttributes: {controlsCamera, fillsCameraTexture}} = input
        runConfig_ = input.config
        const {defaultEnvironment} = input.config.sessionConfiguration
        defaultEnvironmentConfiguration_ = defaultEnvironment?.disabled
          ? null
          : defaultEnvironment || {}

        hasXrSession_ = !!input.session
        shouldManageCamera_ = !controlsCamera && !fillsCameraTexture
        useRealityPosition_ = !controlsCamera && fillsCameraTexture

        rendersOpaque_ = !!input.rendersOpaque
        shouldClearColor_ = !(rendersOpaque_ && defaultEnvironmentConfiguration_) &&
          !fillsCameraTexture
        updateSize(input)
        const {camera} = scene3_
        // Update camera projection matrix from scene camera to get current session location
        XR8.XrController.updateCameraProjectionMatrix(
          {origin: camera.position, facing: camera.quaternion, updateRecenterPoint: false}
        )
        engaged_ = true
      },
      onSessionDetach: () => {
        const {camera} = scene3_
        const pos = {
          origin: camera.position,
          facing: camera.quaternion,
          updateRecenterPoint: false,
        }
        // Save camera projection matrix from scene camera
        window.XR8.XrController.updateCameraProjectionMatrix(pos)
        runConfig_ = null
        defaultEnvironmentConfiguration_ = null
        hasXrSession_ = false
        shouldManageCamera_ = false
        useRealityPosition_ = false
        shouldClearColor_ = false
        rendersOpaque_ = false
        checkedEnvironment_ = false
        if (addedVoidSpace_) {
          addedVoidSpace_.remove()
          addedVoidSpace_ = null
        }
        desktopCameraManager_.detach()
      },
      onDetach: () => {
        engaged_ = false
        framework_ = null
        cameraFeedRenderer_.destroy()
        cameraFeedRenderer_ = null
        window.XR8.GlTextureRenderer.setForegroundTextureProvider(null)
        Object.values(layerRenderTargets_).forEach((renderTarget) => {
          renderTarget.dispose()
        })
        layerRenderTargets_ = {}
      },
      onUpdate: ({processCpuResult}) => {
        if (!checkedEnvironment_) {
          if (rendersOpaque_ && defaultEnvironmentConfiguration_) {
            addedVoidSpace_ = addVoidSpace({
              renderer: scene3_.renderer,
              scene: scene3_.scene,
              ...defaultEnvironmentConfiguration_,
            })
            addedVoidSpace_.rescale(initialSceneCameraPosition_, scene3_.camera)
          }
          if (hasXrSession_ && xrInputManager_ && scene3_.camera.children.length) {
            xrInputManager_.moveCameraChildrenToController()
          }
          // If we don't have a session natively controlling the camera, and if we don't have any
          // camera texture to infer camera motion, then we need to drive the camera manually.
          if (shouldManageCamera_) {
            desktopCameraManager_.attach({
              camera: scene3_.camera,
              canvas: GLctx_.canvas,
              sessionConfiguration: runConfig_.sessionConfiguration,
            })
            desktopCameraManager_.recenter(initialSceneCameraPosition_, scene3_.camera)
          }
          checkedEnvironment_ = true
        }
        if (!useRealityPosition_) {
          return
        }

        // TODO(yuyan): need a better way to check the realitySource
        const realitySource = processCpuResult.reality || processCpuResult.facecontroller ||
          processCpuResult.layerscontroller

        // TODO(nb): remove intrinsics check once facecontroller provides it.
        if (!(realitySource && realitySource.intrinsics)) {
          return
        }

        const realityTexture = realitySource.realityTexture || realitySource.cameraFeedTexture

        if (options_.renderCameraTexture) {
          texProps.__webglTexture = cameraFeedRenderer_.render({
            renderTexture: realityTexture,
            viewport: window.XR8.GlTextureRenderer.fillTextureViewport(
              videoWidth_,
              videoHeight_,
              canvasWidth_,
              canvasHeight_
            ),
          })
        }

        const {rotation, position, intrinsics} = realitySource
        const {camera} = scene3_

        // Update scene camera.
        updateCamera(camera, position, rotation, intrinsics)
      },
      onProcessCpu: () => {
        // force initialization
        const {renderer, cameraTexture} = scene3_
        texProps = renderer.properties.get(cameraTexture)
      },
      onDeviceOrientationChange: ({videoWidth, videoHeight, GLctx}) => {
        updateSize(
          {videoWidth, videoHeight, canvasWidth: canvasWidth_, canvasHeight: canvasHeight_, GLctx}
        )
      },
      onVideoSizeChange: ({videoWidth, videoHeight, canvasWidth, canvasHeight, GLctx}) => {
        updateSize({videoWidth, videoHeight, canvasWidth, canvasHeight, GLctx})
      },
      onCanvasSizeChange: ({videoWidth, videoHeight, canvasWidth, canvasHeight, GLctx}) => {
        if (!engaged_) {
          return
        }
        updateSize({videoWidth, videoHeight, canvasWidth, canvasHeight, GLctx})

        const {renderer} = scene3_
        renderer.setSize(canvasWidth, canvasHeight)
      },
      onRender: () => {
        const {scene, renderer, camera} = scene3_
        const clearColor = shouldClearColor_
        const clearDepth = true
        const clearStencil = false
        renderer.clear(clearColor, clearDepth, clearStencil)

        if (needsPrerenderFinish) {
          GLctx_.finish()
        }

        renderer.render(scene, camera)
      },
      listeners: [
        {event: 'reality.cameraconfigured', process: processCameraConfigured},
        {event: 'reality.meshfound', process: realityEventEmitter},
        {event: 'reality.meshupdated', process: realityEventEmitter},
        {event: 'reality.meshlost', process: realityEventEmitter},
        {event: 'facecontroller.cameraconfigured', process: processCameraConfigured},
        {event: 'layerscontroller.cameraconfigured', process: processCameraConfigured},
      ],
      sessionManager: HeadsetSessionManager,
    }
  }

  // Get a handle to the xr scene, camera, renderer, (optionally) layer scene + camera, and
  // (optionally) the camera feed rendered to a texture.
  const xrScene = (): Scene3 => scene3_

  // Deprecated in 9.1. Prefer Threejs.pipelineModule().
  const ThreejsRenderer = () => Object.assign(
    pipelineModule(),
    {
      name: 'deprecatedthreejsrenderer',
      xrScene,
      onInitScene: cb => onInitSceneCallbacks.push(cb),
    }
  )

  const configure = (args: ThreejsRendererArgs) => {
    const {renderCameraTexture, layerScenes} = args
    if (renderCameraTexture !== undefined) {
      options_.renderCameraTexture = renderCameraTexture
    }
    if (layerScenes !== undefined) {
      // NOTE(paris): We could allow you to change these after calling XR8.run() with a refactor.
      // There is no real reason we don't support it.
      if (engaged_) {
        throw new Error('[XR] ThreeJs \'layerScenes\' can only be changed before calling XR8.run()')
      }
      if (!Array.isArray(layerScenes)) {
        throw new Error('[XR] ThreeJs \'layerScenes\' should be array')
      }
      layerScenes.forEach((layer) => {
        if (!SUPPORTED_LAYERS.includes(layer)) {
          throw new Error(
            `[XR] Invalid layer '${layer}'. Supported layers are: ${SUPPORTED_LAYERS
              .join(', ')}.`
          )
        }
      })

      const layersSet = new Set(layerScenes)
      if (layerScenes.length !== layersSet.size) {
        // eslint-disable-next-line no-console
        console.warn(
          `[XR] Warning - removing duplicate layers from provided values: ${layerScenes.join(', ')}`
        )
      }

      options_.layerNames = Array.from(layersSet)
    }
  }

  return {
    pipelineModule,
    xrScene,
    ThreejsRenderer,
    configure,
  }
}

export {
  ThreejsFactory,
}

export type {
  Scene3,
  ThreejsRendererArgs,
}
