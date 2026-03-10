// Helper for creating a singleton factory. This is not a memo. We might specialize its behavior
// later.
//
// @param create should be a function that returns an object.
const singleton = <T extends NonNullable<object>>(create: (...args: any[]) => T) => {
  let value: T | null = null

  return (...args: Parameters<typeof create>): T => {
    if (!value) {
      value = create(...args)
    }
    return value!
  }
}

const memo = <T extends any>(create: (...args: any[]) => T) => {
  let value: T | null = null
  let didCall = false

  return (...args: Parameters<typeof create>): T => {
    if (!didCall) {
      value = create(...args)
      didCall = true
    }
    return value!
  }
}

export {
  memo,
  singleton,
}
