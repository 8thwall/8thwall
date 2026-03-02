// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)
import type {DeviceInfo as DeviceInfoCapnp} from 'reality/engine/api/device/info.capnp'

import {XrConfigFactory} from './config'
import {singleton} from './factory'
import type {Compatibility} from './types/common'
import {
  getDeviceInfo, deviceCompatibility, getLocale, getFixedUAResult, getDeviceInfoBytes,
  getFixedUAResultWithClientHints, hasWasmSimd, hasWasm, hasWasmThreads, UaResult,
  hasDeviceOrientation, hasGetUserMedia,
} from './devices/compatibility'
import {getFingerprint} from './devices/fingerprint'
import {getRendererAsync} from './devices/override'
import {getSupportedHeadsetSessionType} from './webxr-compatibility'
import type {DeviceInfo, FullDeviceInfo} from './types/device'

// Force XrDevice.isDeviceBrowserCompatible() to return false.
let incompatibilityOverride_ = false

const setIncompatibilityOverride = (incompatible) => {
  incompatibilityOverride_ = !!incompatible
}

// isSupportedHeadset will usually be set to "false" immediately after, but in some cases it will
// need a promise to check its value, and then it will be set to true or false depending on the
// outcome of the promise. We leave the default as null as a way to signal whether the promise has
// completed.
let isSupportedHeadset: boolean | null = null

// start checking headset support right away.
getFixedUAResultWithClientHints().then(async (uaResult: UaResult) => {
  const supportedType = await getSupportedHeadsetSessionType(uaResult.browser?.name,
    uaResult.device?.model)
  isSupportedHeadset = !!supportedType
})

interface Compatibilities {
  os?: string
  hasDevice?: boolean
  hasBrowser?: boolean
  hasUserMedia?: boolean
  hasWebAssembly?: boolean
  hasDeviceOrientation?: boolean
  inAppBrowser?: string
}

