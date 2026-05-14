import ErrorStackParser from 'error-stack-parser'

import {getPrintableArgs} from './printable'
import {
  getSourceLocationForStackFrame, getSourceLocationForErrorEvent, processStack,
  SourceLocationPromise, StackPromise, SourceLocation, BaseInfoStack,
} from './source-location'
import {maybeWarnCrossOriginError} from './cross-origin'
import type {XrHudManager} from './xrhud/xr-hud-manager'
import type {DebugStream} from './shared/ecs/shared/debug-messaging'
import {getUniqueTimestamp} from './unique-timestamp'

type LogData = {
  fn: string
  args: string[]
  timestamp: number
  sourceLocationPromise: SourceLocationPromise | null
  stackPromise: StackPromise | null
  sourceLocation?: SourceLocation
  stack?: BaseInfoStack
}

type DeviceInfo = {
  deviceId: string
  sessionId: string
  ua: string
  simulatorId: string | undefined
}

const captureLogs = (
  eventStream: DebugStream,
  xrHud: ReturnType<typeof XrHudManager>,
  deviceInfo: DeviceInfo
) => {
  if (!window.console) {
    return
  }

  let logClearingTimer: ReturnType<typeof setTimeout> | null = null
  let messageQueue: LogData[] = []
  const INITIAL_TIMER_TIMEOUT = 250
  const NORMAL_TIMER_TIMEOUT = 1000
  const MAX_QUEUE_SIZE = 100
  const MAX_MSG_LENGTH = 1000

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

  const clearLog = async () => {
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

    eventStream.send({
      action: 'CONSOLE_ACTIVITY',
      logs,
      ...deviceInfo,
      screenHeight,
      screenWidth,
    })
  }

  const logConsoleActivity = (logData: LogData) => {
    if (messageQueue.length < MAX_QUEUE_SIZE) {
      messageQueue.push(logData)
    }
    if (logClearingTimer == null) {
      logClearingTimer = setTimeout(clearLog, INITIAL_TIMER_TIMEOUT)
    }
  }

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
}

export {
  captureLogs,
}
