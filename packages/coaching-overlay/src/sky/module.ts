import {showCoachingOverlay, hideCoachingOverlay} from './render'
import {
  ScanningData,
  CoachingOverlayParameters,
  defaultParameters,
  defaultScanningData,
  SCANNING_STATUS,
} from './parameters'
import type {Framework, SkyFoundDetails} from '../types/camera-pipeline'
import {updateVisibility} from './visibility'

const options_: CoachingOverlayParameters = {...defaultParameters}
const scanningData_: ScanningData = {...defaultScanningData}
let currentPercentage_: number = 0

// Reference to camera pipeline framework so we can dispatch event
let framework_: Framework | null = null
// Is the coaching overlay currently being shown
let isShown_ = true

const renderOverlay = () => {
  // Only call render when configure is changed or we get a new tracking status event.
  showCoachingOverlay({...options_, shouldShow: isShown_})
}

// TODO(dat): Add sky percentage to trigger showing/hiding the overlay
// see processCpuResult.layerscontroller.sky.percentage
const configure = (params: Partial<CoachingOverlayParameters>) => {
  Object.keys(options_).forEach((key) => {
    if (params[key] !== undefined) {
      options_[key] = params[key]
    }
  })

  renderOverlay()
}

const updateScanningStatus = (
  currentStatus: SCANNING_STATUS,
  percentage: number,
  {showThreshold, hideThreshold}
): SCANNING_STATUS => {
  if (percentage >= showThreshold) {
    return SCANNING_STATUS.SKY_VISIBLE
  } else if (percentage < hideThreshold) {
    return SCANNING_STATUS.SKY_NOT_VISIBLE
  } else {
    return currentStatus
  }
}

const onScanning = ({detail}) => {
  if (detail.name !== 'sky') {
    return
  }
  framework_?.dispatchEvent('show')
  isShown_ = true
  scanningData_.scanningStatus = SCANNING_STATUS.UNKNOWN
  renderOverlay()
}

const showOverlay = () => {
  framework_?.dispatchEvent('show')
  isShown_ = true
  scanningData_.scanningStatus = SCANNING_STATUS.UNKNOWN
  renderOverlay()
}

const hide = () => {
  framework_?.dispatchEvent('hide')
  isShown_ = false
  scanningData_.scanningStatus = SCANNING_STATUS.SKY_VISIBLE
  renderOverlay()
}

const onLayerUpdate = ({detail}: {detail: SkyFoundDetails}) => {
  if (detail.name !== 'sky') {
    return
  }

  if (!options_.autoByThreshold) {
    return
  }

  scanningData_.scanningStatus = updateScanningStatus(scanningData_.scanningStatus,
    detail.percentage, options_)
  const {shouldUpdate, show} = updateVisibility(isShown_, scanningData_.scanningStatus)

  if (!shouldUpdate) {
    return
  }

  framework_?.dispatchEvent(show ? 'show' : 'hide')
  isShown_ = show
  renderOverlay()
}

const cleanup = () => {
  hideCoachingOverlay()

  if (isShown_) {
    isShown_ = false
    framework_?.dispatchEvent('hide')
  }

  Object.assign(options_, defaultParameters)
  Object.assign(scanningData_, defaultScanningData)
}

const pipelineModule = () => ({
  name: 'sky-coaching-overlay',
  onAttach: ({framework}) => { framework_ = framework },
  onException: () => { cleanup() },
  onRemove: () => { cleanup() },
  onDetach: () => { cleanup() },
  onUpdate: ({processCpuResult}) => {
    const {layerscontroller} = processCpuResult
    if (!layerscontroller?.layers?.sky) {
      return
    }
    currentPercentage_ = layerscontroller.layers.sky.percentage
    onLayerUpdate({
      detail: {
        name: 'sky',
        percentage: currentPercentage_,
      },
    })
  },
  listeners: [
    {event: 'layerscontroller.layerscanning', process: onScanning},
    {event: 'layerscontroller.layerloading', process: showOverlay},
    {event: 'layerscontroller.layerfound', process: onLayerUpdate},
  ],
})

// Advance usage: directly control the coaching overlay
const control = {
  show: showOverlay,
  hide,
  setAutoShowHide: (isAuto: boolean) => { options_.autoByThreshold = isAuto },
  cleanup,
}

export {
  configure,
  pipelineModule,
  control,
}
