import type {Permission, PermissionsErrorEnvironment} from './types'

declare const XR8: any

type BrowserFamily = 'safari' | 'chrome' | 'other'
type PlatformFamily = 'ios' | 'android' | 'desktop' | 'other'

const platformFamily = (os?: string): PlatformFamily => {
  if (os === 'iOS') {
    return 'ios'
  }
  if (os === 'Android') {
    return 'android'
  }
  if (os && /Mac|Windows|Linux|Chrome OS/i.test(os)) {
    return 'desktop'
  }
  return 'other'
}

const browserFamily = (name?: string): BrowserFamily => {
  if (name && /Safari/i.test(name)) {
    return 'safari'
  }
  if (name && /Chrome|Chromium|Samsung|Edge/i.test(name)) {
    return 'chrome'
  }
  return 'other'
}

const requestedPermissions = (): Permission[] => {
  const result = new Set<Permission>(['camera'])
  const permissions = XR8.XrPermissions.permissions()
  const required = XR8.requiredPermissions()
  if (required.has(permissions.MICROPHONE)) {
    result.add('microphone')
  }
  if (required.has(permissions.DEVICE_MOTION) || required.has(permissions.DEVICE_ORIENTATION)) {
    result.add('motion')
  }
  return [...result]
}

const permissionFromEngine = (permission?: string): Permission | undefined => {
  if (!permission) {
    return undefined
  }
  const permissions = XR8.XrPermissions.permissions()
  if (permission === permissions.CAMERA) {
    return 'camera'
  }
  if (permission === permissions.MICROPHONE) {
    return 'microphone'
  }
  if (permission === permissions.DEVICE_MOTION || permission === permissions.DEVICE_ORIENTATION) {
    return 'motion'
  }
  return undefined
}

const permissionFromFailureReason = (reason?: string): Permission | undefined => {
  if (reason?.includes('MICROPHONE')) {
    return 'microphone'
  }
  if (reason?.includes('CAMERA')) {
    return 'camera'
  }
  return undefined
}

const currentPermissionsErrorEnvironment = (): PermissionsErrorEnvironment => {
  const device = XR8.XrDevice.deviceEstimate()
  const platform = platformFamily(device.os)
  const browser = browserFamily(device.browser?.name)
  if (platform === 'ios' && (browser === 'safari' || browser === 'chrome')) {
    return `ios-${browser}`
  }
  if (platform === 'android' && browser === 'chrome') {
    return 'android-chrome'
  }
  if (platform === 'desktop' && (browser === 'safari' || browser === 'chrome')) {
    return `desktop-${browser}`
  }
  return 'other'
}

type CompatibilityScreen = 'in-app-browser' | 'unsupported-browser' | 'unsupported-device'

const compatibilityScreen = (runConfig?: unknown): CompatibilityScreen | undefined => {
  if (XR8.XrDevice.isDeviceBrowserCompatible(runConfig)) {
    return undefined
  }

  const reasons = XR8.XrDevice.incompatibleReasons(runConfig)
  const incompatibilityReasons = XR8.XrDevice.IncompatibilityReasons
  if (reasons.includes(incompatibilityReasons.UNSUPPORTED_OS)) {
    return 'unsupported-device'
  }

  const device = XR8.XrDevice.deviceEstimate()
  const details = XR8.XrDevice.incompatibleReasonDetails(runConfig)
  const inAppBrowser = details.inAppBrowser || details.inAppBrowserType ||
    device.browser?.inAppBrowser

  return inAppBrowser ? 'in-app-browser' : 'unsupported-browser'
}

export {
  compatibilityScreen,
  currentPermissionsErrorEnvironment,
  permissionFromEngine,
  permissionFromFailureReason,
  requestedPermissions,
}
