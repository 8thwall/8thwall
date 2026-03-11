const SOCKET_CLOSE_TIMEOUT = 10 * 1000

interface UrlWithParams {
  baseUrl: string
  params?: object
}

// A socket specifier specifies the connection url and url parameters sent in the request
// also includes information on how to compare two specifiers to reuse a socket.
export interface SocketSpecifier extends UrlWithParams {
}

// A socket identifier is the unique key used to access an instance
type SocketIdentifier = string

export type SocketCallback = (object) => void

// The time between socket close an open means we could miss important messages.
export type SocketRestartListener = () => void

// An instance is a container for a socket connection that holds queued messages and callbacks
interface SocketInstance {
  identifier: SocketIdentifier
  rawSocket?: WebSocket
  status: 'pending' | 'ready' | 'closed'
  messageQueue: string[]
  subscribers: SocketCallback[]
  restartListeners: SocketRestartListener[]
  firstOpenHappened: boolean
}

const tryParse = (s) => {
  try {
    return JSON.parse(s)
  } catch (err) {
    return null
  }
}

// Get the url for the UrlWithParams descriptor
export const generateUrlString = ({baseUrl, params}: UrlWithParams): string => {
  let result = baseUrl

  if (params) {
    const queryParams = new URLSearchParams()
    Object.keys(params).forEach((key) => {
      queryParams.set(key, params[key])
    })
    queryParams.sort()
    const queryString = queryParams.toString()
    if (queryString) {
      result += `?${queryString}`
    }
  }

  return result
}

// Maps identifier to instances of socket connections
const instances: Map<SocketIdentifier, SocketInstance> = new Map()

// Maps identifier to timeout references to close the connection
const closeTimeouts: Map<SocketIdentifier, ReturnType<typeof setTimeout>> = new Map()

const getSocketInstance = (specifier: SocketSpecifier): SocketInstance | undefined => {
  const identifier = generateUrlString(specifier)
  return instances.get(identifier)
}

const closeInstance = (instance: SocketInstance) => {
  if (instance.rawSocket) {
    instance.rawSocket.onclose = null
    instance.rawSocket.onopen = null
    instance.rawSocket.onmessage = null
    instance.rawSocket.close()
  }

  instance.status = 'closed'
  instances.delete(instance.identifier)
}

const startClose = (instance: SocketInstance) => {
  if (closeTimeouts.get(instance.identifier)) {
    return
  }

  const timeout = setTimeout(() => {
    closeInstance(instance)
  }, SOCKET_CLOSE_TIMEOUT)

  closeTimeouts.set(instance.identifier, timeout)
}

const maybeStartClose = (instance: SocketInstance) => {
  if (instance.messageQueue.length === 0 &&
      instance.subscribers.length === 0 &&
      instance.restartListeners.length === 0
  ) {
    startClose(instance)
  }
}

const cancelClose = (instance: SocketInstance) => {
  clearTimeout(closeTimeouts.get(instance.identifier))
  closeTimeouts.delete(instance.identifier)
}

const createSocketInstance = (specifier: SocketSpecifier): SocketInstance => {
  const identifier = generateUrlString(specifier)
  const instance: SocketInstance = {
    identifier,
    rawSocket: null,
    status: 'pending',
    messageQueue: [],
    subscribers: [],
    restartListeners: [],  // When a socket a re-opened
    firstOpenHappened: false,
  }

  instances.set(instance.identifier, instance)

  const startSocket = async () => {
    // Handle close while fetching auth
    if (instance.status === 'closed') {
      return
    }

    const socket = new WebSocket(identifier)

    socket.onopen = () => {
      instance.status = 'ready'
      instance.messageQueue.forEach(msg => socket.send(msg))
      if (instance.firstOpenHappened) {
        instance.restartListeners.forEach(listener => listener())
      } else {
        instance.firstOpenHappened = true
      }
      maybeStartClose(instance)
    }

    socket.onmessage = ({data}) => {
      const msg = tryParse(data)
      if (!msg) {
        return
      }

      instance.subscribers.forEach((cb) => {
        cb(msg)
      })
    }

    socket.onclose = () => {
      instance.status = 'pending'
      startSocket()
    }

    instance.rawSocket = socket
  }

  startSocket()

  return instance
}

const getOrCreateSocketInstance = (specifier: SocketSpecifier): SocketInstance => {
  const existingInstance = getSocketInstance(specifier)
  if (existingInstance) {
    return existingInstance
  }

  return createSocketInstance(specifier)
}

/* eslint-disable-next-line arrow-parens */
const broadcastRawMessage = <T extends object>(specifier: SocketSpecifier, data: T) => {
  const instance = getOrCreateSocketInstance(specifier)

  const messageData = JSON.stringify(data)

  if (instance.status === 'ready') {
    instance.rawSocket.send(messageData)
  } else {
    instance.messageQueue.push(messageData)
  }
}

/* eslint-disable-next-line arrow-parens */
const broadcastMessage = <T extends object>(specifier: SocketSpecifier, data: T) => {
  broadcastRawMessage(specifier, {action: 'BROADCAST', broadcast_data: data})
}

const addHandler = (specifier: SocketSpecifier, callback: SocketCallback) => {
  const instance = getOrCreateSocketInstance(specifier)
  if (instance.subscribers.indexOf(callback) === -1) {
    instance.subscribers.push(callback)
    cancelClose(instance)
  }
}

const removeHandler = (specifier: SocketSpecifier, callback: SocketCallback) => {
  const instance = getSocketInstance(specifier)
  if (!instance) {
    return
  }

  const location = instance.subscribers.indexOf(callback)
  if (location === -1) {
    return
  }
  instance.subscribers.splice(location, 1)

  maybeStartClose(instance)
}

const addRestartListener = (specifier: SocketSpecifier, listener: SocketRestartListener) => {
  const instance = getOrCreateSocketInstance(specifier)
  if (instance.restartListeners.indexOf(listener) !== -1) {
    instance.restartListeners.push(listener)
    cancelClose(instance)
  }
}

const removeRestartListener = (
  specifier: SocketSpecifier,
  listener: SocketRestartListener
) => {
  const instance = getSocketInstance(specifier)
  if (!instance) {
    return
  }

  const location = instance.restartListeners.indexOf(listener)
  if (location === -1) {
    return
  }
  instance.restartListeners.splice(location, 1)

  maybeStartClose(instance)
}

export const socketSpecifiersEqual = (a: SocketSpecifier, b: SocketSpecifier) => {
  if (a === b) {
    return true
  }
  if (!a || !b) {
    return !a && !b
  }

  if (a.baseUrl !== b.baseUrl) {
    return false
  }

  return generateUrlString(a) === generateUrlString(b)
}

export default {
  broadcastRawMessage,
  broadcastMessage,
  addHandler,
  removeHandler,
  addRestartListener,
  removeRestartListener,
  socketSpecifiersEqual,
}
