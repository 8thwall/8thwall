import type {Quaternion} from '../quaternion'

interface Position {
  x: number
  y: number
  z: number
}

interface Pose {
  position: Position
  rotation: Quaternion
}

interface Uv {
  u: number
  v: number
}

interface OptionalPose {
  position?: Position
  rotation?: Quaternion
}

interface ImageData2D {
  rows: number
  cols: number
  rowBytes: number
  pixels: Uint8Array
}

interface Coordinates {
  // The position and rotation of the camera.
  // [Optional] {position: {x, y, z}, rotation: {w, x, y, z}} of the camera.
  origin?: OptionalPose
  // [Optional] scale of the scene.
  scale?: number
  // The axes.
  // [Optional] 'LEFT_HANDED' or 'RIGHT_HANDED', default is 'RIGHT_HANDED'
  axes?: 'LEFT_HANDED' | 'RIGHT_HANDED'
  // [Optional] If true, flip left and right in the output.
  mirroredDisplay?: boolean
}

type XrPermission = 'camera' | 'devicemotion' | 'deviceorientation' | 'geolocation' | 'microphone'

type XrDevice = 'any' | 'mobile-and-headsets' | 'mobile-only'

type CameraDirection = 'front' | 'back' | 'any'

/* eslint-disable camelcase */
type Compatibility = {
  os?: string
  has_device?: boolean
  has_browser?: boolean
  has_user_media?: boolean
  has_web_assembly?: boolean
  has_device_orientation?: boolean
  browser?: string
  inAppBrowser?: string
  inAppBrowserType?: string
}
/* eslint-enable camelcase */

export type {
  Position,
  Coordinates,
  Uv,
  Pose,
  XrPermission,
  XrDevice,
  CameraDirection,
  Compatibility,
  ImageData2D,
}
