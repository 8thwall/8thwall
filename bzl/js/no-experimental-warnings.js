const originalEmit = process.emit
process.emit = function emit(event, error, ...args) {
  if (event === 'warning' && error.name === 'ExperimentalWarning') {
    return false
  }
  return originalEmit.apply(process, [event, error, ...args])
}
