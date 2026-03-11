import React from 'react'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'
import {
  ButtonHeight, ButtonSpacing, useRoundedButtonStyling,
} from '../hooks/use-rounded-button-styling'
import {LoadableButtonChildren} from './loadable-button-children'

const useStyles = createThemedStyles(theme => ({
  primaryButton: {
    'background': theme.primaryBtnBg,
    '&:not(:disabled)': {
      'boxShadow': theme.primaryBtnBoxShadow,
      '& > $primaryButtonContent': {
        'filter': theme.primaryBtnContentFilter,
      },
    },
    'color': theme.primaryBtnFg,
    '&:hover': {
      background: theme.primaryBtnHoverBg,
    },
    '&:disabled': {
      'cursor': 'default',
      'color': theme.primaryBtnDisabledFg,
      'background': theme.primaryBtnDisabledBg,
    },
  },
  primaryButtonContent: {},
}))

interface IPrimaryButton extends React.ButtonHTMLAttributes<HTMLButtonElement> {
  type?: 'button' | 'submit' | 'reset'
  height?: ButtonHeight
  spacing?: ButtonSpacing
  loading?: boolean
  a8?: string
}

const PrimaryButton = React.forwardRef<HTMLButtonElement, IPrimaryButton>(({
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
      className={combine(classes.primaryButton, roundedStylingClass, className)}
      ref={ref}
    >
      <LoadableButtonChildren loading={loading}>
        <span className={classes.primaryButtonContent}>{children}</span>
      </LoadableButtonChildren>
    </button>
  )
})

export {
  PrimaryButton,
}

export type {
  IPrimaryButton,
}
