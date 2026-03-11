import React from 'react'

import {StandardFieldLabel} from '../../ui/components/standard-field-label'
import {StandardToggleInput} from '../../ui/components/standard-toggle-input'
import {useStyles} from './row-styles'
import {useIdFallback} from '../use-id-fallback'

interface IRowToggleField {
  id?: string
  label: React.ReactNode
  checked: boolean
  onChange: (checked: boolean) => void
  disabled?: boolean
}

const RowToggleField: React.FC<IRowToggleField> = ({
  id: idOverride, label, checked, onChange, disabled,
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
      <div className={classes.flexItemShrink}>
        <StandardToggleInput
          id={id}
          checked={checked}
          onChange={onChange}
          label=''
          disabled={disabled}
        />
      </div>
    </label>
  )
}

export {RowToggleField}
export type {IRowToggleField}
