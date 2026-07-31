// Parameters that we pass from the 8thWall modules to the React renderer

// The options that the user can specify.
type CoachingOverlayParameters = {
  promptColor: string
  animationColor: string
  promptText: string
  disablePrompt: boolean
}

type TrackingData = {
  trackingStatus: string
  trackingReason: string
}

// User parameters + trackingStatus and trackingReason.  This is what we pass to the UI.
type CoachingOverlayUiConfig = CoachingOverlayParameters & TrackingData

const TRACKING_STATUS = {
  LIMITED: 'LIMITED',
  NORMAL: 'NORMAL',
}

const TRACKING_REASON = {
  INITIALIZING: 'INITIALIZING',
  UNDEFINED: 'UNDEFINED',
}

const defaultParameters: CoachingOverlayParameters = {
  promptText: 'Move device forward and back',
  promptColor: 'white',
  animationColor: 'white',
  disablePrompt: false,
}

const defaultTrackingStatus: TrackingData = {
  trackingStatus: TRACKING_STATUS.LIMITED,
  trackingReason: TRACKING_REASON.UNDEFINED,
}

export {
  CoachingOverlayParameters,
  CoachingOverlayUiConfig,
  TrackingData,
  defaultParameters,
  defaultTrackingStatus,
  TRACKING_STATUS,
  TRACKING_REASON,
}
