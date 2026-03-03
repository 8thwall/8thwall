// @attr(npm_rule = "@npm-jsxr//:npm-jsxr")
import * as capnp from 'capnp-ts'
import {IResult, UAParser} from 'ua-parser-js'

import {DeviceInfo} from 'reality/engine/api/device/info.capnp'

import type {Compatibility} from '../types/common'
import {correctDeviceManufacturer, correctDeviceModel} from './override'

// Detect In App Browsers
const getAppFromUserAgent = (ua) => {
  if (ua.includes('AppleNews')) {
    return 'Apple News'
  } if (ua.includes('FBAN/MessengerForiOS')) {
    return 'Facebook Messenger'
  } else if (ua.includes('FBAN/FBIOS')) {
    return 'Facebook'
  } else if (ua.includes('FB_IAB')) {
    return 'Facebook Android'
  } else if (ua.includes('Instagram')) {
    return 'Instagram'
  } else if (ua.includes('Pinterest')) {
    return 'Pinterest'
  } else if (ua.includes('Snapchat')) {
    return 'Snapchat'
  } else if (ua.includes('Tumblr')) {
    return 'Tumblr'
  } else if (ua.includes('Twitter')) {
    return 'Twitter'
  } else if (ua.includes('MicroMessenger')) {
    return 'WeChat'
  } else if (ua.includes('Line')) {
    return 'Line'
  } else if (ua.includes('LinkedInApp')) {
    return 'LinkedIn'
  } else if (ua.includes('Baidu')) {
    return 'Baidu'
  } else if (ua.includes('Weibo')) {
    return 'Sino Weibo'
  } else if (ua.includes('Kik')) {
    return 'Kik'
  } else if (ua.includes('QQ')) {
    return 'QQ'
  } else if (ua.includes('NAVER')) {
    return 'Naver'
  } else if (ua.includes('KAKAOTALK')) {
    return 'Kakao Talk'
  } else if (ua.includes('EdgiOS') || ua.includes('EdgA') || ua.includes('Edg/')) {
    return 'Microsoft Edge'
  } else if (ua.includes('CriOS')) {
    return 'Google Chrome'
  } else if (ua.includes('FxiOS/21')) {
    return 'Mozilla Firefox'
  } else if (ua.includes('FxiOS/8')) {
    return 'Mozilla Firefox Focus'
  } else if (ua.includes('OPT')) {
    return 'Opera Touch'
  } else if (ua.includes('Tesla')) {
    return 'Tesla'
  } else if (ua.includes('Quest')) {
    return 'Oculus Quest'
  } else if (ua.includes('Pacific')) {
    return 'Oculus Go'
  } else if (ua.includes('musical_ly')) {
    return 'TikTok'
  } else if (ua.includes('HuaweiBrowser')) {
    return 'Huawei Browser'
  } else if (navigator && (navigator as any).brave) {
    // Note: Most correct is navigator.brave && await navigator.brave.isBrave()
    // But that would lead to a breaking change to make things async, and navigator.brave is
    // undefined if not in Brave.
    return 'Brave'
  }
  return undefined
}

type UaResult = IResult & {
  inAppBrowser?: string
}

const addExtraResultData = (result: UaResult): UaResult => {
  // Rewrite "Desktop" Safari on iPadOS to iOS
  if (result.browser && result.browser.name === 'Safari' && window.TouchEvent) {
    result.os.name = 'iOS'
    result.os.version = '13.0'

    result.device = {
      vendor: 'Apple',
      model: 'iPad',
      type: 'tablet',
    }
  }

  const inAppBrowser = getAppFromUserAgent(navigator.userAgent)
  if (inAppBrowser) {
    result.inAppBrowser = inAppBrowser
  }

  return result
}

const getFixedUAResult = (): UaResult => {
  const result: UaResult = new UAParser().getResult()
  return addExtraResultData(result)
}

// NOTE(datchu): Remove this whenever @types/ua-parser-js update to have this type
declare global {
  namespace UAParser {
    interface IResult {
      withClientHints(): Promise<UAParser.IResult>
      withFeatureCheck(): UAParser.IResult
    }
  }
}

const getFixedUAResultWithClientHints = async (): Promise<UaResult> => {
  const ua = new UAParser()
  const result: UaResult = await ua.getResult().withFeatureCheck().withClientHints()
  return addExtraResultData(result)
}

// Info that are only available in async
interface AsyncInfo {
  extraRendererInfo?: string
}

const exportDeviceInfo = (result: IResult, deviceInfo: DeviceInfo, asyncInfo?: AsyncInfo) => {
  deviceInfo.setOs((result.os && result.os.name) || '')
  deviceInfo.setOsVersion((result.os && result.os.version) || '')
  const manufacturer = correctDeviceManufacturer(result.device?.vendor || '')
  deviceInfo.setManufacturer(manufacturer)
  const model = correctDeviceModel((result.device && result.device.model) || '',
    asyncInfo?.extraRendererInfo)
  deviceInfo.setModel(model)
}

