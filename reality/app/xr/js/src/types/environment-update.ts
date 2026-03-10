import type {DeviceInfo} from './device'

interface EnvironmentUpdate {
  videoWidth?: number
  videoHeight?: number
  canvasWidth?: number
  canvasHeight?: number
  orientation?: number
  deviceEstimate?: DeviceInfo
}

export type {
  EnvironmentUpdate,
}
