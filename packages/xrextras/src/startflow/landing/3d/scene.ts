import type {
  AnimationClip,
  AnimationMixer,
  BufferGeometry,
  Mesh,
  MeshBasicMaterial,
  Object3D,
} from 'three'

import type {
  LandingOrbitIdle as OrbitIdle,
  LandingOrbitInteraction as OrbitInteraction,
} from '../../types'

import {getModelBoundingBox} from './bounding-box'

type AnimationName = string
type Model = {
  animations: AnimationClip[]
  scene: Object3D
  scenes?: Object3D[]
}

const BASE_AMBIENT_INTENSITY = 1
const BASE_DIRECTIONAL_INTENSITY = 0.6

const BOUNDING_BOX_SIZE = 9
const BOUNCE_AZIMUTH_LIMIT = Math.PI / 4
const BOUNCE_EDGE_EPSILON = 0.001

const createScene = (canvas: HTMLCanvasElement) => {
  const {THREE} = window
  const {OrbitControls, GLTFLoader} = THREE as any
  const scene = new THREE.Scene()

  const renderer = new THREE.WebGLRenderer({antialias: true, alpha: true, canvas})
  renderer.setPixelRatio(window.devicePixelRatio)
  renderer.outputColorSpace = THREE.SRGBColorSpace

  const camera = new THREE.PerspectiveCamera(80, 1, 1, 1000)
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
  const defaultAutoRotateSpeed = Math.abs(controls.autoRotateSpeed)

  let model: Model = null
  let mixer: AnimationMixer = null
  let animationName: string = null
  let prevTimestamp = Date.now()
  let frameRequest: ReturnType<typeof requestAnimationFrame> = null
  let skyMesh: Mesh<BufferGeometry, MeshBasicMaterial> = null
  let errorHandler: () => void = null
  let orbitIdle: OrbitIdle = 'none'

  const loader = new GLTFLoader()
  const textureLoader = new THREE.TextureLoader()

  const handleResize = () => {
    const width = Math.max(1, canvas.clientWidth)
    const height = Math.max(1, canvas.clientHeight)
    camera.aspect = width / height
    camera.updateProjectionMatrix()
    renderer.setSize(width, height, false)
  }
  const resizeObserver = typeof ResizeObserver === 'undefined'
    ? null
    : new ResizeObserver(handleResize)

  const render = (time: number) => {
    const dt = time - prevTimestamp
    frameRequest = requestAnimationFrame(render)
    renderer.render(scene, camera)
    controls.update()
    if (orbitIdle === 'bounce') {
      const azimuth = controls.getAzimuthalAngle()
      if (azimuth <= controls.minAzimuthAngle + BOUNCE_EDGE_EPSILON) {
        controls.autoRotateSpeed = -defaultAutoRotateSpeed
      } else if (azimuth >= controls.maxAzimuthAngle - BOUNCE_EDGE_EPSILON) {
        controls.autoRotateSpeed = defaultAutoRotateSpeed
      }
    }
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
        console.warn('[XRExtras StartFlow UI] Unrecognized animation:', animationName)
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

  const setModel = (newModel: Model) => {
    if (model) {
      modelBase.remove(model.scene)
      model = null
      refreshAnimation()
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
    orbitIdle = orbit
    switch (orbit) {
      case 'spin':
        controls.maxAzimuthAngle = Infinity
        controls.minAzimuthAngle = -Infinity
        controls.autoRotateSpeed = defaultAutoRotateSpeed
        controls.autoRotate = true
        break
      case 'bounce':
        controls.maxAzimuthAngle = BOUNCE_AZIMUTH_LIMIT
        controls.minAzimuthAngle = -BOUNCE_AZIMUTH_LIMIT
        controls.autoRotateSpeed = defaultAutoRotateSpeed
        controls.autoRotate = true
        break
      case 'none':
        controls.maxAzimuthAngle = Infinity
        controls.minAzimuthAngle = -Infinity
        controls.autoRotateSpeed = defaultAutoRotateSpeed
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
    resizeObserver?.disconnect()
    window.removeEventListener('resize', handleResize)
    errorHandler = null
  }

  resizeObserver?.observe(canvas)
  window.addEventListener('resize', handleResize)
  handleResize()
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
