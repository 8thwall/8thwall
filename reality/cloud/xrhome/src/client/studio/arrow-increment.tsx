import type React from 'react'

import {derivePrecision} from './configuration/number-formatting'

const useArrowKeyIncrement = <T extends string | number>(
  // eslint-disable-next-line arrow-parens
  value: T,
  onChange: (newValue: number | string) => void,
  min: number = -Infinity,
  max: number = Infinity,
  step: number = 1
) => {
  const handleKeydown = (e: React.KeyboardEvent<HTMLInputElement>) => {
    if (e.key !== 'ArrowUp' && e.key !== 'ArrowDown') {
      return
    }
    e.preventDefault()

    const number = parseFloat(value as string)
    if (Number.isNaN(number)) {
      return
    }
    const direction = e.key === 'ArrowUp' ? 1 : -1

    const scaledStep = step * (e.shiftKey ? 5 : 1)
    const valueInSteps = (
      direction > 0
        ? Math.floor((number / scaledStep) + 0.01)
        : Math.ceil((number / scaledStep) - 0.01)
    )

    const newValue = Math.min(max, Math.max(min, (valueInSteps + direction) * scaledStep))

    const isPercent = typeof value === 'string' && value.endsWith('%')
    if (isPercent) {
      onChange(`${newValue.toFixed(derivePrecision(step))}%`)
    } else {
      onChange(newValue)
    }
  }

  return handleKeydown
}

export {
  useArrowKeyIncrement,
}
