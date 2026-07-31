// Utility module for determining the visibility of the coaching flow
import type {VisibilityUpdate} from '../types/coaching-overlay'
import {SCANNING_STATUS} from './parameters'

const updateVisibility = (
  currentlyVisible: boolean,
  scanningStatus: SCANNING_STATUS
): VisibilityUpdate => {
  const shouldShowCoachingOverlay = scanningStatus !== SCANNING_STATUS.SKY_VISIBLE

  if (!currentlyVisible && shouldShowCoachingOverlay) {
    return {shouldUpdate: true, show: true}
  } else if (currentlyVisible && !shouldShowCoachingOverlay) {
    return {shouldUpdate: true, show: false}
  }
  return {shouldUpdate: false, show: currentlyVisible}
}

export {
  updateVisibility,
}
