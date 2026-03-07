import * as Types from '../types'

import {registerAttribute} from '../registry'
import {VIDEO_DEFAULTS} from '../../shared/video-constants'

const VideoControls = registerAttribute('video-controls', {
  loop: Types.boolean,
  paused: Types.boolean,
  volume: Types.f32,
  positional: Types.boolean,
  speed: Types.f32,
  refDistance: Types.f32,
  rolloffFactor: Types.f32,
  distanceModel: Types.string,
  maxDistance: Types.f32,
}, {
  ...VIDEO_DEFAULTS,
})

export {VideoControls}
