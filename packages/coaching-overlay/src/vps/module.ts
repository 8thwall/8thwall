import {showVpsCoachingOverlay, hideVpsCoachingOverlay} from './render'
import {
  TrackingData,
  VpsCoachingOverlayParameters,
  defaultParameters,
  defaultTrackingStatus,
} from './parameters'
import {updateVisibility} from './visibility'
import type {Framework} from '../types/camera-pipeline'

declare const XR8: any

const options_: VpsCoachingOverlayParameters = {...defaultParameters}
const trackingData_: TrackingData = {...defaultTrackingStatus}
let framework_: Framework | null = null
let isShown_ = false
let projectWayspots_ = []
const foundProjectWayspots_ = []
let wayspotWatcher_ = null
const nearbyWayspots_ = []
let watchPositionId_ = 0
let lastGeolocation_ = {latitude: 0, longitude: 0, accuracy: 0}
let didWarnMissing = false

// Records the time between getting each wayspot from the wayspotWatcher.
let gotAllWayspotsTimeout_ = 0

const distance = (from: IGeoLocation, to: IGeoLocation): number => {
  // Computational optimization for no change.
  if (from.lat === to.lat && from.lng === to.lng) {
    return 0
  }
  const lat1R = (from.lat * Math.PI) / 180
  const lat2R = (to.lat * Math.PI) / 180
  const halfLatD = 0.5 * (lat2R - lat1R)
  const halfLngD = 0.5 * ((to.lng * Math.PI) / 180 - (from.lng * Math.PI) / 180)
  const v = Math.sin(halfLatD) ** 2 + (Math.sin(halfLngD) ** 2) * Math.cos(lat1R) * Math.cos(lat2R)
  const arc = 2 * Math.atan2(Math.sqrt(v), Math.sqrt(1 - v))
  return arc * 6371008.8  // Earth arithmetic mean radius, per en.wikipedia.org/wiki/Earth_radius
}

type Wayspot = {
  id: string
  imageUrl: string
  title: string
  lat: number
  lng: number
  // Distance from the user to the wayspot, computed internally.
  distance?: number
}

const getClosestWayspot = (wayspots: Wayspot[]): Wayspot | undefined => {
  let selectedWayspot: Wayspot | undefined

  // Choose the nearest projectWayspot to render if we have any.
  wayspots.forEach((wayspot: Wayspot) => {
    const distanceToWayspot = distance(
      {lat: lastGeolocation_.latitude, lng: lastGeolocation_.longitude},
      {lat: wayspot.lat, lng: wayspot.lng}
    )
    wayspot.distance = distanceToWayspot

    if (!selectedWayspot) {
      selectedWayspot = wayspot
      return
    }

    if (wayspot.distance < selectedWayspot.distance) {
      selectedWayspot = wayspot
    }
  })

  return selectedWayspot
}

const haveValidGeoLocation = () => !(lastGeolocation_.latitude === 0 &&
  lastGeolocation_.longitude === 0 &&
  lastGeolocation_.accuracy === 0)

const getWayspotInfoForName = (wayspotName: string, wayspots: Wayspot[]) => (
  wayspots.find(wayspot => wayspot.title === wayspotName)
)

