import React from 'react'

type EphemeralEditState<EDITVALUE> = {
  editValue: EDITVALUE
  setEditValue: (value: EDITVALUE) => void
  clear: () => void
}

type EphemeralEditStateWithSubmit<EDITVALUE> = {
  editValue: EDITVALUE
  setEditValue: (value: EDITVALUE) => void
  clear: () => void
  submit: () => boolean
}

// parseEditValue returns [true, something] if it parses, or [false, whatever] if invalid.
type ParseResult<VALUE> = readonly [false] | readonly [false, unknown] | readonly [true, VALUE]

type EphemeralEditStateOptions<VALUE, EDITVALUE> = {
  value: VALUE
  deriveEditValue: (value: VALUE) => EDITVALUE
  parseEditValue: (editValue: EDITVALUE) => ParseResult<VALUE>
  compareValue?: (left: VALUE, right: VALUE) => boolean
  onChange: (newValue: VALUE) => void
}

type EphemeralEditStateWithSubmitOptions<VALUE, EDITVALUE> = {
  value: VALUE
  deriveEditValue: (value: VALUE) => EDITVALUE
  parseEditValue: (editValue: EDITVALUE) => ParseResult<VALUE>
  compareValue?: (left: VALUE, right: VALUE) => boolean
  onSubmit: (newValue: VALUE) => void
}

function useEphemeralEditState<VALUE, EDITVALUE>({
  value,
  deriveEditValue,
  parseEditValue,
  compareValue,
  onChange,
}: EphemeralEditStateOptions<VALUE, EDITVALUE>): EphemeralEditState<EDITVALUE> {
  const [pendingEdit, setPendingEdit] = React.useState<
    null | {for: VALUE, editValue: EDITVALUE}
  >(null)
  const isSameValue = compareValue
    ? compareValue(pendingEdit?.for, value)
    : value === pendingEdit?.for
  const isEditActive = pendingEdit && isSameValue

  const editValue = isEditActive ? pendingEdit.editValue : deriveEditValue(value)

  const setEditValue = (newEditValue: EDITVALUE) => {
    const [newValueIsValid, newValue] = parseEditValue(newEditValue)
    if (newValueIsValid) {
      onChange(newValue)
      setPendingEdit({for: newValue, editValue: newEditValue})
    } else {
      setPendingEdit({for: value, editValue: newEditValue})
    }
  }

  return {
    editValue,
    setEditValue,
    clear: () => setPendingEdit(null),
  }
}

function useEphemeralEditStateWithSubmit<VALUE, EDITVALUE>({
  value,
  deriveEditValue,
  parseEditValue,
  compareValue,
  onSubmit,
}: EphemeralEditStateWithSubmitOptions<VALUE, EDITVALUE>): EphemeralEditStateWithSubmit<EDITVALUE> {
  const [pendingEdit, setPendingEdit] = React.useState<
    null | {for: VALUE, editValue: EDITVALUE}
  >(null)
  const isSameValue = compareValue
    ? compareValue(pendingEdit?.for, value)
    : value === pendingEdit?.for

  const isEditActive = pendingEdit && isSameValue

  const editValue = isEditActive ? pendingEdit.editValue : deriveEditValue(value)

  const setEditValue = (newEditValue: EDITVALUE) => {
    setPendingEdit({for: value, editValue: newEditValue})
  }

  const submit = () => {
    const [newValueIsValid, newValue] = parseEditValue(editValue)
    if (newValueIsValid) {
      onSubmit(newValue)
      return true
    }
    return false
  }

  return {
    editValue,
    setEditValue,
    clear: () => setPendingEdit(null),
    submit,
  }
}

export {
  useEphemeralEditState,
  useEphemeralEditStateWithSubmit,
}
