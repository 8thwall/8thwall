import React from 'react'

import {createCustomUseStyles} from '../../common/create-custom-use-styles'
import {UiTheme, useUiTheme} from '../theme'

const useStyles = createCustomUseStyles<{
  color: string
  trackColor: string
  fillPercentage: number
  disabled: boolean
}>()((theme: UiTheme) => ({
  container: {
    position: 'relative',
    width: '100%',
    height: '20px',
  },
  rangeSlider: {
    'width': '100%',
    'appearance': 'none',
    'background': 'transparent',
    'height': '20px',
    'position': 'relative',
    '--slider-thumb-bg': 'var(--slider-thumb-bg, #8083a2)',
    '--slider-thumb-cursor': 'var(--slider-thumb-cursor, pointer)',
    '--slider-track-bg': 'var(--slider-track-bg, #e0e0e0)',
    '&::-webkit-slider-runnable-track': {
      width: '100%',
      height: '6px',
      cursor: 'pointer',
      background: 'transparent',
      borderRadius: '3px',
    },
    '&::-webkit-slider-thumb': {
      appearance: 'none',
      width: '18px',
      height: '18px',
      borderRadius: '50%',
      backgroundColor: 'var(--slider-thumb-bg)',
      cursor: 'var(--slider-thumb-cursor)',
      marginTop: '-6px',
      boxShadow: '0 2px 4px rgba(0, 0, 0, 0.2)',
      transition: 'background-color 0.2s ease',
    },
    '&:focus::-webkit-slider-thumb': {
      boxShadow: '0 0 0 3px rgba(0, 0, 0, 0.1)',
    },
  },
  rangeSliderTrack: {
    position: 'absolute',
    top: '7px',
    width: '100%',
    height: '6px',
    backgroundColor: ({trackColor, disabled}) => (disabled ? theme.fgDisabled : trackColor),
    borderRadius: '3px',
  },
  rangeSliderFill: {
    position: 'absolute',
    height: '6px',
    borderRadius: '3px',
    top: '7px',
    left: 0,
    pointerEvents: 'none',
    backgroundColor: ({color, disabled}) => (disabled ? theme.fgDisabled : color),
    width: ({fillPercentage}) => `${Math.min(fillPercentage, 100)}%`,
  },
  rangeSliderCenter: {
    position: 'absolute',
    left: '50%',
    right: '50%',
    transform: 'translateX(-50%)',
    width: '2px',
    height: '100%',
    background: ({color, disabled}) => (disabled ? theme.fgDisabled : color),
  },
}))

interface IRangeSliderInput {
  id: string
  value: number
  step: number
  min: number
  max: number
  disabled?: boolean
  onChange: (value: number) => void
  color?: string | ((theme: UiTheme) => string)
  trackColor?: string | ((theme: UiTheme) => string)
  centerMark?: boolean
}

const RangeSliderInput: React.FC<IRangeSliderInput> = ({
  id, value, min, max, step, color, trackColor, disabled, onChange, centerMark = false,
}) => {
  const theme = useUiTheme()
  const resolvedColor = (typeof color === 'function' ? color(theme) : color) ?? theme.toggleBtnOn
  const resolvedTrackColor =
    (typeof trackColor === 'function' ? trackColor(theme) : trackColor) ?? theme.mainEditorPane

  const classes = useStyles({
    fillPercentage: ((value - min) / (max - min)) * 100,
    color: resolvedColor,
    trackColor: resolvedTrackColor,
    disabled,
  })

  // Set CSS variables for dynamic pseudo-element styling
  const sliderStyle: React.CSSProperties = {
    // Thumb color
    '--slider-thumb-bg': disabled ? theme.fgDisabled : resolvedColor,
    '--slider-thumb-cursor': disabled ? 'auto' : 'pointer',
    // Track color (if you want to use it in pseudo-element, not used here)
    '--slider-track-bg': disabled ? theme.fgDisabled : resolvedTrackColor,
  } as React.CSSProperties

  return (
    <div className={classes.container}>
      <div className={classes.rangeSliderTrack} />
      <div className={classes.rangeSliderFill} />
      {centerMark && <div className={classes.rangeSliderCenter} />}
      <input
        id={id}
        type='range'
        className={classes.rangeSlider}
        value={value}
        min={min}
        max={max}
        step={step}
        disabled={disabled}
        onChange={e => onChange?.(e.target.valueAsNumber)}
        style={sliderStyle}
      />
    </div>
  )
}

export {
  RangeSliderInput,
  IRangeSliderInput,
}
