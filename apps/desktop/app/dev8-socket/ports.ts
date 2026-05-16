import {BrowserWindow, MessageChannelMain} from 'electron'

import type {ScopedDebugMessage} from '@repo/c8/ecs/src/shared/debug-messaging'

import {fromDevicePool, toDevicePool} from './listeners'

let prevCleanup: (() => void) | null = null

const setupDev8SocketPort = (browserWindow: BrowserWindow) => {
  if (prevCleanup) {
    prevCleanup()
  }

  const {port1: mainPort, port2: rendererPort} = new MessageChannelMain()

  mainPort.addListener('message', (e) => {
    toDevicePool.dispatch(e.data)
  })

  const handleDeviceLog = (d: ScopedDebugMessage) => {
    mainPort.postMessage(d)
  }

  fromDevicePool.addListener(handleDeviceLog)

  const cleanup = () => {
    fromDevicePool.removeListener(handleDeviceLog)
    mainPort.close()
    rendererPort.close()
  }

  prevCleanup = cleanup

  browserWindow.webContents.postMessage('dev8-socket-port', null, [rendererPort])
  mainPort.start()
}

export {
  setupDev8SocketPort,
}