// NOTE(dat): Once we have deprecated the old getDeviceInfo non-promise version
//            extraRendererInfo can be read directly in detectIosModel instead of being plumbed
const getDeviceInfo = (result: IResult, asyncInfo?: AsyncInfo): DeviceInfo => {
  const message = new capnp.Message()
  const deviceInfo = message.initRoot(DeviceInfo)
  exportDeviceInfo(result, deviceInfo, asyncInfo)
  return deviceInfo
}

const getDeviceInfoBytes = (result: IResult, asyncInfo?: AsyncInfo): ArrayBuffer => {
  const message = new capnp.Message()
  const deviceInfo = message.initRoot(DeviceInfo)
  exportDeviceInfo(result, deviceInfo, asyncInfo)
  return message.toArrayBuffer()
}

const getLocale = () => {
  if (navigator.languages && navigator.languages.length) {
    return navigator.languages[0]
  } else {
    return navigator.language
  }
}

const hasGetUserMedia = () => !!(navigator.mediaDevices && navigator.mediaDevices.getUserMedia)
const hasDeviceOrientation = () => !!(window.DeviceOrientationEvent)
const hasWasm = (): boolean => !!window.WebAssembly

const hasWasmSimd = (): boolean => {
  let simdSupported = false
  try {
    // This was taken from the minified version of wasm-feature-detect's simd() function:
    // https://unpkg.com/wasm-feature-detect@1.2.11/dist/umd/index.js
    WebAssembly.validate(new Uint8Array(
      [0, 97, 115, 109, 1, 0, 0, 0, 1, 5, 1, 96, 0, 1, 123, 3, 2, 1, 0, 10, 10, 1, 8, 0, 65, 0,
        253, 15, 253, 98, 11]
    ))
    simdSupported = true
  } catch (e) {
    // Simd is not supported
  }
  return simdSupported
}

const hasWasmThreads = (): boolean => {
  let threadsSupported = false
  try {
    if (typeof MessageChannel !== 'undefined') {
      // Test for transferability of SABs (needed for Firefox)
      // https://groups.google.com/forum/#!msg/mozilla.dev.platform/IHkBZlHETpA/dwsMNchWEQAJ
      new MessageChannel().port1.postMessage(new SharedArrayBuffer(1))
    }
    WebAssembly.validate(new Uint8Array(
      [0, 97, 115, 109, 1, 0, 0, 0, 1, 4, 1, 96, 0, 0, 3, 2, 1, 0, 5, 4, 1, 3, 1, 1, 10, 11, 1, 9,
        0, 65, 0, 254, 16, 2, 0, 26, 11, 0, 10, 4, 110, 97, 109, 101, 2, 3, 1, 0, 0]
    ))
    threadsSupported = true
  } catch (e) {
    // Threads is not supported
  }
  return threadsSupported
}

const setBrowserFeatures = (compat: Compatibility) => {
  compat.has_user_media = hasGetUserMedia()
  compat.has_web_assembly = hasWasm()
  compat.has_device_orientation = hasDeviceOrientation()
}

const getAndroidCompatibility = (): Compatibility => {
  const compatible: Compatibility = {
    // If Android, user is probably on the right kind of device
    'has_device': true,
    'has_browser': true,
  }
  const inAppBrowser = getAppFromUserAgent(navigator.userAgent)
  if (inAppBrowser) {
    compatible.browser = inAppBrowser
  }
  setBrowserFeatures(compatible)
  return compatible
}

const getIosCompatibility = (): Compatibility => {
  const compatible: Compatibility = {
    // If iOS, user is probably on the right kind of device
    'has_device': true,
  }
  const ua = navigator.userAgent
  const inAppBrowser = getAppFromUserAgent(ua)
  if (inAppBrowser) {
    compatible.browser = inAppBrowser
  } else {
    // Standalone Mobile Safari on iOS is required
    const isStandaloneSafari = ['AppleWebKit/', 'Mobile/', 'Version/'].every(s => ua.includes(s))
    const isFakingDesktopSafari = ua.includes('Mac OS X') && !!window.TouchEvent

    compatible.has_browser = isStandaloneSafari || isFakingDesktopSafari
  }

  setBrowserFeatures(compatible)
  return compatible
}

const deviceCompatibility = (deviceInfo: DeviceInfo): Compatibility => {
  const deviceOs = deviceInfo.getOs()
  let compat: Compatibility = {}
  if (deviceOs === 'Android') {
    compat = getAndroidCompatibility()
  } else if (deviceOs === 'iOS') {
    compat = getIosCompatibility()
  } else {
    setBrowserFeatures(compat)
  }
  compat.os = deviceOs
  return compat
}

export {
  UaResult,
  AsyncInfo,
  getDeviceInfo,
  getDeviceInfoBytes,
  deviceCompatibility,
  getLocale,
  getFixedUAResult,
  getFixedUAResultWithClientHints,
  hasGetUserMedia,
  hasDeviceOrientation,
  hasWasm,
  hasWasmSimd,
  hasWasmThreads,
}
