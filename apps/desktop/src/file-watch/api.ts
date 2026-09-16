import {ipcRenderer, IpcRendererEvent} from 'electron'

import type {LocalSyncHandler, FileWatchApi} from '@repo/reality/shared/desktop/electron-api'

type Handler = (event: Event) => void

const createFileWatchApi = (): FileWatchApi => {
  const handlers: Map<string, Handler> = new Map()
  let rendererPort: MessagePort

  ipcRenderer.once('file-watch-port', (event: IpcRendererEvent) => {
    const port = event.ports[0]
    rendererPort = port
    rendererPort.start()
    handlers.clear()
  })

  return {
    addHandler: (appKey: string, handleData: LocalSyncHandler) => {
      if (!rendererPort) {
        throw new Error('File watch port not initialized.')
      }
      if (handlers.has(appKey)) {
        throw new Error(`Handler for appKey ${appKey} already exists.`)
      }
      const handler = (event: Event) => {
        if (!(event instanceof MessageEvent)) {
          return
        }
        handleData(event.data)
      }
      rendererPort.addEventListener('message', handler)
      rendererPort.postMessage({action: 'LOCAL_CONNECT_FILE_WATCH', appKey})
      handlers.set(appKey, handler)
    },
    removeHandler: (appKey: string) => {
      const handler = handlers.get(appKey)
      if (handler) {
        rendererPort.removeEventListener('message', handler)
        rendererPort.postMessage({action: 'LOCAL_CLOSE_FILE_WATCH', appKey})
        handlers.delete(appKey)
      }
    },
  }
}

export {
  createFileWatchApi,
}
