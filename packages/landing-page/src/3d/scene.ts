import type {AnimationClip, AnimationMixer, BufferGeometry, Mesh, MeshBasicMaterial} from 'three'

import type {AnimationName, OrbitIdle, OrbitInteraction} from '../parameters'

import {getModelBoundingBox} from './bounding-box'

const BASE_AMBIENT_INTENSITY = 1
const BASE_DIRECTIONAL_INTENSITY = 0.6

const BOUNDING_BOX_SIZE = 9

const createScene = (canvas: HTMLCanvasElement) => {
  const {THREE} = window
  const {OrbitControls, GLTFLoader} = THREE as any
  const scene = new THREE.Scene()

  const renderer = new THREE.WebGLRenderer({antialias: true, alpha: true, canvas})
  renderer.setPixelRatio(window.devicePixelRatio)
  renderer.setSize(window.innerWidth, window.innerHeight)
  renderer.outputColorSpace = THREE.SRGBColorSpace

  const camera = new THREE.PerspectiveCamera(80, window.innerWidth / window.innerHeight, 1, 1000)
  camera.position.set(5, 5, 10)

  const ambientLight = new THREE.AmbientLight(0xbbbbbb)
  ambientLight.intensity = BASE_AMBIENT_INTENSITY
  scene.add(ambientLight)

  const directionalLight = new THREE.DirectionalLight(0xffffff)

  directionalLight.position.set(-0.5, 1, 1).normalize()
  directionalLight.intensity = BASE_DIRECTIONAL_INTENSITY
  scene.add(directionalLight)

  const modelBase = new THREE.Object3D()

  scene.add(modelBase)

  const controls = new OrbitControls(camera, renderer.domElement)
  controls.enablePan = false
  controls.minDistance = 7
  controls.maxDistance = 35
  controls.target.set(0, 0, 0)
  controls.update()

  let model = null
  let mixer: AnimationMixer = null
  let animationName: string = null
  let prevTimestamp = Date.now()
  let frameRequest: ReturnType<typeof requestAnimationFrame> = null
  let skyMesh: Mesh<BufferGeometry, MeshBasicMaterial> = null
  let errorHandler: () => void = null

  const loader = new GLTFLoader()
  const textureLoader = new THREE.TextureLoader()

  const handleResize = () => {
    camera.aspect = window.innerWidth / window.innerHeight
    camera.updateProjectionMatrix()
    renderer.setSize(window.innerWidth, window.innerHeight)
  }

  const render = (time: number) => {
    const dt = time - prevTimestamp
    frameRequest = requestAnimationFrame(render)
    renderer.render(scene, camera)
    controls.update()
    if (mixer) {
      mixer.update(dt / 1000)
    }
    prevTimestamp = time
  }

  const refreshAnimation = () => {
    if (mixer) {
      mixer.stopAllAction()
      mixer.uncacheRoot(mixer.getRoot())
      mixer = null
    }

    if (!model || !model.animations) {
      return
    }

    let clips: AnimationClip[]
    if (!animationName) {
      clips = model.animations.slice(0, 1)
    } else if (animationName === 'none') {
      clips = []
    } else if (animationName === '*') {
      clips = model.animations
    } else {
      clips = model.animations.filter(a => a.name === animationName)
      if (!clips.length) {
        // eslint-disable-next-line no-console
        console.warn('[landing8] Unrecognized animation:', animationName)
      }
    }

    if (!clips.length) {
      return
    }

    mixer = new THREE.AnimationMixer(model.scene)
    const actions = clips.map(c => mixer.clipAction(c))
    actions.forEach((a) => {
      a.enabled = true
      a.play()
    })
  }

  const setModel = (newModel) => {
    if (model) {
      scene.remove(model.scene)
      model = null
    }

    if (newModel) {
      model = newModel

      const modelBounds = getModelBoundingBox(model)

      const longestEdgeLength = Math.max(
        modelBounds.xmax - modelBounds.xmin,
        modelBounds.ymax - modelBounds.ymin,
        modelBounds.zmax - modelBounds.zmin
      )

      const scale = BOUNDING_BOX_SIZE / longestEdgeLength

      modelBase.scale.set(scale, scale, scale)
      modelBase.position.set(
        modelBounds.xmax + modelBounds.xmin,
        modelBounds.ymax + modelBounds.ymin,
        modelBounds.zmax + modelBounds.zmin
      ).multiplyScalar(-0.5 * scale)

      modelBase.add(model.scene)

      refreshAnimation()
    }
  }

  const setModelSrc = (src: string) => {
    if (!src) {
      setModel(null)
      return
    }
    loader.load(src, setModel, null, (err) => {
      // eslint-disable-next-line no-console
      console.error('Model load error: ', err)
      if (errorHandler) {
        errorHandler()
      }
    })
  }

  const setLightingIntensity = (scalar: number) => {
    ambientLight.intensity = BASE_AMBIENT_INTENSITY * scalar
    directionalLight.intensity = BASE_DIRECTIONAL_INTENSITY * scalar
  }

  const setMediaAnimation = (name: AnimationName) => {
    animationName = name
    if (model) {
      refreshAnimation()
    }
  }

  const setOrbitIdle = (orbit: OrbitIdle) => {
    switch (orbit) {
      case 'spin':
        controls.maxAzimuthAngle = Infinity
        controls.minAzimuthAngle = -Infinity
        controls.autoRotate = true
        break
      case 'none':
        controls.maxAzimuthAngle = Infinity
        controls.minAzimuthAngle = -Infinity
        controls.autoRotate = false
        break
      default:
        // eslint-disable-next-line no-console
        console.warn('Unknown sceneOrbitIdle value:', orbit)
    }
  }

  const setOrbitInteraction = (orbit: OrbitInteraction) => {
    switch (orbit) {
      case 'drag':
        controls.enabled = true
        break
      case 'none':
        controls.enabled = false
        break
      default:
        // eslint-disable-next-line no-console
        console.warn('Unknown sceneOrbitInteraction value:', orbit)
    }
  }

  const loadEnvMap = (src: string) => {
    const texture = textureLoader.load(src)
    texture.mapping = THREE.EquirectangularReflectionMapping
    texture.colorSpace = THREE.SRGBColorSpace
    return texture
  }

  const setEnvMap = (src: string) => {
    if (!src) {
      scene.environment = null
      scene.remove(skyMesh)
      skyMesh = null
      return
    }

    scene.environment = loadEnvMap(src)

    if (!skyMesh) {
      skyMesh = new THREE.Mesh(
        new THREE.SphereGeometry(500, 60, 40).scale(-1, 1, 1),
        new THREE.MeshBasicMaterial({map: loadEnvMap(src)})
      )
      skyMesh.rotateY(Math.PI)
      scene.add(skyMesh)
    } else {
      skyMesh.material.map = loadEnvMap(src)
    }
  }

  const setErrorHandler = (handler_: () => void) => {
    errorHandler = handler_
  }

  const destroy = () => {
    controls.dispose()
    cancelAnimationFrame(frameRequest)
    window.removeEventListener('resize', handleResize)
    errorHandler = null
  }

  window.addEventListener('resize', handleResize)
  render(prevTimestamp)

  return {
    setModelSrc,
    setMediaAnimation,
    setLightingIntensity,
    setOrbitIdle,
    setOrbitInteraction,
    setEnvMap,
    setErrorHandler,
    destroy,
  }
}

export {
  createScene,
}
