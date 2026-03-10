import {BASE_TABLET_SCALE} from './constants'
import {makeGltfLoader} from '../../threejs-support'
import {ResourceUrls} from '../../resources'

const createDomWristModel = () => {
  let modelRoot_
  let maximizeIcon_
  let minimizeIcon_
  let isMinimized_ = false

  const updateMinimized = () => {
    if (maximizeIcon_) {
      maximizeIcon_.visible = isMinimized_
    }
    if (minimizeIcon_) {
      minimizeIcon_.visible = !isMinimized_
    }
  }

  // TODO(christoph): Get final model
  const modelPromise_ = new Promise((resolve) => {
    makeGltfLoader().load(ResourceUrls.resolveDomTabletButtonGlb(), (gltf) => {
      modelRoot_ = gltf.scene
      maximizeIcon_ = modelRoot_.getObjectByName('plus-icon')
      minimizeIcon_ = modelRoot_.getObjectByName('minus-icon')

      modelRoot_.rotation.x = Math.PI / 2

      // Scale to match the size of the button on the tablet
      modelRoot_.scale.setScalar(BASE_TABLET_SCALE / 8)

      updateMinimized()

      resolve(modelRoot_)
    })
  })

  const setMinimized = (isMinimized) => {
    isMinimized_ = isMinimized
    updateMinimized()
  }

  return {
    setMinimized,
    onLoad: () => modelPromise_,
  }
}

export {
  createDomWristModel,
}
