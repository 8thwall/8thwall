import ErrorStackParser from 'error-stack-parser'

import {getPrintableArgs} from './printable'
import {
  getSourceLocationForStackFrame, getSourceLocationForErrorEvent, processStack,
  SourceLocationPromise, StackPromise, SourceLocation, BaseInfoStack,
} from './source-location'
import {maybeWarnCrossOriginError} from './cross-origin'
import {XrHudManager} from './xrhud/xr-hud-manager'
import {XrSimulatorManager} from './xrsimulator/xr-simulator'
import {
  broadcastSequenceProgress, broadcastReloadConfirmation,
} from './xrsimulator/broadcast-messages'
import {createStudioDebugManager} from './studio-debug'
import {createStudioEventStreamManager} from './studio-event-stream'
import type {SimulatorConfig} from './xrsimulator/simulator-types'
import {loadParameters} from './parameters'

declare global {
  interface Window {
    XR8: any
    THREE: any
    AFRAME: any
    DEV_8W_NO_BUILD_RELOAD: boolean
  }
  interface Element {
    object3D: any
    data: any
  }
}

type LogData = {
  fn: string
  args: any[]
  timestamp: number
  sourceLocationPromise: SourceLocationPromise | null
  stackPromise: StackPromise | null
  sourceLocation?: SourceLocation
  stack?: BaseInfoStack
}

/* eslint-disable no-console, no-eval */

const {
  ua, debugFlag, debugHudKey, deviceId, sessionId, simulatorConfig,
  simulatorRendererConfig, simulatorId, originalUrl, channel,
} = loadParameters()

const injectGoogleNoTranslateRule = () => {
  const meta = document.createElement('meta')
  meta.setAttribute('name', 'google')
  meta.content = 'notranslate'
  document.getElementsByTagName('head')[0].appendChild(meta)
}

const needsUpdate = (config: SimulatorConfig): boolean => (
  !!config?.cameraUrl || !!config?.poiId || !!config?.mockLat ||
  !!config?.mockLng || !!config?.mockCoordinateValue
)

let xrHud: ReturnType<typeof XrHudManager>
let xrSimulator: ReturnType<typeof XrSimulatorManager>
let ws: WebSocket | null = null
let logClearingTimer: ReturnType<typeof setTimeout> | null = null
let sentStartEvent = false
let socketRestartCount = 0
let socketFailureCount = 0
let messageQueue: LogData[] = []
const INITIAL_TIMER_TIMEOUT = 250
const NORMAL_TIMER_TIMEOUT = 1000
const MAX_QUEUE_SIZE = 100
const MAX_MSG_LENGTH = 1000
const MAX_SOCKET_RESTARTS = 50
const BACKOFF_SCALE_MS = 250
const MAX_WAIT_MS = 10000

// Make sure any pending promises for logs are complete before sending
const resolveLog = async ({sourceLocationPromise, stackPromise, ...log}: LogData) => {
  try {
    const [sourceLocation, stack] = await Promise.all([sourceLocationPromise, stackPromise])
    if (sourceLocation) {
      log.sourceLocation = sourceLocation
    }
    if (stack && stack.length) {
      log.stack = stack
    }
  } catch (err) {
    // Ignore
  }
  return log
}

const broadcastInitialDebugStatus = () => {
  const screenHeight = window.screen.height * window.devicePixelRatio
  const screenWidth = window.screen.width * window.devicePixelRatio
  const status = debugFlag

  const msg = JSON.stringify({
    action: 'BROADCAST',
    broadcast_data: {
      action: 'INITIAL_DEBUG_HUD_STATUS',
      data: {
        deviceId,
        sessionId,
        status,
        screenHeight,
        screenWidth,
        ua,
        simulatorId,
      },
      // Do not broadcast console to connections with deviceId
      FilterExpression: 'attribute_not_exists(deviceId)',
    },
  })
  ws?.send(msg)
}

const broadcastSetDebugStatus = (status: boolean) => {
  localStorage.setItem(debugHudKey, status.toString())
  const msg = JSON.stringify({
    action: 'BROADCAST',
    broadcast_data: {
      action: 'SET_DEBUG_HUD_STATUS',
      data: {
        deviceId,
        sessionId,
        status,
        simulatorId,
      },
      // Do not broadcast console to connections with deviceId
      FilterExpression: 'attribute_not_exists(deviceId)',
    },
  })
  ws?.send(msg)
}

const clearLog = async () => {
  if (!ws) {
    return
  }

  if (messageQueue.length === 0) {
    logClearingTimer = null
    return
  }

  const screenHeight = window.screen.height * window.devicePixelRatio
  const screenWidth = window.screen.width * window.devicePixelRatio

  const messagesToSend = messageQueue
  messageQueue = []
  logClearingTimer = setTimeout(clearLog, NORMAL_TIMER_TIMEOUT)

  // clear out the queue
  const logs = await Promise.all(messagesToSend.map(resolveLog))

  ws.send(JSON.stringify({
    action: 'BROADCAST',
    broadcast_data: {
      action: 'CONSOLE_ACTIVITY',
      data: {
        logs,
        deviceId,
        sessionId,
        ua,
        screenHeight,
        screenWidth,
        simulatorId,
      },
      // Do not broadcast console to connections with deviceId
      FilterExpression: 'attribute_not_exists(deviceId)',
    },
  }))
}

