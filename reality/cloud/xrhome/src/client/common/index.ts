import type {
  RawActions, CallableAction, DispatchifiedActions, Dispatch,
} from './types/actions'

const onError = (msg: string) => (dispatch: Dispatch) => dispatch({type: 'ERROR', msg})

/* eslint-disable-next-line arrow-parens */
const dispatchify = <T extends RawActions>(actions: T) => (dispatch: Dispatch) => (
  Object.keys(actions).reduce((o, key: keyof T) => {
    o[key] = ((...args: any) => dispatch(actions[key](...args))) as CallableAction<T[typeof key]>
    return o
  }, {} as DispatchifiedActions<T>)
)

export {
  dispatchify,
  onError,
}
