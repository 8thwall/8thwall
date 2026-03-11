import React from 'react'
import {Helmet} from 'react-helmet-async'

import {SpaceBetween} from '../ui/layout/space-between'
import {createThemedStyles} from '../ui/theme'

const useStyles = createThemedStyles(theme => ({
  'draggableTitleBar': {
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    cursor: 'grab',
    WebkitAppRegion: 'drag',
    flex: '0 0 auto',
    height: '2.5em',
    zIndex: 10000,
  },
  'clickableButton': {
    WebkitAppRegion: 'no-drag',
  },
  'desktopFullHeight': {
    height: '100%',
    display: 'flex',
    flexDirection: 'column',
  },
  '@global': {
    body: {
      background: theme.bgMain,
      color: theme.fgMain,
    },
  },
}))

interface DesktopTopBarProps {
  windowTitle?: string
}

const DesktopTopBar: React.FC<DesktopTopBarProps> = ({windowTitle}) => {
  const classes = useStyles()
  return (
    <div className={classes.draggableTitleBar}>
      <SpaceBetween direction='horizontal' between>
        <Helmet>
          <title>8th Wall</title>
        </Helmet>
        <span>{windowTitle ?? '8th Wall'}</span>
      </SpaceBetween>
    </div>
  )
}

const DesktopWithTopBar: React.FC<{children: React.ReactNode} & DesktopTopBarProps> = ({
  windowTitle, children,
}) => {
  const classes = useStyles()
  return (
    <div className={classes.desktopFullHeight}>
      <DesktopTopBar windowTitle={windowTitle} />
      {children}
    </div>
  )
}

export {DesktopTopBar, DesktopWithTopBar}
