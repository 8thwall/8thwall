import type {ScopedDebugMessage} from '@repo/c8/ecs/src/shared/debug-messaging'

import type {LocalSyncMessage} from './local-sync-types'
import type {StudiohubProtocol} from './desktop-protocol-types'
import type {SystemLogHandler} from './system-log-types'
import type {ListenerPool} from '../listener-pool'

const ELECTRON_API_KEY = 'electron'

type OsType = 'mac' | 'other'

type LocalSyncHandler = (msg: LocalSyncMessage) => void
type FileWatchApi = {
  addHandler: (appKey: string, handler: LocalSyncHandler) => void
  removeHandler: (appKey: string) => void
}

type SystemLogApi = {
  setHandler: (appKey: string, handler: SystemLogHandler) => void
  clearHandler: (appKey: string) => void
}

type Dev8SocketApi = {
  setListener: (l: null | ((d: ScopedDebugMessage) => void)) => void
  toDevice: Pick<ListenerPool<ScopedDebugMessage>, 'dispatch'>
}

type ElectronApi = {
  os: OsType
  onExternalNavigate: (callback: (pathAndQuery: string) => void) => () => void
  fileWatch: FileWatchApi
  systemLog: SystemLogApi
  dev8Socket: Dev8SocketApi
  studiohubProtocol: StudiohubProtocol
  // For custom title bar
  minimizeWindow: () => void
  maximizeWindow: () => void
  closeWindow: () => void
}

export {ELECTRON_API_KEY}

export type {
  ElectronApi,
  LocalSyncHandler,
  FileWatchApi,
  SystemLogApi,
  Dev8SocketApi,
  SystemLogHandler,
}
