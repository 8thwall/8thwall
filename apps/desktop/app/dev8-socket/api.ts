import {ipcRenderer, IpcRendererEvent} from 'electron'

import type {ScopedDebugMessage} from '@repo/c8/ecs/src/shared/debug-messaging'
import type {Dev8SocketApi} from '@repo/reality/shared/desktop/electron-api'
import {createListenerPool} from '@repo/reality/shared/listener-pool'

const createDev8SocketApi = (): Dev8SocketApi => {
  let rendererPort: MessagePort
  const fromDevice = createListenerPool<ScopedDebugMessage>()
  const toDevice = createListenerPool<ScopedDebugMessage>()

  ipcRenderer.once('dev8-socket-port', (event: IpcRendererEvent) => {
    const port = event.ports[0]
    rendererPort = port
    rendererPort.start()
    rendererPort.addEventListener('message', (e) => {
      if (!(e instanceof MessageEvent)) {
        return
      }
      fromDevice.dispatch(e.data)
    })
  })

  toDevice.addListener((d) => {
    rendererPort?.postMessage(d)
  })

  let listener: null | undefined | ((d: ScopedDebugMessage) => void)

  fromDevice.addListener(d => listener?.(d))

  return {
    setListener: (l) => {
      listener = l
    },
    toDevice,
  }
}

export {
  createDev8SocketApi,
}
