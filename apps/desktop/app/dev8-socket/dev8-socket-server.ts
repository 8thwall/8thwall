import * as WebSocket from 'ws'

interface BroadcastData {
  action: string
  data: Record<string, unknown>
}

interface WebSocketMessage {
  action: string
  // TODO(cindyhu): fix naming to camelCase
  // eslint-disable-next-line camelcase
  broadcast_data?: BroadcastData
}

const createDev8WebSocketServer = (portNumber: number) => {
  const wss = new WebSocket.WebSocketServer({port: portNumber})

  const broadcastToOthers = (sender: WebSocket.WebSocket, data: string | Buffer) => {
    wss.clients.forEach((client) => {
      if (client !== sender && client.readyState === WebSocket.WebSocket.OPEN) {
        client.send(data)
      }
    })
  }

  wss.on('connection', (ws: WebSocket.WebSocket) => {
    ws.on('error', (err) => {
      // eslint-disable-next-line no-console
      console.error('WebSocket error:', err)
    })

    ws.on('message', (data: unknown) => {
      let parsedBody: WebSocketMessage
      try {
        const dataString = data instanceof Buffer ? data.toString() : String(data)
        parsedBody = JSON.parse(dataString) as WebSocketMessage
      } catch (err) {
        // eslint-disable-next-line no-console
        console.error('Failed to parse WebSocket message:', err)
        return
      }

      const action = parsedBody && parsedBody.action
      if (action === 'BROADCAST' && parsedBody.broadcast_data) {
        // For BROADCAST messages, flatten the broadcast_data structure
        const broadcastData = parsedBody.broadcast_data as BroadcastData
        const broadcastMessage = JSON.stringify({
          action: broadcastData.action,
          ...broadcastData.data,
        })
        broadcastToOthers(ws, broadcastMessage)
      } else {
        // ignore other actions
      }
    })
  })

  return wss
}

export {createDev8WebSocketServer}
