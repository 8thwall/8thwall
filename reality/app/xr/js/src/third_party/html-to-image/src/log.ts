const logError = (options, ...args) => {
  if (!options.suppressErrors) {
    // eslint-disable-next-line no-console
    console.error(...args)
  }
}

export {
  logError,
}
