let previousTimestamp = 0
const getUniqueTimestamp = () => {
  let timestamp = Date.now()
  if (timestamp <= previousTimestamp) {
    timestamp = previousTimestamp + 1
  }

  previousTimestamp = timestamp
  return timestamp
}

export {
  getUniqueTimestamp,
}
