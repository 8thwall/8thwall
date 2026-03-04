import type * as THREE_TYPES from '../runtime/three-types'

// eslint-disable-next-line max-len
// based on https://github.com/mrdoob/three.js/blob/70d0eb132f3d2f87965bee2e13a38508b839ed9b/src/core/Raycaster.js#L96
const customRaycastSort = (a: THREE_TYPES.Intersection, b: THREE_TYPES.Intersection): number => {
  const rootA = a.object.userData.rootUi
  const rootB = b.object.userData.rootUi
  if (rootA && rootA === rootB) {
    // If both objects are part of the same UI root, sort by their order
    const aStackingContext = a.object.userData.stackingContext ?? 0
    const bStackingContext = b.object.userData.stackingContext ?? 0
    if (aStackingContext !== bStackingContext) {
      return bStackingContext - aStackingContext
    }
    const aUiOrder = a.object.userData.uiOrder ?? 0
    const bUiOrder = b.object.userData.uiOrder ?? 0
    return bUiOrder - aUiOrder
  } else if (a.distance !== b.distance) {
    return a.distance - b.distance
  } else if (rootA !== rootB) {
    // note(owenmech): if two UI elements have different root UIs but are the same distance,
    // (or one is UI and one isn't), we must break the tie the same way the roots themselves would
    // to ensure that UI families are grouped together
    return (rootA?.id ?? a.object.id) - (rootB?.id ?? b.object.id)
  }
  return a.object.id - b.object.id
}

const makeRaycastIntersections = (
  initialVal: Array<THREE_TYPES.Intersection<THREE_TYPES.Object3D>> = []
) => {
  const raycastIntersections = initialVal
  raycastIntersections.sort = () => Array.prototype.sort.call(
    raycastIntersections, customRaycastSort
  )
  return raycastIntersections
}

type RenderItem = THREE_TYPES.WebGLRenderList['transparent'][number]
type RenderList8w = THREE_TYPES.WebGLRenderList & {
  objectIdToRenderItem: WeakMap<THREE_TYPES.Object3D, RenderItem>
}

// based on https://github.com/mrdoob/three.js/blob/r172/src/renderers/webgl/WebGLRenderLists.js#L27
const getReversePainterSortWithUi = (
  renderer: THREE_TYPES.WebGLRenderer, scene: THREE_TYPES.Scene
) => {
  const renderList = renderer.renderLists.get(scene, 0) as RenderList8w
  return (a: RenderItem, b: RenderItem): number => {
    // note(owenmech): to group UI elements together, sort them as if they were their root.
    const rootA = renderList.objectIdToRenderItem.get(a.object.userData.rootUi?.frame.id)
    const rootB = renderList.objectIdToRenderItem.get(b.object.userData.rootUi?.frame.id)
    const zEffectiveA = rootA?.z ?? a.z
    const zEffectiveB = rootB?.z ?? b.z

    if (a.groupOrder !== b.groupOrder) {
      return a.groupOrder - b.groupOrder
    } else if (a.renderOrder !== b.renderOrder) {
      return a.renderOrder - b.renderOrder
    } else if (rootA && rootA.id === rootB?.id) {
      // note(owenmech): when UI elements are in the same family, apply the custom UI sorting
      const aStackingContext = a.object.userData.stackingContext ?? 0
      const bStackingContext = b.object.userData.stackingContext ?? 0
      if (aStackingContext !== bStackingContext) {
        return aStackingContext - bStackingContext
      }
      const aUiOrder = a.object.userData.uiOrder ?? 0
      const bUiOrder = b.object.userData.uiOrder ?? 0
      if (aUiOrder !== bUiOrder) {
        return aUiOrder - bUiOrder
      } else if (a.object.userData.isText !== b.object.userData.isText) {
        return a.object.userData.isText ? 1 : -1
      } else {
        return a.id - b.id
      }
    } else if (zEffectiveA !== zEffectiveB) {
      return zEffectiveB - zEffectiveA
    } else {
      // note(owenmech): if two UI elements have different root UIs which happen to have the same Z
      // (or one is UI and one isn't), we must break the tie the same way the roots themselves would
      // to ensure that UI families are grouped together, as IDs are more-or-less arbitrary.
      return (rootA?.id ?? a.id) - (rootB?.id ?? b.id)
    }
  }
}

export {
  customRaycastSort,
  makeRaycastIntersections,
  getReversePainterSortWithUi,
}
