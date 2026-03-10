// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Christoph Bartschat (christoph@8thwall.com)
//
// Provides helper functions for constraining getUserMedia requests
//

import {XrDeviceFactory} from './js-device'
import {XrConfigFactory} from './config'

// Constraints for GetUserMedia
type GumConstraints = Partial<{
  deviceId: {exact?: string} | string
  facingMode: {exact?: string} | string[] | string
  width: {exact?: number, ideal?: number, min?: number, max?: number}
  height: {exact?: number, ideal?: number, min?: number, max?: number}
  frameRate: {exact?: number, ideal?: number, min?: number, max?: number}
}>

const androidResolutions: GumConstraints[] = []

const deviceEstimate = XrDeviceFactory().deviceEstimate()

const CROPPED_LOW_RES_MODELS = ['Pixel 4', 'Pixel 4 XL', 'Pixel 4a', 'Pixel 5', 'Pixel 5a',
  'Pixel 6', 'Pixel 6 Pro', 'I007D']
const LOW_RES_MODELS = ['SM-G950', 'SM-G960']

const deviceSupportsHighRes = () => {
  // Huawei P20 Lite - only supports 1080p. In recent versions of Chrome on this device
  // getUserMedia doesn't properly fail when attempting to use higher, unsupported resolutions.
  // For now, just fallback to low res.
  if (deviceEstimate.model === 'ANE-LX1') {
    return false
  }
  // Default
  return true
}

// NOTE(dat): ua-parser-js distinguish between `Mobile Chrome` and `Chrome`
const highResAndroid = deviceEstimate.browser.fullName === 'Mobile Chrome' &&
                       deviceEstimate.browser.majorVersion > 71 &&
                       deviceSupportsHighRes()

// These android models provide a cropped sensor camera feed at lower resolutions, so we need to
// force them to be high res.
if (CROPPED_LOW_RES_MODELS.includes(deviceEstimate.model)) {
  // You can check chrome://media-internals (when there is another tab with camera feed)
  // and find the native video resolution and their corresponding frameRate
  // Pixel 6: 30 to 35 fps
  // Pixel 5a: 30 to 35 fps
  // Pixel 4: 35 to 45 fps
  androidResolutions.push({width: {exact: 1280}, height: {exact: 720}, frameRate: {ideal: 60}})
  androidResolutions.push({width: {exact: 1600}, height: {exact: 1200}})
}

// Samsung phone has suffix for their region of sale. e.g. SM-G960U1 will return SM-G960
const deviceModel = deviceEstimate.manufacturer === 'Samsung'
  ? deviceEstimate.model.substr(0, 7)
  : deviceEstimate.model

// These android models provide a crop at our default resolution but full at a low resolution
if (LOW_RES_MODELS.includes(deviceModel)) {
  androidResolutions.push({width: {exact: 640}, height: {exact: 480}})
}

// Preferred android resolution for performance:
androidResolutions.push({width: {exact: 960}, height: {exact: 720}})

if (highResAndroid) {
  androidResolutions.push(
    {width: {min: 1920}, height: {min: 1440}},  // texSubImage: pixel2 ~20ms
    {width: {min: 1600}, height: {min: 1200}},  // texSubImage: pixel2 ~13ms
    {width: {min: 1440}, height: {min: 1080}},  // texSubImage: pixel2 ~13ms
    {width: {min: 1280}, height: {min: 960}}  // texSubImage: pixel2 ~10ms
  )
}
androidResolutions.push(
  {width: {min: 960}, height: {min: 720}},  // texSubImage: pixel2 ~6ms
  {width: {min: 640}, height: {min: 480}},  // texSubImage: pixel2 ~3ms
  {width: {min: 320}, height: {min: 240}}  // texSubImage: pixel2 ~3ms
)

const iOSResolutions = [
  {width: {min: 960}, height: {min: 720}},
  {width: {min: 640}, height: {min: 480}},
]

// SamsungBrowser do not give us the correct resolution if we request 960x720.
// It gives us a video with resolution 450x600. Requesting 640x480 works though, so
// we're going with that for launch.
const samsungBrowserResolutions: GumConstraints[] = [
  {width: {min: 640}, height: {min: 480}},
  {width: {min: 320}, height: {min: 240}},
]

const getSupportedResolutions = (): GumConstraints[] => {
  if (deviceEstimate.os === 'iOS') {
    return iOSResolutions
  }
  if (navigator.userAgent.includes('SamsungBrowser')) {
    return samsungBrowserResolutions
  }
  return androidResolutions
}

const getResolutionConstraints =
  (tryResolutionsIndex: number): GumConstraints => getSupportedResolutions()[tryResolutionsIndex]

const hasResolutionIndex = tryResolutionsIndex => (
  getSupportedResolutions().length > tryResolutionsIndex
)

