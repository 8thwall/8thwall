import React from 'react'

import {createThemedStyles} from '../theme'
import {combine} from '../../common/styles'
import {
  type ButtonHeight, type ButtonSpacing, useRoundedButtonStyling,
} from '../hooks/use-rounded-button-styling'
import {LoadableButtonChildren} from './loadable-button-children'

const useStyles = createThemedStyles(theme => ({
  floatingTrayButton: {
    'fontFamily': 'inherit',
    'background': theme.studioPanelBtnBg,
    'color': theme.fgMain,
    'cursor': 'pointer',
    'user-select': 'none',
    'borderRadius': '0.4em',
    '&:hover:not(:disabled)': {
      background: theme.studioPanelBtnHoverBg,
    },
    '&:disabled': {
      cursor: 'default',
      color: theme.sfcDisabledColor,
    },
    '&:active:not(:disabled)': {
      background: theme.studioBtnActiveBg,
    },
  },
  dark: {
    background: theme.listBoxBg,
  },
  transparentOnDisable: {
    '&:disabled': {
      'background': 'transparent',
      'border': `1px solid ${theme.studioPanelBtnBg}`,
    },
  },
  grow: {
    flexGrow: 1,
  },
}))

interface IFloatingPanelButton extends Pick<React.ButtonHTMLAttributes<HTMLButtonElement>,
  'type' | 'children' | 'onClick' | 'aria-label'> {
  height?: ButtonHeight
  spacing?: ButtonSpacing
  loading?: boolean
  disabled?: boolean
  dark?: boolean
  transparentOnDisable?: boolean
  a8?: string
  grow?: boolean
}

const FloatingPanelButton: React.FC<IFloatingPanelButton> = ({
  type,
  children,
  height = 'tiny',
  spacing = 'normal',
  loading,
  dark,
  transparentOnDisable,
  disabled,
  grow,
  ...rest
}) => {
  const classes = useStyles()
  const roundedStylingClass = useRoundedButtonStyling(height, spacing)

  return (
    <button
      {...rest}
      // eslint-disable-next-line react/button-has-type
      type={type || 'button'}
      className={combine(
        roundedStylingClass,
        classes.floatingTrayButton,
        dark && classes.dark,
        transparentOnDisable && classes.transparentOnDisable,
        grow && classes.grow
      )}
      disabled={disabled || loading}
    >
      <LoadableButtonChildren loading={loading}>
        {children}
      </LoadableButtonChildren>
    </button>
  )
}

export {
  useStyles as useFloatingPanelButtonStyles,
  FloatingPanelButton,
}

export type {
  IFloatingPanelButton,
}
