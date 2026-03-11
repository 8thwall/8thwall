import React from 'react'
import type {DeepReadonly} from 'ts-essentials'

import {createThemedStyles} from '../theme'
import {Icon, IconStroke} from './icon'

const useStyles = createThemedStyles(theme => ({
  toggleGroupContainer: {
    display: 'flex',
    flexDirection: 'row',
    gap: '8px',
    width: '100%',
  },
  toggleButton: {
    'display': 'flex',
    'alignItems': 'center',
    'justifyContent': 'center',
    'flex': 1,
    'cursor': 'pointer',
    'borderRadius': '4px',
    'border': theme.iconBtnBorder,
    'minHeight': '24px',
    'backgroundColor': theme.mainEditorPane,
    'color': theme.sfcHighlight,
    '&:hover:not(:disabled)': {
      color: theme.fgMain,
    },
    '&:disabled': {
      'cursor': 'default',
      'backGroundColor': theme.mainEditorPane,
      'color': theme.fgDisabled,
    },
    '&[aria-pressed=true]': {
      color: theme.fgMain,
      background: theme.studioPanelBtnBg,
    },
  },
}))

type ToggleButtonOption = {
  value: string
  disabled?: boolean
  icon: IconStroke
  text: string
}
interface IToggleButtonGroup {
  value?: number | string
  disabled?: boolean
  options: DeepReadonly<ToggleButtonOption[]>
  onChange: (value: string) => void
}

const ToggleButtonGroup = ({
  value: currentValue, disabled: groupDisabled, options, onChange,
}: IToggleButtonGroup) => {
  const classes = useStyles()

  return (
    <div className={classes.toggleGroupContainer}>
      {options.map(({value: optionValue, disabled: optionDisabled, icon: stroke, text}) => {
        const disabled = groupDisabled || optionDisabled

        const active = !disabled && currentValue === optionValue
        return (
          <button
            key={optionValue}
            type='button'
            disabled={disabled}
            className={classes.toggleButton}
            onClick={() => onChange(optionValue)}
            aria-pressed={active}
            aria-label={text}
          >
            <Icon stroke={stroke} />
          </button>
        )
      })}
    </div>
  )
}

export {
  ToggleButtonGroup,
  IToggleButtonGroup,
}
