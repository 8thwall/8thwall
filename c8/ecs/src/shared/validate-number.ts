/**
 * Validates a value and returns the numeric representation of the value.
 *
 * @param value The value to validate, can be either a string or number
 * @returns The validated (numeric) value. Returns 0 if the input string cannot be converted to a
 * number.
 * @example
 * validateNumber(50)    // returns 50
 * validateNumber(50.5)  // returns 50.5
 * validateNumber("50")  // returns 50
 * validateNumber("50.5") // returns 50.5
 * validateNumber("abc") // returns 0
 */
const validateNumber = (value: string | number): number => {
  if (typeof value === 'number') {
    return value
  }

  const num = Number(value)
  return Number.isNaN(num) ? 0 : num
}

export {validateNumber}
