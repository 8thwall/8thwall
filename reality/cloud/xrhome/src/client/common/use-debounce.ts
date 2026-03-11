import React from 'react'

const useDebounce = (saveFunc: React.RefObject<Function>, debounceIntervalMs = 1500) => {
  const timeoutRef = React.useRef(null)
  return (...args) => {
    if (timeoutRef.current) {
      clearTimeout(timeoutRef.current)
    }

    timeoutRef.current = setTimeout(() => {
      saveFunc.current?.(...args)
      timeoutRef.current = null
    }, debounceIntervalMs)
  }
}

const useCancelableDebounce = (func: React.RefObject<Function>, debounceIntervalMs = 1500) => {
  const timeoutRef = React.useRef(null)
  const debounced = (...args) => {
    clearTimeout(timeoutRef.current)
    timeoutRef.current = setTimeout(() => {
      func.current?.(...args)
      timeoutRef.current = null
    }, debounceIntervalMs)
  }

  const cancel = () => clearTimeout(timeoutRef.current)

  return [debounced, cancel]
}

export {
  useDebounce as default,
  useDebounce,
  useCancelableDebounce,
}
