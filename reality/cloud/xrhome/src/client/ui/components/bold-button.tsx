import React from 'react'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'

type BoldButtonColor = 'blue' | 'primary'

const useStyles = createThemedStyles(theme => ({
  boldButton: {
    'fontFamily': 'inherit',
    'fontWeight': 700,
    'background': 'transparent',
    'border': 'none',
    'padding': 0,
    'margin': 0,
    'cursor': 'pointer',
    'color': 'inherit',
    '&:disabled': {
      'cursor': 'default',
      'opacity': '0.5',
    },
    '&:focus-visible': {
      'outline': `1px solid ${theme.sfcBorderFocus}`,
    },
  },
  blue: {
    color: theme.fgBlue,
  },
  primary: {
    color: theme.fgPrimary,
  },
}))

interface IBoldButton extends React.ButtonHTMLAttributes<HTMLButtonElement> {
  color?: BoldButtonColor
}

const BoldButton: React.FC<IBoldButton> = ({className, type, color, children, ...rest}) => {
  const classes = useStyles()

  return (
    <button
      {...rest}
      // eslint-disable-next-line react/button-has-type
      type={type || 'button'}
      className={combine(classes.boldButton, classes[color], className)}
    >{children}
    </button>
  )
}

export {
  BoldButton,
}

export type {
  IBoldButton,
}
