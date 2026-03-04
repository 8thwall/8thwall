import * as Types from '../types'

import {registerAttribute} from '../registry'

const GltfModel = registerAttribute('gltf-model', {
  url: Types.string,
  animationClip: Types.string,
  loop: Types.boolean,
  paused: Types.boolean,
  time: Types.f32,
  timeScale: Types.f32,
  collider: Types.boolean,
  reverse: Types.boolean,
  repetitions: Types.ui32,
  crossFadeDuration: Types.f32,
}, {
  timeScale: 1,
})

export {GltfModel}
