import type {XrCameraType, DeviceSupportType} from './scene-graph'

type DeviceConfigType = {
  camera: XrCameraType
  phone: DeviceSupportType
  desktop: DeviceSupportType
  headset: DeviceSupportType
}

type DeviceSupportTypes = {
  phone: DeviceSupportType
  desktop: DeviceSupportType
  headset: DeviceSupportType
}

const defaultDeviceSupportForCamera: Record<XrCameraType, DeviceSupportTypes> = {
  '3dOnly': {phone: '3D', desktop: '3D', headset: 'disabled'},
  'world': {phone: 'AR', desktop: 'disabled', headset: 'disabled'},
  'face': {phone: 'AR', desktop: 'AR', headset: 'disabled'},
  // Note(Julie): The following are not yet supported
  'hand': {phone: 'disabled', desktop: 'disabled', headset: 'disabled'},
  'layers': {phone: 'disabled', desktop: 'disabled', headset: 'disabled'},
  'worldLayers': {phone: 'disabled', desktop: 'disabled', headset: 'disabled'},
}

const ValidConfigs: DeviceConfigType[] = [
  {camera: '3dOnly', phone: '3D', desktop: '3D', headset: 'disabled'},
  {camera: 'world', phone: 'AR', desktop: '3D', headset: 'AR'},
  {camera: 'world', phone: 'AR', desktop: 'disabled', headset: 'AR'},
  {camera: 'world', phone: 'AR', desktop: 'disabled', headset: 'disabled'},
  {camera: 'face', phone: 'AR', desktop: 'AR', headset: 'disabled'},
  {camera: 'face', phone: 'AR', desktop: 'disabled', headset: 'disabled'},
]

const UnsupportedConfigs: DeviceConfigType[] = [
  {camera: 'world', phone: 'AR', desktop: '3D', headset: 'disabled'},
  {camera: 'world', phone: '3D', desktop: '3D', headset: 'VR'},
  {camera: 'world', phone: '3D', desktop: '3D', headset: 'disabled'},
  {camera: 'world', phone: '3D', desktop: 'disabled', headset: 'AR'},
  {camera: 'world', phone: '3D', desktop: 'disabled', headset: 'VR'},
  {camera: 'world', phone: '3D', desktop: 'disabled', headset: 'disabled'},
  {camera: 'world', phone: 'disabled', desktop: '3D', headset: 'AR'},
  {camera: 'world', phone: 'disabled', desktop: '3D', headset: 'VR'},
  {camera: 'world', phone: 'disabled', desktop: '3D', headset: 'disabled'},
  {camera: 'world', phone: 'disabled', desktop: 'disabled', headset: 'AR'},
  {camera: 'world', phone: 'disabled', desktop: 'disabled', headset: 'VR'},
  {camera: 'world', phone: 'disabled', desktop: 'disabled', headset: 'disabled'},
  {camera: 'face', phone: 'disabled', desktop: 'disabled', headset: 'disabled'},
  {camera: '3dOnly', phone: '3D', desktop: 'disabled', headset: 'disabled'},
  {camera: '3dOnly', phone: 'disabled', desktop: '3D', headset: 'disabled'},
  {camera: '3dOnly', phone: 'disabled', desktop: 'disabled', headset: 'disabled'},
]

const checkDeviceConfig = (currentConfig: DeviceConfigType):
  'valid' | 'not-ready' | 'not-supported' => {
  if (ValidConfigs.some(config => config.camera === currentConfig.camera &&
    config.phone === currentConfig.phone &&
    config.desktop === currentConfig.desktop &&
    config.headset === currentConfig.headset)) {
    return 'valid'
  }
  if (UnsupportedConfigs.some(config => config.camera === currentConfig.camera &&
    config.phone === currentConfig.phone &&
    config.desktop === currentConfig.desktop &&
    config.headset === currentConfig.headset)) {
    return 'not-supported'
  }
  return 'not-ready'
}

const getAllowedXrDevices = (newConfig: DeviceConfigType) => {
  if (newConfig.camera === 'world') {
    if (newConfig.desktop === 'disabled') {
      if (newConfig.headset === 'VR' || newConfig.headset === 'AR') {
        return 'mobile-and-headsets'
      } else {
        return 'mobile'
      }
    } else {
      return 'any'
    }
  } else if (newConfig.camera === 'face') {
    if (newConfig.desktop === 'disabled') {
      return 'mobile'
    }
  }
  return 'any'
}

const getDefaultDeviceSupport = (cameraType: XrCameraType): DeviceSupportTypes => (
  defaultDeviceSupportForCamera[cameraType] ?? {} as DeviceSupportTypes
)

export type {
  DeviceConfigType,
}

export {
  checkDeviceConfig,
  getAllowedXrDevices,
  getDefaultDeviceSupport,
}
