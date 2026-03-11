import React from 'react'

import {combine} from '../../common/styles'
import {gray4} from '../../static/styles/settings'
import {createThemedStyles} from '../../ui/theme'

const MODE_BUTTON_HEIGHT_REM = '3rem'

const useStyles = createThemedStyles(theme => ({
  modeButton: {
    background: 'transparent',
    border: 'none',
    margin: 0,
    padding: '1rem',
    cursor: 'pointer',
    color: gray4,
    flex: '0 0 auto',
    height: MODE_BUTTON_HEIGHT_REM,
  },
  modeActive: {
    color: theme.fgMain,
    fontWeight: 'bold',
    cursor: 'default !important',
  },
}))

interface IModeButton {
  active?: boolean
  className?: string
  onClick?: () => void
  children?: React.ReactNode
}

const ModeButton: React.FC<IModeButton> = ({active, className, onClick, children}) => {
  const classes = useStyles()
  return (
    <button
      type='button'
      className={combine(className, classes.modeButton, active ? classes.modeActive : '')}
      onClick={onClick}
    >
      {children}
    </button>
  )
}

export {
  ModeButton,
  MODE_BUTTON_HEIGHT_REM,
}
