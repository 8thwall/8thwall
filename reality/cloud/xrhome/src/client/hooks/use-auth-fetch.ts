import type {RequestInit} from '../common/public-api-fetch'

const useAuthFetch = () => {
  // eslint-disable-next-line arrow-parens, @typescript-eslint/no-unused-vars
  const authFetch = async <T>(path: string, options?: RequestInit, json = true): Promise<T> => {
    throw new Error('Not implemented: useAuthFetch')
  }

  return authFetch
}

export {
  useAuthFetch,
}
