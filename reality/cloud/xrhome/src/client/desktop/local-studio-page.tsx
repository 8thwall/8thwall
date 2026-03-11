import React from 'react'
import {useParams} from 'react-router-dom'
import {useDispatch} from 'react-redux'
import {useQueryClient} from '@tanstack/react-query'

import {AssetLabStateContextProvider} from '../asset-lab/asset-lab-context'
import {PublishingStateContextProvider} from '../editor/publishing/publish-context'
import {SceneStateContext} from '../studio/scene-state-context'
import {StudioAgentStateContextProvider} from '../studio/studio-agent/studio-agent-context'
import {ErrorMessage as StudioErrorMessage} from '../studio/error-message'
import SceneFileEdit from '../studio/scene-file-edit'
import {getLocalStudioPath, LocalStudioPathParams} from './desktop-paths'
import {RepoIdProvider} from '../git/repo-id-context'
import {EnclosedAppProvider} from '../apps/enclosed-app-context'
import {SceneDebugContext} from '../studio/scene-debug-context'
import {StudioComponentsContextProvider} from '../studio/studio-components-context'
import {gitRepoAction} from '../git/direct-git-actions'
import {LocalSyncContextProvider, useLocalSyncContext} from '../studio/local-sync-context'
import {FileActionsContext} from '../editor/files/file-actions-context'
import {useStudioFileActionState} from '../studio/hooks/use-studio-file-action-state'
import {AppPathsContext} from '../common/app-container-context'
import {useGitRepo, useScopedGit} from '../git/hooks/use-current-git'
import {UiThemeProvider} from '../ui/theme'
import {FloatingNavigation} from '../studio/floating-navigation'
import {BuildControlTray} from '../studio/build-control-tray'
import {AppPreviewWindowContextProvider} from '../common/app-preview-window-context'
import {basename} from '../editor/editor-common'
import type {IApp} from '../common/types/models'
import {useSceneContext} from '../studio/scene-context'
import {PanelSelection, useStudioStateContext} from '../studio/studio-state-context'
import {isAssetPath} from '../common/editor-files'
import {
  EditorFileLocation, extractFilePath, editorFileLocationEqual,
} from '../editor/editor-file-location'
import type {FileRenameHandler} from '../editor/hooks/use-file-actions-state'
import {replaceAssetInScene} from '../studio/replace-asset'
import useActions from '../common/use-actions'
import coreGitActions from '../git/core-git-actions'
import {CurrentSceneDiffModal} from '../studio/current-scene-diff-modal'
import {LogContainerSplit} from '../apps/log-container-split'
import {DebugSessionsMenu} from '../studio/debug-sessions-menu'
import {notifyProjectAccess} from '../studio/local-sync-api'
import {useAbandonableEffect} from '../hooks/abandonable-effect'
import {useTheme} from '../user/use-theme'
import {DismissibleModalContextProvider} from '../editor/dismissible-modal-context'
import {Loader} from '../ui/components/loader'

