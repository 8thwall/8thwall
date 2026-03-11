import React from 'react'

import {combine} from '../../common/styles'
import {
  ButtonHeight, ButtonSpacing, useRoundedButtonStyling,
} from '../hooks/use-rounded-button-styling'
import {createThemedStyles} from '../theme'
import {LoadableButtonChildren} from './loadable-button-children'

const useStyles = createThemedStyles(theme => ({
  textButton: {
    'color': theme.linkBtnFg,
    'backgroundColor': 'transparent',
    '&:hover:not(:disabled)': {
      backgroundColor: theme.textBtnHoverBg,
    },
    '&:disabled': {
      cursor: 'default',
      color: theme.linkBtnDisableFg,
    },
  },
}))

interface ITextButton extends React.ButtonHTMLAttributes<HTMLButtonElement> {
  type?: 'button' | 'submit' | 'reset'
  height?: ButtonHeight
  spacing?: ButtonSpacing
  loading?: boolean
  a8?: string
}

const TextButton = React.forwardRef<HTMLButtonElement, ITextButton>(({
  className, type, height = 'medium', spacing = 'normal', children, loading = false,
  disabled = false, ...rest
}, ref) => {
  const classes = useStyles()
  const roundedStylingClass = useRoundedButtonStyling(height, spacing)
  return (
    <button
      {...rest}
          // eslint-disable-next-line react/button-has-type
      type={type || 'button'}
      disabled={disabled || loading}
      className={combine(classes.textButton, roundedStylingClass, className)}
      ref={ref}
    >
      <LoadableButtonChildren loading={loading}>
        {children}
      </LoadableButtonChildren>
    </button>
  )
})

export {
  TextButton,
}

export type {
  ITextButton,
}
