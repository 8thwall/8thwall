import * as Types from '../types'

import {registerAttribute} from '../registry'

const Splat = registerAttribute('splat', {
  url: Types.string,
  skybox: Types.boolean,
})

export {Splat}
