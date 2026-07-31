// Utility module for determining the visibility of the vps coaching flow
import {TRACKING_STATUS, TRACKING_REASON} from './parameters'

// Creates the root element that will contain the Vps Coaching Overlay UI as children elements.
const updateVisibility = (
  currentlyVisible, wayspotConditionSatisfied, trackingStatus, trackingReason
) => {
  // If we're limited and initializing, we know we're trying to estimate the scale, therefore we
  // want to show the UI for vps coaching the user.
  const shouldBeVisible = (
    trackingStatus === TRACKING_STATUS.LIMITED && trackingReason === TRACKING_REASON.INITIALIZING)

  const shouldBeHidden = (trackingStatus === TRACKING_STATUS.NORMAL && wayspotConditionSatisfied)

  if (!currentlyVisible && shouldBeVisible) {
    return {shouldUpdate: true, show: true}
  } else if (currentlyVisible && shouldBeHidden) {
    return {shouldUpdate: true, show: false}
  }
  // The combination that is not present is LIMITED | UNDEFINED.  This can happen if we're in a
  // LOST state but still have an estimated scale, thus haven't dropped the map.  In this case,
  // just continue doing what we already doing for now.
  // Activate the animations of fade-in/fade-out
  return {shouldUpdate: false, show: currentlyVisible}
}

export {
  updateVisibility,
}
