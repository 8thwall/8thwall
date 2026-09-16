import {BrowserWindow, MessageChannelMain} from 'electron'

import type {SystemLogHandler} from '@repo/reality/shared/desktop/system-log-types'

import {addSystemLogListener, removeSystemLogListener} from './listeners'

let prevCleanup: (() => void) | null = null

const setUpSystemLogPort = (browserWindow: BrowserWindow) => {
  if (prevCleanup) {
    prevCleanup()
  }

  const {port1: mainPort, port2: rendererPort} = new MessageChannelMain()

  const systemLogListener: SystemLogHandler = (d) => {
    mainPort.postMessage(d)
  }

  addSystemLogListener(systemLogListener)

  const cleanup = () => {
    removeSystemLogListener(systemLogListener)
    mainPort.close()
    rendererPort.close()
  }

  prevCleanup = cleanup

  browserWindow.webContents.postMessage('system-log-port', null, [rendererPort])
  mainPort.start()
}

export {
  setUpSystemLogPort,
}
