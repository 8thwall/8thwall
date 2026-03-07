import * as Types from '../types'

import {registerAttribute} from '../registry'
import {CAMERA_DEFAULTS} from '../../shared/camera-constants'

const Camera = registerAttribute('camera', {
  type: Types.string,
  fov: Types.f32,
  zoom: Types.f32,
  left: Types.i32,
  right: Types.i32,
  top: Types.i32,
  bottom: Types.i32,

  xrCameraType: Types.string,
  phone: Types.string,
  desktop: Types.string,
  headset: Types.string,
  nearClip: Types.f32,
  farClip: Types.f32,
  leftHandedAxes: Types.boolean,
  uvType: Types.string,
  direction: Types.string,
  // World
  disableWorldTracking: Types.boolean,
  enableLighting: Types.boolean,
  enableWorldPoints: Types.boolean,
  scale: Types.string,
  enableVps: Types.boolean,
  // Face
  mirroredDisplay: Types.boolean,
  meshGeometryFace: Types.boolean,
  meshGeometryEyes: Types.boolean,
  meshGeometryIris: Types.boolean,
  meshGeometryMouth: Types.boolean,
  enableEars: Types.boolean,
  maxDetections: Types.i32,
}, CAMERA_DEFAULTS)

export {Camera}
