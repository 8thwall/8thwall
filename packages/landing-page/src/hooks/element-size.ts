import {useLayoutEffect, useMemo, useState} from 'preact/hooks'

const useElementSize = (elementRef: {current: HTMLElement}) => {
  const [size, setSize] = useState<{width: number, height: number}>(null)
  const observer = useMemo(() => {
    if (!window.ResizeObserver) {
      return null
    }
    return new window.ResizeObserver((entries) => {
      const newSize = entries[0]?.contentRect
      if (newSize) {
        setSize({width: newSize.width, height: newSize.height})
      }
    })
  }, [])

  useLayoutEffect(() => {
    if (!observer) {
      return undefined
    }

    const el = elementRef.current
    observer.observe(el)
    return () => {
      observer.unobserve(el)
    }
  }, [observer, elementRef])

  return size
}

export {
  useElementSize,
}
