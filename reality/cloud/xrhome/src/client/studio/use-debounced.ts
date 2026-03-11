import React from 'react'

const useDebounced = <T>(value: T, delay: number) => {
  const [debouncedValue, setDebouncedValue] = React.useState(value)
  React.useEffect(() => {
    const timeout = setTimeout(() => {
      setDebouncedValue(value)
    }, delay)
    return () => {
      clearTimeout(timeout)
    }
  }, [value, delay])

  return debouncedValue
}

export {
  useDebounced,
}
