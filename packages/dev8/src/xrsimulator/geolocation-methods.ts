import {DEFAULT_LAT, DEFAULT_LNG} from '../shared/ecs/shared/map-constants'

import type {SimulatorConfig} from './simulator-types'

type PositionCallback = Parameters<typeof navigator.geolocation.watchPosition>[0]
type PositionErrorCallback =
  NonNullable<Parameters<typeof navigator.geolocation.watchPosition>[1]>
type PositionOptions = Parameters<typeof navigator.geolocation.watchPosition>[2]
type GeolocationPositionError = Parameters<PositionErrorCallback>[0]
type GeolocationPosition = Parameters<PositionCallback>[0]

type PositionWatchData = {
  successCallback: PositionCallback
  errorCallback?: PositionErrorCallback | null
  options?: PositionOptions
}

const createGeolocationIntercept = (simulatorConfig: SimulatorConfig) => {
  if (!navigator.geolocation || !simulatorConfig) {
    return null
  }

  let positionWatchId = 1
  let currentSimulatorConfig = simulatorConfig

  const activePositionWatches: Map<number, PositionWatchData> = new Map()

  const dispatchGeolocationCallback = (
    successCallback: PositionCallback,
    errorCallback?: PositionErrorCallback | null
  ) => {
    if (currentSimulatorConfig?.mockCoordinateValue === 'none') {
      if (errorCallback) {
        const error: GeolocationPositionError = {
          code: 2,
          message: 'User denied Geolocation',
          PERMISSION_DENIED: 1,
          POSITION_UNAVAILABLE: 2,
          TIMEOUT: 3,
        }
        errorCallback(error)
      }
    } else {
      const mockPosition: GeolocationPosition = {
        coords: {
          latitude: currentSimulatorConfig?.mockLat || DEFAULT_LAT,
          longitude: currentSimulatorConfig?.mockLng || DEFAULT_LNG,
          altitude: null,
          accuracy: 5,
          altitudeAccuracy: null,
          heading: null,
          speed: null,
          toJSON: () => ({
            latitude: currentSimulatorConfig?.mockLat ?? DEFAULT_LAT,
            longitude: currentSimulatorConfig?.mockLng || DEFAULT_LNG,
          }),
        },
        timestamp: Date.now(),
        toJSON: () => (
          {
            coords:
            {
              latitude: currentSimulatorConfig?.mockLat || DEFAULT_LAT,
              longitude: currentSimulatorConfig?.mockLng || DEFAULT_LNG,
            },
          }),
      }
      successCallback(mockPosition)
    }
  }

  navigator.geolocation.watchPosition = (successCallback, errorCallback, options) => {
    activePositionWatches.set(
      positionWatchId,
      {successCallback, errorCallback, options}
    )
    dispatchGeolocationCallback(successCallback, errorCallback)
    return positionWatchId++
  }

  navigator.geolocation.clearWatch = (watchId) => {
    if (activePositionWatches.has(watchId)) {
      activePositionWatches.delete(watchId)
    }
  }

  navigator.geolocation.getCurrentPosition = (successCallback, errorCallback) => {
    dispatchGeolocationCallback(successCallback, errorCallback)
  }

  const updateWatchPositions = (newSimulatorConfig: SimulatorConfig) => {
    currentSimulatorConfig = newSimulatorConfig
    activePositionWatches.forEach((value) => {
      dispatchGeolocationCallback(
        value.successCallback,
        value.errorCallback
      )
    })
  }

  return {
    update: updateWatchPositions,
  }
}

export {
  createGeolocationIntercept,
}
