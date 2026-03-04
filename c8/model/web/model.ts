import {AFrameModelManager} from './aframe-model-manager'
import {setInternalConfig} from './internal-config'
import {ModelManager} from './model-manager'
import {ThreejsModelManager} from './threejs-model-manager'
import {WebModelView} from './web-model-view'

const Model = {
  AFrameModelManager,
  ModelManager,
  setInternalConfig,  // This interface should be removed for external consumption.
  ThreejsModelManager,
  WebModelView,
}

export {
  Model,
}
