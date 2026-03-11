import React from 'react'

/**
 * Wrap useState to give you an onChange function that can be used with Form.* components in
 * semantic-ui-react, as well as normal <input> onChange events, and
 * onChange(value) where value is a bool or string
 * @param stateDefaultVal G
 */
export const useFormInput = (stateDefaultVal) => {
  const [stateVar, stateUpdateFunc] = React.useState(stateDefaultVal)
  return [stateVar, (e, target) => {
    // Handle getting the direct value onChange instead of the event
    if (typeof e === 'string' || typeof e === 'boolean') {
      stateUpdateFunc(e)
      return
    }
    // NOTE(dat): When target exists (semantic), it contains what we want
    const {checked, value} = target || e?.currentTarget
    stateUpdateFunc(value !== undefined ? value : checked)
  }]
}
