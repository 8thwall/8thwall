import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import {useTranslation} from 'react-i18next'
import {useQueryClient} from '@tanstack/react-query'

import type {
  LocalSyncMessage, LocalSyncFileChanged, LocalSyncDirChanged, LocalSyncAssetChanged,
  LocalSyncInvalidFile, FileSnapshotResponse,
} from '../../shared/desktop/local-sync-types'
import {useCurrentGit} from '../git/hooks/use-current-git'
import {isAssetPath} from '../common/editor-files'
import {useEvent} from '../hooks/use-event'
import {
  watchLocal, stopWatchLocal, pushFile, getFileStateSnapshot, deleteLocalFile,
  pullSrcFile, getProjectStatus,
} from './local-sync-api'
import useActions from '../common/use-actions'
import coreGitActions from '../git/core-git-actions'
import {useAbandonableEffect} from '../hooks/abandonable-effect'
import type {IGit} from '../git/g8-dto'
import {useStudioStateContext} from './studio-state-context'
import {useEnclosedAppKey} from '../apps/enclosed-app-context'
import {getRuntimeMetadataQuery} from './runtime-version/use-runtime-metadata'
import {getProjectConfigStatusQuery} from '../hooks/use-project-config'

type FileSyncStatus =
| 'checking'  // Checking what the local state is
| 'initialized'  // Local sync was already initialized, ready to start listening
| 'listening'  // Listening for local changes
| 'active'  // Actively syncing files, changes are being processed

type ILocalSyncContext = {
  appKey: string
  localBuildUrl?: string
  localBuildRemoteUrl?: string
  assetVersions: Record<string, string>
  fileSyncStatus: FileSyncStatus
  restartServer: () => Promise<void>
}

const LocalSyncContext = React.createContext<ILocalSyncContext | null>(null)

const useLocalSyncContext = () => {
  const ctx = React.useContext(LocalSyncContext)
  if (!ctx) {
    throw new Error('useLocalSyncContext must be used within a LocalSyncContextProvider')
  }
  return ctx
}

type PendingWrites = Record<string, 'redux' | 'disk'>
type PendingWritesRef = {current: PendingWrites | undefined}

const clearPendingWrite = (pendingWrites: PendingWritesRef, path: string) => {
  if (!pendingWrites.current) {
    return
  }
  delete pendingWrites.current[path]
}
const getPendingWrite = (pendingWrites: PendingWritesRef, path: string) => {
  if (!pendingWrites.current) {
    return undefined
  }
  return pendingWrites.current[path]
}

const setPendingWrite = (
  pendingWrites: PendingWritesRef, path: string, target: 'redux' | 'disk'
) => {
  if (!pendingWrites.current) {
    pendingWrites.current = {}
  }
  pendingWrites.current[path] = target
}

// NOTE(christoph): We're receiving changes from disk, and changes from redux constantly
// as the user makes changes.
// If we forwarded every write between the two, we would end up in an infinite loop of writes.
// When we have a pending write from redux, we ignore the disk change, because it'll (probably) be
// what we just wrote. The same applies the other way around.
const maybeIgnoreWrite = (
  pendingWrites: PendingWritesRef,
  path: string,
  source: 'redux' | 'disk'
) => {
  const pendingWrite = getPendingWrite(pendingWrites, path)
  if (pendingWrite === source) {
    // We already have a pending write from the source. This write has made it to the
    // destination and back so we ignore this change and clear our pending record.
    clearPendingWrite(pendingWrites, path)
    return true
  }
  return false
}

const maybeIgnoreDiskWrite = (
  pendingWrites: PendingWritesRef,
  path: string
) => maybeIgnoreWrite(pendingWrites, path, 'disk')

const maybeIgnoreReduxWrite = (
  pendingWrites: PendingWritesRef,
  path: string
) => maybeIgnoreWrite(pendingWrites, path, 'redux')

