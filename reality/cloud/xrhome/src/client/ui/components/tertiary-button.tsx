import React from 'react'

import {combine} from '../../common/styles'
import {
  ButtonHeight, ButtonSpacing, useRoundedButtonStyling,
} from '../hooks/use-rounded-button-styling'
import {createThemedStyles} from '../theme'
import {LoadableButtonChildren} from './loadable-button-children'

const useStyles = createThemedStyles(theme => ({
  tertiaryButton: {
    'background': theme.tertiaryBtnBg,
    'color': theme.tertiaryBtnFg,
    '&:hover': {
      background: theme.tertiaryBtnHoverBg,
    },
    '&:disabled': {
      cursor: 'default',
      color: theme.tertiaryBtnDisabledFg,
      background: theme.tertiaryBtnDisabledBg,
    },
  },
}))

interface ITertiaryButton extends React.ButtonHTMLAttributes<HTMLButtonElement> {
  type?: 'button' | 'submit' | 'reset'
  height?: ButtonHeight
  spacing?: ButtonSpacing
  a8?: string
  loading?: boolean
}

const TertiaryButton: React.FC<ITertiaryButton> = ({
  className, type, height = 'medium', spacing = 'normal', children, loading = false,
  disabled = false, ...rest
}) => {
  const classes = useStyles()
  const roundedStylingClass = useRoundedButtonStyling(height, spacing)

  return (
    <button
      {...rest}
      // eslint-disable-next-line react/button-has-type
      type={type || 'button'}
      disabled={disabled || loading}
      className={combine(classes.tertiaryButton, roundedStylingClass, className)}
    >
      <LoadableButtonChildren loading={loading}>
        {children}
      </LoadableButtonChildren>
    </button>
  )
}

export {
  TertiaryButton,
}

export type {
  ITertiaryButton,
}
