import {routerMiddleware, connectRouter} from 'connected-react-router'
import {createBrowserHistory} from 'history'
import {combineReducers, createStore, applyMiddleware} from 'redux'
import thunk from 'redux-thunk'

import {timeReducer} from 'bzl/examples/web/app/time-reducer'

const history = createBrowserHistory()

const createRootReducer = historyToConnect => combineReducers({
  router: connectRouter(historyToConnect),
  time: timeReducer,
})

const getStore = (preloadedState?) => {
  const middleware = [
    routerMiddleware(history),
    thunk,
  ]
  const store = createStore(
    createRootReducer(history),
    preloadedState,
    applyMiddleware(...middleware)
  )
  return store
}

export {
  getStore,
}