const LocalSyncContextProvider: React.FC<{children: React.ReactNode}> = ({children}) => {
  const {t} = useTranslation('cloud-studio-pages')
  const stateCtx = useStudioStateContext()
  const appKey = useEnclosedAppKey()
  const git = useCurrentGit()
  const {filesByPath, repo} = git
  const [fileSyncStatus, setFileSyncStatus] = React.useState<FileSyncStatus>('checking')
  const {saveFiles, deleteFile, deleteFiles, createFolder} = useActions(coreGitActions)
  const [localBuildUrl, setLocalBuildUrl] = React.useState<string>('')
  const [localBuildRemoteUrl, setLocalBuildRemoteUrl] = React.useState<string>('')
  const prevFilesByPathRef = React.useRef<DeepReadonly<IGit['filesByPath']> | undefined>(undefined)
  const pendingWritesRef = React.useRef<PendingWrites | undefined>(undefined)
  // const socketChannel = getWebsocketChannelName({git, app}, 'current-client')
  const deferredAssetsRef = React.useRef<Set<string>>()
  const [assetVersions, setAssetVersions] = React.useState<Record<string, string> | undefined>({})
  const queryClient = useQueryClient()

  const getDeferredAssets = () => {
    if (!deferredAssetsRef.current) {
      deferredAssetsRef.current = new Set()
    }
    return deferredAssetsRef.current!
  }

  const maybeDeleteItem = async (path: string) => {
    clearPendingWrite(pendingWritesRef, path)
    await deleteFile(repo, path)
  }

  const maybeCreateFolder = async (path: string) => {
    if (filesByPath[path]) {
      return
    }
    await createFolder(repo, path)
  }

  const handleFileChange = async (msg: LocalSyncFileChanged) => {
    const {path, content} = msg
    if (maybeIgnoreDiskWrite(pendingWritesRef, path)) {
      return
    }
    setPendingWrite(pendingWritesRef, path, 'redux')
    await saveFiles(repo, [{filePath: path, content}])
  }

  const handleDirChange = (msg: LocalSyncDirChanged) => {
    switch (msg.change) {
      case 'created':
        maybeCreateFolder(msg.path)
        break
      case 'deleted':
        maybeDeleteItem(msg.path)
        break
      default:
        // eslint-disable-next-line no-console
        console.warn('Unknown directory change: ', msg.change)
    }
  }

  const pullLocalFiles = async (paths: string[]) => {
    try {
      const filesToSave: {filePath: string, content: string}[] = []

      await Promise.all(paths.map(async (path) => {
        if (isAssetPath(path)) {
          const storedAssetKey = filesByPath[path]?.content
          if (storedAssetKey) {
            clearPendingWrite(pendingWritesRef, path)
            return
          }

          filesToSave.push({
            filePath: path,
            content: 'placeholder',
          })
        } else {
          const fileContent = await pullSrcFile(appKey, path)
          filesToSave.push({
            filePath: path,
            content: fileContent,
          })
        }
      }))

      if (filesToSave.length) {
        await saveFiles(repo, filesToSave)
      }
    } catch (error) {
      // eslint-disable-next-line no-console
      console.error('Failed to pull local files:', error)
    }
  }

  const handleAssetChange = async (msg: LocalSyncAssetChanged) => {
    switch (msg.change) {
      case 'created':
      case 'modified': {
        const reduxPath = msg.path
        setAssetVersions(prev => ({
          ...prev,
          [reduxPath]: Date.now().toString(36),
        }))
        if (maybeIgnoreDiskWrite(pendingWritesRef, msg.path)) {
          return
        }
        if (filesByPath[reduxPath]) {
          // If the file already exists on redux, we can pull it later
          getDeferredAssets().add(msg.path)
        } else {
          // If it doesn't exist, we need to create it immediately to show up in the file browser
          await pullLocalFiles([msg.path])
        }
        break
      }
      case 'deleted': {
        getDeferredAssets().delete(msg.path)
        await maybeDeleteItem(msg.path)
        break
      }
      default:
        // eslint-disable-next-line no-console
        console.log('Asset change detected: ', msg)
    }
  }

  const handleInvalidFile = (msg: LocalSyncInvalidFile) => {
    stateCtx.update({
      errorMsg: t('local_sync.error.invalid_file', {filePath: msg.path}),
    })
  }

  const handleLocalSyncMessage = useEvent((msg: LocalSyncMessage) => {
    switch (msg.action) {
      case 'STUDIO_FILE_CHANGE':
        handleFileChange(msg)
        break
      case 'STUDIO_FILE_DELETE':
        maybeDeleteItem(msg.path)
        break
      case 'STUDIO_DIR_CHANGE':
        handleDirChange(msg)
        break
      case 'STUDIO_ASSET_CHANGE':
        handleAssetChange(msg)
        break
      case 'STUDIO_INVALID_FILE':
        handleInvalidFile(msg)
        break
      default:
        // eslint-disable-next-line no-console
        console.warn('Unknown message type: ', msg)
        break
    }
  })

  React.useEffect(() => {
    window.electron.fileWatch?.addHandler(appKey, handleLocalSyncMessage)
    setFileSyncStatus('listening')

    return () => {
      window.electron.fileWatch?.removeHandler(appKey)
    }
  }, [appKey])

  const canSyncFiles = fileSyncStatus === 'listening'
  useAbandonableEffect(async (abandon) => {
    if (!canSyncFiles) {
      return
    }
    try {
      const {
        timestampsByPath,
      }: FileSnapshotResponse = await abandon(getFileStateSnapshot(appKey))

      const filesThatShouldExistOnRedux = new Set<string>()

      const filesToPull = Object.entries(timestampsByPath).filter(([diskPath, diskTimestamp]) => {
        filesThatShouldExistOnRedux.add(diskPath)
        const existingFile = filesByPath[diskPath]
        if (!existingFile) {
          return true
        }
        const reduxTimestamp = existingFile.timestamp.getTime()
        // Sync changes that have been made on disk since the last sync
        return diskTimestamp > reduxTimestamp
      }).map(([path]) => path)

      const filesToDelete = Object.keys(filesByPath).filter(path => (
        !filesThatShouldExistOnRedux.has(path)
      ))

      await abandon(deleteFiles(repo, filesToDelete))

      await abandon(pullLocalFiles(filesToPull))
      setFileSyncStatus('active')
    } catch (error) {
      // eslint-disable-next-line no-console
      console.error('Failed to get file state snapshot:', error)
    }
  }, [appKey, canSyncFiles])

  const handlePushSourceFile = async (path: string, content: string, storeWrite: boolean) => {
    try {
      // handle text files
      if (isAssetPath(path)) {
        // NOTE(christoph): Assets are written directly to disk, this should never be necessary
        clearPendingWrite(pendingWritesRef, path)
        return
      }

      if (storeWrite) {
        setPendingWrite(pendingWritesRef, path, 'disk')
      }
      await pushFile(appKey, path, content)
      return
    } catch (error) {
      stateCtx.update({
        errorMsg: t('local_sync.error.invalid_file', {filePath: path}),
      })
    }
  }

  const refreshServerUrls = async () => {
    const {buildUrl, buildRemoteUrl} = await getProjectStatus(appKey)
    if (buildUrl) {
      const previewUrl = new URL(buildUrl)
      // previewUrl.searchParams.set('channel', socketChannel)
      setLocalBuildUrl(previewUrl.toString())
    } else {
      setLocalBuildUrl('')
    }

    if (buildRemoteUrl) {
      const previewUrl = new URL(buildRemoteUrl)
      // previewUrl.searchParams.set('channel', socketChannel)
      setLocalBuildRemoteUrl(previewUrl.toString())
    } else {
      setLocalBuildRemoteUrl('')
    }

    queryClient.invalidateQueries(getRuntimeMetadataQuery(appKey))
    queryClient.invalidateQueries(getProjectConfigStatusQuery(appKey))
  }

  useAbandonableEffect(async (abandon) => {
    await abandon(watchLocal(appKey))
    await refreshServerUrls()
  }, [appKey])

  const restartServer = async () => {
    await stopWatchLocal(appKey)
    await watchLocal(appKey)
    await refreshServerUrls()
  }

  // Close running dev server on unmount
  React.useEffect(() => () => {
    stopWatchLocal(appKey)
  }, [appKey])

  const canPushToLocal = fileSyncStatus === 'active'
  useAbandonableEffect(async () => {
    if (!canPushToLocal) {
      return
    }

    const prevFilesByPath = prevFilesByPathRef.current
    prevFilesByPathRef.current = filesByPath
    if (!prevFilesByPath) {
      return
    }
    const prevFilePaths = Object.keys(prevFilesByPath)
    const currentFilePaths = Object.keys(filesByPath)
    const removedFiles = prevFilePaths.filter(path => (
      !currentFilePaths.includes(path)
    ))

    try {
      await Promise.all(removedFiles.map(async (path) => {
        clearPendingWrite(pendingWritesRef, path)
        await deleteLocalFile(appKey, path)
      }))

      await Promise.all(Object.values(filesByPath).map(async ({filePath, isDirectory, content}) => {
        // NOTE(christoph): We never sync assets from redux to disk because they're just placeholder
        // files.
        if (isDirectory || isAssetPath(filePath)) {
          return
        }
        const prevFile = prevFilesByPath[filePath]
        if (prevFile && prevFile.content === content) {
          return
        }

        if (maybeIgnoreReduxWrite(pendingWritesRef, filePath)) {
          return
        }
        await handlePushSourceFile(filePath, content, /* storeWrite */ true)
      }))
    } catch (error) {
      // eslint-disable-next-line no-console
      console.error('Failed to push studio changes to local: ', error)
    }
  }, [appKey, canPushToLocal, filesByPath])

  const ctx: ILocalSyncContext = {
    appKey,
    localBuildUrl,
    localBuildRemoteUrl,
    assetVersions,
    fileSyncStatus,
    restartServer,
  }

  return (
    <LocalSyncContext.Provider value={ctx}>
      {children}
    </LocalSyncContext.Provider>
  )
}

type DeviceRemoteType = 'same-device' | 'remote-device'
const useLocalBuildUrl = (deviceType: DeviceRemoteType = 'same-device') => {
  const ctx = React.useContext(LocalSyncContext)
  return deviceType === 'remote-device' ? ctx?.localBuildRemoteUrl : ctx?.localBuildUrl
}

const useMaybeLocalSyncContext = (): ILocalSyncContext | null => (
  React.useContext(LocalSyncContext)
)

export {
  DeviceRemoteType,
  LocalSyncContextProvider,
  useLocalSyncContext,
  useLocalBuildUrl,
  useMaybeLocalSyncContext,
}

export type {
  FileSyncStatus,
}
