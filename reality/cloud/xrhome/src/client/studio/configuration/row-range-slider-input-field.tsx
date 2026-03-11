import React from 'react'

import {RangeSliderInput, type IRangeSliderInput} from '../../ui/components/range-slider-input'
import {useStyles} from './row-styles'
import {combine} from '../../common/styles'
import {StandardFieldLabel} from '../../ui/components/standard-field-label'
import {DeleteButton} from './row-delete-button'
import {useIdFallback} from '../use-id-fallback'

interface IRowRangeSliderInputField extends IRangeSliderInput {
  label: React.ReactNode
  leftContent?: React.ReactNode
  rightContent?: React.ReactNode
  onDelete?: () => void
}

const RowRangeSliderInputField: React.FC<IRowRangeSliderInputField> = ({
  id: idOverride, value, min, max, step, label, disabled, leftContent, rightContent, color,
  trackColor, onChange, onDelete,
}) => {
  const id = useIdFallback(idOverride)
  const classes = useStyles()

  return (
    <label htmlFor={id} className={classes.row}>
      <div className={classes.flexItem}>
        <StandardFieldLabel
          label={label}
          mutedColor
          disabled={disabled}
        />
      </div>
      <div
        className={combine(
          classes.flexItemGroup, onDelete && classes.flexItemGroupSpaceBetween
        )}
      >
        {leftContent}
        <RangeSliderInput
          id={id}
          value={value}
          min={min}
          max={max}
          step={step}
          color={color}
          trackColor={trackColor}
          disabled={disabled}
          onChange={onChange}
        />
        {rightContent}
        {onDelete && <DeleteButton onDelete={onDelete} />}
      </div>
    </label>
  )
}

export {RowRangeSliderInputField}
export type {IRowRangeSliderInputField}