const logConsoleActivity = (logData: LogData) => {
  if (messageQueue.length < MAX_QUEUE_SIZE) {
    messageQueue.push(logData)
  }
  if (logClearingTimer == null) {
    logClearingTimer = setTimeout(clearLog, INITIAL_TIMER_TIMEOUT)
  }
}

let previousTimestamp = 0
const getUniqueTimestamp = () => {
  let timestamp = Date.now()
  if (timestamp <= previousTimestamp) {
    timestamp = previousTimestamp + 1
  }

  previousTimestamp = timestamp
  return timestamp
}

const startupTimestamp = getUniqueTimestamp()

const maybeLogConsoleActivity = (
  fn: string,
  args: any[],
  sourceLocationPromise: SourceLocationPromise | null,
  stackPromise: StackPromise | null
) => {
  let logString = getPrintableArgs(args)
  if (logString.length > MAX_MSG_LENGTH) {
    logString = `${logString.slice(0, MAX_MSG_LENGTH - 3)}...`
  }

  const logOpts = {
    fn,
    args: [logString],
    timestamp: getUniqueTimestamp(),
    sourceLocationPromise,
    stackPromise,
  }
  logConsoleActivity(logOpts)
  return true
}

const reloadSimulator = (currentSimulatorConfig = simulatorConfig) => {
  const url = new URL(originalUrl)
  url.searchParams.set('simulatorConfig', JSON.stringify(currentSimulatorConfig))
  window.location.href = url.toString()
  broadcastReloadConfirmation(simulatorId)
}

const simulatorIdMatches = (incomingSimulatorId: string) => (
  incomingSimulatorId && incomingSimulatorId === simulatorId
)

const studioEventStream = createStudioEventStreamManager(() => ws)
const studioDebug = createStudioDebugManager(
  sessionId, ua, simulatorId, studioEventStream
)

const startWebSocket = () => {
  if (!channel) {
    return
  }
  const queryParams = new URLSearchParams()
  queryParams.set('channel', channel)
  queryParams.set('deviceId', deviceId)
  queryParams.set('ua', ua)
  const queryString = queryParams.toString()

  // eslint-disable-next-line max-len
  const webSocketUrl = `wss://<REMOVED_BEFORE_OPEN_SOURCING>.execute-api.us-west-2.amazonaws.com/prod?${queryString}`
  const _ws = new WebSocket(webSocketUrl)

  _ws.onopen = () => {
    ws = _ws
    socketFailureCount = 0
    clearLog()
    if (xrHud) {
      broadcastInitialDebugStatus()
    }
    if (!sentStartEvent) {
      sentStartEvent = true
      ws.send(JSON.stringify({
        action: 'BROADCAST',
        broadcast_data: {
          action: 'SESSION_START',
          data: {
            deviceId,
            sessionId,
            timestamp: startupTimestamp,
          },
          // Do not broadcast console to connections with deviceId
          FilterExpression: 'attribute_not_exists(deviceId)',
        },
      }))
    }
  }

  _ws.onmessage = (event) => {
    let msg
    try {
      msg = JSON.parse(event.data)
    } catch (err) {
      return
    }

    // Handle the message for studio debug events
    studioEventStream.handleSocketMessage(msg)

    if (msg.action === 'EVAL') {
      console.log(eval(msg.cmd))
    } else if (msg.action === 'DEBUG_HUD') {
      if (xrHud) {
        if (msg.enable) {
          xrHud.enable({console: true, version: true, verbose: true})
        } else {
          xrHud.disable()
        }
      }
    }
  }

  _ws.onclose = () => {
    ws = null
    socketFailureCount++
    if (socketRestartCount++ < MAX_SOCKET_RESTARTS) {
      const backoff = BACKOFF_SCALE_MS * ((2 ** socketFailureCount) + Math.random())
      setTimeout(startWebSocket, Math.min(backoff, MAX_WAIT_MS))
    }
  }
}

const wrapConsoleMethods = () => {
  if (!window.console) {
    return
  }
  Object.keys(window.console).forEach((fn) => {
    if (typeof window.console[fn] !== 'function') {
      return
    }
    const oldFn = window.console[fn].bind(window.console)
    window.console[fn] = (...args) => {
      let sourceLocationPromise: SourceLocationPromise | null = null
      let stackPromise: StackPromise | null = null
      try {
        const parsedStack = ErrorStackParser.parse(new Error())
        if (parsedStack && parsedStack.length > 1) {
          // Have to start from the second frame in the stack to get the caller's frame
          const stack = parsedStack.slice(1)
          const sourceFrame = stack[0]
          sourceLocationPromise = getSourceLocationForStackFrame(sourceFrame)
          if (fn === 'warn' || fn === 'error') {
            stackPromise = processStack(stack)
          }
        }
      } catch (err) {
        // Ignore
      }
      xrHud?.notifyLog(fn, args)
      maybeLogConsoleActivity(fn, args, sourceLocationPromise, stackPromise)
      Function.prototype.apply.call(oldFn, window.console, args)
    }
  })
}

