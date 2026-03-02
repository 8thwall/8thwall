interface BrowserInfo {
  name: string  // same as fullName but "Mobile [Chrome,Firefox]" => "[Chrome,Firefox]"
  fullName: string  // browser name as returned by ua-parser-js
  version: string
  majorVersion: number
  inAppBrowser: string
}
interface DeviceInfo {
  locale: string
  os: string
  osVersion: string
  manufacturer: string
  model: string
  browser: BrowserInfo
  type: string  // 'console' | 'mobile' | 'tablet' | 'smarttv' | 'wearable' | 'embedded' | undefined
}

interface DeviceFeatures {
  hasSimd: boolean
  hasThreads: boolean
  hasGetUserMedia: boolean
  hasDeviceOrientation: boolean
}

interface FullDeviceInfo extends DeviceInfo {
  // Extra information only available with new deviceInfo API
  features: DeviceFeatures
}

interface Compatibilities {
  os?: string
  hasDevice?: boolean
  hasBrowser?: boolean
  hasUserMedia?: boolean
  hasWebAssembly?: boolean
  hasDeviceOrientation?: boolean
  inAppBrowser?: string
}

export type {
  DeviceInfo,
  FullDeviceInfo,
  Compatibilities,
}
