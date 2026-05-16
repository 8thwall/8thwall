import {XrHudManager} from './xrhud/xr-hud-manager'
import {XrSimulatorManager} from './xrsimulator/xr-simulator'
import {broadcastSequenceProgress} from './xrsimulator/broadcast-messages'
import {createStudioDebugManager} from './studio-debug'
import {createStudioEventStreamManager} from './studio-event-stream'
import {loadParameters} from './parameters'
import {getUniqueTimestamp} from './unique-timestamp'
import {captureLogs} from './capture-logs'
import {handleSimulatorEvents, simulatorEnabledForConfig} from './simulator-events'

declare global {
  interface Window {
    XR8: any
    THREE: any
    AFRAME: any
    DEV_8W_NO_BUILD_RELOAD: boolean
  }
  interface Element {
    object3D: any
    data: any
  }
}

const {
  ua, debugFlag, debugHudKey, deviceId, sessionId, simulatorConfig,
  simulatorRendererConfig, simulatorId, originalUrl, webSocketUrl,
} = loadParameters()

const studioEventStream = createStudioEventStreamManager(webSocketUrl)
const studioDebug = createStudioDebugManager(
  sessionId, ua, simulatorId, studioEventStream
)

studioEventStream.send({
  action: 'SESSION_START',
  deviceId,
  sessionId,
  timestamp: getUniqueTimestamp(),
})

const injectGoogleNoTranslateRule = () => {
  const meta = document.createElement('meta')
  meta.setAttribute('name', 'google')
  meta.content = 'notranslate'
  document.getElementsByTagName('head')[0].appendChild(meta)
}

let xrHud: ReturnType<typeof XrHudManager>
let xrSimulator: ReturnType<typeof XrSimulatorManager>

const broadcastInitialDebugStatus = () => {
  const screenHeight = window.screen.height * window.devicePixelRatio
  const screenWidth = window.screen.width * window.devicePixelRatio
  const status = debugFlag

  studioEventStream.send({
    action: 'INITIAL_DEBUG_HUD_STATUS',
    deviceId,
    sessionId,
    status,
    screenHeight,
    screenWidth,
    ua,
    simulatorId,
  })
}

const broadcastSetDebugStatus = (status: boolean) => {
  localStorage.setItem(debugHudKey, status.toString())
  studioEventStream.send({
    action: 'SET_DEBUG_HUD_STATUS',
    deviceId,
    sessionId,
    status,
    simulatorId,
  })
}

studioEventStream.listen((msg) => {
  if (msg.action === 'EVAL') {
    // eslint-disable-next-line no-eval, no-console
    console.log(eval(msg.cmd))
  } else if (msg.action === 'DEBUG_HUD') {
    if (xrHud) {
      if (msg.enable) {
        xrHud.enable({console: true, version: true, verbose: true})
      } else {
        xrHud.disable()
      }
    }
  }
})

const initialSetup = () => {
  xrHud = XrHudManager()
  xrSimulator = XrSimulatorManager()
  broadcastInitialDebugStatus()
  if (debugFlag) {
    xrHud.enable({console: true, version: true, verbose: true})
  }
  if (simulatorEnabledForConfig(simulatorConfig)) {
    xrSimulator.enable(simulatorConfig, simulatorRendererConfig, broadcastSequenceProgress)
  }
  xrHud.onDisable(() => broadcastSetDebugStatus(false))
  xrHud.onEnable(() => broadcastSetDebugStatus(true))
  studioDebug.ready()
  handleSimulatorEvents(xrSimulator, simulatorConfig, simulatorRendererConfig, originalUrl)
}
const state = document.readyState
if (state === 'interactive' || state === 'complete') {
  initialSetup()
} else {
  document.addEventListener('DOMContentLoaded', initialSetup)
}

window.addEventListener('beforeunload', () => {
  studioDebug.close()
})

captureLogs(studioEventStream, xrHud, {
  simulatorId,
  sessionId,
  deviceId,
  ua,
})

injectGoogleNoTranslateRule()
