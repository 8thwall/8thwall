import {showCoachingOverlay, hideCoachingOverlay} from './render'
import {
  TrackingData,
  CoachingOverlayParameters,
  defaultParameters,
  defaultTrackingStatus,
} from './parameters'
import {updateVisibility} from './visibility'
import type {Framework} from '../types/camera-pipeline'

const options_: CoachingOverlayParameters = {...defaultParameters}
const trackingData_: TrackingData = {...defaultTrackingStatus}
let framework_: Framework | null = null
let isShown_ = false

const renderOverlay = () => {
  // Only call render when configure is changed or we get a new tracking status event.
  showCoachingOverlay({...options_, ...trackingData_})
}

const configure = (params: Partial<CoachingOverlayParameters>) => {
  Object.keys(options_).forEach((key) => {
    if (params[key] !== undefined) {
      options_[key] = params[key]
    }
  })

  renderOverlay()
}

// Called whenever the tracking status or reason changes.
const onTrackingStateUpdate = ({detail}) => {
  const {shouldUpdate, show} = updateVisibility(isShown_, detail.status, detail.reason)
  trackingData_.trackingStatus = detail.status
  trackingData_.trackingReason = detail.reason

  if (!shouldUpdate) {
    return
  }

  if (show) {
    framework_?.dispatchEvent('show')
  } else {
    framework_?.dispatchEvent('hide')
  }
  isShown_ = !isShown_
  renderOverlay()
}

const cleanup = () => {
  hideCoachingOverlay()

  if (isShown_) {
    isShown_ = false
    framework_?.dispatchEvent('hide')
  }

  Object.assign(options_, defaultParameters)
  Object.assign(trackingData_, defaultTrackingStatus)

  framework_ = null
}

const pipelineModule = () => ({
  name: 'coaching-overlay',
  onAttach: ({framework}) => { framework_ = framework },
  onException: () => { cleanup() },
  onRemove: () => { cleanup() },
  onDetach: () => { cleanup() },
  listeners: [
    {event: 'reality.trackingstatus', process: onTrackingStateUpdate},
  ],
})

export {
  configure,
  pipelineModule,
}
