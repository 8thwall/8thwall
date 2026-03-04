import THREE from './three'
import type {Camera} from './components'
import type {CameraObject} from './camera-manager-types'
import type {SchemaOf} from './world-attribute'
import type {ReadData} from '../shared/schema'
import {THREE_LAYERS} from '../shared/three-layers'

type CameraConfig = ReadData<SchemaOf<typeof Camera>>

const sameCameraType = (object: CameraObject, type: string) => {
  switch (type) {
    case 'perspective':
      return object instanceof THREE.PerspectiveCamera
    case 'orthographic':
      return object instanceof THREE.OrthographicCamera
    default:
      throw new Error('Unexpected camera type')
  }
}

const editCameraObject = (object: CameraObject, config: CameraConfig) => {
  object.zoom = config.zoom
  if (object instanceof THREE.OrthographicCamera) {
    object.left = config.left
    object.right = config.right
    object.top = config.top
    object.bottom = config.bottom
  } else if (object instanceof THREE.PerspectiveCamera) {
    object.fov = config.fov
  }
  object.far = config.farClip
  object.near = config.nearClip
  object.updateProjectionMatrix()
}

const XR_CAMERA_PROPERTIES = [
  'xrCameraType',
  'phone',
  'desktop',
  'headset',
  'nearClip',
  'farClip',
  'leftHandedAxes',
  'uvType',
  'direction',
  'disableWorldTracking',
  'enableLighting',
  'enableWorldPoints',
  'scale',
  'enableVps',
  'mirroredDisplay',
  'meshGeometryFace',
  'meshGeometryEyes',
  'meshGeometryIris',
  'meshGeometryMouth',
  'enableEars',
  'maxDetections',
] as const

const editXrCameraObject = (object: CameraObject, config: CameraConfig) => {
  XR_CAMERA_PROPERTIES.forEach((property) => {
    object.userData.xr[property] = config[property]
  })
}

const createCameraObject = (config: CameraConfig): CameraObject => {
  let cameraObject: CameraObject
  switch (config.type) {
    case 'perspective':
      cameraObject = new THREE.PerspectiveCamera(config.fov)
      break
    case 'orthographic':
      cameraObject =
        new THREE.OrthographicCamera(
          config.left,
          config.right,
          config.top,
          config.bottom
        )
      break
    default:
      throw new Error('Unexpected camera type')
  }
  cameraObject.zoom = config.zoom
  cameraObject.far = config.farClip
  cameraObject.near = config.nearClip
  cameraObject.updateProjectionMatrix()
  cameraObject.rotateOnAxis(new THREE.Vector3(0, 1, 0), Math.PI)
  cameraObject.layers.enable(THREE_LAYERS.renderedNotRaycasted)
  cameraObject.userData.xr = {
    xrCameraType: config.xrCameraType,
    phone: config.phone,
    desktop: config.desktop,
    headset: config.headset,
    nearClip: config.nearClip,
    farClip: config.farClip,
    disableWorldTracking: config.disableWorldTracking,
    enableLighting: config.enableLighting,
    enableWorldPoints: config.enableWorldPoints,
    leftHandedAxes: config.leftHandedAxes,
    mirroredDisplay: config.mirroredDisplay,
    scale: config.scale,
    enableVps: config.enableVps,
    meshGeometryFace: config.meshGeometryFace,
    meshGeometryEyes: config.meshGeometryEyes,
    meshGeometryIris: config.meshGeometryIris,
    meshGeometryMouth: config.meshGeometryMouth,
    uvType: config.uvType,
    maxDetections: config.maxDetections,
    enableEars: config.enableEars,
    direction: config.direction,
  }
  return cameraObject
}

export {
  XR_CAMERA_PROPERTIES,
  editXrCameraObject,
  sameCameraType,
  createCameraObject,
  editCameraObject,
}

export type {
  CameraConfig,
}
