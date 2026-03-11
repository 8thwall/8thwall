import React from 'react'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'

const useStyles = createThemedStyles(theme => ({
  tagButton: {
    'fontFamily': 'inherit',
    'fontSize': '14px',
    'fontWeight': 400,
    'border': theme.tagBorder,
    'boxSizing': 'border-box',
    'backgroundColor': theme.tagBg,
    'color': theme.tagFgDefault,
    'cursor': 'pointer',
    'display': 'flex',
    'alignItems': 'center',
    'whiteSpace': 'pre',
    '&:hover': {
      backgroundColor: theme.tagHoverBg,
      color: theme.tagFgHover,
    },
  },
  tagButtonCloseIcon: {
    minWidth: '8px',
    height: '0.5rem',
    marginLeft: '0.5em',
    color: theme.tagFgHover,
  },
  medium: {
    minHeight: '32px',
    padding: '6px 12px',
    borderRadius: theme.tagBorderRadius,
  },
  small: {
    minHeight: '24px',
    padding: '2px 8px',
    borderRadius: theme.tagBorderRadiusSmall,
  },
  tiny: {
    minHeight: '18px',
    padding: '1px 8px',
    fontSize: '12px',
    borderRadius: theme.tagBorderRadiusTiny,
  },
  wrap: {
    whiteSpace: 'normal',
  },
}))

interface ITag extends React.ButtonHTMLAttributes<HTMLButtonElement> {
  type?: 'button' | 'submit' | 'reset'
  height?: 'medium' | 'small' | 'tiny'
  wrap?: boolean
}

const Tag: React.FC<ITag> = ({
  className, type = 'button', height = 'medium', wrap, children, ...rest
}) => {
  const classes = useStyles()

  return (
    <button
      {...rest}
      // eslint-disable-next-line react/button-has-type
      type={type}
      className={combine(classes.tagButton, classes[height], wrap && classes.wrap, className)}
    >{children}
    </button>
  )
}

export {
  Tag,
}

export type {
  ITag,
}
