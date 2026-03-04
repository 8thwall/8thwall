import * as Types from '../types'

import {registerAttribute} from '../registry'
import {LIGHT_DEFAULTS} from '../../shared/light-constants'

const Light = registerAttribute('light', {
  type: Types.string,
  r: Types.ui8,
  g: Types.ui8,
  b: Types.ui8,
  intensity: Types.f32,
  castShadow: Types.boolean,
  targetX: Types.f32,
  targetY: Types.f32,
  targetZ: Types.f32,
  shadowNormalBias: Types.f32,
  shadowBias: Types.f32,
  shadowAutoUpdate: Types.boolean,
  shadowBlurSamples: Types.ui32,
  shadowRadius: Types.f32,
  shadowMapSizeHeight: Types.i32,
  shadowMapSizeWidth: Types.i32,
  shadowCameraNear: Types.f32,
  shadowCameraFar: Types.f32,
  shadowCameraLeft: Types.f32,
  shadowCameraRight: Types.f32,
  shadowCameraTop: Types.f32,
  shadowCameraBottom: Types.f32,
  distance: Types.f32,
  decay: Types.f32,
  followCamera: Types.boolean,
  angle: Types.f32,
  penumbra: Types.f32,
  colorMap: Types.string,
  width: Types.f32,
  height: Types.f32,
}, LIGHT_DEFAULTS)

export {Light}
