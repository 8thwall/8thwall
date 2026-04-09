import {aframeComponent} from './aframe'
import {configure, pipelineModule} from './module'

const LandingPage = {
  configure,
  pipelineModule,
  aframeComponent,
}

Object.assign(window, {LandingPage})

/* @ts-ignore */
if (window.AFRAME) {
  /* @ts-ignore */
  window.AFRAME.registerComponent('landing-page', aframeComponent())
}
