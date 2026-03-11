import React, {ChangeEventHandler} from 'react'

import {createThemedStyles} from '../theme'
import {combine} from '../../common/styles'
import {SrOnly} from './sr-only'
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
    '& input:disabled + .indicator': {
      cursor: 'not-allowed',
      opacity: 0.5,
    },
    '& input:focus + .indicator': {
      boxShadow: `0 0 0 1px ${theme.sfcBorderFocus}`,
    },
    '& input:focus:not(:focus-visible) + .indicator': {
      boxShadow: 'none',
    },
    '& input:checked + .indicator:before': {
      background: theme.fgMain,
      border: 'none',
      // eslint-disable-next-line max-len
      clipPath: 'path(\'M12.8,4.2c-.2-.2-.4-.2-.7-.2h0c-.2,0-.5.1-.6.3l-5,5.5-1.8-2c-.2-.2-.4-.3-.6-.3-.2,0-.5,0-.7.2-.2.2-.3.4-.3.6,0,.2,0,.5.2.6l2.5,2.7c.2.2.4.3.7.3s.5-.1.7-.3l5.7-6.3c.2-.2.2-.4.2-.6,0-.2-.1-.4-.3-.6Z\')',
    },
    '& .indicator': {
      display: 'block',
      cursor: 'pointer',
      marginRight: '0.5em',
      borderRadius: '2px',
      width: '16px',
      height: '16px',
      border: `1px solid ${theme.studioPanelBtnBg}`,
      background: theme.studioBgOpaque,
      position: 'relative',
    },
    '& .indicator:before': {
      content: '""',
      display: 'block',
      width: '16px',
      height: '16px',
      position: 'absolute',
      left: '-7%',
      top: '-7%',
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
  disabled: {
    cursor: 'not-allowed',
  },
  removeMargin: {
    marginRight: '0 !important',
  },
}))

interface IFloatingTrayCheckboxField {
  id?: string
  label: React.ReactNode
  checked: boolean
  onChange: ChangeEventHandler<HTMLInputElement>
  nowrap?: boolean
  sr?: boolean
}

const FloatingTrayCheckboxField: React.FC<IFloatingTrayCheckboxField> = ({
  checked,
  onChange,
  id: idOverride,
  label,
  nowrap,
  sr,
}) => {
  const id = useIdFallback(idOverride)
  const classes = useStyles()

  return (
    <label
      tabIndex={-1}
      htmlFor={id}
      className={combine(classes.checkboxField, nowrap && classes.nowrap)}
    >
      <input
        id={id}
        type='checkbox'
        checked={checked}
        onChange={onChange}
      />
      <span className={combine('indicator', sr && classes.removeMargin)} />
      {sr
        ? <SrOnly>{label}</SrOnly>
        : (
          <span className={classes.labelText}>
            {label}
          </span>
        )}
    </label>
  )
}

export {
  useStyles as useFloatingTrayCheckboxInputStyles,
  FloatingTrayCheckboxField,
}
