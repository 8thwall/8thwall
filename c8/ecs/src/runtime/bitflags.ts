const DEFER_GEOMETRY_FLAG = 1
const DEFER_MATERIAL_FLAG = 2

// Functions for manipulating bit flags up to 32 bits.
/* eslint-disable no-bitwise */
const setBitFlag = (flag: number): number => ((1 << flag) | 0) >>> 0

const enableBitFlag = (mask: number, flag: number): number => mask | setBitFlag(flag)

const disableBitFlag = (mask: number, flag: number): number => mask & ~setBitFlag(flag)

const isBitFlagEnabled = (mask: number, flag: number): boolean => (mask & setBitFlag(flag)) !== 0
/* eslint-enable no-bitwise */

export {
  setBitFlag,
  enableBitFlag,
  disableBitFlag,
  isBitFlagEnabled,
  DEFER_GEOMETRY_FLAG,
  DEFER_MATERIAL_FLAG,
}
