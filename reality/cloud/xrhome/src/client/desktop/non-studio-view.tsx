import React from 'react'
import {useTranslation} from 'react-i18next'
import {useHistory} from 'react-router-dom'

import {useEnclosedApp} from '../apps/enclosed-app-context'
import {IMAGE_TARGET_SIMULATOR_PANEL_GALLERY_ID} from '../apps/image-targets/image-target-constants'

import {LogContainerSplit} from '../apps/log-container-split'
import {useAppPathsContext} from '../common/app-container-context'
import {INLINE_SIMULATOR_SESSION_ID} from '../editor/app-preview/app-preview-constants'
import {InlineAppPreviewPane} from '../editor/app-preview/inline-app-preview-pane'
import {
  deriveEditorRouteParams, EditorFileLocation, editorFileLocationEqual,
} from '../editor/editor-file-location'
import {FileActionsContext} from '../editor/files/file-actions-context'
import {useConsoleActivity} from '../editor/hooks/use-console-activity'
import {useFileActionsState} from '../editor/hooks/use-file-actions-state'
import {usePersistentEditorSession} from '../editor/hooks/use-persistent-editor-session'
import {useSystemLog} from '../editor/hooks/use-system-log'
import {BuildControlTray} from '../studio/build-control-tray'
import {DebugSessionsMenu} from '../studio/debug-sessions-menu'
import {FileBrowser} from '../studio/file-browser'
import {RIGHT_PANEL_WIDTH} from '../studio/floating-right-panel'
import {useLocalSyncContext} from '../studio/local-sync-context'
import {FloatingRightPanel} from './non-studio-right-panel'
import {PanelSelection, useStudioStateContext} from '../studio/studio-state-context'
import {FloatingIconButton} from '../ui/components/floating-icon-button'
import {FloatingTray} from '../ui/components/floating-tray'
import {SpaceBetween} from '../ui/layout/space-between'
import {createThemedStyles} from '../ui/theme'
import {HOME_PATH} from './desktop-paths'
import ErrorMessage from '../home/error-message'
import {StaticBanner} from '../ui/components/banner'

const useStyles = createThemedStyles(theme => ({
  nonStudioView: {
    position: 'absolute',
    top: 0,
    right: 0,
    left: 0,
    bottom: 0,
    background: theme.nonStudioViewBg,
    padding: '4px',
    display: 'grid',
    gridTemplateColumns: 'auto 1fr auto',
    justifyContent: 'stretch',
    alignContent: 'stretch',
    gap: '4px',
  },
  leftColumn: {
    justifySelf: 'stretch',
    flexGrow: 1,
    display: 'flex',
    gap: '4px',
    flexDirection: 'column',
    width: `${RIGHT_PANEL_WIDTH}px`,
    overflow: 'hidden',
    flexBasis: 0,
  },
}))

const NonStudioView: React.FC = () => {
  const classes = useStyles()
  useSystemLog()
  useConsoleActivity()
  const stateCtx = useStudioStateContext()
  const {t} = useTranslation('common')
  const history = useHistory()
  const app = useEnclosedApp()
  const debugReady = !!useLocalSyncContext().localBuildUrl

  const {getStudioRoute} = useAppPathsContext()

  const getLocationFromFile = (file: EditorFileLocation) => getStudioRoute(
    deriveEditorRouteParams(file), {}
  )

  const editorSession = usePersistentEditorSession(
    '',
    getLocationFromFile,
    ''
  )

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

  const {
    actionsContext, fileActionModals, fileUploadState, uploadDropRef, handleFileUpload,
  } = useFileActionsState({
    editorSession,
    checkProtectedFile: () => false,
    onBeforeFileSelect: handleBeforeFileSelect,
  })

  let res = (
    <>
      <div className={classes.nonStudioView}>
        <div className={classes.leftColumn}>
          <SpaceBetween narrow>
            <FloatingTray overflowHidden>
              <FloatingIconButton
                a8='click;studio;navigation-menu-button'
                text={t('button.home', {ns: 'common'})}
                stroke='home'
                onClick={() => history.push(HOME_PATH)}
              />
            </FloatingTray>
            <BuildControlTray />
          </SpaceBetween>
          <FloatingTray fillContainer orientation='vertical' overflowHidden>
            {stateCtx.state.errorMsg &&
              <StaticBanner
                type='danger'
                onClose={() => stateCtx.update(p => ({...p, errorMsg: ''}))}
              >
                {stateCtx.state.errorMsg}
              </StaticBanner>
            }
            <ErrorMessage />
            <FileBrowser
              uploadDropRef={uploadDropRef}
              handleFileUpload={handleFileUpload}
              fileUploadState={fileUploadState}
              activeFileLocation={null}
              isStudio={false}
            />
          </FloatingTray>
        </div>
        <FloatingTray fillContainer>
          <InlineAppPreviewPane
            app={app}
            simulatorId={INLINE_SIMULATOR_SESSION_ID}
            sessionId={INLINE_SIMULATOR_SESSION_ID}
            isDragging={false}
            hidePreviewBottom={false}
            targetsGalleryUuid={IMAGE_TARGET_SIMULATOR_PANEL_GALLERY_ID}
            showLoadingOverlay={!debugReady}
            hideCloseButton
          />
        </FloatingTray>
        <FloatingRightPanel topBar={null} />
      </div>
      {fileActionModals}
    </>
  )

  res = <FileActionsContext.Provider value={actionsContext}>{res}</FileActionsContext.Provider>
  res = <LogContainerSplit extraTabContent={<DebugSessionsMenu />}>{res}</LogContainerSplit>
  return res
}

export {
  NonStudioView,
}
