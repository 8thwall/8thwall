import {fetch, Agent} from 'undici'
import type {ChildProcess} from 'child_process'
import getPort, {portNumbers} from 'get-port'

import {guessIp} from '@repo/c8/cli/ip'

import {
  DEV_SERVER_POLLING_INTERVAL, DEV_SERVER_POLLING_TIMEOUT,
} from './constants'
import {runServeCommand, runInstallCommand} from './app/file-sync/run-commands'
import {forwardProcessOutput} from './app/system-log/listeners'
import {startLocalProxy} from './app/file-sync/local-proxy'

interface LocalServer {
  stop: () => Promise<void>
  checkRunning: () => Promise<boolean>
  getLocalBuildUrl: () => Promise<string>
  getLocalBuildRemoteUrl: () => Promise<string>
}

const LOCAL_BUILD_URL_BASE = 'http://localhost:'

const isProcessRunning = (process: ChildProcess | undefined): process is ChildProcess => (
  !!process && !process.killed && process.exitCode === null
)

const killProcess = async (process: ChildProcess | undefined): Promise<void> => {
  if (!isProcessRunning(process)) {
    return
  }

  await new Promise<void>((resolve) => {
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
  const [proxyPort, webpackPort] = await Promise.all([
    getPort({port: portNumbers(58000, 58100)}),
    getPort({port: portNumbers(58100, 58200)}),
  ])

  const webpackDevServer = runServeCommand(savePath, webpackPort)
  forwardProcessOutput(appKey, webpackDevServer)
  const proxy = startLocalProxy(proxyPort, webpackPort)

  // Note(juliesoohoo): This is a temporary solution to check if the local server is running.
  // 'rejectUnauthorized: false' is needed to bypass the issue with having a
  // self-signed certificate.
  const localServerCheck = async () => {
    try {
      const res = await fetch(`${LOCAL_BUILD_URL_BASE}${proxyPort}`, {
        dispatcher: new Agent({
          connect: {rejectUnauthorized: false},
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
      await new Promise(r => setTimeout(r, DEV_SERVER_POLLING_INTERVAL))
      if (await localServerCheck()) {
        return true
      }
    }
    return false
  }

  const handleStop = async () => {
    proxy.stop()
    if (webpackDevServer) {
      await killProcess(webpackDevServer)
    }
  }

  const handleGetLocalBuildUrl = async (): Promise<string> => {
    if (!webpackPort) {
      return ''
    }

    try {
      const isRunning = await localServerCheck()
      return isRunning ? `${LOCAL_BUILD_URL_BASE}${webpackPort}` : ''
    } catch (error) {
      return ''
    }
  }

  // We don't check that webpack is running. User should call checkRunning() if needed.
  // This should return a URL string (includes the schema)
  const handleGetLocalBuildRemoteUrl = async (): Promise<string> => {
    if (!proxyPort) {
      return ''
    }

    return `https://${guessIp()}:${proxyPort}`
  }

  return {
    stop: handleStop,
    checkRunning: waitForServerReady,
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
