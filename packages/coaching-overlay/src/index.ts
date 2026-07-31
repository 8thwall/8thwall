import {aframeComponent as absoluteAframeComponent} from './absolute/aframe'
import {
  configure as absoluteConfigure,
  pipelineModule as absolutePipelineModule,
} from './absolute/module'

import {aframeComponent as skyAframeComponent} from './sky/aframe'
import {
  configure as skyConfigure,
  pipelineModule as skyPipelineModule,
  control as skyControl,
} from './sky/module'

const CoachingOverlay = {
  configure: absoluteConfigure,
  pipelineModule: absolutePipelineModule,
  aframeComponent: absoluteAframeComponent,
}

const SkyCoachingOverlay = {
  configure: skyConfigure,
  pipelineModule: skyPipelineModule,
  aframeComponent: skyAframeComponent,
  control: skyControl,
}

Object.assign(
  window,
  {CoachingOverlay, SkyCoachingOverlay}
)
/* @ts-ignore */
if (window.AFRAME) {
  /* @ts-ignore */
  window.AFRAME.registerComponent('coaching-overlay', absoluteAframeComponent())
  /* @ts-ignore */
  window.AFRAME.registerComponent('sky-coaching-overlay', skyAframeComponent())
}
