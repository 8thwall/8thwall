import {singleton} from './factory'

const permissionsEnum = {
  CAMERA: 'camera',
  DEVICE_MOTION: 'devicemotion',
  DEVICE_ORIENTATION: 'deviceorientation',
  DEVICE_GPS: 'geolocation',
  MICROPHONE: 'microphone',
} as const

const XrPermissionsFactory = singleton(() => {
  const permissions = () => ({...permissionsEnum})

  return {
    // Adds a pipeline module that displays an error image and stops the camera
    // feed when an error is encountered after the camera starts running.
    permissions,
  }
})

export {
  XrPermissionsFactory,
}
