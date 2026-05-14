import type {DebugMessage} from './shared/ecs/shared/debug-messaging'

type Listener = (data: DebugMessage) => void

const createWebsocket = (websocketUrl: string) => {
  let ws: WebSocket | null = null
  let socketRestartCount = 0
  let socketFailureCount = 0
  const MAX_SOCKET_RESTARTS = 50
  const BACKOFF_SCALE_MS = 250
  const MAX_WAIT_MS = 10000
  const pendingMessages: string[] = []

  const listeners = new Set<Listener>()

  const openSocket = () => {
    const _ws = new WebSocket(websocketUrl)

    _ws.onopen = () => {
      ws = _ws
      socketFailureCount = 0
      const messagesToDispatch = [...pendingMessages]
      pendingMessages.length = 0
      messagesToDispatch.forEach(m => ws.send(m))
    }

    _ws.onmessage = (event) => {
      let msg: DebugMessage
      try {
        msg = JSON.parse(event.data)
      } catch (err) {
        return
      }

      // Handle the message for studio debug events
      listeners.forEach(e => e(msg))
    }

    _ws.onclose = () => {
      ws = null
      socketFailureCount++
      if (socketRestartCount++ < MAX_SOCKET_RESTARTS) {
        const backoff = BACKOFF_SCALE_MS * ((2 ** socketFailureCount) + Math.random())
        setTimeout(openSocket, Math.min(backoff, MAX_WAIT_MS))
      }
    }
  }

  const listen = (listener: Listener) => {
    listeners.add(listener)
  }

  const broadcast = (message: DebugMessage) => {
    const data = JSON.stringify(message)
    if (ws) {
      ws.send(data)
    } else {
      pendingMessages.push(data)
    }
  }

  openSocket()

  return {
    listen,
    broadcast,
  }
}

export {
  createWebsocket,
}
