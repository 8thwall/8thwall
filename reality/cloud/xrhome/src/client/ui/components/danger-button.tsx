import React from 'react'

import {createUseStyles} from 'react-jss'

import {combine} from '../../common/styles'
import {
  ButtonHeight, ButtonSpacing, useRoundedButtonStyling,
} from '../hooks/use-rounded-button-styling'
import {brandWhite, cherry, cherryLight} from '../../static/styles/settings'

const useStyles = createUseStyles({
  dangerButton: {
    'background': cherry,
    'color': brandWhite,
    '&:hover': {
      background: cherryLight,
    },
    '&:disabled': {
      background: cherry,
      opacity: 0.45,
    },
  },
})

interface IDangerButton extends React.ButtonHTMLAttributes<HTMLButtonElement>{
  type?: 'button' | 'submit' | 'reset'
  height?: ButtonHeight
  spacing?: ButtonSpacing
}

const DangerButton: React.FC<IDangerButton> = ({
  type = 'button', height = 'medium', spacing = 'normal', children, ...rest
}) => {
  const classes = useStyles()
  const roundedStylingClass = useRoundedButtonStyling(height, spacing)
  return (
    <button
      {...rest}
      // eslint-disable-next-line react/button-has-type
      type={type}
      className={combine(classes.dangerButton, roundedStylingClass)}
    >{children}
    </button>
  )
}

export {
  DangerButton,
}

export type {
  IDangerButton,
}
