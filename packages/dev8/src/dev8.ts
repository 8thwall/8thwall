import {XrHudManager} from './xrhud/xr-hud-manager'
import {XrSimulatorManager} from './xrsimulator/xr-simulator'
import {
  broadcastSequenceProgress, broadcastReloadConfirmation,
} from './xrsimulator/broadcast-messages'
import {createStudioDebugManager} from './studio-debug'
import {createStudioEventStreamManager} from './studio-event-stream'
import type {SimulatorConfig} from './xrsimulator/simulator-types'
import {loadParameters} from './parameters'
import {getUniqueTimestamp} from './unique-timestamp'
import {captureLogs} from './capture-logs'

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
  simulatorRendererConfig, simulatorId, originalUrl, channel,
} = loadParameters()

const webSocketUrl = channel && `${channel}?${new URLSearchParams({
  sessionId,
  deviceId,
  ua,
})}`

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

const needsUpdate = (config: SimulatorConfig): boolean => (
  !!config?.cameraUrl || !!config?.poiId || !!config?.mockLat ||
  !!config?.mockLng || !!config?.mockCoordinateValue
)

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
const reloadSimulator = (currentSimulatorConfig = simulatorConfig) => {
  const url = new URL(originalUrl)
  url.searchParams.set('simulatorConfig', JSON.stringify(currentSimulatorConfig))
  window.location.href = url.toString()
  broadcastReloadConfirmation(simulatorId)
}

const simulatorIdMatches = (incomingSimulatorId: string) => (
  incomingSimulatorId && incomingSimulatorId === simulatorId
)

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
  if (needsUpdate(simulatorConfig)) {
    xrSimulator.enable(simulatorConfig, simulatorRendererConfig, broadcastSequenceProgress)
  }
  xrHud.onDisable(() => broadcastSetDebugStatus(false))
  xrHud.onEnable(() => broadcastSetDebugStatus(true))
  studioDebug.ready()
}
const state = document.readyState
if (state === 'interactive' || state === 'complete') {
  initialSetup()
} else {
  document.addEventListener('DOMContentLoaded', initialSetup)
}

window.addEventListener('message', (event) => {
  if (xrSimulator) {
    let config
    switch (event.data.action) {
      case 'SIMULATOR_CONFIG_UPDATE':
        config = event.data.data.simulatorConfig
        if (simulatorIdMatches(config?.simulatorId)) {
          if (needsUpdate(config)) {
            // Fire a Location Lost event when switching to a different location so that the
            // simulator stops rendering the objects of the previously selected location.
            if (config?.poiId !== simulatorConfig.poiId) {
              xrSimulator.dispatchLocationLost()
            }
            Object.assign(simulatorConfig, config)
            Object.assign(simulatorRendererConfig, event.data.data.simulatorRendererConfig)
            xrSimulator.updateSimulator(simulatorConfig, simulatorRendererConfig)
          } else {
            xrSimulator.disable()
          }
        }
        break
      case 'SIMULATOR_RELOAD':
        config = event.data.data.simulatorConfig
        if (simulatorIdMatches(config?.simulatorId)) {
          reloadSimulator(config)
        }
        break
      case 'SIMULATOR_SCRUB':
        if (event.data.data) {
          const {progress} = event.data.data
          if (progress) {
            xrSimulator.scrub(progress)
          }
        }
        break
      case 'SIMULATOR_STOP_SCRUB':
        xrSimulator.stopScrub()
        break
      case 'SIMULATOR_RECENTER':
        window.XR8.XrController.recenter()
        break
      case 'SIMULATOR_DISPATCH_LOCATIONSCANNING':
        xrSimulator.dispatchLocationScanning()
        break
      case 'SIMULATOR_DISPATCH_LOCATIONFOUND':
        xrSimulator.dispatchLocationFound()
        break
      case 'SIMULATOR_DISPATCH_LOCATIONLOST':
        xrSimulator.dispatchLocationLost()
        break
      case 'SIMULATOR_DISPATCH_MESHFOUND':
        xrSimulator.dispatchMeshFound()
        break
      case 'SIMULATOR_DISPATCH_MESHLOST':
        xrSimulator.dispatchMeshLost()
        break
      case 'SIMULATOR_GPS8W':
        window.dispatchEvent(new CustomEvent('gps8w', {detail: event.data.gps8w}))
        break
      default:
        break
    }
  }
})

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
