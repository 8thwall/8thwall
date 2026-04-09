import {showAlmostThereCollisionError} from './almost-there'
import {configure, pipelineModule} from './module'
import {defaultParameters} from './parameters'

declare const XR8: any

const generateSchema = () => {
  const schema = {}
  Object.keys(defaultParameters).forEach((param) => {
    schema[param] = {default: defaultParameters[param]}
  })
  return schema
}

const resolveSrc = (src: string) => {
  if (src && src.startsWith('#')) {
    return document.querySelector(src)?.getAttribute('src')
  } else {
    return src
  }
}

const aframeComponent = () => ({
  schema: generateSchema(),
  init() {
    const module = pipelineModule()
    this.moduleName = module.name
    XR8.addCameraPipelineModule(module)

    if (this.el.attributes['xrextras-almost-there']) {
      showAlmostThereCollisionError()
    }
  },
  update() {
    configure({
      ...this.data,
      mediaSrc: resolveSrc(this.data.mediaSrc),
      logoSrc: resolveSrc(this.data.logoSrc),
      backgroundSrc: resolveSrc(this.data.backgroundSrc),
      sceneEnvMap: resolveSrc(this.data.sceneEnvMap),
    })
  },
  remove() {
    XR8.removeCameraPipelineModule(this.moduleName)
  },
})

export {
  aframeComponent,
}
