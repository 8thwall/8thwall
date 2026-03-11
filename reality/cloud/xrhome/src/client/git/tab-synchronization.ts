import {makeRunQueue} from '../../shared/run-queue'
import type {AsyncThunk, Dispatch} from '../common/types/actions'

const fsRunQueue = makeRunQueue()

const withRunQueue = <
ARGS extends any[],
RET extends AsyncThunk<any>
>(dispatchFactoryFunc: (...args: ARGS) => RET) => (
    (...args: ARGS) => (dispatch: Dispatch) => fsRunQueue.next(
      () => dispatch(dispatchFactoryFunc(...args))
    )
  )

export {
  withRunQueue,
}
