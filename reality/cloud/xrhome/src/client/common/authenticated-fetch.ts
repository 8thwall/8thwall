import type {AsyncThunk} from './types/actions'

const authenticatedFetch = <ResponseType>(

  path: string,
  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  options = {}
): AsyncThunk<ResponseType> => async () => {
    throw new Error('authenticatedFetch is not a thing anymore.')
  }

export default authenticatedFetch
