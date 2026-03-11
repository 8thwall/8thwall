import React from 'react'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'

const useStyles = createThemedStyles(theme => ({
  standardField: {
    'position': 'relative',
    'color': theme.fgMain,
    'boxSizing': 'border-box',
    'boxShadow': `0 0 0 1px ${theme.sfcBorderDefault} inset`,
    'background': theme.sfcBackgroundDefault,
    'borderRadius': theme.sfcBorderRadius,
    '&:hover': {
      background: theme.sfcBackgroundFocus,
    },
    '&:focus-within': {
      'boxShadow': `0 0 0 1px ${theme.sfcBorderFocus} inset`,
    },
    '&::before': {
      position: 'absolute',
      backdropFilter: theme.sfcBackdropFilter,
      content: '""',
      height: '100%',
      width: '100%',
      borderRadius: theme.sfcBorderRadius,
      zIndex: '-1',
    },
  },
  invalid: {
    background: theme.sfcBackgroundError,
    boxShadow: `0 0 0 1px ${theme.sfcBorderError} inset`,
  },
  grow: {
    display: 'flex',
    height: '100%',
    flex: '1 0 0',
  },
  disabled: {
    'background': theme.sfcDisabledBackground,
    '& > :disabled': {
      'opacity': 0.5,
    },
  },
}))

interface IStandardFieldContainer {
  invalid?: boolean
  grow?: boolean
  disabled?: boolean
  children?: React.ReactNode
}

const StandardFieldContainer: React.FC<IStandardFieldContainer> = (
  {invalid, grow, disabled, children}
) => {
  const classes = useStyles()

  return (
    <div
      className={combine(
        classes.standardField,
        invalid && classes.invalid,
        grow && classes.grow,
        disabled && classes.disabled
      )}
    >
      {children}
    </div>
  )
}

export {
  StandardFieldContainer,
}

export type {
  IStandardFieldContainer,
}
