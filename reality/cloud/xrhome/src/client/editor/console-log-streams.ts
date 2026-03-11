import type {DeepReadonly} from 'ts-essentials'

import {getDeviceTitle} from './device-models'
import {BUILD_TAG, REPO_TAG, SYSTEM_STREAM_NAME} from './logs/log-constants'
import {getTruncatedHash} from '../git/g8-commit'
import type {ILog, ILogStream} from './logs/types'
import type {ScopedEditorState} from './editor-reducer'

const LOG_ITEMS_LIMIT = 2000

const tryParse = (s: string) => {
  try {
    return JSON.parse(s)
  } catch (err) {
    return null
  }
}

// NOTE(christoph): The input is expected to be a JSON-encoded array of strings
const getLineStrings = (s: string) => {
  const lines = tryParse(s)

  if (!lines) {
    return ''
  }

  return lines.join('\n')
}

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

type Dev8Log = Pick<ILog, 'timestamp' | 'sourceLocation' | 'stack'> & {
  fn: string      // This gets translated to `type`
  args: string[]  // This gets translated to `text`
}

type ConsoleActivityMessage = {
  action: 'CONSOLE_ACTIVITY'
  deviceId: string
  screenHeight: number
  screenWidth: number
  ua: string
  sessionId: string
  simulatorId?: string
  logs?: Dev8Log[]
} & Partial<Dev8Log>  // NOTE(christoph): We used to have one log per message, the added "logs"

type NewBuildMessage = {
  action: 'NEW_BUILD'
  branch: string
  commitId: string
  errors: string  // JSON encoded array
  warnings: string  // JSON encoded array
  buildStatus: 'ERROR' | 'WARNING' | 'OK'
}

type BuildRequestMessage = {
  action: 'BUILD_REQUEST'
  branch: string
  commitId: string
}

type IncomingMessage = ConsoleActivityMessage | NewBuildMessage | BuildRequestMessage

const devOnlyRawMessageLog = (msg) => {
  if (BuildIf.LOCAL_DEV) {
    return {streamName: '[DEV ONLY] Raw', log: {text: JSON.stringify(msg, null, 2)}}
  }
  return null
}

const messageLog = (msg: IncomingMessage, title = ''): InsertLogRequest[] | InsertLogRequest => {
  if (msg.action === 'CONSOLE_ACTIVITY') {
    if (msg.logs) {
      return msg.logs.map(l => (
        {
          streamName: msg.sessionId || msg.deviceId,
          screenHeight: msg.screenHeight,
          screenWidth: msg.screenWidth,
          log: {
            timestamp: l.timestamp,
            type: l.fn,
            text: l.args.join(' '),
            ua: msg.ua,
            deviceId: msg.deviceId,
            numRedundant: 1,
            sourceLocation: l.sourceLocation,
            stack: l.stack,
          },
        }
      ))
    } else {
      return [{
        streamName: msg.sessionId || msg.deviceId,
        screenHeight: msg.screenHeight,
        screenWidth: msg.screenWidth,
        log: {
          timestamp: msg.timestamp,
          type: msg.fn,
          text: msg.args.join(' '),
          ua: msg.ua,
          deviceId: msg.deviceId,
          numRedundant: 1,
          sourceLocation: msg.sourceLocation,
        },
      }]
    }
  }

  const isRepoBranch = msg.branch && ['master', 'staging', 'production'].includes(msg.branch)
  const shortid = getTruncatedHash(msg.commitId)

  if (msg.action === 'BUILD_REQUEST') {
    if (isRepoBranch) {
      return {
        streamName: SYSTEM_STREAM_NAME,
        log: {
          text: `${title}New commit on ${msg.branch}. [${shortid}]`,
          tags: [REPO_TAG],
        },
      }
    }
    return {
      streamName: SYSTEM_STREAM_NAME,
      log: {
        text: `${title}Build started. [${shortid}]`,
        tags: [BUILD_TAG],
      },
    }
  }

  if (msg.action === 'NEW_BUILD') {
    if (isRepoBranch) {
      if (msg.buildStatus === 'ERROR') {
        const errs = getLineStrings(msg.errors)
        return {
          streamName: SYSTEM_STREAM_NAME,
          log: {
            type: 'error',
            text: `${title}Build failed on ${msg.branch} [${shortid}] with errors:\n${errs}`,
            tags: [REPO_TAG],
          },
        }
      }

      if (msg.buildStatus === 'WARNING') {
        const warns = getLineStrings(msg.warnings)
        return {
          streamName: SYSTEM_STREAM_NAME,
          log: {
            type: 'warn',
            text: `${title}Build completed on ${msg.branch} [${shortid}] with warnings:\n${warns}`,
            tags: [REPO_TAG],
          },
        }
      }

      return {
        streamName: SYSTEM_STREAM_NAME,
        log: {
          type: 'success',
          text: `${title}Build completed on ${msg.branch}. [${shortid}]`,
          tags: [REPO_TAG],
        },
      }
    }

    if (msg.buildStatus === 'ERROR') {
      const errs = getLineStrings(msg.errors)
      return {
        streamName: SYSTEM_STREAM_NAME,
        log: {
          type: 'error',
          text: `${title}Build failed for [${shortid}] with errors:\n${errs}`,
          tags: [BUILD_TAG],
        },
      }
    }

    if (msg.buildStatus === 'WARNING') {
      const warns = getLineStrings(msg.warnings)
      return {
        streamName: SYSTEM_STREAM_NAME,
        log: {
          type: 'warn',
          text: `${title}Build completed for [${shortid}] with warnings:\n${warns}`,
          tags: [BUILD_TAG],
        },
      }
    }

    return {
      streamName: SYSTEM_STREAM_NAME,
      log: {
        type: 'success',
        text: `${title}Build completed. [${shortid}]`,
        tags: [BUILD_TAG],
      },
    }
  }

  return null
}

const ConsoleLogStreams = {
  devOnlyRawMessageLog,
  messageLog,
  updateLogStates,
  createStream,
  addStreamToStreams,
}

export default ConsoleLogStreams

export type {
  IncomingMessage,
}
