import * as Types from '../types'

import {registerAttribute} from '../registry'

const Face = registerAttribute('face', {
  id: Types.i32,
  addAttachmentState: Types.boolean,
}, {
  id: 1,
  addAttachmentState: false,
})

export {Face}
