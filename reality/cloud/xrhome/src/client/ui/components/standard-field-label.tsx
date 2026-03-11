import React from 'react'

import {combine} from '../../common/styles'
import {tinyViewOverride} from '../../static/styles/settings'
import {createThemedStyles} from '../theme'

const useStyles = createThemedStyles(theme => ({
  standardLabel: {
    'color': theme.standardLabelColor,
  },
  block: {
    'display': 'block',
  },
  inline: {
    'width': '100%',
    'display': 'flex',
    'fontWeight': '700',
    [tinyViewOverride]: {
      'display': 'block',
    },
  },
  disabled: {
    'color': theme.sfcDisabledColor,
  },
  starred: {
    '&:after': {
      content: '\' *\'',
      color: theme.fgError,
      fontWeight: 'normal',
    },
  },
  bold: {
    fontWeight: 'bold',
  },
  mutedColor: {
    color: theme.fgMuted,
  },
  noUserSelect: {
    userSelect: 'none',
  },
}))

interface IStandardLabel {
  label: React.ReactNode
  id?: string
  inline?: boolean
  disabled?: boolean
  bold?: boolean
  starred?: boolean
  mutedColor?: boolean
  selectableText?: boolean
}

const StandardFieldLabel: React.FC<IStandardLabel> = (
  {label, id, inline, disabled, bold, starred, mutedColor, selectableText}
) => {
  const classes = useStyles()
  const labelStyles = combine(
    classes.standardLabel,
    disabled && classes.disabled,
    bold && classes.bold,
    starred && classes.starred,
    inline ? classes.inline : classes.block,
    mutedColor && classes.mutedColor,
    !selectableText && classes.noUserSelect
  )

  return (
    <span id={id} className={labelStyles}>{label}</span>
  )
}

export {
  StandardFieldLabel,
}

export type {
  IStandardLabel,
}
