// Parameters that we pass from the 8thWall modules to the React renderer

// The options that the user can specify.
type VpsCoachingOverlayParameters = {
  // The user specified name. If empty, we will automatically select a wayspot to render.
  wayspotName: string
  hintImage: string
  promptColor: string
  animationColor: string
  // time in milliseconds
  animationDuration: number
  promptPrefix: string
  promptSuffix: string
  statusText: string
  disablePrompt: boolean
}

type TrackingData = {
  trackingStatus: string
  trackingReason: string
  // If we are tracking and the wayspot condition is satisfied, then we'll hide the overlay.
  // In order for the wayspot condition to be satisfied, either:
  // 1. We got the wayspot the user specified.
  // 2. We found one of the project wayspots
  // 3. We don't have any project wayspots and we localized against anything.
  wayspotConditionSatisfied: boolean

  // This is the name of the wayspot that we will render.
  // 1. If the user specified a wayspotName, then this can only be that name.
  // 2. If the user did not specify a wayspotName but has projectWayspots, this will be the nearest
  //    projectWayspot.
  // 3. If the user did not specify a wayspotName and has no projectWayspots, this will be the
  //    nearest wayspot.
  selectedWayspotName: string
  selectedWayspotHintImage: string
}

// User parameters + trackingStatus and trackingReason.  This is what we pass to the UI.
type VpsCoachingOverlayUiConfig = VpsCoachingOverlayParameters & TrackingData

const TRACKING_STATUS = {
  LIMITED: 'LIMITED',
  NORMAL: 'NORMAL',
}

const TRACKING_REASON = {
  INITIALIZING: 'INITIALIZING',
  UNDEFINED: 'UNDEFINED',
}

const defaultParameters: VpsCoachingOverlayParameters = {
  wayspotName: '',
  hintImage: '',
  promptColor: 'white',
  animationColor: 'white',
  animationDuration: 5000,
  promptPrefix: 'Point your camera at',
  promptSuffix: 'and move around',
  statusText: 'Scanning',
  disablePrompt: false,
}

const defaultTrackingStatus: TrackingData = {
  trackingStatus: TRACKING_STATUS.LIMITED,
  // By making the default reason initializing, we make the default distinct from a LOST state and
  // instead make it match the starting engine's state.  This is important for when the module is
  // detached, configured, and then re-attached so that the module will show the a fresh coaching
  // overlay instead of performing the LOST behavior: which is to continue doing whatever the
  // previous state was.
  trackingReason: TRACKING_REASON.INITIALIZING,
  wayspotConditionSatisfied: false,
  selectedWayspotName: '',
  selectedWayspotHintImage: '',
}

export {
  VpsCoachingOverlayParameters,
  VpsCoachingOverlayUiConfig,
  TrackingData,
  defaultParameters,
  defaultTrackingStatus,
  TRACKING_STATUS,
  TRACKING_REASON,
}
