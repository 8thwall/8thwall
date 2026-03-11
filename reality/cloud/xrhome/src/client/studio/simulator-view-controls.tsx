import React from 'react'

import {useTranslation} from 'react-i18next'

import {MARGIN_SIZE} from './interface-constants'
import {useStudioStateContext} from './studio-state-context'
import {IconButton} from '../ui/components/icon-button'
import {useAppPreviewStyles} from '../editor/app-preview/app-preview-utils'
import {createThemedStyles} from '../ui/theme'
import {useProjectPreviewUrl} from '../editor/app-preview/use-project-preview-url'
import useCurrentApp from '../common/use-current-app'
import {useAppPreviewWindow} from '../common/app-preview-window-context'
import {useStudioDebug} from '../editor/app-preview/use-studio-debug-state'
import {useSimulator} from '../editor/app-preview/use-simulator-state'

const useStyles = createThemedStyles(theme => ({
  // NOTE(christoph): This is not a FloatingTray, it is attempting to match the
  // simulator top bar which is slightly different.
  simulatorViewControls: {
    position: 'absolute',
    right: 0,
    top: MARGIN_SIZE,
    zIndex: 1750,
    pointerEvents: 'auto',
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'center',
    padding: '0.25em 0.5em',
    height: '32px',
    background: theme.tabBarBg,
    borderRadius: '0.5em',
    overflow: 'hidden',
    color: theme.studioBtnHoverFg,
  },
}))

interface ISimulatorView {
  simulatorId: string
}

const SimulatorViewActions: React.FC<ISimulatorView> = ({simulatorId}) => {
  const app = useCurrentApp()
  const stateCtx = useStudioStateContext()
  const {t} = useTranslation('cloud-studio-pages')
  const appPreviewStyles = useAppPreviewStyles()
  const sameDeviceUrl = useProjectPreviewUrl(app, 'same-device')
  const {setPreviewWindow} = useAppPreviewWindow()
  const {closeDebugSimulatorSession} = useStudioDebug()
  const {updateSimulatorState} = useSimulator()

  const handleOpenInBrowser = () => {
    const newTab = window.open(sameDeviceUrl)
    setPreviewWindow(newTab)
    updateSimulatorState({inlinePreviewVisible: false})
    closeDebugSimulatorSession(simulatorId)
  }

  const handleToggleCollapsed = () => {
    stateCtx.update(p => ({
      ...p,
      simulatorCollapsed: !p.simulatorCollapsed,
    }))
  }

  const handleToggleMode = () => {
    stateCtx.update(p => ({
      ...p,
      simulatorMode: p.simulatorMode === 'full' ? 'inline' : 'full',
      simulatorCollapsed: false,
    }))
  }

  return (
    <>
      {!BuildIf.STUDIO_OFFLINE_LOG_CONTAINER_20260205 && (
        <div className={appPreviewStyles.actionButton}>
          <IconButton
            onClick={handleOpenInBrowser}
            stroke='popOut'
            text={t('simulator_view_controls.button.open_in_browser')}
            disabled={!sameDeviceUrl}
          />
        </div>
      )}
      <div className={appPreviewStyles.actionButton}>
        <IconButton
          onClick={handleToggleCollapsed}
          stroke={stateCtx.state.simulatorCollapsed ? 'unfurl' : 'furl'}
          text={t('simulator_view_controls.button.toggle_collapsed')}
        />
      </div>
      <div className={appPreviewStyles.actionButton}>
        <IconButton
          onClick={handleToggleMode}
          stroke='pictureInPicture'
          text={t('simulator_view_controls.button.toggle_picture_in_picture')}
        />
      </div>
    </>
  )
}

const SimulatorViewControls: React.FC<ISimulatorView> = ({simulatorId}) => {
  const classes = useStyles()

  return (
    <div className={classes.simulatorViewControls}>
      <SimulatorViewActions simulatorId={simulatorId} />
    </div>
  )
}

export {
  SimulatorViewControls,
  SimulatorViewActions,
}