const initialSetup = () => {
  xrHud = XrHudManager()
  xrSimulator = XrSimulatorManager()
  if (ws) {
    broadcastInitialDebugStatus()
  }
  if (debugFlag) {
    xrHud.enable({console: true, version: true, verbose: true})
  }
  if (needsUpdate(simulatorConfig)) {
    xrSimulator.enable(simulatorConfig, simulatorRendererConfig, broadcastSequenceProgress)
  }
  xrHud.onDisable(() => broadcastSetDebugStatus(false))
  xrHud.onEnable(() => broadcastSetDebugStatus(true))
  studioDebug.ready()
}
const state = document.readyState
if (state === 'interactive' || state === 'complete') {
  initialSetup()
} else {
  document.addEventListener('DOMContentLoaded', initialSetup)
}

window.addEventListener('error', (event) => {
  maybeWarnCrossOriginError(event)
  let stackPromise: StackPromise | null = null
  try {
    const parsedStack = ErrorStackParser.parse(event.error)
    stackPromise = processStack(parsedStack)
  } catch (err) {
    // Ignore
  }

  xrHud?.notifyLog('error', [event.message])
  maybeLogConsoleActivity(
    'error',
    [event.message],
    getSourceLocationForErrorEvent(event),
    stackPromise
  )
})

window.addEventListener('unhandledrejection', ({reason}) => {
  let message
  let sourceLocationPromise: SourceLocationPromise | null = null
  let stackPromise: StackPromise | null = null
  if (reason instanceof Error) {
    message = reason.toString()
    try {
      const parsedStack = ErrorStackParser.parse(reason)
      const [topFrame] = parsedStack
      stackPromise = processStack(parsedStack)
      sourceLocationPromise = getSourceLocationForStackFrame(topFrame)
    } catch (err) {
      // Ignore
    }
  } else {
    message = reason
  }

  const logArgs = ['Unhandled promise rejection:', message]
  xrHud?.notifyLog('error', logArgs)
  maybeLogConsoleActivity(
    'error',
    logArgs,
    sourceLocationPromise,
    stackPromise
  )
})

window.addEventListener('message', (event) => {
  studioEventStream.handlePostMessage(event.data)
})

window.addEventListener('message', (event) => {
  if (xrSimulator) {
    let config
    switch (event.data.action) {
      case 'SIMULATOR_CONFIG_UPDATE':
        config = event.data.data.simulatorConfig
        if (simulatorIdMatches(config?.simulatorId)) {
          if (needsUpdate(config)) {
            // Fire a Location Lost event when switching to a different location so that the
            // simulator stops rendering the objects of the previously selected location.
            if (config?.poiId !== simulatorConfig.poiId) {
              xrSimulator.dispatchLocationLost()
            }
            Object.assign(simulatorConfig, config)
            Object.assign(simulatorRendererConfig, event.data.data.simulatorRendererConfig)
            xrSimulator.updateSimulator(simulatorConfig, simulatorRendererConfig)
          } else {
            xrSimulator.disable()
          }
        }
        break
      case 'SIMULATOR_RELOAD':
        config = event.data.data.simulatorConfig
        if (simulatorIdMatches(config?.simulatorId)) {
          reloadSimulator(config)
        }
        break
      case 'SIMULATOR_SCRUB':
        if (event.data.data) {
          const {progress} = event.data.data
          if (progress) {
            xrSimulator.scrub(progress)
          }
        }
        break
      case 'SIMULATOR_STOP_SCRUB':
        xrSimulator.stopScrub()
        break
      case 'SIMULATOR_RECENTER':
        window.XR8.XrController.recenter()
        break
      case 'SIMULATOR_DISPATCH_LOCATIONSCANNING':
        xrSimulator.dispatchLocationScanning()
        break
      case 'SIMULATOR_DISPATCH_LOCATIONFOUND':
        xrSimulator.dispatchLocationFound()
        break
      case 'SIMULATOR_DISPATCH_LOCATIONLOST':
        xrSimulator.dispatchLocationLost()
        break
      case 'SIMULATOR_DISPATCH_MESHFOUND':
        xrSimulator.dispatchMeshFound()
        break
      case 'SIMULATOR_DISPATCH_MESHLOST':
        xrSimulator.dispatchMeshLost()
        break
      case 'SIMULATOR_GPS8W':
        window.dispatchEvent(new CustomEvent('gps8w', {detail: event.data.gps8w}))
        break
      default:
        break
    }
  }
})

window.addEventListener('beforeunload', () => {
  studioDebug.close()
})

wrapConsoleMethods()

startWebSocket()
injectGoogleNoTranslateRule()
