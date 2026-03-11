import type {LocalSyncMessage} from './local-sync-types'
import type {StudiohubProtocol} from './desktop-protocol-types'

const ELECTRON_API_KEY = 'electron'

type OsType = 'mac' | 'other'

type LocalSyncHandler = (msg: LocalSyncMessage) => void
type FileWatchApi = {
  addHandler: (appKey: string, handler: LocalSyncHandler) => void
  removeHandler: (appKey: string) => void
}

type ElectronApi = {
  os: OsType
  onExternalNavigate: (callback: (pathAndQuery: string) => void) => () => void
  fileWatch: FileWatchApi
  studiohubProtocol: StudiohubProtocol
  // For custom title bar
  minimizeWindow: () => void
  maximizeWindow: () => void
  closeWindow: () => void
}

export {ELECTRON_API_KEY}

export type {ElectronApi, LocalSyncHandler, FileWatchApi}
