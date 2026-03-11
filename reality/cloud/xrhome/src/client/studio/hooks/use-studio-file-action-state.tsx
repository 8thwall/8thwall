import React from 'react'
import {useLocation} from 'react-router-dom'

import {useStringUrlState} from '../../hooks/url-state'
import {extractRemainingUrlParams, parseStudioLocation} from '../studio-route'
import {usePersistentEditorSession} from '../../editor/hooks/use-persistent-editor-session'
import {useTabActions} from '../../editor/hooks/use-tab-actions'
import {useStudioUrlSync} from './use-studio-url-sync'
import {
  deriveEditorRouteParams,
  type EditorFileLocation,
} from '../../editor/editor-file-location'
import {
  BeforeFileSelectHandler, FileRenameHandler, useFileActionsState,
} from '../../editor/hooks/use-file-actions-state'
import {FileBrowser} from '../file-browser'
import {useAppPathsContext} from '../../common/app-container-context'

const useStudioFileActionState = (
  openFileFromLocalForage: boolean = true,
  onBeforeFileSelect?: BeforeFileSelectHandler,
  onFileRename?: FileRenameHandler
) => {
  const [fileUrlParam, setFileUrlParam] = useStringUrlState('file', undefined)
  const location = useLocation()
  const studioEditorParams = parseStudioLocation(location)
  const studioStateParams = extractRemainingUrlParams(location.search, ['file', 'module'])

  const {getStudioRoute} = useAppPathsContext()

  const getLocationFromFile = (file: EditorFileLocation) => getStudioRoute(
    deriveEditorRouteParams(file), studioStateParams
  )

  const editorSession = usePersistentEditorSession(
    '',
    getLocationFromFile,
    studioEditorParams,
    openFileFromLocalForage
  )

  const {currentFileLocation, isTabsFromLocalForageLoaded, tabState} = editorSession

  const {switchTab} = useTabActions(editorSession)

  useStudioUrlSync(
    currentFileLocation,
    isTabsFromLocalForageLoaded,
    switchTab,
    setFileUrlParam
  )

  React.useEffect(() => {
    if (tabState.tabs.length === 1 && !tabState.tabs[0].filePath) {
      setFileUrlParam(undefined)
    }
  }, [tabState.tabs])

  const checkProtectedFile = () => false

  const {
    actionsContext, fileActionModals, fileUploadState, uploadDropRef, handleFileUpload,
  } = useFileActionsState({
    editorSession,
    checkProtectedFile,
    onBeforeFileSelect,
    onFileRename,
  })

  const fileBrowser = (
    <FileBrowser
      uploadDropRef={uploadDropRef}
      handleFileUpload={handleFileUpload}
      fileUploadState={fileUploadState}
      activeFileLocation={currentFileLocation}
      isStudio
    />
  )

  return {
    actionsContext,
    tabState,
    editorSession,
    fileUrlParam,
    fileBrowser,
    fileActionModals,
  }
}

export {useStudioFileActionState}
