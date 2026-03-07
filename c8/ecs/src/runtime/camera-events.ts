import type {
  ActiveCameraChangeEvent, ActiveCameraEidChangeEvent,
} from './camera-manager-types'
import {events} from './event-ids'

const ACTIVE_CAMERA_CHANGE = 'active-camera-change' as const
const ACTIVE_CAMERA_EID_CHANGE = 'active-camera-entity-change' as const

const XR_CAMERA_EDIT = 'xr-camera-edit' as const
const XR_CAMERA_STOP = 'xr.stop' as const

const XR_FACE_FOUND = events.FACE_FOUND
const XR_FACE_UPDATED = events.FACE_UPDATED
const XR_FACE_LOST = events.FACE_LOST

const CAMERA_TRANSFORM_UPDATE = 'cameraupdate' as const

const CameraEvents = {
  ACTIVE_CAMERA_CHANGE,
  ACTIVE_CAMERA_EID_CHANGE,
  XR_CAMERA_EDIT,
  XR_CAMERA_STOP,
  CAMERA_TRANSFORM_UPDATE,
}

export {
  CameraEvents,
  XR_FACE_FOUND,
  XR_FACE_UPDATED,
  XR_FACE_LOST,
}

export type {
  ActiveCameraChangeEvent,
  ActiveCameraEidChangeEvent,
}
