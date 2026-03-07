import * as Types from '../types'

import {registerAttribute} from '../registry'
import {
  DEFAULT_VOLUME, DEFAULT_DISTANCE_MODEL, DEFAULT_MAX_DISTANCE, DEFAULT_REF_DISTANCE,
  DEFAULT_ROLLOFF_FACTOR, DEFAULT_PITCH,
} from '../../shared/audio-constants'

// TODO(Julie): Add more configurable properties/options
const Audio = registerAttribute('audio', {
  url: Types.string,
  volume: Types.f32,
  loop: Types.boolean,
  paused: Types.boolean,
  pitch: Types.f32,
  positional: Types.boolean,
  refDistance: Types.f32,
  rolloffFactor: Types.f32,
  distanceModel: Types.string,
  maxDistance: Types.f32,
}, {
  volume: DEFAULT_VOLUME,
  pitch: DEFAULT_PITCH,
  maxDistance: DEFAULT_MAX_DISTANCE,
  refDistance: DEFAULT_REF_DISTANCE,
  distanceModel: DEFAULT_DISTANCE_MODEL,
  rolloffFactor: DEFAULT_ROLLOFF_FACTOR,
})

export {Audio}
