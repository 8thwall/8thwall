// @sublibrary(three-types)
import type {Lights} from './lights-types'
import type {CameraObject} from './camera-manager-types'
import type {GLTF, AnimationClip, Object3D, Matrix4} from './three-types'
import type {Eid} from '../shared/schema'

// TODO(christoph): Code isn't currently depending on this type, but it needs to be defined
// independently of the Camera component to avoid circular dependencies.
type XrCameraInfo = unknown

interface UserData {
  eid: Eid | null
  entity: boolean
  // Light
  light: any | Lights
  // GLTF model
  url: string
  _gltfUrl: string | null
  collider: boolean
  animationClip: any | AnimationClip
  // TODO add model interface
  model: any
  gltf: any | GLTF
  time: number
  // Splat
  lastWidth?: number
  lastHeight?: number
  isSplat: boolean
  // Camera
  camera: any | CameraObject
  xr: XrCameraInfo | null
  // Material
  // TODO add material interface
  material: any
  // UI
  ui3d: Object3D | null
  rootUi: Object3D | null
  uiOrder?: number
  // Map
  isMap: boolean

  // Motion Render Pass
  spaceWarpEnabled: boolean
  prevWorldMatrix: Matrix4
  prevCameraProjectionMatrix: Matrix4
  prevCameraMatrixWorldInverse: Matrix4
  skipMotionRenderPass: boolean

  // Manual matrix update tracking
  childMatrixNeedsUpdate: boolean
}
export type {
  UserData,
}
