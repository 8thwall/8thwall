const ELECTRON_PROTOCOL = 'electron:'

// NOTE(johnny): This is used to create a url that will open in a new electron app window.
const createElectronUrl = (path: string, queryParams: Record<string, string> = {}): string => {
  const cleanPath = path.replace(/^https:\/\//, '')
  const url = new URL(`${ELECTRON_PROTOCOL}//${cleanPath}`)
  Object.entries(queryParams).forEach(([key, value]) => {
    url.searchParams.append(key, value)
  })
  return url.toString()
}

export {
  ELECTRON_PROTOCOL,
  createElectronUrl,
}
