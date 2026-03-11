import {useSelector} from '../../hooks'
import type {ILogStream} from './types'
import {SYSTEM_STREAM_NAME} from './log-constants'

const DEFAULT_MAIN_LOG_STREAM: ILogStream = {
  name: SYSTEM_STREAM_NAME,
  title: SYSTEM_STREAM_NAME,
  logs: [],
  isClearOnRunActive: false,
  isDebugHudActive: false,
}

type LogStreams = readonly ILogStream[]

const useLogStreams = (key: string): LogStreams => (
  useSelector(s => s.editor.byKey?.[key]?.logStreams) || []
)

const useHasDeviceTab = (key: string): boolean => (
  useSelector(s => s.editor.byKey?.[key]?.logStreams)?.some(log => !!log.deviceId)
)

const getSystemStream = (logStreams: LogStreams): ILogStream => (
  logStreams.find(({name}) => name === SYSTEM_STREAM_NAME)
)

const getMainLogStream = (logStreams: LogStreams): ILogStream => (
  getSystemStream(logStreams) || DEFAULT_MAIN_LOG_STREAM
)

const getAvailableStreams = (logStreams: LogStreams): LogStreams => (
  getSystemStream(logStreams) ? logStreams : [DEFAULT_MAIN_LOG_STREAM, ...logStreams]
)

export {
  useLogStreams,
  useHasDeviceTab,
  getSystemStream,
  getMainLogStream,
  getAvailableStreams,
}
