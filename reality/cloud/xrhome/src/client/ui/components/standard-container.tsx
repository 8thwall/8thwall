import React from 'react'

import {createThemedStyles} from '../theme'
import {combine} from '../../common/styles'

const useStyles = createThemedStyles(theme => ({
  standardField: {
    borderRadius: theme.sfcBorderRadius,
    border: `1px solid ${theme.sfcBorderDefault}`,
    background: theme.sfcBackgroundDefault,
    backdropFilter: theme.sfcBackdropFilter,
    overflow: 'hidden',
  },
  overflow: {
    overflow: 'visible',
  },
  small: {
    padding: '1.5rem',
  },
  medium: {
    padding: '2rem',
  },
  large: {
    padding: '2.5rem',
  },
}))

interface IStandardContainer {
  overflow?: boolean
  padding?: 'none' | 'small' | 'medium' | 'large'
  children?: React.ReactNode
}

const StandardContainer: React.FC<IStandardContainer> = (
  {overflow, padding = 'medium', children}
) => {
  const classes = useStyles()
  return (
    <div className={combine(classes.standardField, classes[padding], overflow && classes.overflow)}>
      {children}
    </div>
  )
}

export {
  StandardContainer,
}
