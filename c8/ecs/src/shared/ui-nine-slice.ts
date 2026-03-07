/**
 *  Validates a string and checks if it is a numerical or percentage value.
 *
 *
 * @param value The value to validate, can be either a string or number
 * @param textureSize The size of the side of the texture being compared to
 * @returns The validated (numeric) value. Returns 0 if the input string cannot be converted to a
 * number.
 * @example
 * parseNineSliceValue('50%;, 200)    // returns 100
 * parseNineSliceValue('50', 200)  // returns 50
 * parseNineSliceValue(undefined, 100)  // returns 0
 * parseNineSliceValue(50, 200) // returns 50
 */
const parseNineSliceValue = (value: string, textureSize: number): number => {
  if (value === undefined) return 0

  if (typeof value === 'number') return value

  if (value.endsWith('%')) return (parseFloat(value) / 100.0) * textureSize

  return parseFloat(value)
}

export {parseNineSliceValue}
