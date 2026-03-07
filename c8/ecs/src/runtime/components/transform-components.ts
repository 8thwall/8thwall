import * as Types from '../types'

import {registerAttribute} from '../registry'
import {
  POSITION_DEFAULTS, QUATERNION_DEFAULTS, SCALE_DEFAULTS,
} from '../../shared/transform-constants'

const Vector3 = {x: Types.f32, y: Types.f32, z: Types.f32} as const
const Vector4 = {x: Types.f32, y: Types.f32, z: Types.f32, w: Types.f32} as const

const Position = registerAttribute('position', Vector3, POSITION_DEFAULTS)

const Scale = registerAttribute('scale', Vector3, SCALE_DEFAULTS)

const Quaternion = registerAttribute('quaternion', Vector4, QUATERNION_DEFAULTS)

const ThreeObject = registerAttribute('three-object', {order: Types.f32})

export {
  Position,
  Scale,
  Quaternion,
  ThreeObject,
}
