// Bridge between our 8thWall modules and Preact.

import * as Preact from 'preact'

import {VpsCoachingFlow} from './components/vps-coaching-flow'
import type {VpsCoachingOverlayUiConfig} from './parameters'

import './styles.scss'

let root: HTMLDivElement | null

// Creates the root element that will contain the Vps Coaching Overlay UI as children elements.
const showVpsCoachingOverlay = (params: VpsCoachingOverlayUiConfig) => {
  if (!root) {
    root = document.createElement('div')
    document.body.appendChild(root)
  }
  Preact.render(Preact.h(VpsCoachingFlow, params), root)
}

// Removes the root element that contains the Vps Coaching Overlay UI
const hideVpsCoachingOverlay = () => {
  if (!root) {
    return
  }
  Preact.render(null, root)
  root.parentNode?.removeChild(root)
  root = null
}

export {
  showVpsCoachingOverlay,
  hideVpsCoachingOverlay,
}
