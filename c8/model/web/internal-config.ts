const CONFIG = {
  workerUrl: '',
}

const setInternalConfig = (newConfig) => {
  if (newConfig.workerUrl !== undefined) {
    CONFIG.workerUrl = newConfig.workerUrl
  }
}

const internalConfig = () => ({
  workerUrl: CONFIG.workerUrl,
})

export {
  internalConfig,
  setInternalConfig,
}
