const derivePrecision = (step: number) => {
  const precision = Math.max(0, -Math.floor(Math.log10(step)))
  return precision
}

const MIN_FIXED_PRECISION = 3

const derivePreciseEditValue = (value: number, step: number, fixed?: number): string => {
  const derivedFixed = Math.max(MIN_FIXED_PRECISION, derivePrecision(step) + 2)
  return value.toFixed(fixed ?? derivedFixed).replace(/\.?0*$/, '')
}

export {
  derivePrecision,
  derivePreciseEditValue,
}