// Sets the selected wayspot that we will render.
const setSelectedWayspot = () => {
  // If the user specified a wayspot name and image manually, we will use those.
  if (options_.wayspotName && options_.hintImage) {
    trackingData_.selectedWayspotName = options_.wayspotName
    trackingData_.selectedWayspotHintImage = options_.hintImage
    return
  }

  // Make sure we avoid the following example scenarios by clearing selected wayspot info from
  // trackingData:
  // 1. App updates wayspotName, but it isn't in the nearby wayspots, so we show the new name but
  //    an old image.
  // 2. User leaves the range of any wayspots, but continues to see the old name/image.
  trackingData_.selectedWayspotName = ''
  trackingData_.selectedWayspotHintImage = ''

  if (!haveValidGeoLocation()) {
    return
  }

  let selectedWayspot: Wayspot | undefined
  // If the user has specified a wayspotName but not a hintImage, then get the corresponding
  // selectedWayspot for that wayspotName which will contain the associated hintImage.
  if (options_.wayspotName) {
    selectedWayspot =
      getWayspotInfoForName(options_.wayspotName, projectWayspots_) ||
      getWayspotInfoForName(options_.wayspotName, nearbyWayspots_)

    // If the specified wayspot name is not part of the projectWayspots or nearby wayspots, log
    // a warning. We need to check nearbyWayspots length to make sure we've actually loaded the
    // wayspots at this point.
    // TODO(nathan): update the makeWayspotWatcher() api so that we can know if no wayspots are
    // nearby. Currently, we won't warn in the scenario where there are no nearby wayspots.
    if (!selectedWayspot && nearbyWayspots_.length && !didWarnMissing) {
      // eslint-disable-next-line no-console
      console.warn(`[vps-coaching-overlay] Requested wayspot ${options_.wayspotName} is not a ` +
        'project wayspot or nearby wayspot')
      didWarnMissing = true
    }
  } else {
    // If the user did not specify a wayspot, choose the nearest projectWayspot. If there are no
    // projectWayspots, choose the nearest wayspot.
    selectedWayspot =
      getClosestWayspot(projectWayspots_) ||
      getClosestWayspot(nearbyWayspots_)
  }

  if (!selectedWayspot) {
    return
  }

  // Use the automatic name/image unless one of them is manually specified. For example, this allows
  // the user to use a single hint image for all possible wayspots.
  trackingData_.selectedWayspotName = options_.wayspotName || selectedWayspot.title
  trackingData_.selectedWayspotHintImage = options_.hintImage || selectedWayspot.imageUrl
}

const renderOverlay = () => {
  // Only call render when configure is changed or we get a new tracking status event.
  showVpsCoachingOverlay({...options_, ...trackingData_})
}

const configure = (params: Partial<VpsCoachingOverlayParameters>) => {
  Object.keys(options_).forEach((key) => {
    if (key === 'wayspotName') {
      didWarnMissing = false
    }

    if (params[key] !== undefined) {
      options_[key] = params[key]
    }
  })

  setSelectedWayspot()

  renderOverlay()
}

// Called whenever the tracking status or reason changes.
const onTrackingStateUpdate = ({detail}) => {
  const {shouldUpdate, show} = updateVisibility(
    isShown_,
    trackingData_.wayspotConditionSatisfied,
    detail.status,
    detail.reason
  )
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
  hideVpsCoachingOverlay()

  if (isShown_) {
    isShown_ = false
    framework_?.dispatchEvent('hide')
  }

  navigator.geolocation.clearWatch(watchPositionId_)
  watchPositionId_ = 0
  projectWayspots_ = []
  wayspotWatcher_ = null
  nearbyWayspots_.length = 0
  lastGeolocation_ = {latitude: 0, longitude: 0, accuracy: 0}
  didWarnMissing = false

  Object.assign(options_, defaultParameters)
  Object.assign(trackingData_, defaultTrackingStatus)

  framework_ = null
}

interface IGeoLocation {
  lat: number
  lng: number
}

const onWayspotVisible = (wayspot) => {
  // id : "seROU9L/1IWoRC//6MiI"
  // imageUrl : "https://path/to/image.png"
  // lat : 37.43888
  // lng : -122.158444
  // title : "Giant Urn"
  nearbyWayspots_.push(wayspot)

  window.clearTimeout(gotAllWayspotsTimeout_)
  gotAllWayspotsTimeout_ = window.setTimeout(() => {
    // We have now waited long enough so that we can calculate the distance from the current
    // latitude and longitude to the available wayspots.
    setSelectedWayspot()
    renderOverlay()
  }, 0)
}

