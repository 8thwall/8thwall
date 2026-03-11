import {
  HelpCenterActionType,
  HelpCenterReduxState,
  ErrorAction,
  GetSearchResultsAction,
} from '../../../shared/help-center/help-center-types'

const initialState: HelpCenterReduxState = {
  searchResults: [],
  error: {
    sendSupportTicket: false,
  },
}

const setError = (
  state: HelpCenterReduxState,
  action: ErrorAction
): HelpCenterReduxState => ({
  ...state,
  error: {
    ...state.error,
    ...action.error,
  },
})

const clearSupportTicket = (state: HelpCenterReduxState): HelpCenterReduxState => ({
  ...state,
  error: {
    ...state.error,
    sendSupportTicket: false,
  },
})

const getSearchResults = (
  state: HelpCenterReduxState, action: GetSearchResultsAction
): HelpCenterReduxState => ({
  ...state,
  searchResults: [...(action.searchResults ? action.searchResults : [])],
})

const clearSearchResults = (state: HelpCenterReduxState): HelpCenterReduxState => ({
  ...state,
  searchResults: [],
})

const clearErrors = (state: HelpCenterReduxState): HelpCenterReduxState => ({
  ...state,
  error: {},
})

const actions = {
  [HelpCenterActionType.ERROR]: setError,
  [HelpCenterActionType.CLEAR_SUPPORT_TICKET]: clearSupportTicket,
  [HelpCenterActionType.GET_SEARCH_RESULTS]: getSearchResults,
  [HelpCenterActionType.CLEAR_SEARCH_RESULTS]: clearSearchResults,
  [HelpCenterActionType.CLEAR_ERRORS]: clearErrors,
} as const

const Reducer = (state = {...initialState}, action): HelpCenterReduxState => {
  const handler = actions[action.type]
  if (!handler) {
    return state
  }
  return handler(state, action)
}

export default Reducer
