import React from 'react'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'

const useStyles = createThemedStyles(theme => ({
  linkButton: {
    'display': 'inline',
    'lineHeight': 'normal',
    'fontFamily': 'inherit',
    'fontSize': '1rem',
    'fontWeight': 600,
    'background': 'transparent',
    'border': 'none',
    'margin': 0,
    'padding': 0,
    'cursor': 'pointer',
    'color': theme.linkBtnFg,
    '&:disabled': {
      cursor: 'default',
      color: theme.linkBtnDisableFg,
    },
  },
}))

interface ILinkButton extends React.ButtonHTMLAttributes<HTMLButtonElement> {
  type?: 'button' | 'submit' | 'reset'
  a8?: string
}

const LinkButton: React.FC<ILinkButton> = ({className, type, children, ...rest}) => {
  const classes = useStyles()

  return (
    <button
      {...rest}
      // eslint-disable-next-line react/button-has-type
      type={type || 'button'}
      className={combine(classes.linkButton, className)}
    >{children}
    </button>
  )
}

export {
  LinkButton,
}

export type {
  ILinkButton,
}
