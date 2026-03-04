// @sublibrary(three-types)
import type {Eid} from '../shared/schema'
import type {OrthographicCamera, PerspectiveCamera} from './three-types'

type CameraObject = OrthographicCamera | PerspectiveCamera

type CameraManager = {
  getActiveEid: () => Eid
  setActiveEid: (eid: Eid) => void
  notifyCameraAdded: (eid: Eid) => void
  notifyCameraRemoved: (eid: Eid) => void
  attach: () => void
  detach: () => void
}

interface ActiveCameraChangeEvent {
  camera: CameraObject
}

interface ActiveCameraEidChangeEvent {
  eid: Eid
}

interface XrCameraEditEvent {
  camera: CameraObject
}

export type {
  CameraObject,
  CameraManager,
  ActiveCameraChangeEvent,
  ActiveCameraEidChangeEvent,
  XrCameraEditEvent,
}
