import {aframeComponent as absoluteAframeComponent} from './absolute/aframe'
import {
  configure as absoluteConfigure,
  pipelineModule as absolutePipelineModule,
} from './absolute/module'

import {aframeComponent as vpsAframeComponent} from './vps/aframe'
import {configure as vpsConfigure, pipelineModule as vpsPipelineModule} from './vps/module'

import {aframeComponent as skyAframeComponent} from './sky/aframe'
import {
  configure as skyConfigure,
  pipelineModule as skyPipelineModule,
  control as skyControl,
} from './sky/module'

import {aframeComponent as handAframeComponent} from './hand/aframe'
import {
  configure as handConfigure,
  pipelineModule as handPipelineModule,
  control as handControl,
} from './hand/module'

const VpsCoachingOverlay = {
  configure: vpsConfigure,
  pipelineModule: vpsPipelineModule,
  aframeComponent: vpsAframeComponent,
}

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

const HandCoachingOverlay = {
  configure: handConfigure,
  pipelineModule: handPipelineModule,
  aframeComponent: handAframeComponent,
  control: handControl,
}

Object.assign(
  window,
  {VpsCoachingOverlay, CoachingOverlay, SkyCoachingOverlay, HandCoachingOverlay}
)

/* @ts-ignore */
if (window.AFRAME) {
  /* @ts-ignore */
  window.AFRAME.registerComponent('vps-coaching-overlay', vpsAframeComponent())
  /* @ts-ignore */
  window.AFRAME.registerComponent('coaching-overlay', absoluteAframeComponent())
  /* @ts-ignore */
  window.AFRAME.registerComponent('sky-coaching-overlay', skyAframeComponent())
  /* @ts-ignore */
  window.AFRAME.registerComponent('hand-coaching-overlay', handAframeComponent())
}
