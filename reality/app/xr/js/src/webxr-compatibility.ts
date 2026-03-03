const IMMERSIVE_AR = 'immersive-ar'
const IMMERSIVE_VR = 'immersive-vr'

const VR_BROWSERS = ['Oculus Browser', 'Firefox Reality']
const AR_BROWSERS = ['Edge', 'Oculus Browser']
const AR_MODELS = ['Magic Leap 2']

// https://immersive-web.github.io/webxr/#xrsystem-interface
type XrSessionMode = typeof IMMERSIVE_AR | typeof IMMERSIVE_VR
type XrSystem = {
  isSessionSupported: (mode: XrSessionMode) => Promise<boolean>
}

// Some devices (e.g. Magic Leap 2) depends on deviceModel to determine if it supports AR
// We use deviceModel to determine support in a few cases
//   * Magic Leap 2: supports immersive AR.
//   * Apple Vision Pro: appears as iPad, supports immersive VR, when WebXR is on.
const getSupportedHeadsetSessionType = async (
  browser: string, deviceModel?: string
): Promise<XrSessionMode | ''> => {
  try {
    const xrSystem = (navigator as any as {xr?: XrSystem}).xr
    if (!xrSystem) {
      return ''
    }
    // Allow users to override metaversal mode
    if (window._XR8MetaversalMode) {
      // eslint-disable-next-line no-console
      console.log('[XR] Metaversal mode overridden to', window._XR8MetaversalMode)
      return window._XR8MetaversalMode
    }

    const isArSupported = await xrSystem.isSessionSupported(IMMERSIVE_AR)
    const maybeAr = AR_BROWSERS.includes(browser) || AR_MODELS.includes(deviceModel)
    if (maybeAr && isArSupported) {
      return IMMERSIVE_AR
    }

    // Vision Pro appears as iPad as of 2/6/2024 via DeviceEstimate.
    // The Screen details is 2612x1470
    if ((VR_BROWSERS.includes(browser) || deviceModel === 'iPad') &&
      await xrSystem.isSessionSupported(IMMERSIVE_VR)) {
      return IMMERSIVE_VR
    }
  } catch (err) {
    // Ignore
  }

  return ''
}

export {
  getSupportedHeadsetSessionType,
}
