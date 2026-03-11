enum SupportTicketType {
  ACCOUNT_BILLING_LICENSING_SUPPORT = 'account_support',
  TECHNICAL_SUPPORT = 'tech_support',
  OTHER_SUPPORT = 'other_support',
  REPORT_BUG = 'report_bug',
}

enum SearchResultType {
  DOCS = 'docs'
}

interface SupportTicketCreateParams {
  subject: string
  body: string
  type: SupportTicketType
  AccountUuid: string
  attachmentTokens?: string[]
}

interface SupportTicketCreateResponse {
  result: {
    email: string
  }
}

interface SearchResult {
  linkText: string
  url: string
  id: string
  type: SearchResultType
}
// Redux Action types.
enum HelpCenterActionType {
  ERROR = 'HELP_CENTER/ERROR',
  CLEAR_SUPPORT_TICKET = 'HELP_CENTER/CLEAR_SUPPORT_TICKET',
  GET_SEARCH_RESULTS = 'HELP_CENTER/GET_SEARCH_RESULTS',
  CLEAR_SEARCH_RESULTS = 'HELP_CENTER/CLEAR_SEARCH_RESULTS',
  CLEAR_ERRORS = 'HELP_CENTER/CLEAR_ERRORS',
}

type ErrorState = {
  getSearchResults?: boolean
  sendSupportTicket?: boolean
  uploadAttachment?: boolean
}

// Top-level redux state.
type HelpCenterReduxState = {
  searchResults: SearchResult[]
  error: ErrorState
}

// Action Creators
type ErrorAction = {
  type: HelpCenterActionType.ERROR
  error: ErrorState
}

type GetSearchResultsAction = {
  type: HelpCenterActionType.GET_SEARCH_RESULTS
  searchResults: SearchResult[]
}

export type {
  SupportTicketCreateParams,
  SupportTicketCreateResponse,
}

export {
  SupportTicketType,
  SearchResultType,
  HelpCenterReduxState,
  HelpCenterActionType,
  GetSearchResultsAction,
  ErrorAction,
  SearchResult,
}
