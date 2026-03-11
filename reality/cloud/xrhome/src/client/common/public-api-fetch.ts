import {join} from 'path'

import {getHeaders, responseToJsonOrText} from './fetch-utils'
import type {AsyncThunk} from './types/actions'
import {resolveServerRoute} from './paths'

type RequestInit = Parameters<typeof fetch>[1]

type FetchOptions = RequestInit & {
  json?: boolean
}

const publicApiFetch = <T extends unknown>(
  path: string,
  options: FetchOptions = {}
): AsyncThunk<T> => async () => {
    const fullPath = join('/api/public/', path)
    const {headers, json, ...rest} = options
    const url = resolveServerRoute(fullPath)
    const res = await fetch(url, {
      // NOTE(christoph): We'd prefer 'omit', but it would break OAuth on internal domains.
      credentials: 'same-origin',
      headers: getHeaders(undefined, {json, headers}),
      ...rest,
    })
    return responseToJsonOrText(res)
  }

export {
  publicApiFetch,
}

export type {
  RequestInit,
}
