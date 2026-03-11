import React from 'react'

import {combine} from '../../common/styles'

import {fiftyPercentBlack, tinyViewOverride, twentyPercentBlack} from '../../static/styles/settings'
import {createThemedStyles} from '../theme'

const useStyles = createThemedStyles(theme => ({
  disabledPointer: {
    '& .slider': {
      cursor: 'not-allowed !important',
    },
    'opacity': 0.5,
  },
  toggle: {
    'display': 'flex',
    'alignItems': 'center',
    'gap': '1rem',
    [tinyViewOverride]: {
      display: 'block',
    },
    '& input': {
      width: 1,
      height: 1,
      overflow: 'hidden',
      position: 'absolute',
      clip: 'rect(1px, 1px, 1px, 1px)',
    },
    '& input:focus + .slider': {
      boxShadow: `0 0 0 1px ${theme.sfcBorderFocus}`,
    },
    '& input:checked + .slider': {
      backgroundColor: theme.toggleBackgroundEnabled,
    },
    '& input:checked + .slider:before': {
      transform: 'translateX(18px)',
      boxShadow: `0 0 0.5px 0.5px ${fiftyPercentBlack}`,
    },
    '& .slider': {
      display: 'block',
      position: 'relative',
      width: '36px',
      height: '18px',
      cursor: 'pointer',
      backgroundColor: theme.toggleBackgroundDisabled,
      borderRadius: '9px',
    },
    '& .slider:before': {
      position: 'absolute',
      content: '""',
      height: '14px',
      width: '14px',
      left: '2px',
      bottom: '2px',
      backgroundColor: theme.toggleIndicator,
      transition: 'transform .4s',
      borderRadius: '50%',
      boxShadow: `0 0 0.5px 0.5px ${twentyPercentBlack}`,
    },
  },
  labelText: {
    'display': 'flex',
    'color': theme.fgMain,
    [tinyViewOverride]: {
      'display': 'block',
    },
  },
  toggleRow: {
    display: 'flex',
    alignItems: 'center',
    flexWrap: 'wrap',
  },
  reverse: {
    flexDirection: 'row-reverse',
    justifyContent: 'flex-end',
  },
  nowrap: {
    display: 'flex',
  },
}))

interface IStandardToggleInput {
  id: string
  label: React.ReactNode
  checked: boolean
  reverse?: boolean
  onChange: (checked: boolean) => void
  nowrap?: boolean
  disabled?: boolean
}

const StandardToggleInput: React.FC<IStandardToggleInput> = ({
  checked,
  onChange,
  id,
  label,
  reverse,
  nowrap = false,
  disabled,
}) => {
  const classes = useStyles()
  return (
    <div
      className={combine(classes.toggle,
        reverse && classes.reverse, nowrap && classes.nowrap, disabled && classes.disabledPointer)}
    >
      <span className={classes.labelText}>{label}</span>
      <span className={classes.toggleRow}>
        <input
          id={id}
          type='checkbox'
          checked={checked}
          onChange={e => onChange(e.target.checked)}
          disabled={disabled}
        />
        <span className='slider' />
      </span>
    </div>
  )
}

export {
  StandardToggleInput,
}

export type {
  IStandardToggleInput,
}
