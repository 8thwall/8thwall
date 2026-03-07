import * as Types from '../types'

import {registerAttribute} from '../registry'

const ImageTarget = registerAttribute('image-target', {
  name: Types.string,
})

export {ImageTarget}