// Utilities for querying estimated properties of the user's device. These are primarily used by
// the XR system to estimate parameters of the device's camera, e.g. the camera focal length.
const XrDeviceFactory = singleton(() => {
  const IncompatibilityReasons = {
    UNSPECIFIED: 0,
    UNSUPPORTED_OS: 1,
    UNSUPPORTED_BROWSER: 2,
    MISSING_DEVICE_ORIENTATION: 3,
    MISSING_USER_MEDIA: 4,
    MISSING_WEB_ASSEMBLY: 5,
  }

  // Before ua-parser-js 2.0.0, there is no Mobile version of Chrome and Firefox
  // This reserves our compatibility if user code uses Chrome/Firefox
  const noMobileName = (name: string): string => {
    if (name === 'Mobile Chrome') {
      return 'Chrome'
    }
    if (name === 'Mobile Firefox') {
      return 'Firefox'
    }
    return name
  }

  const combineInfoAndResult = (info: DeviceInfoCapnp, result: UaResult): DeviceInfo => {
    const browser = result.browser && {
      name: noMobileName(result.browser.name),
      fullName: result.browser.name,
      version: result.browser.version,
      majorVersion: (parseInt && parseInt(result.browser.major, 10)) || 0,
      inAppBrowser: result.inAppBrowser,
    }

    return {
      locale: getLocale(),
      os: info.getOs(),
      osVersion: info.getOsVersion(),
      manufacturer: info.getManufacturer(),
      model: info.getModel(),
      browser,
      type: result.device?.type,
    }
  }

  const guessDeviceInternal = (): DeviceInfo => {
    const uaResult = getFixedUAResult()
    return combineInfoAndResult(getDeviceInfo(uaResult), uaResult)
  }

  const guessDeviceInfo = async (): Promise<FullDeviceInfo> => {
    const uaResult = await getFixedUAResultWithClientHints()
    const deviceInfo = getDeviceInfo(uaResult, {extraRendererInfo: await getRendererAsync()})
    return {
      ...combineInfoAndResult(deviceInfo, uaResult),
      features: {
        hasSimd: hasWasm() && hasWasmSimd(),
        hasThreads: hasWasm() && hasWasmThreads(),
        hasDeviceOrientation: hasDeviceOrientation(),
        hasGetUserMedia: hasGetUserMedia(),
      },
    }
  }

  const deviceEstimate = singleton(guessDeviceInternal)
  const deviceInfo = singleton(guessDeviceInfo)

  const getCompatibilitiesInternal = (): Compatibilities => {
    const info = getDeviceInfo(getFixedUAResult())
    const compat = deviceCompatibility(info)
    const osVersion = info.getOsVersion()

    const hasWebAssembly = !!(compat.has_web_assembly) &&
                           !(compat.os === 'iOS' && osVersion.startsWith('11.2'))

    return {
      os: compat.os || '',
      inAppBrowser: compat.browser || '',
      hasDevice: !!(compat.has_device),
      hasBrowser: !!(compat.has_browser),
      hasUserMedia: !!(compat.has_user_media),
      hasWebAssembly,
      hasDeviceOrientation: !!(compat.has_device_orientation),
    }
  }

  // NOTE(datchu): This is not async so it's missing info on newer Chrome.
  const compatibilities = singleton(getCompatibilitiesInternal)

  const allowedDevices = (runConfig) => {
    if (runConfig && runConfig.allowedDevices) {
      return runConfig.allowedDevices
    } else {
      return XrConfigFactory().device().MOBILE_AND_HEADSETS
    }
  }

  // Some browsers on iOS are broken:
  // - https://docs.google.com/spreadsheets/d/<REMOVED_BEFORE_OPEN_SOURCING>
  // Huawei browser is broken:
  // - https://github.com/8thwall/code8/issues/21863
  const unsupportedBrowser = compat => (
    compat.os === 'iOS' && ['TikTok'].includes(compat.inAppBrowser)) ||
    (compat.inAppBrowser === 'Huawei Browser')

  const checkSupportedConfig = (runConfig, compat) => {
    const supportedOSes = ['iOS', 'Android']
    const configDevices = allowedDevices(runConfig)
    // Any devices are ok
    if (configDevices === XrConfigFactory().device().ANY) {
      return true
    }

    // This is a mobile device, which is OK on mobile only or mobile + headset
    if (supportedOSes.includes(compat.os) && compat.hasDeviceOrientation) {
      return true
    }

    // Headsets are not ok.
    if (configDevices === XrConfigFactory().device().MOBILE) {
      return false
    }

    // Check headset support. This requires check for isSupportedHeadset to have completed. This
    // check will complete instantly in nearly all cases. In the cases where it doesn't, we
    // are either on an oculus browser or an edge browser that is still checking for ar support,
    // we err on the side of assuming the session is supported.
    if (isSupportedHeadset === null) {
      // eslint-disable-next-line no-console
      console.warn('Checking headset support before session support verified')
    }

    return isSupportedHeadset !== false
  }

  const isDeviceBrowserCompatible = (runConfig) => {
    if (incompatibilityOverride_) {
      return false
    }
    const compat = compatibilities()
    return checkSupportedConfig(runConfig, compat) &&
      compat.hasUserMedia &&
      compat.hasWebAssembly &&
      !unsupportedBrowser(compat)
  }

  const incompatibleReasons = (runConfig) => {
    if (isDeviceBrowserCompatible(runConfig)) {
      return []
    }

    const compat = compatibilities()
    const supportedOSes = ['iOS', 'Android']

    const reasons = []
    const configDevices = allowedDevices(runConfig)
    const checkMobileOs = configDevices === XrConfigFactory().device().MOBILE ||
      configDevices === XrConfigFactory().device().MOBILE_AND_HEADSETS

    // If this were a compatible headset, we wouldn't be at this line, so there's nothing left to
    // check for headset compatibility.
    if (checkMobileOs && !supportedOSes.includes(compat.os)) {
      reasons.push(IncompatibilityReasons.UNSUPPORTED_OS)
    }
    if (!compat.hasBrowser || unsupportedBrowser(compat)) {
      reasons.push(IncompatibilityReasons.UNSUPPORTED_BROWSER)
    }
    if (!compat.hasDeviceOrientation) {
      reasons.push(IncompatibilityReasons.MISSING_DEVICE_ORIENTATION)
    }
    if (!compat.hasUserMedia) {
      reasons.push(IncompatibilityReasons.MISSING_USER_MEDIA)
    }
    if (!compat.hasWebAssembly) {
      reasons.push(IncompatibilityReasons.MISSING_WEB_ASSEMBLY)
    }
    return reasons
  }

  function getInAppBrowserTypeIos(browser) {
    const safariShown = [
      'Apple News',
      'Twitter',
      'Tumblr',
    ]
    if (safariShown.includes(browser)) {
      return 'Safari'
    }
    const ellipsisShown = [
      'Facebook Messenger',
    ]
    if (ellipsisShown.includes(browser)) {
      return 'Ellipsis'
    }
    return ''
  }

  const incompatibleReasonDetails = (runConfig) => {
    const details: Compatibility = {}
    if (isDeviceBrowserCompatible(runConfig)) {
      return details
    }

    const compat = compatibilities()
    if (compat.inAppBrowser) {
      details.inAppBrowser = compat.inAppBrowser
    }

    if (compat.os === 'iOS') {
      const type = getInAppBrowserTypeIos(details.inAppBrowser)
      if (type) {
        details.inAppBrowserType = type
      }
    }

    return details
  }

  return {

    // The possible reasons for why a device and browser may not be compatible with 8th Wall
    // Web.
    //
    // Returns:
    // {
    //  // The incompatible reason is not specified.
    //  UNSPECIFIED: 0,
    //  // The estimated operating system is not supported.
    //  UNSUPPORTED_OS: 1,
    //  // The estimated browser is not supported.
    //  UNSUPPORTED_BROWSER: 2,
    //  // The browser does not support device orientation events.
    //  MISSING_DEVICE_ORIENTATION: 3,
    //  // The browser does not support user media acccess.
    //  MISSING_USER_MEDIA: 4,
    //  // The browser does not support web assembly.
    //  MISSING_WEB_ASSEMBLY: 5,
    // }
    IncompatibilityReasons,

    // Returns an estimate of the user's device (e.g. make / model) based on user agent string and
    // other factors. This information is only an estimate, and should not be assumed to be
    // complete or reliable.
    //
    // Returns:
    // {
    //   locale:  The user's locale.
    //   os: The device's operating system.
    //   osVersion: The device's operating system version.
    //   manufacturer: The device's manufacturer.
    //   model: The device's model.
    //   browser: The current browser: {name, version, majorVersion, inAppBrowser}
    // }
    deviceEstimate,

    // Returns information about the device.
    // Returns: a Promise to
    // {
    //   locale:  The user's locale.
    //   os: The device's operating system.
    //   osVersion: The device's operating system version.
    //   manufacturer: The device's manufacturer.
    //   model: The device's model.
    //   type: 'console' | 'mobile' | 'tablet' | 'smarttv' | 'wearable' | 'embedded' | undefined
    //   browser: The current browser: {name, version, majorVersion, inAppBrowser}
    //   features: hasSimd, hasThreads
    // }
    deviceInfo,

    // TODO(alvin): The raw compatibilies may not need to be publicly accessible anymore.
    //
    // Returns attributes describing whether the current environment is compatbile with 8th Wall
    // Web. This information is only an estimate, and should not be assumed to be
    // complete or reliable.
    //
    // Returns:
    // {
    //   os: The device's operating system.
    //   inAppBrowser: The detected in-app browser.
    //   hasDevice: Whether the estimated device is supported.
    //   hasBrowser: Whether a supported browser is used.
    //   hasUserMedia: Whether the browser has user media access.
    //   hasWebAssembly: Whether the browser supports web assembly.
    //   hasDeviceOrientation: Whether the browser supports orientation events.
    // }
    compatibilities,

    // Returns an estimate of whether the user's device and browser is compatible with 8th Wall
    // Web. If this returns false, XrDevice.incompatibleReasons() will return reasons about why
    // the device and browser are not supported.
    //
    // Input:
    //  {
    //    allowedDevices: [optional] Supported device classes, a value in XrConfig.device().
    //  }
    isDeviceBrowserCompatible,

    // Returns an array of XrDevice.IncompatibilityReasons why the device the device and browser
    // are not supported. This will only contain entries if XrDevice.isDeviceBrowserCompatible()
    // returns false.
    //
    // Input:
    //  {
    //    allowedDevices: [optional] Supported device classes, a value in XrConfig.device().
    //  }
    incompatibleReasons,

    // Returns extra details about the reasons why the device and browser are incompatible.
    // This information should only be used as a hint to help with further error handling.
    // These should not be assumed to be complete or reliable. This will only contain
    // entries if XrDevice.isDeviceBrowserCompatible() returns false.
    //
    // Input:
    //  {
    //    allowedDevices: [optional] Supported device classes, a value in XrConfig.device().
    //  }
    incompatibleReasonDetails,
  }
})

// Deprecated values
function JSDevice() {
  this.getDeviceInfo = getDeviceInfo
  this.getDeviceInfoBytes = getDeviceInfoBytes
  this.getLocale = getLocale
  this.getFingerprint = getFingerprint
  this.getCompatibility = deviceCompatibility
}

const getCompatibility = () => deviceCompatibility(getDeviceInfo(getFixedUAResult()))
// End deprecated values

export {
  XrDeviceFactory,
  setIncompatibilityOverride,
  JSDevice,
  getCompatibility,
}
