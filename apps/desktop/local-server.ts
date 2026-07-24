import {fetch, Agent} from 'undici'
import getPort, {portNumbers, clearLockedPorts} from 'get-port'

import {guessIp} from '@repo/c8/cli/ip'

import {
  DEV_SERVER_POLLING_INTERVAL, DEV_SERVER_POLLING_TIMEOUT,
} from './constants'
import {runServeCommand, runInstallCommand} from './app/file-sync/run-commands'
import {dispatchSystemLog, forwardProcessOutput} from './app/system-log/listeners'
import {startLocalProxy} from './app/file-sync/local-proxy'
import {createDev8WebSocketServer} from './app/dev8-socket/dev8-socket-server'

interface LocalServer {
  stop: () => Promise<void>
  checkRunning: () => Promise<boolean>
  waitForServerReady: () => Promise<boolean>
  getLocalBuildUrl: () => Promise<string>
  getLocalBuildRemoteUrl: () => Promise<string>
}

const LOCAL_BUILD_URL_BASE = 'http://localhost:'

const createLocalServer = async (
  appKey: string,
  savePath: string
): Promise<LocalServer> => {
  dispatchSystemLog({appKey, 'type': 'log', 'text': 'Installing packages'})
  await runInstallCommand(appKey, savePath)
  const [primaryPort, buildPort, dev8SocketPort] = await Promise.all([
    getPort({port: portNumbers(58000, 58999)}),
    getPort({port: portNumbers(59000, 59999)}),
    getPort({port: portNumbers(60000, 60999)}),
  ])

  const webpackDevServer = runServeCommand(savePath, buildPort)
  const dev8Socket = createDev8WebSocketServer(appKey, dev8SocketPort)
  forwardProcessOutput(appKey, webpackDevServer.nodeChildProcess)
  const proxy = startLocalProxy({primaryPort, buildPort, dev8SocketPort})

  const localServerCheck = async () => {
    try {
      const res = await fetch(`${LOCAL_BUILD_URL_BASE}${primaryPort}`, {
        signal: AbortSignal.timeout(1000),
        dispatcher: new Agent({
          bodyTimeout: 1000,
        }),
      })
      return res.status === 200
    } catch (error) {
      return false
    }
  }

  const waitForServerReady = async () => {
    const end = performance.now() + DEV_SERVER_POLLING_TIMEOUT
    /* eslint-disable no-await-in-loop */
    while (performance.now() < end) {
      if (!webpackDevServer.isRunning()) {
        return false
      }
      if (await localServerCheck()) {
        return true
      }
      await new Promise(r => setTimeout(r, DEV_SERVER_POLLING_INTERVAL))
    }
    return false
  }

  const handleStop = async () => {
    proxy.stop()
    dev8Socket.close()
    if (webpackDevServer) {
      await webpackDevServer.kill()
    }
    clearLockedPorts()
  }

  const handleGetLocalBuildUrl = async (): Promise<string> => {
    try {
      const isRunning = await localServerCheck()
      return isRunning ? `${LOCAL_BUILD_URL_BASE}${primaryPort}` : ''
    } catch (error) {
      return ''
    }
  }

  // We don't check that webpack is running. User should call checkRunning() if needed.
  // This should return a URL string (includes the schema)
  const handleGetLocalBuildRemoteUrl = async (): Promise<string> => {
    if (!primaryPort) {
      return ''
    }

    return `http://${guessIp()}:${primaryPort}`
  }

  return {
    stop: handleStop,
    checkRunning: localServerCheck,
    waitForServerReady,
    getLocalBuildUrl: handleGetLocalBuildUrl,
    getLocalBuildRemoteUrl: handleGetLocalBuildRemoteUrl,
  }
}

export {
  createLocalServer,
}

export type {

  LocalServer,
}
