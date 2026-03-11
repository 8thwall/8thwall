import React from 'react'
import {createUseStyles} from 'react-jss'

import {useEvent} from '../../hooks/use-event'

const useStyles = createUseStyles({
  slidable: {
    cursor: 'ew-resize',
  },
  disabled: {
    cursor: 'not-allowed',
  },
})

type DragState = {
  baseValue: number
  startClientX: number
  cancel: () => void
}

type SliderInputProps = {
  value: number
  step?: number
  min?: number
  max?: number
  normalizer?: (value: number) => number
  onChange: (value: number) => void
  onDrag?: (difference: number) => void
  disabled?: boolean
}

const useSliderInput = ({
  value, step, min, max, onDrag, normalizer, onChange, disabled = false,
}: SliderInputProps) => {
  const dragRef = React.useRef<DragState | null>(null)
  const classes = useStyles()

  const handleMove = useEvent((e: MouseEvent) => {
    const difference = e.movementX * (step ?? 0.1)
    const newRawValue = dragRef.current.baseValue + difference
    const withMin = min === undefined ? newRawValue : Math.max(min, newRawValue)
    const withMax = max === undefined ? withMin : Math.min(max, withMin)
    const normalized = normalizer ? normalizer(withMax) : withMax
    dragRef.current.baseValue = normalized

    if (onDrag) {
      onDrag(difference)
    } else {
      onChange(normalized)
    }
  })

  const onStartDrag = (startEvent: React.MouseEvent<HTMLElement>) => {
    if (dragRef.current) {
      return
    }

    const cancel = () => {
      dragRef.current = null
      document.exitPointerLock()
      document.removeEventListener('mousemove', handleMove)
      document.removeEventListener('mouseup', cancel)
    }

    document.addEventListener('mouseup', cancel)
    document.addEventListener('mousemove', handleMove)

    dragRef.current = {
      baseValue: Number.isNaN(value) ? 0 : value,
      startClientX: startEvent.clientX,
      cancel,
    }
    document.body.requestPointerLock()
  }

  React.useEffect(() => () => {
    if (dragRef.current) {
      dragRef.current.cancel()
    }
  }, [])

  return disabled
    ? {className: classes.disabled}
    : {onMouseDown: onStartDrag, className: classes.slidable}
}

export {
  useSliderInput,
  SliderInputProps,
}
