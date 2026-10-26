import type {ComponentDefinition} from 'aframe'

import {defaultStrings} from '../../startflow/strings'
import type {StartFlowStringOverrides} from '../../startflow/types'

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

const startFlowComponent: ComponentDefinition = {
  schema: {
    previewSrc: {type: 'asset'},
    showStartScreen: {default: false},
    waitForCameraTexture: {default: true},
    ...stringSchema,
  },
  init() {
    this.configure = () => {
      const {previewSrc, showStartScreen, waitForCameraTexture} = this.data
      XRExtras.StartFlow.reset()
      if (typeof this.attrValue.previewSrc !== 'undefined') {
        XRExtras.StartFlow.configure({
          previewSrc: typeof previewSrc === 'string' ? previewSrc : previewSrc.src,
        })
      }
      XRExtras.StartFlow.configure({
        showStartScreen,
        waitForCameraTexture,
      })
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
    this.removed = true
    window.removeEventListener('xrandextrasloaded', this.load)
    if (!this.loaded) {
      return
    }
    XRExtras.StartFlow.hide()
    XRExtras.StartFlow.reset()
    XR8.removeCameraPipelineModule('startflow')
  },
}

export {getStartFlowStringOverrides, startFlowComponent}