// Get list of available media devices with their labels
const getLabelledDevices = () => {
  if (!navigator.mediaDevices.enumerateDevices) {
    return Promise.resolve([])
  }
  return navigator.mediaDevices.enumerateDevices()
    .then((devices) => {
      if (devices.length === 0 || devices[0].label) {
        return devices
      }
      // If we don't get labels, we need to get a granted status by opening a feed and closing it.
      return navigator.mediaDevices.getUserMedia({video: true})
        .then((stream) => {
          stream.getTracks().forEach(track => track.stop())
          return navigator.mediaDevices.enumerateDevices()
        })
    }).catch(() => [])
}

// On phones with multiple rear facing cameras, we could get a camera with an FOV we don't want.
// This gives us a chance to select a deviceId based on specialized rules.
//
// Currently this fix is for S10, S10e, S10+, P30 Pro, and the Qualcomm EXP21 (ASUS I007D).
const getPreferredDeviceId = (direction) => {
  // Bypass all of this special handling if you are trying to access front camera.
  const {FRONT, ANY} = XrConfigFactory().camera()
  if (direction === FRONT || direction === ANY) {
    return null
  }

  // We only want to return a preferred id for for
  // - Samsung (S10's)
  // - Huawei (P30 Pro)
  // - Qualcomm EXP21 (ASUS I007D)
  // - Phones without manufacturer field set (all Android phones running Chrome when not using
  //   Client Hints API).
  // See https://developer.chrome.com/blog/user-agent-reduction-android-model-and-version/

  if (!(
    deviceEstimate.manufacturer === 'Samsung' ||
    deviceEstimate.manufacturer === 'Huawei' ||
    (deviceEstimate.manufacturer === 'ASUS' && deviceEstimate.model === 'I007D') ||
    !deviceEstimate.manufacturer
  )) {
    return null
  }

  // If the browser doesn't support constraining deviceId, don't try.
  if (!navigator.mediaDevices.getSupportedConstraints ||
      !navigator.mediaDevices.getSupportedConstraints().deviceId) {
    return null
  }

  // If the browser doesn't support enumerating devices, don't try.
  if (!navigator.mediaDevices.enumerateDevices) {
    return null
  }
  return getLabelledDevices().then((devices) => {
    if (!devices.length) {
      return null
    }

    const videoInputDevices = devices.filter(e => e.kind === 'videoinput')
    if (videoInputDevices.length <= 2) {
      return null
    }

    const audioInputDeviceIds = new Set(
      devices.filter(e => e.kind === 'audioinput').map(d => d.deviceId)
    )

    // Sort device by their label.
    const sortedInputDevices = [...videoInputDevices]
    if (videoInputDevices[0].label) {
      sortedInputDevices.sort((deviceA, deviceB) => deviceA.label.localeCompare(deviceB.label))
    }
    // Pick the first device that has a coresponding audio
    const videoWithAudioDevice = sortedInputDevices.find(e => audioInputDeviceIds.has(e.deviceId))

    if (videoWithAudioDevice) {
      return videoWithAudioDevice.deviceId
    }

    // For Samsung S10's and Huaweo P30 Pro's, use the following camera
    // This will pick the first back-facing device that uses Camera2 API.
    // Unfortunately, in Huawei Browser, devices don't have labels. The ordering of these
    // cameras are also not good across manufacturers.
    const preferredDevice = videoInputDevices.find(e => e.label === 'camera2 0, facing back')
    return preferredDevice && preferredDevice.deviceId
  })
}

const getCameraConstraints = (tryResolutionsIndex, {direction}) => (
  Promise.resolve(getPreferredDeviceId(direction)).then((deviceId) => {
    const videoConstraints = getResolutionConstraints(tryResolutionsIndex)

    if (deviceId) {
      videoConstraints.deviceId = {exact: deviceId}
    }

    // Note: 'exact' seems to work fine on iOS12, but not 11. Could probably add a version check.
    if (deviceEstimate.os === 'iOS') {
      switch (direction) {
        case XrConfigFactory().camera().ANY:
          // Intentionally not setting facingMode here
          break
        case XrConfigFactory().camera().FRONT:
          videoConstraints.facingMode = ['user']
          break
        default:
          // Default = XrConfigFactory().camera().BACK
          videoConstraints.facingMode = ['environment']
      }
    } else if (deviceEstimate.os === 'Android') {
      // Use 'exact' here to ensure some devices don't open the front camera unless specified.
      // This is not supported by iOS Safari.
      switch (direction) {
        case XrConfigFactory().camera().ANY:
          // Intentionally not setting facingMode here
          break
        case XrConfigFactory().camera().FRONT:
          videoConstraints.facingMode = {exact: 'user'}
          break
        default:
          // Default = XrConfigFactory().camera().BACK
          videoConstraints.facingMode = {exact: 'environment'}
      }
    }
    // else: for non-mobile-OSes, don't set constraints.
    return {video: videoConstraints}
  })
)

export {
  getCameraConstraints,
  hasResolutionIndex,
}
