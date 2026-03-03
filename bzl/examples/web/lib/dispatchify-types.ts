type Dispatch = <T>(t: T) => T
type BaseRawAction = (...rest: any) => (dispatch: Dispatch, getState: () => any) => any
type Dispatchify<T extends BaseRawAction> = (...rest: Parameters<T>) => ReturnType<ReturnType<T>>
type DispatchifiedActions<T extends Record<string, BaseRawAction>> = {
  [K in keyof T]: Dispatchify<T[K]>
}

export {
  BaseRawAction,
  Dispatch,
  DispatchifiedActions,
  Dispatchify,
}
