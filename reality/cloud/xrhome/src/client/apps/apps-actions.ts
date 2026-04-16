import {dispatchify, onError} from '../common'
import type {DispatchifiedActions} from '../common/types/actions'

const unimplemented = (name: string): any => () => ({
  type: 'ERROR',
  // eslint-disable-next-line local-rules/hardcoded-copy
  msg: `${name} is not supported in offline mode`,
})

const rawActions = {
  error: onError,
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
}
