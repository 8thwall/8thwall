import React from 'react'

import {createThemedStyles} from '../theme'
import {combine} from '../../common/styles'

const useStyles = createThemedStyles(theme => ({
  floatingMenuButton: {
    'width': '100%',
    'fontFamily': 'inherit',
    'lineHeight': '16px',
    'display': 'flex',
    'gap': '0.25em',
    'alignItems': 'center',
    'justifyContent': 'flex-start',
    'padding': '0.25rem 0.5rem',
    'color': theme.fgMain,
    'cursor': 'pointer',
    'borderRadius': '0.25em',
    'userSelect': 'none',
    ':is(&:hover, &:focus-visible):not(&:disabled)': {
      color: theme.studioBtnHoverFg,
      background: theme.studioBtnHoverBg,
    },
    '& svg': {
      color: theme.fgMuted,
    },
    '&:hover svg': {
      color: theme.fgMain,
    },
  },
  active: {
    color: theme.studioBtnHoverFg,
    background: theme.studioBtnHoverBg,
  },
  disabled: {
    'cursor': 'default',
    'color': `${theme.fgMuted}80`,
  },
  iconPadding: {
    padding: '0.425rem 0.5rem',
  },
  isDanger: {
    backgroundColor: theme.fgError,
  },
}))

interface IFloatingMenuButton extends Pick<React.ButtonHTMLAttributes<HTMLButtonElement>,
  'type' | 'children' | 'onClick' | 'aria-label' | 'id'> {
  isActive?: boolean
  isDisabled?: boolean
  isDanger?: boolean
  hasIcon?: boolean
  a8?: string
}

const FloatingMenuButton: React.FC<IFloatingMenuButton> = ({
  isActive = false, isDisabled = false, children, type, onClick,
  hasIcon = false, isDanger = false, ...rest
}) => {
  const classes = useStyles()

  return (
    <button
      {...rest}
      // eslint-disable-next-line react/button-has-type
      type={type || 'button'}
      disabled={isDisabled}
      className={combine(
        classes.floatingMenuButton, isDisabled && classes.disabled, isActive && classes.active,
        'style-reset', hasIcon && classes.iconPadding, isDanger && classes.isDanger
      )}
      onClick={(e) => {
        e.stopPropagation()
        onClick(e)
      }}
    >
      {children}
    </button>
  )
}

export {
  FloatingMenuButton,
  useStyles as useFloatingMenuButtonStyles,
}
