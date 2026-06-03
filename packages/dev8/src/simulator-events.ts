import type {XrSimulatorManager} from './xrsimulator/xr-simulator'
import type {SimulatorConfig, SimulatorRendererConfig} from './xrsimulator/simulator-types'
import {broadcastReloadConfirmation} from './xrsimulator/broadcast-messages'
import {saveSimulatorConfig} from './parameters'

const simulatorEnabledForConfig = (config: SimulatorConfig): boolean => (
  !!config?.cameraUrl || !!config?.poiId || !!config?.mockLat ||
  !!config?.mockLng || !!config?.mockCoordinateValue
)
const handleSimulatorEvents = (
  xrSimulator: ReturnType<typeof XrSimulatorManager>,
  simulatorConfig: SimulatorConfig,
  simulatorRendererConfig: SimulatorRendererConfig,
  originalUrl: string
) => {
  const simulatorIdMatches = (simulatorId: string) => (
    simulatorId && simulatorId === simulatorConfig?.simulatorId
  )

  const reloadSimulator = (currentSimulatorConfig = simulatorConfig) => {
    const url = new URL(originalUrl)
    url.searchParams.set('simulatorConfig', JSON.stringify(currentSimulatorConfig))
    window.location.href = url.toString()
    broadcastReloadConfirmation(simulatorConfig.simulatorId)
  }

  window.addEventListener('message', (event) => {
    if (xrSimulator) {
      let config
      switch (event.data.action) {
        case 'SIMULATOR_CONFIG_UPDATE':
          config = event.data.data.simulatorConfig
          if (simulatorIdMatches(config?.simulatorId)) {
            saveSimulatorConfig(config)
            if (simulatorEnabledForConfig(config)) {
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
}

export {
  handleSimulatorEvents,
  simulatorEnabledForConfig,
}
