// Copyright (c) 2023 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

/* globals XR8:readonly THREE:readonly */

import {getCompatibility, XrDeviceFactory} from '../js-device'

import {
  showResumeImmersivePrompt,
  showImmersivePrompt,
  hideImmersivePrompt,
} from '../permissions-helper'
import {
  maybeAddPreventDefaultTouchListeners, cleanupPreventDefaultTouchListeners,
} from '../prevent-default-touch'
import {desktopCameraManager} from '../webxrcontrollersthreejs/desktop-camera-manager'
import {addVoidSpace} from '../webxrcontrollersthreejs/void-space'
import {XrInputManagerFactory} from '../webxrcontrollersthreejs/xr-input-manager'
import {rawGeoToThreejsBufferGeometry} from '../geometry-conversions'
import {NON_SCENE_COMPONENTS, addSceneListener, clearSceneListeners, xrConfigSchema}
  from './common'
import {getSupportedHeadsetSessionType} from '../webxr-compatibility'
import type * as Pipeline from '../types/pipeline'
import type * as Callbacks from '../types/callbacks'

interface XrConfigState {
  aScene: any
  paused: any
  originalRenderFn: any
  doOverlay: any
  harmfulCameraDefaultsWereRemoved: any
  initialCameraHeightWasSet: any
  cameraFeedRenderer: any
  videoViewport: any
  foregroundVideoViewport: {
    offsetX: number
    offsetY: number
    width: number
    height: number
  }
  firstTick: any
  loadingTexture: any
  realityDidInitialize: any
  framesSinceLoad: any
  displaySize: any
  captureSize: any
  clipPlane: any
  suppressFurtherErrors: any
  cameraDirection: any
  allowedDevices: any
  runConfig: Pipeline.RunConfig | null
  sceneListeners: any
  initialSceneCameraPosition: any
  headsetSessionTick: any
  rendersOpaque: boolean
  addedVoidSpace: ReturnType<typeof addVoidSpace> | null
  // Is the current session position drives by AI through camera feed or not
  useRealityPosition: boolean
  // Is the current session position be controlled by manual input (like keyboard & mouse) or not
  shouldManageCamera: boolean
  engaged: boolean
  sceneCameraObj: any
  sceneCamera: any
  sessionAttributes: any
  desktopCameraManager: any
  layerTextureProviders: any
}

// Handles THREE v111
const getRendererContext = (renderer) => {
  if (renderer.getContext) {
    return renderer.getContext()
  }
  return renderer.context
}

const FIRST_FRAME_DELAY = 5  // should be at least 1

const makeViewport = ({width, height}) => ({
  offsetX: 0,
  offsetY: 0,
  width,
  height,
})

