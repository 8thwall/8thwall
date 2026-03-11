import React from 'react'

import type {IRowTextField} from './row-text-field'
import type {SliderInputProps} from '../../ui/hooks/use-slider-input'
import {DeleteButton} from './row-delete-button'
import {SliderInput} from '../ui/slider-input'
import {StandardFieldContainer} from '../../ui/components/standard-field-container'
import {RowFieldLabel} from './row-field-label'
import {cleanDragValue} from './clean-drag-value'
import {combine} from '../../common/styles'
import {handleBlurInputField} from './handle-blur-input-field'
import {useArrowKeyIncrement} from '../arrow-increment'
import {useEphemeralEditState} from './ephemeral-edit-state'
import {useStyles} from './row-styles'
import {useIdFallback} from '../use-id-fallback'

interface IRowNumberOrPercentField extends Omit<IRowTextField,
  'onDrag' | 'value' | 'onChange' | 'min' | 'max' | 'step'>,
  Pick<SliderInputProps, 'min' | 'max' | 'normalizer'> {
  value: string | number  // NOTE(jeffha): override `value` type since it also could be a string[]
  onChange: (newValue: string) => void
  acceptAuto?: boolean
  placeholder?: string
  step?: number
  fixed?: number
  defaultValue?: string | number
  onDrag?: ((newValue: number) => void)
  onFocus?: () => void
  onBlur?: () => void
  onDelete?: () => void
}

interface INumberOrPercentInput extends Omit<IRowNumberOrPercentField, 'label' | 'id'> {
  id: string
  ariaLabel?: string
}

const NumberOrPercentInput: React.FC<INumberOrPercentInput> = ({
  id, icon, ariaLabel, onChange, value, disabled, acceptAuto, placeholder,
  step = 1, fixed = 3, min, max, onBlur, onFocus, normalizer, onDelete, onDrag,
  defaultValue,
}) => {
  const handleKeydown = useArrowKeyIncrement(value, onChange, min, max, step)

  const classes = useStyles()
  const {editValue, setEditValue, clear} = useEphemeralEditState({
    value,
    deriveEditValue: (v: string) => v,
    parseEditValue: (v: string) => {
      if (acceptAuto && v === 'auto') {
        return [true, v] as const
      }
      if (!v) {
        if (defaultValue !== undefined) {
          return [true, defaultValue] as const
        }
        return [false]
      }
      const number = parseFloat(v)
      if (Number.isNaN(number)) {
        return [false]
      }
      if (min !== undefined && number < min) {
        return [false]
      }
      if (max !== undefined && number > max) {
        return [false]
      }
      if (v.endsWith('%')) {
        return [true, v] as const
      }
      return [true, number] as const
    },
    onChange,
  })

  return (
    <div className={combine(classes.flexItem, onDelete && classes.flexItemGroupSpaceBetween)}>
      <StandardFieldContainer disabled={disabled} grow>
        {icon && (
          <SliderInput
            className={classes.selectIconInputContainer}
            value={typeof value === 'string' ? parseFloat(value) : value}
            min={min}
            max={max}
            step={step}
            onChange={(v: number) => {
              const isPercentage = typeof value === 'string' && value.endsWith('%')
              setEditValue(cleanDragValue(v, isPercentage, fixed))
            }}
            disabled={disabled || value === 'auto'}
            normalizer={normalizer}
            onDrag={onDrag}
          >
            {icon}
          </SliderInput>
        )}
        <input
          id={id}
          type='text'
          aria-label={ariaLabel}
          value={editValue}
          onChange={e => setEditValue(e.target.value)}
          className={classes.input}
          disabled={disabled}
          placeholder={placeholder}
          onFocus={(e) => {
            e.target.select()
            if (onFocus) {
              onFocus()
            }
          }}
          onBlur={() => {
            onBlur?.()
            clear()
          }}
          onKeyDown={(e) => {
            handleBlurInputField(e)
            handleKeydown(e)
          }}
        />
      </StandardFieldContainer>
      {onDelete && <DeleteButton onDelete={onDelete} />}
    </div>
  )
}

const RowNumberOrPercentField: React.FC<IRowNumberOrPercentField> = ({
  id: idOverride, label, disabled, step = 1, value, min, max, onChange, fixed, normalizer, onDrag,
  expanseField, ...rest
}) => {
  const classes = useStyles()
  const id = useIdFallback(idOverride)

  return (
    <label htmlFor={id} className={classes.row}>
      <SliderInput
        className={classes.flexItem}
        value={typeof value === 'string' ? parseFloat(value) : value}
        min={min}
        max={max}
        step={step}
        onChange={(v: number) => {
          const isPercentage = typeof value === 'string' && value.endsWith('%')
          onChange(cleanDragValue(v, isPercentage, fixed))
        }}
        disabled={disabled || value === 'auto'}
        normalizer={normalizer}
        onDrag={onDrag}
      >
        <RowFieldLabel
          label={label}
          disabled={disabled}
          expanseField={expanseField}
        />
      </SliderInput>
      <NumberOrPercentInput
        id={id}
        disabled={disabled}
        step={step}
        value={value}
        onChange={onChange}
        fixed={fixed}
        min={min}
        max={max}
        onDrag={onDrag}
        normalizer={normalizer}
        expanseField={expanseField}
        {...rest}
      />
    </label>
  )
}

export {RowNumberOrPercentField, NumberOrPercentInput}
export type {IRowNumberOrPercentField, INumberOrPercentInput}
