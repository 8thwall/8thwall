import {pipelineModule, configure} from './module'
import {defaultParameters} from './parameters'

declare const XR8: any

const resolveSrc = (src: string) => {
  if (src && src.startsWith('#')) {
    return document.querySelector(src)?.getAttribute('src')
  } else {
    return src
  }
}

const aframeComponent = () => ({
  schema: {
    wayspotName: {type: 'string'},
    hintImage: {type: 'string'},
    promptColor: {type: 'string', default: defaultParameters.promptColor},
    animationColor: {type: 'string', default: defaultParameters.animationColor},
    animationDuration: {type: 'number', default: defaultParameters.animationDuration},
    promptPrefix: {type: 'string', default: defaultParameters.promptPrefix},
    promptSuffix: {type: 'string', default: defaultParameters.promptSuffix},
    statusText: {type: 'string', default: defaultParameters.statusText},
    disablePrompt: {type: 'boolean', default: defaultParameters.disablePrompt},
  },
  init() {
    const module = pipelineModule()
    this.moduleName = module.name

    // Emit the vps coaching overlay events via the AFrame scene.
    module.listeners.push(
      {
        event: 'vps-coaching-overlay.show',
        process: () => {
          this.el.sceneEl.emit('vps-coaching-overlay.show')
        },
      },
      {
        event: 'vps-coaching-overlay.hide',
        process: () => {
          this.el.sceneEl.emit('vps-coaching-overlay.hide')
        },
      }
    )
    XR8.addCameraPipelineModule(module)
  },
  update() {
    configure({...this.data, hintImage: resolveSrc(this.data.hintImage)})
  },
  remove() {
    XR8.removeCameraPipelineModule(this.moduleName)
  },
})

export {
  aframeComponent,
}
