import type {DeepReadonly} from 'ts-essentials'

import type {SceneGraph, XrCameraType} from '../shared/scene-graph'
import THREE from './three'
import type {World} from './world'
import type {InternalEffectsManager} from './effects-manager-types'
import {createWorld} from './world-create'
import type {IApplication} from '../shared/application-type'
import {ready} from './init-ready'
import {getActiveCameraFromSceneGraph} from '../shared/get-camera-from-scene-graph'
import {maybeRefreshWorldMatrices} from './matrix-refresh'
import {createMotionRenderPass} from './motion-render-pass'
import type {CameraObject} from './camera-manager-types'
import {THREE_LAYERS} from '../shared/three-layers'

const getCameraTypeFromWorld = (world: World): XrCameraType => {
  const activeCamera = world.camera.getActiveEid() ?? null
  if (activeCamera) {
    const cameraObject = world.three.entityToObject.get(activeCamera)
    return cameraObject?.userData?.camera?.userData?.xr?.xrCameraType ?? '3dOnly'
  }
  return '3dOnly'
}

const getCameraTypeFromSceneGraph = (
  sceneGraph: DeepReadonly<SceneGraph>
): XrCameraType => {
  const activeCameraObject = getActiveCameraFromSceneGraph(sceneGraph, sceneGraph.entrySpaceId)
  return activeCameraObject?.camera?.xr?.xrCameraType ?? '3dOnly'
}

const mountApplication = (sceneGraph: DeepReadonly<SceneGraph>) => {
  let needsSkyboxUpdate = false
  let pendingCameraType: XrCameraType | null = null

  const scene = new THREE.Scene()

  // Read the current cameraType and update the skybox
  const updateSkybox = async (world: World, cameraType: XrCameraType) => {
    const internalEffects = world.effects as InternalEffectsManager
    if (cameraType !== '3dOnly') {
      internalEffects._hideSkyForXr()
      return
    }

    internalEffects._showSkyForXr()
  }

  const requestSkyboxUpdate = (cameraType: XrCameraType) => {
    pendingCameraType = cameraType
    needsSkyboxUpdate = true
  }

  // On initialization, the world is not yet constructed, we need to inspect the sceneGraph
  // for the camera type
  requestSkyboxUpdate(getCameraTypeFromSceneGraph(sceneGraph))

  const camera = new THREE.PerspectiveCamera(
    75, window.innerWidth / window.innerHeight, 0.1, 2000
  )
  camera.position.set(0, 5, 10)
  camera.lookAt(scene.position)
  camera.layers.enable(THREE_LAYERS.renderedNotRaycasted)
  let currentCamera: CameraObject = camera

  const renderer = new THREE.WebGLRenderer({antialias: true})
  renderer.shadowMap.enabled = true
  // TODO(dat): conditional on dev8 debug mode?
  renderer.debug.checkShaderErrors = false

  document.body.appendChild(renderer.domElement)
  document.body.style.margin = '0'

  // disable default touch and selection behavior
  renderer.domElement.style.touchAction = 'none'
  renderer.domElement.style.userSelect = 'none'
  renderer.domElement.style.webkitUserSelect = 'none'

  const world = createWorld(scene, renderer, camera)

  // Set default matrix update mode to manual
  world.three.setMatrixUpdateMode('manual')

  const updateCameraAspectRatio = () => {
    if (currentCamera instanceof THREE.PerspectiveCamera) {
      currentCamera.aspect = (window.innerWidth / window.innerHeight)
    }
    currentCamera.updateProjectionMatrix()
  }

  const changeCamera = (newCamera: CameraObject) => {
    currentCamera = newCamera
    updateCameraAspectRatio()
    requestSkyboxUpdate(getCameraTypeFromWorld(world))
  }

  // Start the world
  const sceneHandle = world.loadScene(sceneGraph, handle => world.setSceneHook(handle))
  world.pointer.attach()
  world.input.attach()
  world.camera.attach()
  world.xr.attach()
  world.effects.attach()

  const motionPass = createMotionRenderPass(scene)

  const render = (dt: number, frame?: any) => {
    if (needsSkyboxUpdate && pendingCameraType) {
      updateSkybox(world, pendingCameraType)
      needsSkyboxUpdate = false
      pendingCameraType = null
    }

    world.tick(dt)
    maybeRefreshWorldMatrices(scene)
    if (world.three.activeCamera !== currentCamera) {
      changeCamera(world.three.activeCamera)
    }

    if (currentCamera) {
      renderer?.render(scene, currentCamera)
    }

    if (world.three.scene.userData.spaceWarpEnabled) {
      motionPass.render(renderer, frame)
    }

    world.tock()
  }

  world.xr.setEcsRenderOverride({
    engage: () => {
      // User wants us to stop our loop and give them a function to call so rendering happens
      renderer?.setAnimationLoop(null)
    },
    disengage: () => {
      // User no longer wants loop themselves, we should set up our loop
      renderer?.setAnimationLoop(render)
    },
    render: (dt: number) => {
      render(dt)
    },
  })

  const onWindowResize = () => {
    updateCameraAspectRatio()
    renderer?.setSize(window.innerWidth, window.innerHeight)
    renderer?.setPixelRatio(window.devicePixelRatio)
  }

  onWindowResize()

  // NOTE(yuyansong): our renderer will loop the render() function at the start.
  // XR might null this when XR mode is engaged.
  renderer.setAnimationLoop(render)
  window.addEventListener('resize', onWindowResize)

  window.dispatchEvent(new Event('ecsInit'))

  const cleanup = () => {
    window.removeEventListener('resize', onWindowResize)
    world.destroy()
    renderer.domElement.remove()
    renderer.setAnimationLoop(null)
    renderer.dispose()
    renderer.forceContextLoss()
  }

  return {
    world,
    sceneHandle,
    cleanup,
  }
}

const makeApplication = (): IApplication => {
  let active: ReturnType<typeof mountApplication>
  return {
    getWorld: () => active?.world,
    getScene: () => active?.sceneHandle,
    init: async (sceneGraph: DeepReadonly<SceneGraph>) => {
      await ready()
      active?.cleanup()
      active = mountApplication(sceneGraph)
    },
  }
}

const application = makeApplication()

export {
  // TODO(christoph): Remove ecs.Application after migration period
  application as Application,
  application,
}
