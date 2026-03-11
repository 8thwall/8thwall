import React from 'react'

import {createThemedStyles} from '../theme'
import {StandardFieldContainer} from './standard-field-container'
import {useEphemeralEditState} from '../../studio/configuration/ephemeral-edit-state'
import {combine} from '../../common/styles'
import {handleBlurInputField} from '../../studio/configuration/row-fields'
import {useSliderInput} from '../hooks/use-slider-input'
import {useIdFallback} from '../../studio/use-id-fallback'

const useStyles = createThemedStyles(theme => ({
  sliderInputLabel: {
    color: theme.fgMuted,
    display: 'flex',
    alignItems: 'center',
    padding: '0.25em',
    userSelect: 'none',
  },
  sliderAxisLabel: {
    padding: '0 0.25em',
  },
  sliderInput: {
    'width': '3.5em',
    'background': 'transparent',
    'border': 'none',
    'color': theme.fgMain,
    'appearance': 'textfield',
    'textOverflow': 'ellipsis',
    '&::placeholder': {
      'color': theme.fgMuted,
    },
    '&:focus-visible': {
      'outline': 'none',
      '&::placeholder': {
        color: 'transparent',
      },
    },
    '&::-webkit-inner-spin-button': {
      display: 'none',
      margin: 0,
    },
    '&::-webkit-outer-spin-button': {
      display: 'none',
      margin: 0,
    },
    '&::selection': {
      background: theme.sfcHighlight,
      color: theme.fgMain,
    },
    '&:disabled': {
      color: theme.fgMuted,
      cursor: 'not-allowed',
    },
  },
  fullWidth: {
    width: '100%',
  },
}))
interface ISliderInputField {
  id?: string
  value: number
  onChange: ((newValue: number) => void)
  onDrag?: ((newValue: number) => void)
  min?: number
  max?: number
  step: number
  label: React.ReactNode
  disabled?: boolean
  normalizer?: (value: number) => number
  fullWidth?: boolean
  mixed?: boolean
  fixed?: number
}

const SliderInputAxisField: React.FC<ISliderInputField> = ({
  id: idOverride, label, onChange, onDrag, min, max, step, value, disabled, normalizer, fullWidth,
  mixed, fixed,
}) => {
  const classes = useStyles()
  const inputRef = React.useRef<HTMLInputElement>(null)
  const draggableProps = useSliderInput({
    value, step, min, max, normalizer, onChange, onDrag, disabled,
  })
  const id = useIdFallback(idOverride)

  const {editValue, setEditValue, clear} = useEphemeralEditState({
    value,
    deriveEditValue: (v: number) => (
      typeof v === 'number' ? Number(v.toFixed(fixed ?? 3)).toString() : String(v)
    ),
    parseEditValue: (v: string) => {
      const newValue = v ? Number(v) : NaN
      if (Number.isNaN(newValue)) {
        return [false]
      }
      const normalizedValue = normalizer ? normalizer(newValue) : newValue
      if (min !== undefined && normalizedValue < min) {
        return [false]
      }
      if (max !== undefined && normalizedValue > max) {
        return [false]
      }
      return [true, normalizedValue] as const
    },
    onChange,
  })

  return (
    <StandardFieldContainer disabled={disabled}>
      <label
        htmlFor={id}
        className={classes.sliderInputLabel}
      >
        <div
          {...draggableProps}
          className={combine(classes.sliderAxisLabel, draggableProps.className)}
        >
          {label}
        </div>
        <input
          id={id}
          ref={inputRef}
          // TODO(Dale): Update sliderInput to be full width by default and fix transform gizmo
          className={combine(classes.sliderInput, fullWidth && classes.fullWidth)}
          disabled={disabled}
          type='number'
          value={mixed ? '' : editValue}
          placeholder={mixed ? '—' : ''}
          onChange={e => setEditValue(e.target.value)}
          onBlur={() => clear()}
          onFocus={e => e.target.select()}
          onKeyDown={handleBlurInputField}
        />
      </label>
    </StandardFieldContainer>
  )
}

export {
  SliderInputAxisField,
}

export type {
  ISliderInputField,
}
