import git from './git/git-reducer'
import editor from './editor/editor-reducer'
import imageTargets from './image-targets/reducer'
import helpCenter from './editor/product-tour/help-center-reducer'
import assetLab from './asset-lab/asset-lab-reducer'

const FULL_STACK_REDUCERS = {
  editor,
  git,
  imageTargets,
  helpCenter,
  assetLab,
} as const

export {
  FULL_STACK_REDUCERS,
}
