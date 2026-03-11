import React from 'react'

import {combine} from '../../common/styles'
import {createThemedStyles} from '../theme'
import {Tooltip} from './tooltip'

const useStyles = createThemedStyles(theme => ({
  toggleSwitchContainer: {
    display: 'flex',
    flex: 1,
  },
  switchButtonContainer: {
    'display': 'flex',
    'flex': 1,
    '& span': {
      width: '100%',
    },
  },
  toggleSwitchBtn: {
    'display': 'flex',
    'alignItems': 'center',
    'justifyContent': 'center',
    'flex': 1,
    'cursor': 'pointer',
    'border': 'none',
    'minHeight': '24px',
    'whiteSpace': 'nowrap',
    'textOverflow': 'ellipsis',
    'lineHeight': 'normal',
    'fontFamily': 'inherit',
    'color': theme.fgMuted,
    'fontSize': '12px',
    'fontWeight': 600,
    '&:hover:not(:disabled)': {
      color: theme.fgMain,
      background: theme.studioPanelBtnHoverBg,
    },
    '&:disabled': {
      'cursor': 'default',
      'color': theme.fgDisabled,
    },
    '&:first-child': {
      borderTopLeftRadius: '4px',
      borderBottomLeftRadius: '4px',
    },
    '&:last-child': {
      borderTopRightRadius: '4px',
      borderBottomRightRadius: '4px',
    },
    'background': theme.mainEditorPane,
  },
  toggleSwitchBtnActive: {
    color: theme.fgMain,
    background: theme.studioPanelBtnBg,
  },
}))

interface JointToggleOption<T extends string> {
  value: T
  content: React.ReactNode
  disabled?: boolean
  tooltip?: React.ReactNode
}

interface IJointToggleButton<T extends string> {
  disabled?: boolean
  value: T
  readonly options: readonly [JointToggleOption<T>, JointToggleOption<T>, ...JointToggleOption<T>[]]
  onChange: (selected: T) => void
}

const JointToggleButton = <T extends string>({
  disabled, options, value, onChange,
}: IJointToggleButton<T>) => {
  const classes = useStyles()

  return (
    <div className={classes.toggleSwitchContainer}>
      {options.map((option) => {
        const button = (
          <button
            key={option.value}
            type='button'
            disabled={disabled || option.disabled}
            className={combine(
              'style-reset',
              classes.toggleSwitchBtn,
              !(disabled || option.disabled) &&
              value === option.value && classes.toggleSwitchBtnActive
            )}
            onClick={() => onChange(option.value)}
          >
            {option.content}
          </button>
        )

        return option.tooltip
          ? (
            <div key={option.value} className={classes.switchButtonContainer}>
              <Tooltip
                content={option.tooltip}
                zIndex={100}
              >
                {button}
              </Tooltip>
            </div>
          )
          : button
      })}
    </div>
  )
}

export {
  JointToggleButton,
  IJointToggleButton,
  JointToggleOption,
}
