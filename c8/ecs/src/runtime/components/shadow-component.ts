import * as Types from '../types'

import {registerAttribute} from '../registry'

const Shadow = registerAttribute('shadow', {
  castShadow: Types.boolean,
  receiveShadow: Types.boolean,
})

export {Shadow}
