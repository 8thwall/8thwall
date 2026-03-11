import React from 'react'

import {createThemedStyles} from '../theme'
import {Icon, IconStroke} from './icon'
import {combine} from '../../common/styles'

const useStyles = createThemedStyles(theme => ({
  baseChip: {
    'display': 'flex',
    'flexDirection': 'row',
    'alignItems': 'center',
    'justifyContent': 'center',
    'borderRadius': '.5em',
    'backgroundColor': theme.sfcBackgroundDefault,
    'color': theme.fgMuted,
    'userSelect': 'none',
  },
  standardChip: {
    'fontSize': '14px',
    'height': '32px',
    'margin': '0 0.25rem',
    'padding': '0.25rem 0.75rem',
  },
  inlineChip: {
    padding: '0.5rem 1rem',
    lineHeight: '0.9rem',
    marginLeft: '0.5rem',
  },
  standardChipIcon: {
    'marginRight': '.5rem',
    'marginLeft': '-0.25rem',
    'marginTop': '.25rem',
    'color': theme.fgMuted,
  },
  secondaryText: {
    'marginLeft': '0.75rem',
    'color': theme.fgMain,
  },
}))
interface IStandardChipProps {
  text: string
  secondaryText?: string
  iconStroke?: IconStroke
  inline?: boolean
}

const StandardChip: React.FC<IStandardChipProps> = ({text, secondaryText, iconStroke, inline}) => {
  const classes = useStyles()

  return (
    <span className={combine('standard-chip', classes.baseChip,
      inline ? classes.inlineChip : classes.standardChip)}
    >
      {iconStroke && <div className={classes.standardChipIcon}><Icon stroke={iconStroke} /></div>}
      {text}
      {secondaryText &&
        <span className={classes.secondaryText}>
          {secondaryText}
        </span>}
    </span>
  )
}

export {
  StandardChip,
}
