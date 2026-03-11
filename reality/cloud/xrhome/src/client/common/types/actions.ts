import type {ThunkAction} from 'redux-thunk'
import type {AnyAction} from 'redux'

import type {RootState} from '../../reducer'

type Action = {type: string}

type Fn<T extends Array<any>, U> = (...args: T) => U

type Dispatchable = Action | ThunkAction<any, RootState, unknown, any>

type DispatchedActionReturnType<T extends Dispatchable> =
  T extends Fn<any, Fn<any, infer P>> ? P : (T extends Fn<any, any> ? ReturnType<T>: T)

type Dispatch = <T extends Dispatchable>(a: T) => DispatchedActionReturnType<T>

// A "raw action" is a function written as (ownArgs) => (dispatch, getState) => {} or in some cases,
// (ownArgs) => action. After being called, it has to be dispatched to take effect
type RawAction = Fn<any, Dispatchable>
type RawActions = Record<string, RawAction>

// A "callable action" is an action that you call with the action-specific parameters, which will
// call dispatch on the return value of the raw action automatically
type CallableAction<T extends RawAction> = Fn<Parameters<T>, DispatchedActionReturnType<T>>

type DispatchifiedActions<T extends RawActions> = {
  [K in keyof T]: CallableAction<T[K]>
}

// The base "Actions" type is the default export from each actions file and can be used with
// useActions(actions) or connect(..., actions)(Component).
type Actions<T extends RawActions> = (dispatch: Dispatch) => DispatchifiedActions<T>

type Thunk<ReturnType = void> = ThunkAction<ReturnType, RootState, unknown, AnyAction>
type AsyncThunk<ReturnType = void> = Thunk<Promise<ReturnType>>

export type {
  DispatchedActionReturnType,
  Dispatch,
  RawAction,
  RawActions,
  CallableAction,
  DispatchifiedActions,
  Action,
  Actions,
  Thunk,
  AsyncThunk,
}
