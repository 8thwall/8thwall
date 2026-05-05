import {createLogger} from 'redux-logger'
import thunk from 'redux-thunk'
import {combineReducers, Middleware} from 'redux'
import {RouterState, createReduxHistoryContext} from 'redux-first-history'
import {createMemoryHistory} from 'history'

import {configureStore, EnhancedStore} from '@reduxjs/toolkit'

import type {DeepReadonly} from 'ts-essentials'

import {FULL_STACK_REDUCERS} from './full-stack-reducers'

const VERBOSE_LOGGING = false

const history = createMemoryHistory()

// Scroll to the top on every history change
if (typeof window !== 'undefined') {
  history.listen(() => window.scrollTo(0, 0))
}

const initialCommonState = {
  error: '',
  message: '',
  success: '',
  history,
}

type CommonState = DeepReadonly<typeof initialCommonState & {
  imageTargetPreview?: string | undefined
}>

const common = (state: CommonState = initialCommonState, action) => {
  switch (action.type) {
    case 'ERROR':
      return {...state, error: action.msg}
    case 'MESSAGE':
      return {...state, message: action.msg}
    case 'SUCCESS':
      return {...state, success: action.msg}
    case 'ACKNOWLEDGE_ERROR':
      return {...state, error: ''}
    case 'ACKNOWLEDGE_MESSAGE':
      return {...state, message: ''}
    case 'ACKNOWLEDGE_SUCCESS':
      return {...state, success: ''}
    default:
      return state
  }
}

const {
  createReduxHistory,
  routerMiddleware,
  routerReducer,
} = createReduxHistoryContext({history})

const rootReducer = combineReducers<RootState>({
  ...FULL_STACK_REDUCERS,
  router: routerReducer,
  common,
})

type RootState = DeepReadonly<{
  router: RouterState
  common: Parameters<typeof common>[0]
} & {
  [k in keyof typeof FULL_STACK_REDUCERS]: Parameters<typeof FULL_STACK_REDUCERS[k]>[0]
}>

const reInitializer: Middleware = store => next => (action) => {
  next(action)
  if (action.type === 'ERROR' && action.msg) {
    setTimeout(() => store.dispatch({type: 'ACKNOWLEDGE_ERROR'}), 30000)
  }
  if (action.type === 'MESSAGE' && action.msg) {
    setTimeout(() => store.dispatch({type: 'ACKNOWLEDGE_MESSAGE'}), 30000)
  }
}

let store

type ExtendedStoreResult = {
  store: EnhancedStore<RootState>
  initializePromise: Promise<void>
}

const getExtendedStore = (preloadedState?: RootState): ExtendedStoreResult => {
  if (store) {
    return {
      store,
      initializePromise: Promise.resolve(),
    }
  }

  const middleware = [
    routerMiddleware,
    thunk,
  ]

  if (VERBOSE_LOGGING && BuildIf.LOCAL_DEV) {
    middleware.push(createLogger())
  }

  middleware.push(reInitializer)

  // Wrapper over createStore()
  // https://redux-toolkit.js.org/api/configureStore
  // https://redux.js.org/api/createstore
  store = configureStore({
    reducer: rootReducer,
    preloadedState,
    middleware,
    devTools: BuildIf.ALL_QA || window.location.origin === 'https://www-rc.8thwall.com',
  })

  // TODO(christoph): Clean up
  const initializePromise = Promise.resolve()
  return {store, initializePromise}
}

const getStore = (preloadedState?: RootState): EnhancedStore<RootState> => (
  getExtendedStore(preloadedState).store
)

let connectedHistory: ReturnType<typeof createReduxHistory>
const getHistory = () => {
  if (!connectedHistory) {
    connectedHistory = createReduxHistory(getStore())
  }
  return connectedHistory
}

export {
  getHistory,
  getStore,
}

export type {
  RootState,
  CommonState,
}