const onWayspotHidden = (wayspot) => {
  const index = nearbyWayspots_.indexOf(wayspot)
  if (index > -1) {
    foundProjectWayspots_.splice(index, 1)
  }

  // TODO(nathan): add more logic around making sure we are updating the selected wayspot based
  // off the user's latest location.
}

const onGeolocationError = (error) => {
  // eslint-disable-next-line no-console
  console.error('[vps-coaching-overlay] Failed to get geolocation with error ', error)
}

const updateDeviceGeolocation = ({coords}) => {
  lastGeolocation_ = {
    latitude: coords.latitude, longitude: coords.longitude, accuracy: coords.accuracy,
  }
  // TODO(nathan): do more work here so that we're not updating the selected wayspot each second.
  // Instead, we should only update if the user has actually moved a significant amount. The concern
  // is that the user could be right in between two wayspots and the overlay is switching between
  // the two.
  setSelectedWayspot()
}

// Set whether we have found a wayspot that is needed to hide the VpsCoachingOverlay.
const setWayspotCondionSatisfied = () => {
  // If the user specified a wayspotName, we can only set that we've found the required wayspot once
  // we have that corresponding wayspot.
  if (options_.wayspotName) {
    // TODO(nathan): currently, we only emit project wayspots. We don't emit all wayspots.
    // We don't have a way for us to check that they localized against the wayspot that is
    // specified. So we are just going to hide the overlay if we localize against anything.
    trackingData_.wayspotConditionSatisfied = true
    return
  }

  // If we have projectWayspots and we have received any reality.projectwayspotfound, then we have
  // found a required wayspot.
  if (projectWayspots_.length) {
    trackingData_.wayspotConditionSatisfied = !!foundProjectWayspots_.length
    return
  }

  // If we're here, then this means the user has no project wayspot or specified wayspot.
  trackingData_.wayspotConditionSatisfied = true
}

// Called whenever we localized against one of our project wayspots, which is when our engine emits
// the reality.projectwayspotfound event.
const projectWayspotFound = (wayspot) => {
  // Mark that we've found a project wayspot.
  foundProjectWayspots_.push(wayspot.name)

  // Update whether we've found a required wayspot or not.
  setWayspotCondionSatisfied()

  // Update the UI.
  renderOverlay()
}

// Mark that we've lost a project wayspot.
const projectWayspotLost = (wayspot) => {
  const index = foundProjectWayspots_.indexOf(wayspot.name)
  if (index > -1) {
    foundProjectWayspots_.splice(index, 1)
  }

  // Update whether we still have a required wayspot or not.
  setWayspotCondionSatisfied()

  // Update the UI.
  renderOverlay()
}

const onAttach = ({framework}) => {
  // Request GPS location.
  watchPositionId_ = navigator.geolocation.watchPosition(
    updateDeviceGeolocation, onGeolocationError
  )

  framework_ = framework

  // Load the project wayspots.
  XR8.Vps.projectWayspots().then((projectWayspots) => {
    projectWayspots_ = projectWayspots
    trackingData_.numProjectWayspots = projectWayspots_.length

    // Even if the user has project wayspots, they still may want to reference a non-project
    // wayspot. So we'll want to load nearby wayspots either way.
    wayspotWatcher_ = XR8.Vps.makeWayspotWatcher(
      {onVisible: onWayspotVisible, onHidden: onWayspotHidden, pollGps: true}
    )
    wayspotWatcher_.pollGps(true)

    setWayspotCondionSatisfied()
    setSelectedWayspot()
    renderOverlay()
  })
}

const pipelineModule = () => ({
  name: 'vps-coaching-overlay',
  onAttach,
  onException: () => { cleanup() },
  onRemove: () => { cleanup() },
  onDetach: () => { cleanup() },
  listeners: [
    {event: 'reality.trackingstatus', process: onTrackingStateUpdate},
    {event: 'reality.projectwayspotfound', process: projectWayspotFound},
    {event: 'reality.projectwayspotlost', process: projectWayspotLost},
  ],
})

export {
  configure,
  pipelineModule,
}
