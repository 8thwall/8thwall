const setDeprecatedProperty = (object, property, value, message) => {
  let warned = false

  Object.defineProperty(object, property, {
    get: () => {
      if (!warned) {
        warned = true
        /* eslint-disable no-console */
        console.warn('[XR] Deprecation Warning:', message)
        console.warn(Error().stack.replace(/^Error.*\n\s*/, '').replace(/\n\s+/g, '\n'))
        /* eslint-enable no-console */
      }
      return value
    },
  })
}

const setRenameDeprecation = (object, oldName, newName, value, version = 'R9.1') => {
  const message = `XR8.${oldName} was deprecated in ${version}. Use ${newName} instead.`
  setDeprecatedProperty(object, oldName, value, message)
}

const setRemovalDeprecation = (object, name, value, replacement = null, version = 'R9.1') => {
  let message = `XR8.${name} was deprecated in ${version} and will be removed in the future.`
  if (replacement) {
    message += ` Equivalent functionality is provided by ${replacement}.`
  }
  setDeprecatedProperty(object, name, value, message)
}

const setNoopDeprecation = (object, name, version = 'R9.1') => {
  const message = `XR8.${name}() has been removed in ${version} and no longer has any effect.`
  setDeprecatedProperty(object, name, () => {}, message)
}

export {
  setDeprecatedProperty,
  setRenameDeprecation,
  setRemovalDeprecation,
  setNoopDeprecation,
}
