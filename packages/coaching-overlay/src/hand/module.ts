import {showCoachingOverlay, hideCoachingOverlay} from './render'
import {
  CoachingOverlayParameters,
  defaultParameters,
} from './parameters'
import type {Framework} from '../types/camera-pipeline'

const options_: CoachingOverlayParameters = {...defaultParameters}

// Reference to camera pipeline framework so we can dispatch event
let framework_: Framework | null = null
// Is the coaching overlay currently being shown
let isShown_ = true

const renderOverlay = () => {
  // Only call render when configure is changed or we get a new tracking status event.
  showCoachingOverlay({...options_, shouldShow: isShown_})
}

const configure = (params: Partial<CoachingOverlayParameters>) => {
  Object.keys(options_).forEach((key) => {
    if (params[key] !== undefined) {
      options_[key] = params[key]
    }
  })

  renderOverlay()
}

const showOverlay = () => {
  framework_?.dispatchEvent('show')
  isShown_ = true
  renderOverlay()
}

const hide = () => {
  framework_?.dispatchEvent('hide')
  isShown_ = false
  renderOverlay()
}

const cleanup = () => {
  hideCoachingOverlay()

  if (isShown_) {
    isShown_ = false
    framework_?.dispatchEvent('hide')
  }

  Object.assign(options_, defaultParameters)
}

const pipelineModule = () => ({
  name: 'hand-coaching-overlay',
  onAttach: ({framework}) => { framework_ = framework },
  onException: () => { cleanup() },
  onRemove: () => { cleanup() },
  onDetach: () => { cleanup() },
  listeners: [
    {event: 'handcontroller.handloading', process: showOverlay},
    {event: 'handcontroller.handfound', process: hide},
    {event: 'handcontroller.handlost', process: showOverlay},
  ],
})


export {
  configure,
  pipelineModule,
}
