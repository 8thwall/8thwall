import React, {ChangeEventHandler} from 'react'

import {combine} from '../../common/styles'
import {useFloatingTrayCheckboxInputStyles} from './floating-tray-checkbox-field'

interface IFloatingTrayCheckboxInput {
  id: string
  checked: boolean
  onChange: ChangeEventHandler<HTMLInputElement>
  tag?: React.ReactNode
  nowrap?: boolean
  disabled?: boolean
  a8?: string
}

const FloatingTrayCheckboxInput: React.FC<IFloatingTrayCheckboxInput> = ({
  checked,
  onChange,
  id,
  tag,
  nowrap = false,
  disabled,
  a8,
}) => {
  const classes = useFloatingTrayCheckboxInputStyles()

  return (
    <div
      className={combine(
        classes.checkboxField, nowrap && classes.nowrap, disabled && classes.disabled
      )}
    >
      <input
        a8={a8}
        id={id}
        type='checkbox'
        checked={checked}
        onChange={onChange}
        disabled={disabled}
      />
      <span className={combine('indicator', !tag && classes.removeMargin)} />
      {tag}
    </div>
  )
}

export {
  FloatingTrayCheckboxInput,
}
