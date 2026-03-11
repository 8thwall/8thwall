import React from 'react'

import {combine} from '../../common/styles'
import type {ISliderInputField} from '../../ui/components/slider-input-axis-field'
import {SliderInput} from '../ui/slider-input'
import {StandardFieldContainer} from '../../ui/components/standard-field-container'
import {RowFieldLabel} from './row-field-label'
import {useArrowKeyIncrement} from '../arrow-increment'
import {derivePreciseEditValue} from './number-formatting'
import {useEphemeralEditState} from './ephemeral-edit-state'
import {useStyles} from './row-styles'
import {DeleteButton} from './row-delete-button'
import {handleBlurInputField} from './handle-blur-input-field'
import {ExpanseFieldDiffChip} from './expanse-field-diff-chip'
import type {ExpanseField} from './expanse-field-types'
import type {DefaultableConfigDiffInfo} from './diff-chip-types'
import {makeRenderValueNumber} from './diff-chip-default-renderers'
import {useIdFallback} from '../use-id-fallback'

interface IRowNumberField extends ISliderInputField {
  placeholder?: string
  icon?: React.ReactNode
  defaultValue?: number
  clearIfDefault?: boolean
  onFocus?: () => void
  onBlur?: () => void
  onDelete?: () => void
  expanseField?: ExpanseField<DefaultableConfigDiffInfo<number, string[][]>>
}
interface INumberInput extends Omit<IRowNumberField, 'fullWidth' | 'label' | 'mixed' | 'id'> {
  id: string
  ariaLabel?: string
}

const NumberInput: React.FC<INumberInput> = ({
  id, ariaLabel, onChange, min, max, step, value, disabled, placeholder, icon, onBlur, onFocus,
  onDelete, normalizer, onDrag, fixed, defaultValue, clearIfDefault = false,
}) => {
  const classes = useStyles()

  const {editValue, setEditValue, clear} = useEphemeralEditState({
    value,
    deriveEditValue: (v: number) => (clearIfDefault && v === defaultValue
      ? ''
      : derivePreciseEditValue(v, step, fixed)
    ),
    parseEditValue: (v: string) => {
      if (!v) {
        if (defaultValue !== undefined) {
          return [true, defaultValue] as const
        }
        return [false]  // NOTE(christoph): Empty string parses to 0
      }
      const number = Number(v)
      if (Number.isNaN(number)) {
        return [false]
      }
      if (min !== undefined && number < min) {
        return [false]
      }
      if (max !== undefined && number > max) {
        return [false]
      }
      return [true, number] as const
    },
    onChange,
  })

  const handleKeydown = useArrowKeyIncrement(
    value, (newValue) => {
      if (typeof newValue === 'number') {
        onChange(newValue)
      } else {
        setEditValue(newValue)
      }
      clear()
    }, min, max, step
  )

  return (
    <div className={combine(classes.flexItem, onDelete && classes.flexItemGroupSpaceBetween)}>
      <StandardFieldContainer disabled={disabled} grow>
        {icon && (
          <SliderInput
            className={classes.selectIconInputContainer}
            value={value}
            min={min}
            max={max}
            step={step}
            onChange={onChange}
            disabled={disabled}
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
          min={min}
          max={max}
          step={step}
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

const RowNumberField: React.FC<IRowNumberField> = ({
  id: idOverride, label, disabled, value, min, max, step, onChange, fixed, normalizer, onDrag,
  expanseField, ...rest
}) => {
  const classes = useStyles()
  const id = useIdFallback(idOverride)

  return (
    <label htmlFor={id} className={classes.row}>
      {expanseField &&
        <ExpanseFieldDiffChip
          field={expanseField}
          defaultRenderers={{defaultRenderValue: makeRenderValueNumber({step})}}
        />}
      <SliderInput
        className={classes.flexItem}
        value={value}
        min={min}
        max={max}
        step={step}
        onChange={onChange}
        disabled={disabled}
        normalizer={normalizer}
        onDrag={onDrag}
      >
        <RowFieldLabel
          label={label}
          disabled={disabled}
          expanseField={expanseField}
        />
      </SliderInput>
      <NumberInput
        {...rest}
        id={id}
        disabled={disabled}
        value={value}
        min={min}
        max={max}
        step={step}
        fixed={fixed}
        normalizer={normalizer}
        onDrag={onDrag}
        onChange={onChange}
      />
    </label>
  )
}

export {
  NumberInput,
  RowNumberField,
  INumberInput,
}
