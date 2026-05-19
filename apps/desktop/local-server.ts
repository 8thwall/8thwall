import {fetch, Agent} from 'undici'
import {exec, ChildProcess} from 'child_process'
import getPort, {portNumbers, clearLockedPorts} from 'get-port'

import {guessIp} from '@repo/c8/cli/ip'

import {
  DEV_SERVER_POLLING_INTERVAL, DEV_SERVER_POLLING_TIMEOUT,
} from './constants'
import {runServeCommand, runInstallCommand} from './app/file-sync/run-commands'
import {forwardProcessOutput} from './app/system-log/listeners'
import {startLocalProxy} from './app/file-sync/local-proxy'
import {createDev8WebSocketServer} from './app/dev8-socket/dev8-socket-server'

interface LocalServer {
  stop: () => Promise<void>
  checkRunning: () => Promise<boolean>
  waitForServerReady: () => Promise<boolean>
  getLocalBuildUrl: () => Promise<string>
  getLocalBuildRemoteUrl: () => Promise<string>
}

const IS_WINDOWS = process.platform === 'win32'

const LOCAL_BUILD_URL_BASE = 'http://localhost:'

const isProcessRunning = (process: ChildProcess | undefined): process is ChildProcess => (
  !!process && !process.killed && process.exitCode === null
)

const killProcess = async (process: ChildProcess | undefined): Promise<void> => {
  if (!isProcessRunning(process)) {
    return
  }

  await new Promise<void>((resolve, reject) => {
    if (IS_WINDOWS) {
      exec(`taskkill /pid ${process.pid} /T /F`, err => (err ? reject(err) : resolve()))
      return
    }
    process.on('exit', () => {
      resolve()
    })

    process.kill('SIGTERM')

    // Fallback: force kill after 5 seconds if SIGTERM doesn't work
    setTimeout(() => {
      if (process && !process.killed) {
        process.kill('SIGKILL')
      }
    }, 5000)
  })
}

const createLocalServer = async (
  appKey: string,
  savePath: string
): Promise<LocalServer> => {
  await runInstallCommand(appKey, savePath)
  const [primaryPort, buildPort, dev8SocketPort] = await Promise.all([
    getPort({port: portNumbers(58000, 58999)}),
    getPort({port: portNumbers(59000, 59999)}),
    getPort({port: portNumbers(60000, 60999)}),
  ])

  const webpackDevServer = runServeCommand(savePath, buildPort)
  const dev8Socket = createDev8WebSocketServer(appKey, dev8SocketPort)
  forwardProcessOutput(appKey, webpackDevServer)
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
      if (!isProcessRunning(webpackDevServer)) {
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
      await killProcess(webpackDevServer)
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
