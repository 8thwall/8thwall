import {makeCodedError} from './errors'

type RequestHandler = (req: Request) => Response | Promise<Response>

type RequestMethod = 'GET' | 'HEAD' | 'PATCH' | 'POST' | string

const methods = (
  methodMap: Record<RequestMethod, RequestHandler>
): RequestHandler => (req: Request) => {
  const handler = methodMap[req.method]
  if (!handler) {
    throw makeCodedError('Invalid request method', 405)
  }
  return handler(req)
}

const branches = (branchMap: Record<string, RequestHandler>): RequestHandler => (req: Request) => {
  const handler = branchMap[new URL(req.url).pathname]
  if (!handler) {
    throw makeCodedError('Unknown request path', 404)
  }

  return handler(req)
}

export {
  methods,
  branches,
}

export type {
  RequestHandler,
  RequestMethod,
}
