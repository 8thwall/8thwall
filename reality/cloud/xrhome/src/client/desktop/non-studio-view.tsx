import React from 'react'
import {useTranslation} from 'react-i18next'
import {createUseStyles} from 'react-jss'
import {useHistory} from 'react-router-dom'

import {useEnclosedApp} from '../apps/enclosed-app-context'
import {IMAGE_TARGET_SIMULATOR_PANEL_GALLERY_ID} from '../apps/image-targets/image-target-constants'

import {LogContainerSplit} from '../apps/log-container-split'
import {useAppPathsContext} from '../common/app-container-context'
import {INLINE_SIMULATOR_SESSION_ID} from '../editor/app-preview/app-preview-constants'
import {InlineAppPreviewPane} from '../editor/app-preview/inline-app-preview-pane'
import {deriveEditorRouteParams, EditorFileLocation} from '../editor/editor-file-location'
import {FileActionsContext} from '../editor/files/file-actions-context'
import {useConsoleActivity} from '../editor/hooks/use-console-activity'
import {useFileActionsState} from '../editor/hooks/use-file-actions-state'
import {usePersistentEditorSession} from '../editor/hooks/use-persistent-editor-session'
import {useSystemLog} from '../editor/hooks/use-system-log'
import {BuildControlTray} from '../studio/build-control-tray'
import {DebugSessionsMenu} from '../studio/debug-sessions-menu'
import {FileBrowser} from '../studio/file-browser'
import {useLocalSyncContext} from '../studio/local-sync-context'
import {FloatingIconButton} from '../ui/components/floating-icon-button'
import {FloatingTray} from '../ui/components/floating-tray'
import {SpaceBetween} from '../ui/layout/space-between'
import {HOME_PATH} from './desktop-paths'

const useStyles = createUseStyles({
  nonStudioView: {
    background: 'black',
    padding: '4px',
    display: 'grid',
    gridTemplateColumns: 'auto 1fr',
    justifyContent: 'stretch',
    alignContent: 'stretch',
    gap: '4px',
    flexGrow: 1,
    height: '100%',
  },
  leftColumn: {
    justifySelf: 'stretch',
    flexGrow: 1,
    // background: '#f003',
    display: 'flex',
    gap: '4px',
    flexDirection: 'column',
    width: '250px',
    overflow: 'hidden',
  },

})

const NonStudioPage: React.FC = () => {
  // Device Logs
  // Simulator
  // Publish flow
  // File browser
  // Image Targets
  // Asset inspector

  const classes = useStyles()
  useSystemLog()
  useConsoleActivity()

  const {t} = useTranslation('common')

  const {getStudioRoute} = useAppPathsContext()

  const getLocationFromFile = (file: EditorFileLocation) => getStudioRoute(
    deriveEditorRouteParams(file), {}
  )

  const editorSession = usePersistentEditorSession(
    '',
    getLocationFromFile,
    ''
  )

  const {
    actionsContext, fileActionModals, fileUploadState, uploadDropRef, handleFileUpload,
  } = useFileActionsState({
    editorSession,
    checkProtectedFile: () => false,
  })

  const fileBrowser = (
    <FileBrowser
      uploadDropRef={uploadDropRef}
      handleFileUpload={handleFileUpload}
      fileUploadState={fileUploadState}
      activeFileLocation={null}
      isStudio
    />
  )
  const history = useHistory()

  const app = useEnclosedApp()

  const debugReady = !!useLocalSyncContext().localBuildUrl

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
          <FloatingTray fillContainer>
            {fileBrowser}
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
            // renderActions={() => <SimulatorViewActions simulatorId={INLINE_SIMULATOR_SESSION_ID} />}
            showLoadingOverlay={!debugReady}
            isStandalone
          />
        </FloatingTray>
      </div>
      {fileActionModals}
    </>
  )

  res = <FileActionsContext.Provider value={actionsContext}>{res}</FileActionsContext.Provider>

  const extraTabContent = BuildIf.REMOTE_DEVICE_CONNECT_20260512
    ? <DebugSessionsMenu />
    : null

  res = <LogContainerSplit extraTabContent={extraTabContent}>{res}</LogContainerSplit>
  return res
}

export {
  NonStudioPage,
}
