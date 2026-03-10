import {isPartOf} from '../scene-graph'
import {makeGltfLoader} from '../../threejs-support'
import {ResourceUrls} from '../../resources'

const getBarWidth = screenWidth => Math.min(0.75 * screenWidth, 1.5)

const createDomTabletModel = () => {
  let modelRoot_
  let tabletLeft_
  let tabletRight_
  let tabletCenter_
  let tabletBarLeft_
  let tabletBarRight_
  let tabletBarCenter_

  let panelOffsetBasis_
  let initialCenterScale_
  let barOffsetBasis_

  let minimizeButton_
  let exitButton_

  let width_ = 1

  const updateAlignment = () => {
    // The panel width is slightly inset to handle the bezel
    const panelWidth = width_ - (1 - initialCenterScale_)
    const panelOffset = panelOffsetBasis_ * panelWidth
    tabletCenter_.scale.x = panelWidth
    tabletLeft_.position.x = -panelOffset
    tabletRight_.position.x = panelOffset

    const barWidth = getBarWidth(width_)
    const barOffset = barOffsetBasis_ * barWidth
    tabletBarCenter_.scale.x = barWidth
    tabletBarLeft_.position.x = -barOffset
    tabletBarRight_.position.x = barOffset
  }

  const modelPromise_ = new Promise((resolve) => {
    makeGltfLoader().load(ResourceUrls.resolveDomTabletFrameGlb(), (gltf) => {
      modelRoot_ = gltf.scene
      tabletLeft_ = modelRoot_.getObjectByName('tablet-left')
      tabletRight_ = modelRoot_.getObjectByName('tablet-right')
      tabletCenter_ = modelRoot_.getObjectByName('tablet-center')
      tabletBarLeft_ = modelRoot_.getObjectByName('tablet-bar-left')
      tabletBarRight_ = modelRoot_.getObjectByName('tablet-bar-right')
      tabletBarCenter_ = modelRoot_.getObjectByName('tablet-bar-center')
      minimizeButton_ = modelRoot_.getObjectByName('minus')
      exitButton_ = modelRoot_.getObjectByName('settings')

      modelRoot_.rotation.x = Math.PI / 2
      modelRoot_.scale.setScalar(1 / 8)  // This model was designed at an 8x scale
      modelRoot_.position.z = -0.01

      modelRoot_.renderOrder = -100

      initialCenterScale_ = tabletCenter_.scale.x
      panelOffsetBasis_ = tabletRight_.position.x / tabletCenter_.scale.x
      barOffsetBasis_ = tabletBarRight_.position.x / tabletBarCenter_.scale.x

      updateAlignment()
      resolve(modelRoot_)
    })
  })

  const isMinimizeButton = object => (!!minimizeButton_ && isPartOf(object, minimizeButton_))

  const isExitButton = object => (!!exitButton_ && isPartOf(object, exitButton_))
  const isHandle = object => (
    isPartOf(object, tabletBarLeft_) ||
    isPartOf(object, tabletBarCenter_) ||
    isPartOf(object, tabletBarRight_)
  )

  const setWidth = (width) => {
    width_ = width
    if (modelRoot_) {
      updateAlignment()
    }
  }

  return {
    setWidth,
    onLoad: () => modelPromise_,
    isMinimizeButton,
    isExitButton,
    isHandle,
  }
}

export {
  createDomTabletModel,
  getBarWidth,
}
