import {useEffect, useRef} from 'react'

import {SocketCallback, SocketSpecifier, socketSpecifiersEqual} from '../websockets/websocket-pool'
import websocketPool from '../websockets/websocket-pool'

export const useWebsocketHandler = (onMsg: SocketCallback, specifier: SocketSpecifier) => {
  const functionRef = useRef<SocketCallback>()
  functionRef.current = onMsg

  const previousSpecToUse = useRef<SocketSpecifier>()
  const specToUse = socketSpecifiersEqual(previousSpecToUse.current, specifier)
    ? previousSpecToUse.current
    : specifier

  useEffect(() => {
    if (!specToUse) {
      return undefined
    }

    const handler = msg => functionRef.current(msg)

    websocketPool.addHandler(specToUse, handler)

    return () => {
      websocketPool.removeHandler(specToUse, handler)
    }
  }, [specToUse])
}