const xrconfigComponent = () => {
  const state: XrConfigState = {
    aScene: null,
    paused: false,
    originalRenderFn: null,
    doOverlay: true,
    harmfulCameraDefaultsWereRemoved: false,
    initialCameraHeightWasSet: false,
    cameraFeedRenderer: null,
    videoViewport: makeViewport({width: 0, height: 0}),
    foregroundVideoViewport: makeViewport({width: 0, height: 0}),
    firstTick: true,
    loadingTexture: null,
    realityDidInitialize: false,
    framesSinceLoad: 0,
    displaySize: {w: 300, h: 150},
    captureSize: {w: 300, h: 150},
    clipPlane: {n: 0.01, f: 1000},
    suppressFurtherErrors: false,
    cameraDirection: null,
    allowedDevices: null,
    runConfig: null,
    sceneListeners: [],
    initialSceneCameraPosition: new THREE.Matrix4(),
    headsetSessionTick: () => {},
    rendersOpaque: false,
    addedVoidSpace: null,
    useRealityPosition: false,
    shouldManageCamera: false,
    engaged: false,
    sceneCameraObj: null,
    sceneCamera: null,
    sessionAttributes: {},
    desktopCameraManager: desktopCameraManager(),
    layerTextureProviders: [],
  }

  const setBackgroundVisibility = (val) => {
    if (state.addedVoidSpace) {
      state.addedVoidSpace.setVisibility(val)
    }
  }

  const makePauseable = (cb, binding) => (...args) => {
    if (state.paused) {
      return
    }
    return cb.call(binding, ...args)
  }

  const initLoadingTexture = (GLctx) => {
    const blackPixel = new Uint8Array([45, 46, 67, 255])

    const restoreTex = GLctx.getParameter(GLctx.TEXTURE_BINDING_2D)

    const loadingTexture = GLctx.createTexture()
    GLctx.bindTexture(GLctx.TEXTURE_2D, loadingTexture)
    GLctx.hint(GLctx.GENERATE_MIPMAP_HINT, GLctx.FASTEST)
    GLctx.texParameteri(GLctx.TEXTURE_2D, GLctx.TEXTURE_WRAP_S, GLctx.CLAMP_TO_EDGE)
    GLctx.texParameteri(GLctx.TEXTURE_2D, GLctx.TEXTURE_WRAP_T, GLctx.CLAMP_TO_EDGE)
    GLctx.texParameteri(GLctx.TEXTURE_2D, GLctx.TEXTURE_MAG_FILTER, GLctx.NEAREST)
    GLctx.texParameteri(GLctx.TEXTURE_2D, GLctx.TEXTURE_MIN_FILTER, GLctx.NEAREST)
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

    return loadingTexture
  }

  const updateCameraParams = ({canvasWidth, canvasHeight, videoWidth, videoHeight}) => {
    state.displaySize.w = canvasWidth
    state.displaySize.h = canvasHeight
    state.captureSize.w = videoWidth
    state.captureSize.h = videoHeight

    state.videoViewport =
      XR8.GlTextureRenderer.fillTextureViewport(videoWidth, videoHeight, canvasWidth, canvasHeight)
    state.foregroundVideoViewport = makeViewport({width: canvasWidth, height: canvasHeight})

    if (!state.realityDidInitialize) {
      return
    }

    const s = state.aScene

    // Compute the number of pixels that will fill the window.
    let dpr = window.devicePixelRatio
    let ncw = s.clientWidth * dpr
    let nch = s.clientHeight * dpr

    // If the ascene has a limit set on canvas size, find out what it is. For older versions of
    // aframe, use the default from newer versions.
    const maxs = s.maxCanvasSize || {height: 1920, width: 1920}
    const maxh = maxs.height > 0 && maxs.height || nch
    const maxw = maxs.width > 0 && maxs.width || ncw

    // If the canvas is too tall, compute a new smaller canvas size and the pixel scaling that's
    // needed to fill the available screen size.
    if (nch > maxh) {
      ncw = maxh * (ncw / nch)
      nch = maxh
      dpr = ncw / s.clientWidth
    }

    // If this canvas is still to wide, compute a new smaller canvas size and the pixel scaling
    // that/s needed to fill the available screen size.
    if (ncw > maxw) {
      nch = maxw * (nch / ncw)
      ncw = maxw
      dpr = ncw / s.clientWidth
    }

    // Update the dpr and canvas to fill the screen with the requested resolution.
    s.renderer.setPixelRatio(dpr)
    s.renderer.setSize(s.clientWidth, s.clientHeight)

    // Update the state so that we don't need to re-resize the canvas.
    state.displaySize.w = s.canvas.width
    state.displaySize.h = s.canvas.height
    state.captureSize.w = videoWidth
    state.captureSize.h = videoHeight

    state.videoViewport = XR8.GlTextureRenderer.fillTextureViewport(
      videoWidth, videoHeight, s.canvas.width, s.canvas.height
    )
    state.foregroundVideoViewport = makeViewport({width: canvasWidth, height: canvasHeight})
  }

  const maybeUpdateCameraParams = ({videoWidth, videoHeight, canvasWidth, canvasHeight}) => {
    if (state.captureSize.w === videoWidth &&
      state.captureSize.h === videoHeight &&
      state.displaySize.w === canvasWidth &&
      state.displaySize.h === canvasHeight) {
      return
    }

    updateCameraParams({canvasWidth, canvasHeight, videoWidth, videoHeight})
  }

  // Looks for a camera element in the scene and removes defaults that don't play well with xr, like
  // vr-mode-ui, wasd-controls, and look-controls.  If there is not yet a camera element in the
  // scene (e.g. because xrweb is a component on the scene so it's added before the camera element)
  // keep looking for one.
  //
  // As a side effect, this method saves the object3d for the camera element and the scene camera on
  // the state.
  const removeHarmfulCameraElementDefaults = () => {
    if (state.harmfulCameraDefaultsWereRemoved) {
      return
    }

    const cam3 = state.aScene.camera
    const camA = cam3.el

    if (!camA) {
      return
    }
    state.sceneCameraObj = camA.object3D
    // eslint-disable-next-line prefer-destructuring
    state.sceneCamera = cam3  // The same object as state.sceneCameraObj.children[0]

    state.harmfulCameraDefaultsWereRemoved = true

    // Always remove vr-mode-ui.
    if (state.aScene.components['vr-mode-ui']) {
      state.aScene.removeComponent('vr-mode-ui')
    }

    // If we will be controlling the camera, remove the default look-controls & wasd-controls
    // components to prevent gyro bad motion.
    if (XR8.XrDevice.isDeviceBrowserCompatible(state.runConfig)) {
      if (camA.components['look-controls']) {
        camA.removeAttribute('look-controls')
      }
      if (camA.components['wasd-controls']) {
        camA.removeAttribute('wasd-controls')
      }
    }
  }

  // Sets the XR scene scale from the camera height if there's a camera element available to read
  // those values from.
  const setInitialCameraHeight = () => {
    if (state.initialCameraHeightWasSet) {
      return
    }

    removeHarmfulCameraElementDefaults()

    if (!state.sceneCamera) {
      // No camera element in the scene yet, can't grab a height yet.
      return
    }

    state.initialCameraHeightWasSet = true

    const camObj = state.sceneCameraObj
    camObj.updateMatrix()
    camObj.updateMatrixWorld(true)
    state.initialSceneCameraPosition = camObj.matrix.clone()

    const {x: px, y: py, z: pz} = camObj.position
    const {w: qw, x: qx, y: qy, z: qz} = camObj.quaternion
    const {width: dw, height: dh} = state.aScene.canvas
    const {near: cn, far: cf} = state.sceneCamera

    const origin = {x: px, y: py, z: pz}
    const facing = {w: qw, x: qx, y: qy, z: qz}
    state.displaySize = {w: dw, h: dh}
    state.clipPlane = {n: cn, f: cf}

    state.aScene.emit('configurecamera', {
      dw,
      dh,
      displaySize: state.displaySize,
      clipPlane: state.clipPlane,
      origin,
      facing,
    })

    if (state.shouldManageCamera) {
      state.desktopCameraManager.attach({
        camera: state.sceneCameraObj,
        canvas: state.aScene.canvas,
        sessionConfiguration: state.runConfig.sessionConfiguration,
      })
      state.desktopCameraManager.recenter(state.initialSceneCameraPosition, state.sceneCamera)
    }
  }

  // A method which GlTextureRenderer will call and use in rendering. Returns a list where each
  // element contains two WebGLTexture's:
  // 1) foregroundTexture: The render target texture which we've rendered layersScene to.
  // 2) foregroundMaskTexture: Semantics results. Used for alpha blending of foregroundTexture.
  const layerTextures = ({processCpuResult}) => {
    if (!state.layerTextureProviders || state.layerTextureProviders.length === 0 ||
      !processCpuResult.layerscontroller || !processCpuResult.layerscontroller.layers) {
      return []
    }

    return state.layerTextureProviders.map(layer => ({
      foregroundTexture: layer.provideTexture(),
      foregroundMaskTexture: processCpuResult.layerscontroller.layers[layer.name].texture,
      foregroundTextureFlipY: true,
      foregroundMaskTextureFlipY: false,
    }))
  }

  const onAttach = ({
    videoWidth, videoHeight, canvasWidth, canvasHeight, sessionAttributes,
  }: Callbacks.AttachInput) => {
    if (state.engaged) {
      return
    }
    state.sessionAttributes = sessionAttributes

    // We need to make sure origin / facing / clipPlane get committed at least once, so force a
    // camera params update.
    updateCameraParams({videoWidth, videoHeight, canvasWidth, canvasHeight})

    maybeAddPreventDefaultTouchListeners(state.aScene.canvas)
  }

  const onSessionAttach = (input: Callbacks.AttachInput) => {
    state.realityDidInitialize = true
    state.aScene.renderer.alpha = true
    state.aScene.renderer.antialias = true
    state.aScene.renderer.autoClearDepth = true
    state.aScene.renderer.autoClearColor = false
    const {sessionAttributes: {controlsCamera, fillsCameraTexture}} = input
    state.runConfig = input.config
    const {defaultEnvironment} = input.config.sessionConfiguration
    state.runConfig.sessionConfiguration.defaultEnvironment = defaultEnvironment?.disabled
      ? null
      : defaultEnvironment || {}

    // If the session reports "rendersOpaque: true", that means the scene doesn't have a
    // camera feed, or the real world, rendered behind it.
    state.rendersOpaque = !!input.rendersOpaque

    // If we don't have a session natively controlling the camera, and if we don't have any camera
    // texture to infer camera motion, then we need to drive the camera manually.
    state.shouldManageCamera = !controlsCamera && !fillsCameraTexture

    // If we don't have a session natively controlling the camera, but we have any camera
    // texture to infer camera motion, then we need to drive the camera using camera motion.
    state.useRealityPosition = !controlsCamera && fillsCameraTexture
    const {videoWidth, videoHeight, canvasWidth, canvasHeight} = input

    maybeUpdateCameraParams({videoWidth, videoHeight, canvasWidth, canvasHeight})
    if (state.rendersOpaque) {
      if (state.runConfig.sessionConfiguration.defaultEnvironment &&
        !state.runConfig.sessionConfiguration.defaultEnvironment.disabled) {
        state.addedVoidSpace = addVoidSpace({
          renderer: state.aScene.renderer,
          scene: state.aScene.object3D,
          ...state.runConfig.sessionConfiguration.defaultEnvironment,
        })
        state.addedVoidSpace.rescale(state.initialSceneCameraPosition, state.sceneCamera)
      }
      if (!state.doOverlay) {
        setBackgroundVisibility(false)
      }
      // We aren't in a context where we can draw the overlay so suppress it in rendering.
      state.doOverlay = false
    }

    // Check for a scene camera and set the initial camera height from it. This uses state so
    // make sure to set whatever you need first.
    setInitialCameraHeight()

    state.engaged = true
  }

  const onDetach = () => {
    cleanupPreventDefaultTouchListeners(state.aScene.canvas)
    state.engaged = false
    window.XR8.GlTextureRenderer.setForegroundTextureProvider(null)
  }

  const onSessionDetach = () => {
    state.doOverlay = true
    state.rendersOpaque = false
    state.runConfig.sessionConfiguration.defaultEnvironment = null
    state.runConfig = null
    state.shouldManageCamera = false
    state.useRealityPosition = false
    state.realityDidInitialize = false
    if (state.addedVoidSpace) {
      state.addedVoidSpace.remove()
      state.addedVoidSpace = null
    }
    state.initialCameraHeightWasSet = false
    state.desktopCameraManager.detach()
  }

  const xrRun = () => {
    XR8.run(state.runConfig)
  }

  function remove() {
    XR8.stop()

    state.paused = false

    if (state.originalRenderFn) {
      state.aScene.renderer.render = state.originalRenderFn
      state.originalRenderFn = null
    }

    const GLctx = getRendererContext(state.aScene.renderer)
    if (GLctx && state.loadingTexture) {
      GLctx.deleteTexture(state.loadingTexture)
      state.loadingTexture = null
    }

    XR8.removeCameraPipelineModules(['canvasscreenshot', 'xraframe'])

    state.harmfulCameraDefaultsWereRemoved = false
    state.initialCameraHeightWasSet = false
    state.desktopCameraManager.detach()

    clearSceneListeners(state.sceneListeners, state.aScene)
  }

  function init() {
    state.aScene = this.el.sceneEl  // TODO(nb): throw if this.el !== this.el.sceneEl

    const {renderer} = state.aScene
    state.originalRenderFn = renderer.render
    state.paused = false
    renderer.render = makePauseable(state.originalRenderFn, renderer)
    const GLctx = getRendererContext(renderer)

    let needsRecenter_ = false

    state.cameraDirection = this.data.cameraDirection
    state.allowedDevices = this.data.allowedDevices

    // Using type: 'asset' means we may be getting the value back as an img element, use the URL
    // instead.
    const floorTextureAsset = this.data.defaultEnvironmentFloorTexture

    const floorTextureUrl = (typeof floorTextureAsset === 'string')
      ? floorTextureAsset
      : floorTextureAsset.src

    state.runConfig = {
      canvas: state.aScene.canvas,
      verbose: this.data.verbose,
      webgl2: state.aScene.renderer.capabilities.isWebGL2,
      ownRunLoop: false,
      cameraConfig: {direction: state.cameraDirection},
      allowedDevices: state.allowedDevices,
      sessionConfiguration: {
        disableDesktopCameraControls: this.data.disableDesktopCameraControls,
        disableDesktopTouchEmulation: this.data.disableDesktopTouchEmulation,
        disableCameraReparenting: this.data.disableCameraReparenting,
        disableXrTablet: this.data.disableXrTablet,
        disableXrTouchEmulation: this.data.disableXrTouchEmulation,
        xrTabletStartsMinimized: this.data.xrTabletStartsMinimized,
        defaultEnvironment: {
          disabled: this.data.disableDefaultEnvironment,
          floorScale: this.data.defaultEnvironmentFloorScale,
          floorTexture: floorTextureUrl,
          floorColor: this.data.defaultEnvironmentFloorColor,
          fogIntensity: this.data.defaultEnvironmentFogIntensity,
          skyTopColor: this.data.defaultEnvironmentSkyTopColor,
          skyBottomColor: this.data.defaultEnvironmentSkyBottomColor,
          skyGradientStrength: this.data.defaultEnvironmentSkyGradientStrength,
        },
      },
    }

    state.cameraFeedRenderer = XR8.GlTextureRenderer.create({
      GLctx,
      mirroredDisplay: !!this.data.mirroredDisplay,
    })
    state.doOverlay = true

    state.loadingTexture = initLoadingTexture(GLctx)

    // XR8 components which are not added on the a-scene may not be registered as AFrame components
    // before AFrame starts initializing entities. So we check the scene for any components that did
    // not get added as AFrame components (i.e. they just exist on the DOM) and then re-add them.
    // NOTE(paris): Don't use forEach() b/c it returns all iterable properties of HTMLCollection.
    // eslint-disable-next-line no-restricted-syntax
    for (const child of this.el.getElementsByTagName('*')) {
      if (!child.components) {
        // eslint-disable-next-line no-continue
        continue
      }
      // NOTE(paris): Don't use forEach() b/c it returns all iterable properties of NamedNodeMap.
      // eslint-disable-next-line no-restricted-syntax
      for (const attr of child.attributes) {
        // Re-init components which 1) are XR8 components 2) are not already AFrame components.
        if (NON_SCENE_COMPONENTS.includes(attr.name) && !child.components[attr.name]) {
          child.setAttribute(attr.name, attr.value)
        }
      }
    }

    addSceneListener(state.sceneListeners, state.aScene, 'hidecamerafeed', () => {
      state.doOverlay = false
      setBackgroundVisibility(false)
    })
    addSceneListener(state.sceneListeners, state.aScene, 'showcamerafeed', () => {
      if (state.rendersOpaque) {
        setBackgroundVisibility(true)
      } else {
        state.doOverlay = true
      }
    })

    // Deprecated in R9.1. Prefer `hidecamerafeed` and `showcamerafeed`.
    addSceneListener(state.sceneListeners, state.aScene, 'hideCameraFeed', () => {
      state.aScene.emit('hidecamerafeed')
    })
    addSceneListener(state.sceneListeners, state.aScene, 'showCameraFeed', () => {
      state.aScene.emit('showcamerafeed')
    })

    XR8.addCameraPipelineModule(XR8.CanvasScreenshot.pipelineModule())
    addSceneListener(state.sceneListeners, state.aScene, 'screenshotrequest', () => {
      XR8.CanvasScreenshot.takeScreenshot().then(
        (data) => {
          state.aScene.emit('screenshotready', data)

          // Deprecatred in R9.1. Prefer `screenshotready`
          state.aScene.emit('screenshotReady', data)
        },
        (error) => {
          state.aScene.emit('screenshoterror', error)

          // Existing apps are unaware of error API. Send them an empty data buffer.
          state.aScene.emit('screenshotready', '')
          // Deprecatred in R9.1. Prefer `screenshotready`
          state.aScene.emit('screenshotReady', '')
        }
      )
    })

    // Deprecated in R9.1. Prefer `screenshotrequest`.
    addSceneListener(state.sceneListeners, state.aScene, 'screenshotRequest', () => {
      state.aScene.emit('screenshotrequest')
    })

    // Deprecated in R9.1. Prefer `recenter` with an {origin: {x, y, z}, facing: {w, x, y, z}}
    addSceneListener(state.sceneListeners, state.aScene, 'recenterWithOrigin', (event) => {
      state.aScene.emit('recenter', event.detail)
    })

    addSceneListener(state.sceneListeners, state.aScene, 'stopxr', () => XR8.stop())

    addSceneListener(state.sceneListeners, state.aScene, 'runxr', () => xrRun())

    // Deprecated in R9.1. Prefer `stopxr`
    addSceneListener(state.sceneListeners, state.aScene, 'stopXr', () => {
      state.aScene.emit('stopxr')
    })

    addSceneListener(state.sceneListeners, state.aScene, 'layertextureprovider', ({detail}) => {
      // state.layers is a list instead of map so that the order of layers is preserved.
      const idx = state.layerTextureProviders.findIndex(layer => layer.name === detail.name)
      if (idx < 0) {
        state.layerTextureProviders.push(detail)
      } else {
        state.layerTextureProviders[idx].provideTexture = detail.provideTexture
      }
    })

    const realityEventEmitter = (event) => {
      if (event.name === 'reality.meshfound') {
        const {position, rotation, geometry, id} = event.detail
        state.aScene.emit(
          'xrmeshfound',
          {position, rotation, bufferGeometry: rawGeoToThreejsBufferGeometry(geometry), id}
        )
      } else if (event.name.startsWith('reality.')) {
        state.aScene.emit(`xr${event.name.substring('reality.'.length)}`, event.detail)
      } else if (event.name.startsWith('facecontroller.')) {
        state.aScene.emit(`xr${event.name.substring('facecontroller.'.length)}`, event.detail)
      } else if (event.name.startsWith('layerscontroller.')) {
        state.aScene.emit(`xr${event.name.substring('layerscontroller.'.length)}`, event.detail)
      }
    }

    const rendererSetMirrorDisplay = (event) => {
      if (state.cameraFeedRenderer) {
        state.cameraFeedRenderer.destroy()
      }
      state.cameraFeedRenderer = XR8.GlTextureRenderer.create({
        GLctx,
        mirroredDisplay: event.detail.mirroredDisplay,
      })
    }

    const HeadsetSessionManager = () => {
      let cameraLoadId_ = 0  // Monotonic counter to handle cancelling camera feed initialization.
      let runOnCameraStatusChange_ = null
      let paused_ = false
      let isResuming_ = false
      let sessionType_ = ''
      let session_ = null
      let sessionInit_ = null
      let frame_ = null
      let lastFrameTime_ = -1
      let frameTime_ = 0
      let xrInputManager_ = null

      let sceneScale_ = 0
      const originTransform_ = new window.THREE.Matrix4()

      let didCameraLoopStop_ = false
      let didCameraLoopPause_ = false

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

      const groundRotation = (quat) => {
        const e = new THREE.Euler(0, 0, 0, 'YXZ')
        e.setFromQuaternion(quat)
        return new THREE.Quaternion().setFromEuler(new THREE.Euler(0, e.y, 0, 'YXZ'))
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
        state.initialSceneCameraPosition.decompose(initPos, initRot, unusedScale)
        if (state.sceneCamera) {
          if (state.addedVoidSpace) {
            state.addedVoidSpace.rescale(state.initialSceneCameraPosition, state.sceneCamera)
          }
        }

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

      // Scale the scene base on your starting height. It should change the scale of the scene based
      // on the starting height of the camera.
      const fixScale = (frame) => {
        if (!frame) {
          return
        }
        const referenceSpace = renderer.xr.getReferenceSpace()
        const pose = frame.getViewerPose(referenceSpace)

        if (!pose) {
          return
        }

        const {views} = pose

        if (!views.length) {
          return
        }

        if (!sceneScale_ || needsRecenter_) {
          recenter(views[0].transform.matrix)
        }

        if (!sceneScale_) {
          return
        }

        // update scene active camera world matrix to match XR camera world matrix
        const {sceneCameraObj: sceneCamObj, sceneCamera: sceneCam} = state

        // In Aframe 1.3.0, they split out WebXRManager's updateCamera() from getCamera(). This
        // check makes our code work for both.
        if (renderer.xr.updateCamera) {
          renderer.xr.updateCamera(sceneCam)
        }

        const cameraVR = renderer.xr.getCamera(sceneCam)
        updateOrigin(cameraVR)

        cameraVR.matrix.decompose(sceneCamObj.position, sceneCamObj.quaternion, sceneCamObj.scale)
        sceneCamObj.updateMatrix()
        sceneCamObj.updateMatrixWorld(true)

        sceneCam.projectionMatrix.copy(cameraVR.projectionMatrix)
        sceneCam.projectionMatrixInverse.copy(cameraVR.projectionMatrixInverse)

        cameraVR.matrix.premultiply(sceneCam.matrixWorldInverse)
        cameraVR.matrix.decompose(cameraVR.position, cameraVR.quaternion, cameraVR.scale)

        cameraVR.cameras.forEach((camera) => {
          updateOrigin(camera)
          // In Three.js r151 and up (used by AFrame 1.5.0), camera.matrix is already updated by
          // WebXRManager's updateCamera so we don't need to call this anymore.
          if (parseFloat(window.THREE.REVISION) < 151) {
            camera.matrix.premultiply(sceneCam.matrixWorldInverse)
          }
        })
      }

      const onSessionStarted = () => {
        xrInputManager_.attach({
          renderer,
          canvas: state.aScene.canvas,
          scene: state.aScene.object3D,
          camera: state.sceneCamera,
          pauseSession: () => state.aScene.exitVR(),
          sessionConfiguration: state.runConfig.sessionConfiguration,
        })
        xrInputManager_.setOriginTransform(originTransform_, sceneScale_)
        if (!isResuming_) {
          xrInputManager_.moveCameraChildrenToController()
        }
        isResuming_ = false
      }

      const onSessionEnded = () => {
        state.aScene.removeEventListener('exit-vr', onSessionEnded)
        if (xrInputManager_) {
          xrInputManager_.detach()
        }

        // If the user didn't call XR8.pause() or XR8.stop(), it's a system stop which should pause
        // the camera runtime and try to auto-resume.
        if (!didCameraLoopPause_ && !didCameraLoopStop_) {
          window.XR8.pause()
          showResumeImmersivePrompt(sessionType_).then(() => XR8.resume())  // auto resume.
        }
        didCameraLoopPause_ = false
        didCameraLoopStop_ = false
      }

      const initialize = async ({runOnCameraStatusChange, config}): Promise<void> => {
        if (config.allowedDevices !== XR8.XrConfig.device().MOBILE_AND_HEADSETS &&
          config.allowedDevices !== XR8.XrConfig.device().ANY) {
          throw new Error('user requested config doesn\'t allow headsets.')
        }

        const rA = parseFloat(window.AFRAME.version)
        if (rA < 1.1) {
          throw new Error(
            `Detected aframe version ${rA}, headset session requires version 1.1.0 or later.`
          )
        }

        runOnCameraStatusChange_ = runOnCameraStatusChange

        const deviceInfo = await XrDeviceFactory().deviceInfo()
        const browserName = deviceInfo.browser.name
        sessionType_ = await getSupportedHeadsetSessionType(browserName, deviceInfo.model)
        if (!sessionType_) {
          throw (new Error('aframe headset session only supports headsets.'))
        }
        // We know now that we want to start an immersive session. Create the input manager so we
        // can start doing any needed prework.
        xrInputManager_ = XrInputManagerFactory()

        if (!state.runConfig.sessionConfiguration.disableXrTablet) {
          xrInputManager_.initializeTablet()
        }
      }

      const startHeadsetSession = (loadId) => {
        if (loadId !== cameraLoadId_) {
          return Promise.resolve(true)
        }

        state.aScene.addEventListener('exit-vr', onSessionEnded)
        const {webxr} = state.aScene.systems
        webxr.sessionReferenceSpaceType = 'local-floor'
        webxr.sessionConfiguration.requiredFeatures = ['local-floor']
        webxr.sessionConfiguration.optionalFeatures =
          ['hand-tracking', 'dom-overlay', 'plane-detection']

        didCameraLoopPause_ = false
        didCameraLoopStop_ = false

        sessionInit_ = webxr.sessionConfiguration

        runOnCameraStatusChange_(
          {status: 'requestingWebXr', sessionType: sessionType_, sessionInit: sessionInit_}
        )

        let resolveFirstFrame
        const firstFramePromise = new Promise((resolve) => {
          resolveFirstFrame = resolve
        })

        state.headsetSessionTick = (time, frame) => {
          frame_ = frame
          lastFrameTime_ = frameTime_
          frameTime_ = time
          fixScale(frame)

          if (resolveFirstFrame) {
            resolveFirstFrame()
            resolveFirstFrame = null
          }

          if (xrInputManager_ && xrInputManager_.isAttached()) {
            xrInputManager_.update(frame)
          }
        }

        if (isResuming_) {
          // We need the scene to be playing to enter vr.
          state.aScene.play()
        }

        const startPromise =
          sessionType_ === 'immersive-vr' ? state.aScene.enterVR() : state.aScene.enterAR()

        return startPromise.then(() => {
          session_ = state.aScene.xrSession
          runOnCameraStatusChange_({
            status: 'hasContextWebXr',
            sessionType: sessionType_,
            sessionInit: sessionInit_,
            session: session_,
            rendersOpaque: session_.environmentBlendMode === 'opaque' ||
              // TODO (tri) remove sessionType check here once Vision Pro fix their spec
              (session_.environmentBlendMode === undefined && sessionType_ === 'immersive-vr'),
          })
          onSessionStarted()
        }).then(() => firstFramePromise).catch((e) => {
          state.headsetSessionTick = () => {}
          throw e
        })
      }

      // We obtain headset permissions by starting a session.
      const obtainPermissions = () => startHeadsetSession(cameraLoadId_).catch(
        () => showImmersivePrompt(sessionType_).then(
          () => startHeadsetSession(cameraLoadId_)
        )
      )

      const stop = () => {
        didCameraLoopStop_ = true
        xrInputManager_ = null
        sessionType_ = ''

        // Increment cameraLoadId to ensure camera loads are cancelled
        ++cameraLoadId_

        hideImmersivePrompt()

        state.headsetSessionTick = () => {}

        const obj = state.sceneCameraObj
        obj.matrix.copy(state.initialSceneCameraPosition)
        obj.matrix.decompose(obj.position, obj.quaternion, obj.scale)
        obj.updateMatrixWorld(true)

        if (state.aScene.xrSession) {
          state.aScene.exitVR()
        }
      }

      // We obtain headset permissions by starting a session.
      const startSession = () => (session_ ? Promise.resolve() : obtainPermissions())

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
        providesRunLoop: false,
        supportsHtmlEmbedded: true,
        supportsHtmlOverlay: false,
        usesMediaDevices: false,
        usesWebXr: true,
      })

      const pauseSession = () => {
        ++cameraLoadId_
        paused_ = true
        didCameraLoopPause_ = true
        state.headsetSessionTick = () => {}
        if (state.aScene.xrSession) {
          state.aScene.exitVR().then(
            () => window.requestAnimationFrame(() => state.aScene.pause())
          )
        }
      }

      return {
        initialize,
        obtainPermissions,
        stop,
        start: startSession,
        pause: pauseSession,
        resume,
        resumeIfAutoPaused,
        hasNewFrame,
        frameStartResult,
        providesRunLoop: () => false,  // provided by outer component.
        getSessionAttributes,
      }
    }

    const processCameraConfigured = (event) => {
      needsRecenter_ = needsRecenter_ || !!event.detail.recenter
      if (!event.detail.origin) {
        return
      }
      const {position, rotation} = event.detail.origin
      const q = new THREE.Quaternion(rotation.x, rotation.y, rotation.z, rotation.w)
      state.initialSceneCameraPosition.compose(position, q, new THREE.Vector3(1, 1, 1))

      if (state.sceneCamera) {
        if (state.addedVoidSpace) {
          state.addedVoidSpace.rescale(state.initialSceneCameraPosition, state.sceneCamera)
        }
        if (state.desktopCameraManager) {
          state.desktopCameraManager.recenter(state.initialSceneCameraPosition, state.sceneCamera)
        }
      }
    }

    const updateWithNewReality = ({frameStartResult, processCpuResult}) => {
      const {
        textureWidth: videoWidth,
        textureHeight: videoHeight,
        session,
      } = frameStartResult
      const canvasWidth = state.aScene.canvas.width
      const canvasHeight = state.aScene.canvas.height

      const {reality, facecontroller, layerscontroller} = processCpuResult

      const realityTexture =
        (reality && reality.realityTexture) ||
        (facecontroller && facecontroller.cameraFeedTexture) ||
        (layerscontroller && layerscontroller.cameraFeedTexture)

      if (!realityTexture && !session) {
        state.aScene.renderer.autoClearDepth = false
        state.cameraFeedRenderer.render({
          renderTexture: state.loadingTexture,
          viewport: state.videoViewport,
        })
        return
      }

      maybeUpdateCameraParams({videoWidth, videoHeight, canvasWidth, canvasHeight})
      state.framesSinceLoad += 1

      if (!state.useRealityPosition) {
        state.aScene.renderer.autoClearColor = true
        state.aScene.renderer.autoClearDepth = true
        if (state.firstTick) {
          state.firstTick = false
          state.aScene.emit('realityready', {}, false)
        }
        return
      }

      if (state.framesSinceLoad <= FIRST_FRAME_DELAY) {
        if (state.framesSinceLoad === FIRST_FRAME_DELAY) {
          // TODO(nb): Sometimes the camera starts rotated for an unknown reason / race condition.
          // This recenter fixes it, but we should fix it at the source.
          state.aScene.emit('recenter')
        }
        state.aScene.renderer.autoClearDepth = false
        state.cameraFeedRenderer.render({
          renderTexture: state.loadingTexture,
          viewport: state.videoViewport,
        })
        return
      }

      if (state.firstTick) {
        state.firstTick = false
        state.aScene.emit('realityready', {}, false)
      }

      state.aScene.renderer.autoClearDepth = true

      const cam3 = state.aScene.camera
      const camA = cam3.el

      const {rotation, position} = (reality || facecontroller || layerscontroller)

      camA.object3D.position.set(position.x, position.y, position.z)
      camA.object3D.setRotationFromQuaternion(rotation)
      if (!camA.object3D.matrixAutoUpdate) {
        camA.object3D.updateMatrix()
      }

      if (state.doOverlay) {
        const {intrinsics} = (reality || facecontroller || layerscontroller)
        for (let i = 0; i < 16; i++) {
          cam3.projectionMatrix.elements[i] = intrinsics[i]
        }
        if (cam3.projectionMatrixInverse) {
          if (cam3.projectionMatrixInverse.invert) {
            // THREE 123 preferred version
            cam3.projectionMatrixInverse.copy(cam3.projectionMatrix).invert()
          } else {
            // Backwards compatible version
            cam3.projectionMatrixInverse.getInverse(cam3.projectionMatrix)
          }
        }
        state.cameraFeedRenderer.render({
          renderTexture: realityTexture,
          viewport: state.videoViewport,
          ...(state.layerTextureProviders.length !== 0 && {
            foregroundTexturesAndMasks: layerTextures({processCpuResult}),
            foregroundViewport: state.foregroundVideoViewport,
          }),
        })
      }
    }

    XR8.addCameraPipelineModule({
      name: 'xraframe',
      onBeforeSessionInitialize: ({sessionAttributes}) => {
        if (!sessionAttributes.fillsCameraTexture) {
          // Reject non-camera sessions (i.e. headset, dekstop) if aframe version < 1.1
          if (parseFloat(window.AFRAME.version) < 1.1) {
            // eslint-disable-next-line max-len
            throw new Error(`Detected A-Frame version ${parseFloat(window.AFRAME.REVISION)}, Non-camera Session requires A-Frame 1.1.0 or later`)
          }
        }
      },
      onException: (e) => {
        // eslint-disable-next-line no-console
        console.error('XR threw an exception', e)
        if (e && e.stack) {
          // eslint-disable-next-line no-console
          console.error(e.stack)
        }
        if (!state.suppressFurtherErrors) {
          state.aScene.emit('realityerror', {
            compatibility: getCompatibility(),
            error: e,
            isDeviceBrowserSupported: XR8.XrDevice.isDeviceBrowserCompatible(state.runConfig),
          }, false)
        }
      },
      onUpdate: updateWithNewReality,
      onAttach,
      onDetach,
      onSessionAttach,
      onSessionDetach,
      onResume: () => { state.paused = false },
      onCameraStatusChange: (data) => {
        state.aScene.emit('camerastatuschange', data)
      },
      listeners: [
        {event: 'reality.cameraconfigured', process: processCameraConfigured},
        {event: 'reality.imageloading', process: realityEventEmitter},
        {event: 'reality.imagescanning', process: realityEventEmitter},
        {event: 'reality.imagefound', process: realityEventEmitter},
        {event: 'reality.imageupdated', process: realityEventEmitter},
        {event: 'reality.imagelost', process: realityEventEmitter},
        {event: 'reality.trackingstatus', process: realityEventEmitter},
        {event: 'reality.meshfound', process: realityEventEmitter},
        {event: 'reality.meshupdated', process: realityEventEmitter},
        {event: 'reality.meshlost', process: realityEventEmitter},
        {event: 'reality.projectwayspotscanning', process: realityEventEmitter},
        {event: 'reality.projectwayspotfound', process: realityEventEmitter},
        {event: 'reality.projectwayspotupdated', process: realityEventEmitter},
        {event: 'reality.projectwayspotlost', process: realityEventEmitter},

        {event: 'facecontroller.cameraconfigured', process: processCameraConfigured},
        {event: 'facecontroller.faceloading', process: realityEventEmitter},
        {event: 'facecontroller.facescanning', process: realityEventEmitter},
        {event: 'facecontroller.facefound', process: realityEventEmitter},
        {event: 'facecontroller.faceupdated', process: realityEventEmitter},
        {event: 'facecontroller.facelost', process: realityEventEmitter},
        {event: 'facecontroller.mouthopened', process: realityEventEmitter},
        {event: 'facecontroller.mouthclosed', process: realityEventEmitter},
        {event: 'facecontroller.lefteyeopened', process: realityEventEmitter},
        {event: 'facecontroller.lefteyeclosed', process: realityEventEmitter},
        {event: 'facecontroller.righteyeopened', process: realityEventEmitter},
        {event: 'facecontroller.righteyeclosed', process: realityEventEmitter},
        {event: 'facecontroller.lefteyebrowraised', process: realityEventEmitter},
        {event: 'facecontroller.lefteyebrowlowered', process: realityEventEmitter},
        {event: 'facecontroller.righteyebrowraised', process: realityEventEmitter},
        {event: 'facecontroller.righteyebrowlowered', process: realityEventEmitter},
        {event: 'facecontroller.righteyewinked', process: realityEventEmitter},
        {event: 'facecontroller.lefteyewinked', process: realityEventEmitter},
        {event: 'facecontroller.blinked', process: realityEventEmitter},
        {event: 'facecontroller.interpupillarydistance', process: realityEventEmitter},
        {event: 'facecontroller.mirrordisplay', process: rendererSetMirrorDisplay},

        {event: 'facecontroller.earfound', process: realityEventEmitter},
        {event: 'facecontroller.earlost', process: realityEventEmitter},
        {event: 'facecontroller.earpointfound', process: realityEventEmitter},
        {event: 'facecontroller.earpointlost', process: realityEventEmitter},

        {event: 'layerscontroller.cameraconfigured', process: processCameraConfigured},
        {event: 'layerscontroller.layerscanning', process: realityEventEmitter},
        {event: 'layerscontroller.layerloading', process: realityEventEmitter},
        {event: 'layerscontroller.layerfound', process: realityEventEmitter},
      ],
      sessionManager: HeadsetSessionManager,
    })

    if (!document.getElementsByTagName('a-camera').length) {
      // This is a developer setup error, the scene is incomplete
      // eslint-disable-next-line no-alert
      window.alert('You need an <a-camera> element for augmented reality.')
      throw new Error('You need an <a-camera> element for augmented reality.')
    }

    state.aScene.emit('xrconfigdata', {data: this.data})

    if (!XR8.XrDevice.isDeviceBrowserCompatible(state.runConfig)) {
      // We don't expect the camera to run due to lack of compatibility, but we still need to try
      // so that error handling can propagate to pipeline modules.
      // TODO(nb): throw an error from engine.js onStart() / onVideoStatusChange() if incompatible.
      state.suppressFurtherErrors = true
      xrRun()
      state.aScene.emit('realityerror', {
        // There is no error here as we use compatibility below to display messaging.
        compatibility: getCompatibility(),
        isDeviceBrowserSupported: false,
      }, false)

      return
    }

    // delayRun is an undocumented feature only used in jini. Is it needed?
    if (this.data.delayRun) {
      state.aScene.addEventListener('runreality', xrRun)
    } else {
      xrRun()
    }
  }

  function pause() {
    XR8.pause()
    state.paused = true
  }

  function play() {
    removeHarmfulCameraElementDefaults()
    if (state.paused) {
      // state.paused will be set to false when xr has resumed.
      XR8.resume()
    } else {
      state.framesSinceLoad = 0
      state.firstTick = true
    }
  }

  function tick() {
    // tick could still be called even if xr is not running. realityDidInitialize (below) would
    // always be false. The current local behavior is to render the screen instead of white. Does
    // drawing white below even do anything?
    if (!XR8.XrDevice.isDeviceBrowserCompatible(state.runConfig) || state.paused) {
      return
    }

    if (!state.realityDidInitialize || !state.framesSinceLoad || !state.videoViewport.width ||
      !state.foregroundVideoViewport.width) {
      maybeUpdateCameraParams({
        videoWidth: state.captureSize.w,
        videoHeight: state.captureSize.h,
        canvasWidth: state.aScene.canvas.width,
        canvasHeight: state.aScene.canvas.height,
      })

      if (!state.useRealityPosition) {
        state.aScene.renderer.autoClearDepth = true
        state.aScene.renderer.autoClearColor = true
      } else {
        state.aScene.renderer.autoClearDepth = false
        state.aScene.renderer.autoClearColor = false
        state.cameraFeedRenderer.render({
          renderTexture: state.loadingTexture,
          viewport: state.videoViewport,
        })
      }
    }

    if (state.realityDidInitialize) {
      XR8.runPreRender(Date.now())
    }

    if (state.aScene.frame) {
      state.headsetSessionTick(state.aScene.time, state.aScene.frame)
    }
  }

  function tock() {
    if (!state.realityDidInitialize) {
      return
    }
    XR8.runPostRender()
  }

  return {
    schema: xrConfigSchema,
    init,
    play,
    pause,
    tick,
    tock,
    remove,
  }
}

export {xrconfigComponent}
