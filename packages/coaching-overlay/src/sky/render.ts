// Bridge between our 8thWall modules and Preact.

import * as Preact from 'preact'

import {CoachingFlow} from './components/coaching-flow'
import type {CoachingOverlayUiConfig} from './parameters'

import './styles.scss'

let root: HTMLDivElement | null

// Creates the root element that will contain the Coaching Overlay UI as children elements.
const showCoachingOverlay = (params: CoachingOverlayUiConfig) => {
  if (!root) {
    root = document.createElement('div')
    document.body.appendChild(root)
  }
  Preact.render(Preact.h(CoachingFlow, params), root)
}

// Removes the root element that contains the Coaching Overlay UI
const hideCoachingOverlay = () => {
  if (!root) {
    return
  }
  Preact.render(null, root)
  root.parentNode?.removeChild(root)
  root = null
}

export {
  showCoachingOverlay,
  hideCoachingOverlay,
}
