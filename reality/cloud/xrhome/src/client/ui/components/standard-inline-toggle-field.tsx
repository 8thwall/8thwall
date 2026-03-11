import React from 'react'

import {StandardToggleInput} from './standard-toggle-input'
import {useIdFallback} from '../../studio/use-id-fallback'

interface IStandardInlineToggleField {
  id?: string
  label: React.ReactNode
  checked: boolean
  reverse?: boolean
  onChange: (checked: boolean) => void
  nowrap?: boolean
}

const StandardInlineToggleField: React.FC<IStandardInlineToggleField> = ({
  checked,
  onChange,
  id: idOverride,
  label,
  reverse,
  nowrap = false,
}) => {
  const id = useIdFallback(idOverride)
  return (
    <label
      htmlFor={id}
    >
      <StandardToggleInput
        checked={checked}
        onChange={onChange}
        id={id}
        label={label}
        reverse={reverse}
        nowrap={nowrap}
      />
    </label>
  )
}

export {
  StandardInlineToggleField,
}

export type {
  IStandardInlineToggleField,
}
