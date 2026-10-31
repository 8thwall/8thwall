// Adapted from reality/cloud/xrhome/src/client/apps/camera-edit-page.tsx.

import type {Object3D} from 'three'

type BoundingBox = {
  xmax: number
  xmin: number
  ymax: number
  ymin: number
  zmax: number
  zmin: number
}

const NO_BOUNDING_BOX: BoundingBox = {
  xmax: -Infinity,
  xmin: Infinity,
  ymax: -Infinity,
  ymin: Infinity,
  zmax: -Infinity,
  zmin: Infinity,
}

const mergeBoundingBoxes = (a: BoundingBox, b: BoundingBox) => ({
  xmax: Math.max(a.xmax, b.xmax),
  xmin: Math.min(a.xmin, b.xmin),
  ymax: Math.max(a.ymax, b.ymax),
  ymin: Math.min(a.ymin, b.ymin),
  zmax: Math.max(a.zmax, b.zmax),
  zmin: Math.min(a.zmin, b.zmin),
})

const objectBoundingBox = (object: Object3D) => {
  object.updateMatrixWorld()
  const b = new window.THREE.Box3().setFromObject(object)
  return {xmax: b.max.x, xmin: b.min.x, ymax: b.max.y, ymin: b.min.y, zmax: b.max.z, zmin: b.min.z}
}

const mergeObjectBoundingBoxes = (bb: BoundingBox, object: Object3D) => (
  mergeBoundingBoxes(objectBoundingBox(object), bb)
)

const sceneBoundingBox = (scene: Object3D) => {
  if (!scene.children || !scene.children.length) {
    return NO_BOUNDING_BOX
  }
  return scene.children.reduce(mergeObjectBoundingBoxes, NO_BOUNDING_BOX)
}

const mergeSceneBoundingBoxes = (bb: BoundingBox, scene: Object3D) => (
  mergeBoundingBoxes(sceneBoundingBox(scene), bb)
)

const getModelBoundingBox = (model: {scene: Object3D, scenes?: Object3D[]}) => {
  const scenes = model.scenes && model.scenes.length ? model.scenes : [model.scene]

  return scenes.reduce(mergeSceneBoundingBoxes, NO_BOUNDING_BOX)
}

export {
  getModelBoundingBox,
}
