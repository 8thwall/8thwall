import {ipcRenderer, IpcRendererEvent} from 'electron'

import type {SystemLogApi, SystemLogHandler} from '@repo/reality/shared/desktop/electron-api'
import type {SystemLogEvent} from '@repo/reality/shared/desktop/system-log-types'

const createSystemLogApi = (): SystemLogApi => {
  const handlers: Map<string, SystemLogHandler> = new Map()
  let rendererPort: MessagePort

  ipcRenderer.once('system-log-port', (event: IpcRendererEvent) => {
    const port = event.ports[0]
    rendererPort = port
    rendererPort.start()
    rendererPort.addEventListener('message', (e) => {
      if (!(e instanceof MessageEvent)) {
        return
      }
      const data = e.data as SystemLogEvent
      handlers.get(data.appKey)?.(data)
    })
    handlers.clear()
  })

  return {
    setHandler: (appKey: string, handler: SystemLogHandler) => {
      handlers.set(appKey, handler)
    },
    clearHandler: (appKey: string) => {
      handlers.delete(appKey)
    },
  }
}

export {
  createSystemLogApi,
}
