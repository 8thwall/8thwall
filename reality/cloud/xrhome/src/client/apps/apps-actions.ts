import {dispatchify, onError} from '../common'
import type {DispatchifiedActions} from '../common/types/actions'

// TODO(christoph): Add topRadius/bottomRadius for conical
type TargetGeometry = {
  originalWidth: number
  originalHeight: number
  isRotated: boolean
  topRadius?: number
  bottomRadius?: number
}

const unimplemented = (name: string): any => () => ({
  type: 'ERROR',
  // eslint-disable-next-line local-rules/hardcoded-copy
  msg: `${name} is not supported in offline mode`,
})

const rawActions = {
  error: onError,
  uploadImageTarget: unimplemented('uploadImageTarget'),
  updateImageTarget: unimplemented('updateImageTarget'),
  deleteImageTarget: unimplemented('deleteImageTarget'),
  testImageTarget: unimplemented('testImageTarget'),
  testImageTargetClear: unimplemented('testImageTargetClear'),
}

 type AppsActions = DispatchifiedActions<typeof rawActions>

export default dispatchify(rawActions)

export {
  rawActions,
}

export type {
  AppsActions,
  TargetGeometry,
}
