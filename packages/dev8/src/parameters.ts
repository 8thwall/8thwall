import type {SimulatorConfig} from './xrsimulator/simulator-types'

const DEBUG_PARAM = 'd'
const SIMULATOR_PARAM = 'simulatorConfig'
const SESSION_PARAM = 'sessionId'
const DEBUG_KEY_PREFIX = '8w.debug-mode/'
const KEY = 'hot-reload-deviceid'

// When we're unloading, store the session ID so it can be reused when the page is reloaded.
// If two tabs are loaded in the same browser, they will not get the same session ID because
// it gets removed from the list while the page is open. Only between refreshes, or after closing
// the tab does it get added to the pool. Technically this could cause two tabs in the same browser
// to swap session IDs if they refresh at the exact same time, but not the worst thing ever,
// if it does happen.
const SESSION_POOL_KEY = 'session-id-pool'

const loadParameters = () => {
  /* Get a random persistent nonce to identify this browser */
  let deviceId = localStorage.getItem(KEY)
  if (deviceId === null) {
    deviceId = Math.random().toString(36).substring(2, 15)
    localStorage.setItem(KEY, deviceId)
  }

  const originalUrl = window.location.href

  // Scope local storage keys by appkey
  const debugHudKey = DEBUG_KEY_PREFIX + deviceId
  const params = new URLSearchParams(window.location.search)

  const simulatorConfigParam = params.get(SIMULATOR_PARAM)
  const simulatorConfig: SimulatorConfig | null = simulatorConfigParam
    ? {
      ...JSON.parse(simulatorConfigParam),
      // NOTE(juliesoohoo) The simulatorConfig version after a first load or reload is always 0 to
      // ensure any updates requested afterwards are applied, since the renderer config cannot
      // be passed in the URL params
      version: 0,
    }
    : null

  const simulatorRendererConfig = {version: 0}

  const ua = simulatorConfig ? 'Simulator' : window.navigator.userAgent

  // For simulator sessions, this will always be provided in the params.
  let sessionId = params.get(SESSION_PARAM)
  const sessionDerivedFromUrl = !!sessionId
  if (!sessionId) {
    const availableSessionIds = localStorage.getItem(SESSION_POOL_KEY)
    if (availableSessionIds) {
      const [firstSessionId, ...otherSessionIds] = availableSessionIds.split(',')
      sessionId = firstSessionId
      localStorage.setItem(SESSION_POOL_KEY, otherSessionIds.join(','))
    } else {
      sessionId = Array.from({length: 8})
        .map(() => Math.floor(Math.random() * 36).toString(36))
        .join('')
    }
  }

  const webSocketUrl = params.get('liveSyncMode') === 'inline'
    ? null
    : `/dev8?${new URLSearchParams({
      sessionId,
    })}`

  let debugFlagParam
  if (params.has(DEBUG_PARAM)) {
  // This is present as ?d=true if the HUD is enabled.
    debugFlagParam = params.get(DEBUG_PARAM)
    localStorage.setItem(debugHudKey, debugFlagParam)
  } else {
    debugFlagParam = localStorage.getItem(debugHudKey)
  }
  const debugFlag = debugFlagParam === 'true'

  // Clear the parameters so that users don't see it in their url
  const paramsToClear = [DEBUG_PARAM]
  if (paramsToClear.some(p => params.has(p))) {
    const replacedUrl = new URL(originalUrl)
    paramsToClear.forEach(p => replacedUrl.searchParams.delete(p))
    window.history.replaceState(null, '', replacedUrl.toString())
  }

  window.addEventListener('beforeunload', () => {
    if (sessionDerivedFromUrl) {
      return
    }
    localStorage.setItem(SESSION_POOL_KEY, [
      sessionId,
      localStorage.getItem(SESSION_POOL_KEY),
    ].filter(Boolean).join(','))
  })

  return {
    webSocketUrl,
    ua,
    deviceId,
    sessionId,
    simulatorConfig,
    simulatorRendererConfig,
    debugHudKey,
    debugFlag,
    simulatorId: simulatorConfig?.simulatorId,
    originalUrl,
  }
}

const saveSimulatorConfig = (config: SimulatorConfig) => {
  const updatedUrl = new URL(window.location.href)
  updatedUrl.searchParams.set(SIMULATOR_PARAM, JSON.stringify(config))
  window.history.replaceState(null, '', updatedUrl.toString())
}

export {
  loadParameters,
  saveSimulatorConfig,
}
