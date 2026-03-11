import React from 'react'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'
import {Icon} from './icon'

const useStyles = createThemedStyles(theme => ({
  textNotification: {
    margin: 0,
    padding: 0,
    lineHeight: '20px',
  },
  info: {
    color: theme.fgBlue,
  },
  warning: {
    color: theme.fgWarning,
  },
  success: {
    color: theme.fgSuccess,
  },
  danger: {
    color: theme.fgError,
  },
}))

interface ITextNotification {
  type: 'info' | 'warning' | 'success' | 'danger'
  children?: React.ReactNode
}

const TextNotification: React.FC<ITextNotification> = ({type, children}) => {
  const classes = useStyles()
  return (
    <p className={combine(classes.textNotification, classes[type])}>
      <Icon inline stroke={type} color={type} />
      {children}
    </p>
  )
}

export {
  TextNotification,
}

export type {
  ITextNotification,
}
