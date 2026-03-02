// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Christoph Bartschat (christoph@8thwall.com)
//
// Enables and disables a double-buffered requestAnimationFrame, while preserving
// cancelAnimationFrame functionality
//

// Maps RAF ID of first RAF to second RAF ID.
// Used to cancel the second RAF as well when the first is cancelled
const idMap = new Map()

const nativeRAF = window.requestAnimationFrame
const nativeCancel = window.cancelAnimationFrame

const doubleRAF = (fn) => {
  const firstId = nativeRAF(() => {
    const secondId = nativeRAF((timestamp) => {
      idMap.delete(firstId)
      fn(timestamp)
    })
    idMap.set(firstId, secondId)
  })
  return firstId
}

const wrappedCancel = (id) => {
  nativeCancel(id)
  const secondId = idMap.get(id)
  if (secondId) {
    nativeCancel(secondId)
    idMap.delete(id)
  }
}

const enableDoubleRAF = () => {
  window.requestAnimationFrame = doubleRAF
  window.cancelAnimationFrame = wrappedCancel
}

const disableDoubleRAF = () => {
  window.requestAnimationFrame = nativeRAF
  // We can't reset cancelAnimationFrame because we still could have pending doubleRAFs that may
  // still need to be cancelled
}

export {
  enableDoubleRAF,
  disableDoubleRAF,
}
