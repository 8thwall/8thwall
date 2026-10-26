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
  values: Record<string, string>, specified: Record<string, unknown> = values
): StartFlowStringOverrides => Object.fromEntries(
  stringKeys
    .filter(key => specified[key] !== undefined)
    .map(key => [key, values[key]])
) as StartFlowStringOverrides

const startFlowComponent: ComponentDefinition = {
  schema: {
    previewSrc: {type: 'string'},
    showStartScreen: {default: false},
    waitForCameraTexture: {default: true},
    ...stringSchema,
  },
  init() {
    this.configure = () => {
      const {previewSrc, showStartScreen, waitForCameraTexture} = this.data
      XRExtras.StartFlow.reset()
      if (typeof previewSrc !== 'undefined') {
        XRExtras.StartFlow.configure({
          previewSrc,
        })
      }
      XRExtras.StartFlow.configure({
        showStartScreen,
        waitForCameraTexture,
      })
      XRExtras.StartFlow.setStringOverrides(
        getStartFlowStringOverrides(this.data, this.attrValue)
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
    XR8.removeCameraPipelineModule('startflow')
  },
}

export {getStartFlowStringOverrides, startFlowComponent}
