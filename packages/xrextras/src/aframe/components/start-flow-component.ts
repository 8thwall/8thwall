import type {ComponentDefinition} from 'aframe'

import {defaultStrings} from '../../startflow/strings'
import type {StartFlowOptions, StartFlowStringOverrides} from '../../startflow/types'

declare const XRExtras: any
declare const XR8: any

const stringKeys = Object.keys(defaultStrings) as Array<keyof typeof defaultStrings>
const stringSchema = Object.fromEntries(
  stringKeys.map(key => [key, {type: 'string'}])
)

const getStartFlowStringOverrides = (
  values: Record<string, string>
): StartFlowStringOverrides => Object.fromEntries(
  stringKeys
    .filter(key => values[key] !== undefined)
    .map(key => [key, values[key]])
) as StartFlowStringOverrides

const assetSrc = (value: string | {src?: string} | null): string | null => (
  typeof value === 'string' ? value : value?.src ?? null
)

const startFlowComponent: ComponentDefinition = {
  schema: {
    previewSrc: {type: 'asset'},
    showStartScreen: {default: false},
    waitForCameraTexture: {default: true},
    url: {type: 'string'},
    mediaSrc: {type: 'asset'},
    logoSrc: {type: 'asset'},
    mediaAutoplay: {default: true},
    mediaAnimation: {type: 'string'},
    mediaControls: {default: 'minimal'},
    sceneEnvMap: {default: 'field'},
    sceneOrbitIdle: {default: 'spin'},
    sceneOrbitInteraction: {default: 'drag'},
    sceneLightingIntensity: {default: 1},
    ...stringSchema,
  },
  init() {
    this.configure = () => {
      const {
        previewSrc,
        showStartScreen,
        waitForCameraTexture,
        url,
        mediaSrc,
        logoSrc,
        mediaAutoplay,
        mediaAnimation,
        mediaControls,
        sceneEnvMap,
        sceneOrbitIdle,
        sceneOrbitInteraction,
        sceneLightingIntensity,
      } = this.data
      XRExtras.StartFlow.reset()
      const options: Partial<StartFlowOptions> = {
        showStartScreen,
        waitForCameraTexture,
        mediaAutoplay,
        mediaAnimation,
        mediaControls,
        sceneEnvMap,
        sceneOrbitIdle,
        sceneOrbitInteraction,
        sceneLightingIntensity,
      }
      if (typeof this.attrValue.previewSrc !== 'undefined') {
        options.previewSrc = assetSrc(previewSrc)
      }
      if (typeof this.attrValue.url !== 'undefined') {
        options.url = url
      }
      if (typeof this.attrValue.mediaSrc !== 'undefined') {
        options.mediaSrc = assetSrc(mediaSrc)
      }
      if (typeof this.attrValue.logoSrc !== 'undefined') {
        options.logoSrc = assetSrc(logoSrc)
      }
      XRExtras.StartFlow.configure(options)
      XRExtras.StartFlow.setStringOverrides(
        getStartFlowStringOverrides(this.attrValue)
      )
    }
    this.load = () => {
      if (this.removed) {
        return
      }
      this.configure()
      XR8.addCameraPipelineModule(XRExtras.StartFlow.pipelineModule())
      this.loaded = true
    }
    if (window.XRExtras && window.XR8) {
      this.load()
    } else {
      window.addEventListener('xrandextrasloaded', this.load, {once: true})
    }
  },
  update() {
    if (!this.loaded) {
      return
    }
    this.configure()
  },
  remove() {
    if (window.XRExtras) {
      XRExtras.StartFlow.hide()
      XRExtras.StartFlow.reset()
    }
    this.removed = true
    window.removeEventListener('xrandextrasloaded', this.load)
    if (this.loaded) {
      XR8.removeCameraPipelineModule('startflow')
    }
  },
}

export {getStartFlowStringOverrides, startFlowComponent}
