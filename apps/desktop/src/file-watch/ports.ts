import {BrowserWindow, MessageChannelMain} from 'electron'
import type {LocalSyncMessage} from '@repo/reality/shared/desktop/local-sync-types'

import {initFileWatcher, type FileWatcher} from './file-watcher'

let prevCleanup: (() => void) | null = null

const setUpMainFileWatchPort = (browserWindow: BrowserWindow) => {
  if (prevCleanup) {
    prevCleanup()
  }

  const fileWatchers = new Map<string, FileWatcher>()
  const {port1: mainPort, port2: rendererPort} = new MessageChannelMain()

  const messageHandler = (event: any) => {
    const {data} = event as {data: LocalSyncMessage}

    switch (data?.action) {
      case 'LOCAL_CONNECT_FILE_WATCH': {
        let fileWatcher = fileWatchers.get(data.appKey)
        if (!fileWatcher) {
          fileWatcher = initFileWatcher(data.appKey, (msg: LocalSyncMessage) => {
            mainPort.postMessage(msg)
          })
          fileWatchers.set(data.appKey, fileWatcher)
        }
        fileWatcher.start()
        break
      }
      case 'LOCAL_CLOSE_FILE_WATCH': {
        const fileWatcher = fileWatchers.get(data.appKey)
        if (!fileWatcher) {
          return
        }
        fileWatcher.close()
        fileWatchers.delete(data.appKey)
        break
      }
      default:
        // eslint-disable-next-line no-console
        console.warn('Unknown action:', data?.action)
    }
  }

  mainPort.on('message', messageHandler)

  const cleanup = () => {
    fileWatchers.forEach((watcher) => {
      watcher.close()
    })
    fileWatchers.clear()

    mainPort.removeListener('message', messageHandler)
    mainPort.close()
    rendererPort.close()
  }

  prevCleanup = cleanup

  browserWindow.webContents.postMessage('file-watch-port', null, [rendererPort])
  mainPort.start()
}

export {
  setUpMainFileWatchPort,
}
