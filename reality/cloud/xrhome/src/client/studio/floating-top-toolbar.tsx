import React from 'react'
import {createUseStyles} from 'react-jss'

import {StudioSceneEditControlTray} from './studio-scene-edit-control-tray'
import {AiAssistant} from './ai-assistant'
import {MARGIN_SIZE} from './interface-constants'
import {isAgentBetaEnabled} from '../../shared/account-utils'
import useCurrentAccount from '../common/use-current-account'

const useStyles = createUseStyles({
  toolbarRow: {
    'display': 'flex',
    'flexDirection': 'row',
    'justifyContent': 'space-between',
    'padding': `${MARGIN_SIZE} 0`,
    'zIndex': '1',
    'gap': MARGIN_SIZE,
    '& > *': {
      pointerEvents: 'auto',
    },
  },
  toolbarRowLeft: {
    display: 'flex',
    flexDirection: 'row',
    justifyContent: 'flex-start',
    gap: MARGIN_SIZE,
    flexGrow: 1,
    minWidth: 0,
    alignItems: 'center',
  },
  toolbarRowRight: {
    display: 'flex',
    flexDirection: 'row',
    justifyContent: 'flex-end',
    gap: MARGIN_SIZE,
    flexGrow: 1,
    minWidth: 0,
    alignItems: 'center',
  },
  topCenterBar: {
    'padding': MARGIN_SIZE,
    'position': 'absolute',
    'top': 0,
    'left': '50%',
    'transform': 'translateX(-50%)',
    'display': 'flex',
    'flexDirection': 'row',
    'zIndex': 2000,
    'justifyContent': 'center',
    '& > *': {
      pointerEvents: 'auto',
    },
  },
})

interface IFloatingLeftTopBar {
  navigationMenu?: React.ReactNode
}

const FloatingLeftTopBar: React.FC<IFloatingLeftTopBar> = ({
  navigationMenu,
}) => {
  const classes = useStyles()
  const account = useCurrentAccount()

  return (
    <div className={classes.toolbarRow}>
      <div className={classes.toolbarRowLeft}>
        {navigationMenu}
        <StudioSceneEditControlTray />
        {(BuildIf.CLOUD_STUDIO_AI_20240413 || isAgentBetaEnabled(account)) &&
        Build8.PLATFORM_TARGET === 'desktop' &&
          <AiAssistant />
        }
      </div>
    </div>
  )
}
interface IFloatingCenterTopBar {
  playbackControls: React.ReactNode
}

const FloatingCenterTopBar: React.FC<IFloatingCenterTopBar> = ({
  playbackControls,
}) => {
  const classes = useStyles()

  return (
    <div className={classes.topCenterBar}>
      {playbackControls}
    </div>
  )
}

interface IFloatingRightTopBar {
  buildControlTray?: React.ReactNode
  codeEditorToggle?: React.ReactNode
}

const FloatingRightTopBar: React.FC<IFloatingRightTopBar> = ({
  buildControlTray, codeEditorToggle,
}) => {
  const classes = useStyles()

  return (
    <div className={classes.toolbarRow}>
      <div className={classes.toolbarRowRight}>
        {buildControlTray}
        {codeEditorToggle}
      </div>
    </div>
  )
}

export {
  FloatingLeftTopBar,
  FloatingRightTopBar,
  FloatingCenterTopBar,
}
