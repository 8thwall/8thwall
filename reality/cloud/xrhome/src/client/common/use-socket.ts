import React from 'react'

import WebsocketPool, {generateUrlString} from '../websockets/websocket-pool'

const useSocket = (specifier, handler) => {
  React.useEffect(() => {
    if (specifier) {
      WebsocketPool.addHandler(specifier, handler)
      return () => {
        WebsocketPool.removeHandler(specifier, handler)
      }
    }
    return undefined
  }, [specifier && generateUrlString(specifier)])
}

export default useSocket
