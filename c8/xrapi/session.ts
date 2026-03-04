import type {XrSession, XrSessionInit, XrSessionMode} from './xrapi-types'
import type {InternalXrView} from './view'

type InternalXrSession = XrSession & {
  // https://immersive-web.github.io/webxr/#xrsession-list-of-views
  _views: InternalXrView[]

  _recommendedWidth: number
  _recommendedHeight: number
}

type XrSessionFeatureConfig = {
  requiredFeatures: readonly string[]
  optionalFeatures: readonly string[]
}

// https://immersive-web.github.io/webxr/#default-features
const defaultSessionFeatures: Record<XrSessionMode, XrSessionFeatureConfig> = {
  'inline': {
    requiredFeatures: ['viewer'],
    optionalFeatures: [],
  },
  'immersive-vr': {
    requiredFeatures: ['viewer', 'local'],
    optionalFeatures: [],
  },
  'immersive-ar': {
    requiredFeatures: ['viewer', 'local'],
    optionalFeatures: [],
  },
} as const

// Return mutable copy of default features
const getDefaultSessionFeatures = (mode: XrSessionMode) => {
  const defaultFeatures = defaultSessionFeatures[mode]
  return {
    requiredFeatures: [...defaultFeatures.requiredFeatures],
    optionalFeatures: [...defaultFeatures.optionalFeatures],
  }
}

// Based on Chromium's Internal Feature Set
const SUPPORTED_XRSESSION_FEATURES = [
  'viewer',
  'local',
  'local-floor',
  'bounded-floor',
  'layers',
  'space-warp',
]

// This is mostly used as an internal reference
// Move items over when they are supported / when Native can properly check for them
const UNSUPPORTED_XRSESSION_FEATURES = [
  'unbounded',
  'dom-overlay',
  'hit-test',
  'light-estimation',
  'anchors',
  'camera-access',
  'plane-detection',
  'depth-sensing',
  'image-tracking',
  'hand-tracking',
  'secondary-views',
  'front-facing',
  'webgpu',
]

// https://immersive-web.github.io/webxr/#dom-xrsessioninit-requiredfeatures
const getEnabledFeatures = (
  mode: XrSessionMode,
  options?: XrSessionInit,
  checkFeatureSupported?: (feature: string) => boolean
) => {
  const enabledFeatureGroup = getDefaultSessionFeatures(mode)

  const requiredFeatureUnsupported = options?.requiredFeatures?.some((feature) => {
    if (SUPPORTED_XRSESSION_FEATURES.includes(feature) && checkFeatureSupported?.(feature)) {
      enabledFeatureGroup.requiredFeatures.push(feature)
    } else {
      // eslint-disable-next-line no-console
      console.warn(`Unsupported required feature: ${feature}`)
      return true
    }
    return false
  })

  // A required feature was unsupported, so we can't support the session
  if (requiredFeatureUnsupported) {
    return null
  }

  options?.optionalFeatures?.forEach((feature) => {
    if (SUPPORTED_XRSESSION_FEATURES.includes(feature) && checkFeatureSupported?.(feature)) {
      enabledFeatureGroup.optionalFeatures.push(feature)
    }
  })

  return [...new Set([
    ...enabledFeatureGroup.requiredFeatures,
    ...enabledFeatureGroup.optionalFeatures,
  ])]
}

export {
  type InternalXrSession,
  SUPPORTED_XRSESSION_FEATURES,
  UNSUPPORTED_XRSESSION_FEATURES,
  getEnabledFeatures,
}
