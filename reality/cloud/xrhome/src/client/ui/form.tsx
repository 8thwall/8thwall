import React from 'react'

import {FloatingLabelInput as BaseFloatingLabelInput} from './components/floating-label-input'
import BaseFloatingLabelDropdown from './components/floating-label-dropdown'
import {useFormInput} from '../common/form-change-hook'

type ValidatorFunction = (value: string) => string

type Validator = ValidatorFunction | ValidatorFunction[]

export const errorIfMissing = (missingText: string) => (value: string) => (value
  ? null
  : missingText)

const validateInput = (value: string, validator?: Validator) => {
  if (!validator) {
    return null
  }
  if (Array.isArray(validator)) {
    for (let i = 0; i < validator.length; i++) {
      const error = validator[i](value)
      if (error) {
        return error
      }
    }
  } else {
    return validator(value)
  }
  return null
}

export const useTextInput = (label: string, validator?: Validator, initialValue = '') => {
  const [value, onChange] = useFormInput(initialValue)
  const [wasFocused, setWasFocused] = React.useState(false)
  const [focused, setFocused] = React.useState(false)

  return React.useMemo(() => {
    const error = validateInput(value, validator)
    const isValid = !error
    const errorText = !focused && wasFocused && error

    const forceValidate = () => {
      setWasFocused(true)
    }

    const onFocus = () => {
      setFocused(true)
      if (!wasFocused) {
        setWasFocused(true)
      }
    }

    const onBlur = () => setFocused(false)

    return {
      label,
      error,
      isValid,
      errorText,
      value,
      onChange,
      onFocus,
      onBlur,
      forceValidate,
    }
  }, [label, validator, value, onChange, wasFocused, setWasFocused, focused, setFocused])
}
export type TextInput = ReturnType<typeof useTextInput>

export const FloatingLabelInput = ({field, ...rest}) => (
  <BaseFloatingLabelInput
    label={field.label}
    value={field.value}
    errorText={field.errorText}
    onChange={field.onChange}
    onFocus={field.onFocus}
    onBlur={field.onBlur}
    {...rest}
  />
)

export const FloatingLabelDropdown = ({field, options, ...rest}) => (
  <BaseFloatingLabelDropdown
    label={field.label}
    value={field.value}
    errorText={field.errorText}
    onChange={field.onChange}
    onOpen={field.onFocus}
    onClose={field.onBlur}
    options={options}
    {...rest}
  />
)
