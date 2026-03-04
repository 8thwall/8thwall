import {
  DEFAULT_VOLUME, DEFAULT_DISTANCE_MODEL, DEFAULT_MAX_DISTANCE, DEFAULT_PITCH, DEFAULT_REF_DISTANCE,
  DEFAULT_ROLLOFF_FACTOR,
} from './audio-constants'

const VIDEO_DEFAULTS = {
  loop: false,
  paused: false,
  volume: DEFAULT_VOLUME,
  positional: false,
  speed: DEFAULT_PITCH,
  refDistance: DEFAULT_REF_DISTANCE,
  rolloffFactor: DEFAULT_ROLLOFF_FACTOR,
  distanceModel: DEFAULT_DISTANCE_MODEL,
  maxDistance: DEFAULT_MAX_DISTANCE,
}

const SUPPORTED_VIDEO_FORMATS = ['video/mp4', 'video/webm']

export {
  VIDEO_DEFAULTS,
  SUPPORTED_VIDEO_FORMATS,
}
