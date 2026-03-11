import React from 'react'

import {combine} from '../../common/styles'
import {
  ButtonHeight, ButtonSpacing, useRoundedButtonStyling,
} from '../hooks/use-rounded-button-styling'
import {createThemedStyles} from '../theme'
import {LoadableButtonChildren} from './loadable-button-children'

const useStyles = createThemedStyles(theme => ({
  secondaryButton: {
    'background': theme.secondaryBtnBg,
    'color': theme.secondaryBtnColor,
    'border': theme.secondaryBtnBorder,
    '&:not(:disabled)': {
      'boxShadow': theme.secondaryBtnBoxShadow,
      '& > $secondaryButtonContent': {
        filter: theme.secondaryBtnContentFilter,
      },
    },
    '&:hover:not(:disabled)': {
      background: theme.secondaryBtnHoverBg,
    },
    '&:disabled': {
      cursor: 'default',
      color: theme.secondaryBtnDisabledFg,
      border: theme.secondaryBtnDisabledBorder,
    },
  },
  secondaryButtonContent: {},
}))

interface ISecondaryButton extends React.ButtonHTMLAttributes<HTMLButtonElement> {
  type?: 'button' | 'submit' | 'reset'
  height?: ButtonHeight
  spacing?: ButtonSpacing
  a8?: string
  loading?: boolean
}

const SecondaryButton: React.FC<ISecondaryButton> = ({
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
      className={combine(classes.secondaryButton, roundedStylingClass, className)}
    >
      <LoadableButtonChildren loading={loading}>
        <span className={classes.secondaryButtonContent}>{children}</span>
      </LoadableButtonChildren>
    </button>
  )
}

export {
  SecondaryButton,
}

export type {
  ISecondaryButton,
}
