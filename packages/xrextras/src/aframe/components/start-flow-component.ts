import type {ComponentDefinition} from 'aframe'

import {defaultStrings} from '../../startflow/strings'
import type {StartFlowStringOverrides} from '../../startflow/types'

declare const XRExtras: any
declare const XR8: any

const stringKeys = Object.keys(defaultStrings) as Array<keyof typeof defaultStrings>
const stringSchema = Object.fromEntries(
  stringKeys.map(key => [key, {type: 'string'}])
)

const startFlowComponent: ComponentDefinition = {
  schema: {
    previewSrc: {default: ''},
    showStartScreen: {default: false},
    ...stringSchema,
  },
  init() {
    const configureStrings = () => {
      const overrides = Object.fromEntries(
        stringKeys
          .filter(key => this.attrValue[key] !== undefined)
          .map(key => [key, this.data[key]])
      ) as StartFlowStringOverrides
      if (!Object.keys(overrides).length && !this.startFlowStringsConfigured) {
        return
      }
      XRExtras.StartFlow.setStringOverrides(overrides)
      this.startFlowStringsConfigured = true
    }
    const load = () => {
      if (this.startFlowRemoved) {
        return
      }
      const {previewSrc, showStartScreen} = this.data
      const waitForCameraTexture =
        !!(this.el.sceneEl.attributes.xrweb ||
          this.el.sceneEl.attributes.xrface ||
          this.el.sceneEl.attributes.xrlayers)
      XRExtras.StartFlow.configure({
        ...(previewSrc ? {previewSrc} : {}),
        showStartScreen,
        waitForCameraTexture,
      })
      configureStrings()
      XR8.addCameraPipelineModule(XRExtras.StartFlow.pipelineModule())
      this.startFlowLoaded = true
    }
    this.configureStartFlowStrings = configureStrings
    this.loadStartFlow = load
    if (window.XRExtras && window.XR8) {
      load()
    } else {
      window.addEventListener('xrandextrasloaded', load, {once: true})
    }
  },
  update(oldData) {
    if (!this.startFlowLoaded || !Object.keys(oldData).length) {
      return
    }
    const options: {previewSrc?: string | null, showStartScreen?: boolean} = {}
    if (this.data.previewSrc !== oldData.previewSrc) {
      options.previewSrc = this.data.previewSrc || null
    }
    if (this.data.showStartScreen !== oldData.showStartScreen) {
      options.showStartScreen = this.data.showStartScreen
    }
    if (Object.keys(options).length) {
      XRExtras.StartFlow.configure(options)
    }
    this.configureStartFlowStrings()
  },
  remove() {
    this.startFlowRemoved = true
    window.removeEventListener('xrandextrasloaded', this.loadStartFlow)
    if (!this.startFlowLoaded) {
      return
    }
    XRExtras.StartFlow.hide()
    if (this.startFlowStringsConfigured) {
      XRExtras.StartFlow.setStringOverrides({})
    }
    XR8.removeCameraPipelineModule('startflow')
  },
}

export {startFlowComponent}
