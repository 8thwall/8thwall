let resourceBaseUrl = ''

const getResourceBase = () => {
  if (!resourceBaseUrl) {
    throw new Error('Resource base URL has not been set.')
  }
  return resourceBaseUrl
}

const setResourceBase = (url: string) => {
  resourceBaseUrl = url
}

const recordCurrentScriptBase = () => {
  const currentScriptUrl = (document.currentScript as HTMLScriptElement).src
  setResourceBase(new URL('./resources/', currentScriptUrl).toString())
}

export {getResourceBase, setResourceBase, recordCurrentScriptBase}
