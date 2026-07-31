import {pipelineModule, configure} from './module'
import {defaultParameters} from './parameters'

declare const XR8: any

const aframeComponent = () => ({
  schema: {
    promptText: {type: 'string', default: defaultParameters.promptText},
    promptColor: {type: 'string', default: defaultParameters.promptColor},
    animationColor: {type: 'string', default: defaultParameters.animationColor},
    disablePrompt: {type: 'boolean', default: defaultParameters.disablePrompt},
  },
  init() {
    const module = pipelineModule()
    this.moduleName = module.name

    // Emit the coaching overlay events via the AFrame scene.
    module.listeners.push(
      {
        event: 'coaching-overlay.show',
        process: () => {
          this.el.sceneEl.emit('coaching-overlay.show')
        },
      },
      {
        event: 'coaching-overlay.hide',
        process: () => {
          this.el.sceneEl.emit('coaching-overlay.hide')
        },
      }
    )
    XR8.addCameraPipelineModule(module)
  },
  update() {
    configure({...this.data})
  },
  remove() {
    XR8.removeCameraPipelineModule(this.moduleName)
  },
})

export {
  aframeComponent,
}
