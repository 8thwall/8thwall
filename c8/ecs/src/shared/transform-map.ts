import type {DeepReadonly} from 'ts-essentials'

import type {SceneGraph} from './scene-graph'

// position: 0 - 2, rotation: 3 - 6, scale: 7 - 9
type Transform = [number, number, number, number, number, number, number, number, number, number]

interface TransformMap {
  [key: string]: Transform
}

const TRANSFORM_MAP_VERSION = 2

const isTransformEqual = (prev: Transform, next: Transform) => (
  prev.every((value, index) => value === next[index])
)

const getTransformMap = (sceneGraph: DeepReadonly<SceneGraph>): TransformMap => {
  const transformMap: TransformMap = {}
  Object.entries(sceneGraph.objects).forEach(([id, object]) => {
    transformMap[id] = [...object.position, ...object.rotation, ...object.scale]
  })
  return transformMap
}

const getSceneGraphWithTransforms = (
  scene: DeepReadonly<SceneGraph>, transformMap: TransformMap
) => {
  const newScene = {...scene, objects: {...scene.objects}}
  newScene.objects = {...scene.objects}
  Object.entries(scene.objects).forEach(([id, object]) => {
    const transform = transformMap[id]
    if (transform) {
      newScene.objects[id] = {
        ...object,
        position: [transform[0], transform[1], transform[2]],
        rotation: [transform[3], transform[4], transform[5], transform[6]],
        scale: [transform[7], transform[8], transform[9]],
      }
    }
  })
  return newScene
}

const getTransformUpdates = (prevMap: TransformMap, nextMap: TransformMap) => {
  const nextObjectIds = new Set(Object.keys(nextMap))

  const updatedTransforms: TransformMap = {}
  const deletedIds: string[] = []

  Object.keys(prevMap).forEach((id) => {
    if (!nextObjectIds.has(id)) {
      deletedIds.push(id)
    } else {
      const prevTransform = prevMap[id]
      const nextTransform = nextMap[id]
      if (!isTransformEqual(prevTransform, nextTransform)) {
        updatedTransforms[id] = nextTransform
      }
      nextObjectIds.delete(id)
    }
  })

  nextObjectIds.forEach((id) => {
    updatedTransforms[id] = nextMap[id]
  })

  return {updatedTransforms, deletedIds}
}

const applyTransformUpdates = (
  currentMap: TransformMap, updatedTransforms: TransformMap, deletedIds: string[]
) => {
  const newMap = {...currentMap}
  deletedIds.forEach((id) => {
    delete newMap[id]
  })
  Object.entries(updatedTransforms).forEach(([id, transform]) => {
    newMap[id] = transform
  })
  return newMap
}

export {
  getTransformMap,
  getSceneGraphWithTransforms,
  TRANSFORM_MAP_VERSION,
  getTransformUpdates,
  applyTransformUpdates,
}

export type {
  TransformMap,
}
