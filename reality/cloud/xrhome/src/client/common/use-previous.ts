import React from 'react'

const usePrevious: <T>(value: T) => T = (value) => {
  const ref = React.useRef<typeof value>()
  const res = ref.current
  ref.current = value
  return res
}

export default usePrevious
