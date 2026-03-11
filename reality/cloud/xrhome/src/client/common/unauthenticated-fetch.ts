import {getHeaders, responseToJsonOrText} from './fetch-utils'
import {resolveServerRoute} from './paths'

interface UnauthenicatedFetchOptions {
  method?: string
  body?: string | Blob
  headers?: object
  json?: boolean
  signal?: AbortSignal
}

// @param resProcessing function to process response from the server
const unauthenticatedFetch = (
  url: string,
  options: UnauthenicatedFetchOptions = {},
  resProcessing = responseToJsonOrText
) => () => {
  const {method, body, headers, json, signal} = options
  return fetch(resolveServerRoute(url), {
    method,
    body,
    // TODO(alvin): 'same-origin' isn't ideal, as it would attach authentication cookies,
    // which is opposite of what this fetch should be doing. OAuth on Console-dev and staging,
    // however, blocks requests without proper authentication.
    credentials: 'same-origin',
    headers: getHeaders(undefined, {json, headers}),
    signal,  // for aborting the request
  }).then(res => resProcessing(res))
}

export default unauthenticatedFetch
