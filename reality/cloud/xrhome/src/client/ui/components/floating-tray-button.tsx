import React from 'react'

import {createThemedStyles} from '../theme'
import {combine} from '../../common/styles'
import {LoadableButtonChildren} from './loadable-button-children'

const useStyles = createThemedStyles(theme => ({
  'floatingTrayButton': {
    'position': 'relative',
    'fontFamily': 'inherit',
    'fontSize': '12px',
    'display': 'flex',
    'gap': '0.25em',
    'alignItems': 'center',
    'justifyContent': 'center',
    'padding': '0.5em 1em',
    'border': 'none',
    'background': 'transparent',
    'color': theme.fgMuted,
    'cursor': 'pointer',
    'user-select': 'none',
    'minWidth': '0',
    'overflow': 'hidden',
    'width': '100%',
    '&:hover': {
      color: theme.studioBtnHoverFg,
    },
    '&:first-child': {
      borderTopLeftRadius: '0.6em',
      borderBottomLeftRadius: '0.6em',
    },
    '&:last-child': {
      borderTopRightRadius: '0.6em',
      borderBottomRightRadius: '0.6em',
    },
  },
  'active': {
    background: theme.studioBtnActiveBg,
    color: theme.studioBtnHoverFg,
  },
  'disabled': {
    'cursor': 'default',
    'color': `${theme.fgMuted}80`,
    '&:hover': {
      color: `${theme.fgMuted}80`,
    },
  },
  'purple': {
    'color': theme.studioPurpleBtnFg,
    'background': theme.studioPurpleBtnBg,
    '&:hover': {
      color: theme.studioPurpleBtnFg,
    },
  },
  'danger': {
    'color': theme.studioDangerBtnFg,
    'background': theme.studioDangerBtnBg,
    '&:hover': {
      color: theme.studioDangerBtnFg,
    },
  },
  'warning': {
    'color': theme.studioWarningBtnFg,
    'background': theme.studioWarningBtnBg,
    '&:hover': {
      color: theme.studioWarningBtnFg,
    },
  },
  'success': {
    'color': theme.studioSuccessBtnFg,
    'background': theme.studioSuccessBtnBg,
    '&:hover': {
      color: theme.studioSuccessBtnFg,
    },
  },
  'fillContainer': {
    width: '100%',
  },
  'progressBarContainer': {
    position: 'relative',
  },
  'progressBar': {
    position: 'absolute',
    bottom: 0,
    left: 0,
    height: '2px',
    transition: 'width 0.5s',
  },
  'grow': {
    flex: 1,
  },
  'shrink': {
    minWidth: '0',
  },
  '@keyframes progressAnimation': {
    '0%': {width: '0%'},
    '25%': {width: '75%'},
    '75%': {width: '80%'},
    '100%': {width: '100%'},
  },
  'fauxProgressBar': {
    position: 'absolute',
    bottom: 0,
    left: 0,
    height: '2px',
    animation: '$progressAnimation 3s',
    animationTimingFunction: 'linear',
    animationIterationCount: 1,
    animationFillMode: 'forwards',
  },
}))

type ButtonColor = 'default' | 'purple' | 'warning' | 'success' | 'danger'

interface IFloatingTrayButton extends Pick<React.ButtonHTMLAttributes<HTMLButtonElement>,
  'type' | 'children' | 'onClick' | 'aria-label' | 'id'> {
  isActive?: boolean
  isDisabled?: boolean
  color?: ButtonColor
  loading?: boolean
  fillContainer?: boolean
  progressBarWidth?: number
  progressBarColor?: string
  shrink?: boolean
  fauxProgressBar?: boolean
  grow?: boolean
  a8?: string
}

const FloatingTrayButton = React.forwardRef<HTMLButtonElement, IFloatingTrayButton>(({
  isActive = false, isDisabled = false, color = 'default', fillContainer = false,
  loading, progressBarWidth = 0, progressBarColor, fauxProgressBar, type, shrink, grow,
  children, ...rest
}, ref) => {
  const classes = useStyles()

  return (
    <button
      {...rest}
      ref={ref}
      // eslint-disable-next-line react/button-has-type
      type={type || 'button'}
      className={combine(
        'style-reset', classes.floatingTrayButton, classes[color],
        isActive && classes.active, isDisabled && classes.disabled,
        fillContainer && classes.fillContainer,
        progressBarWidth && classes.progressBarContainer,
        shrink && classes.shrink,
        grow && classes.grow
      )}
      disabled={isDisabled}
    >
      <LoadableButtonChildren loading={loading} block>
        {children}
      </LoadableButtonChildren>
      {progressBarWidth > 0 && (
        <div
          className={classes.progressBar}
          style={{width: `${progressBarWidth}%`, background: progressBarColor}}
        />
      )}
      {fauxProgressBar &&
        <div className={classes.fauxProgressBar} style={{background: progressBarColor}} />
      }
    </button>
  )
})

FloatingTrayButton.displayName = 'FloatingTrayButton'

export {
  useStyles as useFloatingTrayButtonStyles,
  FloatingTrayButton,
}
