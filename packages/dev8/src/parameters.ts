import type {SimulatorConfig} from './xrsimulator/simulator-types'

const DEBUG_PARAM = 'd'
const SIMULATOR_PARAM = 'simulatorConfig'
const SESSION_PARAM = 'sessionId'
const DEBUG_KEY_PREFIX = '8w.debug-mode/'
const CHANNEL_PARAM = 'channel'
const KEY = 'hot-reload-deviceid'

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
  const channel = params.get(CHANNEL_PARAM)

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
  if (!sessionId) {
    sessionId = Array.from({length: 8})
      .map(() => Math.floor(Math.random() * 36).toString(36))
      .join('')
    params.set(SESSION_PARAM, sessionId)
    window.history.replaceState(null, '', `${window.location.pathname}?${params}`)
  }

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
  const paramsToClear = [DEBUG_PARAM, SIMULATOR_PARAM, CHANNEL_PARAM]
  if (paramsToClear.some(p => params.has(p))) {
    const replacedUrl = new URL(originalUrl)
    paramsToClear.forEach(p => replacedUrl.searchParams.delete(p))
    window.history.replaceState(null, '', replacedUrl.toString())
  }

  return {
    channel,
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

export {
  loadParameters,
}
