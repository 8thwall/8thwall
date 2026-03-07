import type {CameraObject} from './camera-manager-types'
import type {Scene} from './three-types'

interface PointerApi {
  attach: () => void
  detach: () => void
}

type RaycastStage = {
  scene: Scene
  getCamera: () => CameraObject
  includeWorldPosition: boolean
}

export type {
  PointerApi,
  RaycastStage,
}
