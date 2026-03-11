import type {DeepReadonly} from 'ts-essentials'
import {Matrix4, Quaternion, Vector3} from 'three'
import type {GraphObject} from '@ecs/shared/scene-graph'

import type {DerivedScene} from './derive-scene'

const getAncestorMatrix = (object: DeepReadonly<GraphObject>, derivedScene: DerivedScene) => {
  const matrix = new Matrix4()
  let parent = derivedScene.getObject(object.parentId)
  while (parent) {
    matrix.premultiply(new Matrix4().compose(
      new Vector3(...parent.position),
      new Quaternion().fromArray(parent.rotation),
      new Vector3(...parent.scale)
    ))
    parent = derivedScene.getObject(parent.parentId)
  }
  return matrix
}

const getObjectInWorldMatrix = (
  object: DeepReadonly<GraphObject>,
  derivedScene: DerivedScene
) => {
  const matrix = new Matrix4()
  matrix.compose(
    new Vector3(...object.position),
    new Quaternion().fromArray(object.rotation),
    new Vector3(...object.scale)
  )
  return matrix.premultiply(getAncestorMatrix(object, derivedScene))
}

const computeGizmoCenter = (
  objects: DeepReadonly<GraphObject>[],
  derivedScene: DerivedScene
) => {
  const sum = new Vector3()
  objects.forEach((object) => {
    const matrixInWorld = getObjectInWorldMatrix(object, derivedScene)
    const objectVector = new Vector3().setFromMatrixPosition(matrixInWorld)
    sum.add(objectVector)
  })
  return sum.divideScalar(objects.length)
}

export {
  computeGizmoCenter,
  getAncestorMatrix,
  getObjectInWorldMatrix,
}