const LocalStudioPageInner: React.FC<{appKey: string}> = () => {
  const simulatorId = 'local'

  const repo = useGitRepo()
  const {listFiles} = useActions(coreGitActions)
  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()

  const handleBeforeFileSelect = (
    location: EditorFileLocation
  ) => {
    if (!editorFileLocationEqual(location, stateCtx.state.selectedAsset)) {
      stateCtx.update(p => ({
        ...p,
        selectedIds: [],
        selectedAsset: location,
        selectedImageTarget: undefined,
        currentPanelSection: PanelSelection.INSPECTOR,
      }))
    }
    return true
  }

  const handleFileRename: FileRenameHandler = async (oldLocation, newLocation) => {
    const oldPath = extractFilePath(oldLocation)
    const newPath = extractFilePath(newLocation)
    if (!isAssetPath(oldPath) || !isAssetPath(newPath)) {
      return
    }

    const renames: Map<string, string> = new Map()
    renames.set(oldPath, newPath)

    const newFilePaths = await listFiles(repo)

    const oldPrefix = `${oldPath}/`
    const newPrefix = `${newPath}/`
    newFilePaths
      .filter(f => f.startsWith(newPrefix))
      .forEach((newFilePath) => {
        const oldFilePath = newFilePath.replace(newPrefix, oldPrefix)
        renames.set(oldFilePath, newFilePath)
      })

    ctx.updateScene((oldScene) => {
      let newScene = oldScene

      Array.from(renames.entries())
        .forEach(([oldFilePath, newFilePath]) => {
          newScene = replaceAssetInScene(newScene, oldFilePath, newFilePath)
        })

      return newScene
    })

    if (!editorFileLocationEqual(oldLocation, stateCtx.state.selectedAsset)) {
      handleBeforeFileSelect(newLocation)
    }
  }

  const {
    actionsContext,
    fileBrowser,
    fileActionModals,
  } = useStudioFileActionState(false, handleBeforeFileSelect, handleFileRename)

  let res = (
    <SceneFileEdit
      simulatorId={simulatorId}
      errorMessage={<StudioErrorMessage />}
      navigationMenu={<FloatingNavigation />}
      buildControlTray={<BuildControlTray />}
      fileBrowser={fileBrowser}
    />
  )

  if (BuildIf.STUDIO_OFFLINE_LOG_CONTAINER_20260205) {
    res = <LogContainerSplit extraTabContent={<DebugSessionsMenu />}>{res}</LogContainerSplit>
  }

  res = (
    <>
      {stateCtx.state.sceneDiffOpen && <CurrentSceneDiffModal />}
      {res}
      {fileActionModals}
    </>
  )

  res = <FileActionsContext.Provider value={actionsContext}>{res}</FileActionsContext.Provider>
  res = <StudioComponentsContextProvider>{res}</StudioComponentsContextProvider>
  res = <PublishingStateContextProvider>{res}</PublishingStateContextProvider>
  res = <StudioAgentStateContextProvider>{res}</StudioAgentStateContextProvider>
  res = <AssetLabStateContextProvider>{res}</AssetLabStateContextProvider>
  res = <DismissibleModalContextProvider>{res}</DismissibleModalContextProvider>
  return res
}

const FileSyncSuspense: React.FC<{children: React.ReactNode}> = ({children}) => {
  const localSync = useLocalSyncContext()
  if (localSync.fileSyncStatus !== 'active') {
    return <Loader />
  }
  // eslint-disable-next-line react/jsx-no-useless-fragment
  return <>{children}</>
}

const LocalStudioPage: React.FC = () => {
  const appKey = decodeURIComponent(useParams<LocalStudioPathParams>().appKey)
  const theme = useTheme()

  const app = React.useMemo((): IApp => ({
    appKey,
    uuid: appKey,
    appName: basename(appKey),
    repoId: appKey,
  }), [appKey])

  const queryClient = useQueryClient()

  useAbandonableEffect(async () => {
    await notifyProjectAccess(appKey)
    queryClient.invalidateQueries({queryKey: ['listProjects']})
  }, [appKey])

  const appPathsContext = React.useMemo(() => ({
    getPathForApp: () => getLocalStudioPath(appKey),
    getFileRoute: () => getLocalStudioPath(appKey),
    getStudioRoute: () => getLocalStudioPath(appKey),
  }), [appKey])

  // NOTE(dat): Context changes make to this component should ALSO be made to scene-local-edit.tsx
  // which powers our playground view.

  const dispatch = useDispatch()
  React.useEffect(() => {
    dispatch(gitRepoAction(appKey, {
      repositoryName: appKey,
      repoId: appKey,
    }))
  }, [appKey])

  const repoReady = useScopedGit(appKey, g => !!g.repo?.repoId)

  if (!repoReady) {
    return null
  }

  let res = (
    <div className={`studio-editor ${theme}`}>
      <LocalStudioPageInner appKey={appKey} />
    </div>
  )

  res = <SceneDebugContext>{res}</SceneDebugContext>
  res = <FileSyncSuspense>{res}</FileSyncSuspense>
  res = <LocalSyncContextProvider>{res}</LocalSyncContextProvider>
  res = <SceneStateContext>{res}</SceneStateContext>
  res = <RepoIdProvider value={appKey}>{res}</RepoIdProvider>

  return (
    <EnclosedAppProvider value={app}>
      <AppPreviewWindowContextProvider>
        <UiThemeProvider mode={theme}>
          <AppPathsContext.Provider value={appPathsContext}>
            {res}
          </AppPathsContext.Provider>
        </UiThemeProvider>
      </AppPreviewWindowContextProvider>
    </EnclosedAppProvider>
  )
}

export default LocalStudioPage
