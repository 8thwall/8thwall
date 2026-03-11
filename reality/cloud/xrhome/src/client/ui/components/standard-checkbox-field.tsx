import React, {ChangeEventHandler} from 'react'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'
import {useIdFallback} from '../../studio/use-id-fallback'

const useStyles = createThemedStyles(theme => ({
  checkboxField: {
    'boxSizing': 'border-box',
    'display': 'flex',
    'alignItems': 'center',
    'flexWrap': 'wrap',
    'cursor': 'pointer',
    '& input': {
      opacity: 0,
      width: 0,
      height: 0,
      position: 'absolute',
    },
    '& input:focus + .indicator': {
      boxShadow: `0 0 0 1px ${theme.sfcBorderFocus}`,
    },
    '& input:focus:not(:focus-visible) + .indicator': {
      boxShadow: 'none',
    },
    '& input:checked + .indicator:before': {
      background: theme.controlActive,
      border: 'none',
      clipPath: theme.checkmarkClipPath,
    },
    '& .indicator': {
      display: theme.checkboxDisplay,
      cursor: 'pointer',
      marginRight: '0.5em',
      borderRadius: theme.checkboxBorderRadius,
      background: theme.checkboxBg,
      border: theme.checkboxBorder,
      alignItems: theme.checkboxAlignment,
      justifyContent: theme.checkboxAlignment,
      width: theme.checkBoxSize,
      height: theme.checkBoxSize,
    },
    '& .indicator:before': {
      content: '""',
      display: 'block',
      width: theme.checkmarkSize,
      height: theme.checkmarkSize,
      border: theme.checkmarkBorder,
      borderRadius: theme.checkmarkBorderRadius,
    },
    '& .indicator:hover': {
      background: theme.checkboxHover,
    },
  },
  nowrap: {
    'alignItems': 'flex-start',
    'flexWrap': 'nowrap',
    'gap': '0.5em',
    '& .indicator': {
      margin: '0',
      lineHeight: '1em',
    },
  },
  labelText: {
    color: theme.fgMain,
  },
}))

interface IStandardCheckboxField {
  id?: string
  label: React.ReactNode
  checked: boolean
  onChange: ChangeEventHandler<HTMLInputElement>
  nowrap?: boolean
  a8?: string
}

const StandardCheckboxField: React.FC<IStandardCheckboxField> = ({
  checked,
  onChange,
  id: idOverride,
  label,
  nowrap = false,
  a8,
}) => {
  const classes = useStyles()
  const id = useIdFallback(idOverride)

  return (
    <label
      tabIndex={-1}
      htmlFor={id}
      className={combine(classes.checkboxField, nowrap && classes.nowrap)}
      a8={a8}
    >
      <input
        id={id}
        type='checkbox'
        checked={checked}
        onChange={onChange}
      />
      <span className='indicator' />
      <span className={classes.labelText}>{label}</span>
    </label>
  )
}

export {
  StandardCheckboxField,
}

export type {
  IStandardCheckboxField,
}
