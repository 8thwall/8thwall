import React from 'react'

import {createThemedStyles} from '../theme'
import {combine} from '../../common/styles'
import {useIdFallback} from '../../studio/use-id-fallback'

const useStyles = createThemedStyles(theme => ({
  standardRadioButton: {
    'boxSizing': 'border-box',
    'display': 'flex',
    'alignItems': 'center',
    'flex-wrap': 'wrap',
    '& input': {
      opacity: 0,
      width: 0,
      height: 0,
      position: 'absolute',
    },
  },
  indicator: {
    'display': 'block',
    'position': 'relative',
    'width': '16px',
    'height': '16px',
    'backgroundColor': theme.standardRadioButtonIndicatorBackground,
    'border': theme.standardRadioButtonIndicatorBorder,
    'cursor': 'pointer',
    'marginRight': '1em',
    'borderRadius': '100px',
    'input:focus + &': {
      boxShadow: `0 0 0 1px ${theme.sfcBorderFocus}`,
    },
  },
  indicatorChecked: {
    'input:checked + &': {
      border: theme.standardRadioButtonIndicatorBorderChecked,
    },
  },
  indicatorDecorators: {
    'input:checked + &::before': {
      content: '""',
      display: theme.standardRadioButtonPseudoElementDisplay,
      position: 'absolute',
      width: '4px',
      height: '4px',
      transform: 'translate(-50%, -50%)',
      top: '50%',
      left: '50%',
      borderRadius: '50%',
      zIndex: 2,
      backgroundColor: theme.radioButtonBg,
    },
    'input:checked + &::after': {
      content: '""',
      display: theme.standardRadioButtonPseudoElementDisplay,
      position: 'absolute',
      top: '0',
      left: '0',
      width: '14px',
      height: '14px',
      border: `1px solid ${theme.radioButtonBg}`,
      borderRadius: '50%',
      pointerEvents: 'none',
      backgroundColor: theme.fgMain,
    },
  },
  labelText: {
    color: theme.fgMain,
  },
  standardRadioGroup: {
    border: 'none',
    margin: 0,
    padding: 0,
  },
  legendText: {
    color: theme.fgMain,
    display: 'block',
    margin: 0,
    padding: 0,
  },
  bold: {
    fontWeight: 'bold',
  },
  disabled: {
    cursor: 'not-allowed',
    opacity: 0.25,
  },
}))

interface IStandardRadioButton extends React.InputHTMLAttributes<HTMLInputElement> {
  id?: string
  label: React.ReactNode
  disabled?: boolean
}

const StandardRadioButton: React.FC<IStandardRadioButton> = ({
  id: idOverride,
  label,
  disabled,
  ...rest
}) => {
  const id = useIdFallback(idOverride)
  const classes = useStyles()

  return (
    <label
      tabIndex={-1}
      htmlFor={id}
      className={combine(
        classes.standardRadioButton, disabled && classes.disabled
      )}
    >
      <input
        {...rest}
        id={id}
        type='radio'
        disabled={disabled}
      />
      <span className={
        combine(classes.indicator, classes.indicatorChecked, classes.indicatorDecorators)}
      />
      <span className={classes.labelText}>{label}</span>
    </label>
  )
}

interface IStandardRadioGroup {
  label: React.ReactNode
  boldLabel?: boolean
  children?: React.ReactNode
}

const StandardRadioGroup: React.FC<IStandardRadioGroup> = ({label, boldLabel, children}) => {
  const classes = useStyles()

  return (
    <fieldset className={combine(classes.standardRadioGroup, boldLabel && classes.bold)}>
      <legend className={classes.legendText}>{label}</legend>
      {children}
    </fieldset>
  )
}

export {
  StandardRadioButton,
  StandardRadioGroup,
}

export type {
  IStandardRadioButton,
  IStandardRadioGroup,
}
