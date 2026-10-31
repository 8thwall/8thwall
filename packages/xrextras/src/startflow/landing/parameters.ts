import type {
  LandingMediaControlMode,
  LandingOrbitIdle,
  LandingOrbitInteraction,
  StartFlowScreenOptions,
  StartFlowStrings,
} from '../types'

type MediaControlMode = LandingMediaControlMode

type LandingParameters = {
  url: string
  promptPrefix: string
  promptSuffix: string
  vrPromptPrefix: string
  qrAlt: string
  messengerLabel: string
  logoSrc: string
  logoAlt: string
  poweredByLabel: string
  mediaSrc: string
  mediaAlt: string
  mediaAutoplay: boolean
  mediaAnimation: string
  mediaControls: LandingMediaControlMode
  videoTitle: string
  playLabel: string
  pauseLabel: string
  muteLabel: string
  unmuteLabel: string
  sceneEnvMap: string
  sceneOrbitIdle: LandingOrbitIdle
  sceneOrbitInteraction: LandingOrbitInteraction
  sceneLightingIntensity: number
}

const startFlowLandingParameters = (
  options: StartFlowScreenOptions, strings: StartFlowStrings
): LandingParameters => ({
  url: options.url ?? '',
  promptPrefix: strings.landingPromptPrefix,
  promptSuffix: strings.landingPromptSuffix,
  vrPromptPrefix: strings.landingVrPromptPrefix,
  qrAlt: strings.landingQrAlt,
  messengerLabel: strings.landingMessengerLabel,
  logoSrc: options.logoSrc ?? '',
  logoAlt: strings.landingLogoAlt,
  poweredByLabel: strings.poweredByLabel,
  mediaSrc: options.mediaSrc ?? '',
  mediaAlt: strings.landingMediaAlt,
  mediaAutoplay: options.mediaAutoplay,
  mediaAnimation: options.mediaAnimation,
  mediaControls: options.mediaControls,
  videoTitle: strings.landingVideoTitle,
  playLabel: strings.landingPlayLabel,
  pauseLabel: strings.landingPauseLabel,
  muteLabel: strings.landingMuteLabel,
  unmuteLabel: strings.landingUnmuteLabel,
  sceneEnvMap: options.sceneEnvMap,
  sceneOrbitIdle: options.sceneOrbitIdle,
  sceneOrbitInteraction: options.sceneOrbitInteraction,
  sceneLightingIntensity: options.sceneLightingIntensity,
})

export type {LandingParameters, MediaControlMode}
export {startFlowLandingParameters}
