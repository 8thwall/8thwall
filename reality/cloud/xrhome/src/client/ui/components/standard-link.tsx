import React from 'react'

import {createThemedStyles} from '../theme'
import {combine} from '../../common/styles'
import {ICoreLink, CoreLink} from './core-link'

const useStyles = createThemedStyles(theme => ({
  standardLink: {
    'color': 'var(--standard-link-color) !important',
    'textDecoration': 'none',
    '&:hover': {
      color: 'var(--standard-link-color)',
    },
  },
  inherit: {
    'color': 'inherit !important',
    '&:hover': {
      color: 'inherit',
    },
  },
  bold: {
    fontWeight: 700,
  },
  main: {
    '--standard-link-color': theme.fgMain,
  },
  primary: {
    '--standard-link-color': theme.fgPrimary,
  },
  muted: {
    '--standard-link-color': theme.fgMuted,
  },
  blue: {
    '--standard-link-color': theme.fgBlue,
  },
  underline: {
    textDecoration: 'underline',
  },
}))

interface IStandardLink extends ICoreLink {
  color?: 'blue' | 'muted' | 'primary' | 'main' | 'inherit'
  bold?: boolean
  underline?: boolean
}

const StandardLink: React.FC<IStandardLink> = ({
  className, children, color = 'primary', bold, underline, ...rest
}) => {
  const classes = useStyles()

  const classesToUse = combine(
    className,
    color === 'inherit' ? classes.inherit : combine(classes.standardLink, classes[color]),
    bold && classes.bold,
    underline && classes.underline
  )

  return (
    <CoreLink
      // eslint-disable-next-line local-rules/ui-component-styling
      className={classesToUse}
      {...rest}
    >{children}
    </CoreLink>
  )
}

export {
  StandardLink,
}

export type {
  IStandardLink,
}
