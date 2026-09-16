import {makeJsonResponse} from './json-response'

type RequestHandler = (req: Request) => Response | Promise<Response>

type CodedError = Error & {
  code: number
}

const defaultErrorMessage = 'Server error occurred.'

const makeCodedError = (msg: string, code: number = 400): CodedError => {
  const err = new Error(msg) as CodedError
  err.code = code
  return err
}

const returnCodedErrorResponse = (err: CodedError, defaultMessage = defaultErrorMessage) => {
  if (err.code && typeof err.code === 'number') {
    return makeJsonResponse({message: err.message}, err.code)
  }
  // eslint-disable-next-line no-console
  console.error('Unexpected error:', err)
  return makeJsonResponse({message: defaultMessage}, 500)
}

const withErrorHandlingResponse = (handler: RequestHandler) => async (req: Request) => {
  try {
    const response = await handler(req)
    return response
  } catch (err) {
    return returnCodedErrorResponse(err as CodedError, `Unexpected Error on: ${req.url}`)
  }
}

export {
  makeCodedError,
  withErrorHandlingResponse,
}
