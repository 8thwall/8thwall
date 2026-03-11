import type {UnixPath} from './unix-path'

type Project = {
  appKey: string
  location: string
  initialization: 'needs-initialization'  // The app never finished initializing from the cloud
  | 'done'  // The app finished initializing from the cloud
  | 'v2'  // The app is initialized to the "exported" project structure
  accessedAt?: number | null
}

type ProjectClientSide = Omit<Project, 'appKey'> & {
  validLocation: boolean
}

type LocalSyncFileChanged = {
  action: 'STUDIO_FILE_CHANGE'
  appKey: string
  path: UnixPath
  content: string
}

type LocalSyncFileDeleted = {
  action: 'STUDIO_FILE_DELETE'
  appKey: string
  path: UnixPath
}

type LocalSyncDirChanged = {
  action: 'STUDIO_DIR_CHANGE'
  appKey: string
  change: 'created' | 'deleted'
  path: UnixPath
}

type LocalSyncAssetChanged = {
  action: 'STUDIO_ASSET_CHANGE'
  appKey: string
  change: 'created' | 'modified' | 'deleted'
  path: UnixPath
}

type LocalSyncInvalidFile = {
  action: 'STUDIO_INVALID_FILE'
  appKey: string
  path: UnixPath
}

type LocalSyncConnectFileWatch = {
  action: 'LOCAL_CONNECT_FILE_WATCH'
  appKey: string
}

type LocalSyncCloseFileWatch = {
  action: 'LOCAL_CLOSE_FILE_WATCH'
  appKey: string
}

type LocalSyncMessage = LocalSyncFileChanged | LocalSyncFileDeleted | LocalSyncDirChanged
  | LocalSyncAssetChanged | LocalSyncInvalidFile
  | LocalSyncConnectFileWatch | LocalSyncCloseFileWatch

type InitializeResponse = {
  canceled: false
  projectPath: string
  initialization: Project['initialization']
  appKey: string
}

type CanceledInitializeResponse = {
  canceled: true
  appKey?: never
  projectPath?: never
  initialization?: never
}

type FileSnapshotResponse = {
  timestampsByPath: {[path: UnixPath]: number}
}

type ListProjectsResponse = {
  projectByAppKey: {[appKey: string]: ProjectClientSide}
}

type NewProjectLocationResponse = {
  projectPath: string
}

export type {
  Project,
  ProjectClientSide,
  LocalSyncFileChanged,
  LocalSyncFileDeleted,
  LocalSyncDirChanged,
  LocalSyncAssetChanged,
  LocalSyncInvalidFile,
  LocalSyncMessage,
  InitializeResponse,
  FileSnapshotResponse,
  ListProjectsResponse,
  NewProjectLocationResponse,
  UnixPath,
  CanceledInitializeResponse,
}
