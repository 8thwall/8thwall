import type {DeepReadonly} from 'ts-essentials'

import type {ConsoleActivityMessage} from '@ecs/shared/debug-messaging'

import {getDeviceTitle} from './device-models'
import {SYSTEM_STREAM_NAME} from './logs/log-constants'
import type {ILog, ILogStream} from './logs/types'
import type {ScopedEditorState} from './editor-reducer'

const LOG_ITEMS_LIMIT = 2000

const createStream = (
  streamName: string,
  deviceId: string,
  logs: ILog[],
  ua: string,
  screenHeight: number,
  screenWidth: number,
  debugMode = false,
  isClearOnRunActive = true  // by default clear logs on run
): ILogStream => ({
  name: streamName,
  logs,
  title: getDeviceTitle(ua, screenHeight, screenWidth) || streamName,
  deviceId,
  isDebugHudActive: debugMode,
  isClearOnRunActive,
})

const addStreamToStreams = (stream: ILogStream, logStreams: DeepReadonly<ILogStream[]>) => {
  const newLogStreams = logStreams.slice(0)
  // Keep System and [DEV ONLY] Raw at the front of the list.
  if ([SYSTEM_STREAM_NAME, '[DEV ONLY] Raw'].includes(stream.name)) {
    newLogStreams.unshift(stream)
  } else {
    newLogStreams.push(stream)
  }
  return newLogStreams
}

type NewLogData = Omit<ILog, 'timestamp' | 'numRedundant' | 'key'> & {
  deviceId?: string
  ua?: string
  key?: number
  timestamp?: number
  numRedundant?: number
}

const updateLogState = (
  streamName: string, newLog: NewLogData, screenHeight: number, screenWidth: number,
  state: ScopedEditorState
) => {
  const log: ILog = {
    ...newLog,
    key: newLog.key || Math.random(),
    timestamp: newLog.timestamp || Number(new Date()),
    numRedundant: newLog.numRedundant || 1,
  }

  const existingLogStreamIndex = state.logStreams.findIndex(({name}) => name === streamName)

  // Add new stream
  if (existingLogStreamIndex === -1) {
    const newLogStream = createStream(
      streamName, newLog.deviceId, [log], newLog.ua, screenHeight, screenWidth
    )
    return {
      logStreams: addStreamToStreams(newLogStream, state.logStreams),
    }
  }

  // Add log to existing stream
  const newLogStreams = state.logStreams.map((stream, index) => {
    if (index === existingLogStreamIndex) {
      const newLogs = stream.logs.slice(Math.max(0, stream.logs.length - LOG_ITEMS_LIMIT))
      const prevLog = newLogs[newLogs.length - 1]

      // Check for redundant log.
      if (prevLog?.text === log.text && prevLog?.type === log.type) {
        const replacedLog = {...prevLog, numRedundant: prevLog.numRedundant + 1}
        newLogs[newLogs.length - 1] = replacedLog
        return {...stream, logs: newLogs}
      }

      // Insert in sorted order by timestamp
      let indexToInsert = 0
      for (let i = newLogs.length - 1; i >= 0; i--) {
        if (newLogs[i].timestamp < log.timestamp) {
          indexToInsert = i + 1
          break
        }
      }

      newLogs.splice(indexToInsert, 0, log)

      return {...stream, logs: newLogs}
    }
    return stream
  })

  return {
    logStreams: newLogStreams,
  }
}

type InsertLogRequest = {
  streamName: string
  log: NewLogData
  screenHeight?: number
  screenWidth?: number
}

const updateLogStates = (
  newLogs: InsertLogRequest[], state: ScopedEditorState
): ScopedEditorState => newLogs.reduce(
  (s, e) => ({
    ...s,
    ...updateLogState(e.streamName, e.log, e.screenHeight, e.screenWidth, s),
  }),
  state
)

const messageLog = (msg: ConsoleActivityMessage): InsertLogRequest[] => msg.logs.map(l => ({
  streamName: msg.sessionId || msg.deviceId,
  screenHeight: msg.screenHeight,
  screenWidth: msg.screenWidth,
  log: {
    timestamp: l.timestamp,
    type: l.fn,
    text: l.args.join(' '),
    ua: msg.ua,
    deviceId: msg.sessionId || msg.deviceId,
    numRedundant: 1,
    sourceLocation: l.sourceLocation,
    stack: l.stack,
  },
}))

const ConsoleLogStreams = {
  messageLog,
  updateLogStates,
  createStream,
  addStreamToStreams,
}

export default ConsoleLogStreams

export type {
  InsertLogRequest,
}
