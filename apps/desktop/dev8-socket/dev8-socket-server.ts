import type {ScopedDebugMessage} from '@repo/c8/ecs/src/shared/debug-messaging'
import * as WebSocket from 'ws'

import {toDevicePool, fromDevicePool} from './listeners'

const extractSessionId = (url: string | undefined) => {
  if (!url) {
    return null
  }
  try {
    return new URL(url, 'http://localhost').searchParams.get('sessionId')
  } catch {
    return null
  }
}

const createDev8WebSocketServer = (appKey: string, portNumber: number) => {
  const wss = new WebSocket.WebSocketServer({port: portNumber})

  wss.on('connection', (ws: WebSocket.WebSocket, req) => {
    const sessionId = extractSessionId(req.url)

    if (!sessionId) {
      return
    }

    ws.on('error', (err) => {
      // eslint-disable-next-line no-console
      console.error('WebSocket error:', err)
    })

    ws.on('message', (data: unknown) => {
      try {
        const dataString = data instanceof Buffer ? data.toString() : String(data)
        fromDevicePool.dispatch({appKey, sessionId, data: JSON.parse(dataString)})
      } catch (err) {
        // eslint-disable-next-line no-console
        console.error('Failed to parse WebSocket message:', err)
      }
    })

    const handleBroadcast = (d: ScopedDebugMessage) => {
      if (d.appKey === appKey && d.sessionId === sessionId) {
        ws.send(JSON.stringify(d.data))
      }
    }

    toDevicePool.addListener(handleBroadcast)

    ws.on('close', () => {
      toDevicePool.removeListener(handleBroadcast)
    })
  })

  return wss
}

export {createDev8WebSocketServer}
