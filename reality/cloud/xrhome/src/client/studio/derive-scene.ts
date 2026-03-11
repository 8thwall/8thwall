import type {DeepReadonly} from 'ts-essentials'
import memoize from 'memoizee'

import type {BaseGraphObject, GraphObject, SceneGraph, Space} from '@ecs/shared/scene-graph'
import {
  getAllIncludedSpaces, resolveSpaceForObject, getParentPrefabId, getPrefabs,
} from '@ecs/shared/object-hierarchy'
import {
  getActiveCameraFromSceneGraph,
} from '@ecs/shared/get-camera-from-scene-graph'

import {
  deriveFullObject, getChildObjectIds, getTopLevelObjectIds, getSpaceObjects,
  getObjectsOrMergedInstances, deepSortObjects, sortObjectIds, resolveObjectReference,
  hasInstanceDescendant, getDescendantObjectIds, getAllCamerasFromSceneGraph, getCamerasForSpace,
  ParentToChildren,
} from './derive-scene-operations'

type DerivedScene = {
  getObject: (id: string) => DeepReadonly<BaseGraphObject> | undefined
  getChildren: (id: string) => string[]
  getTopLevelObjectIds: (spaceId?: string) => string[]
  getSpaceObjects: (spaceId?: string) => DeepReadonly<BaseGraphObject>[]
  getAllIncludedSpaces: (spaceId?: string) => string[]
  getAllSceneObjects: () => DeepReadonly<BaseGraphObject>[]
  deepSortObjects: (spaceId?: string) => string[]
  sortObjectIds: (ids: string[]) => string[]
  resolveSpaceForObject: (id: string) => DeepReadonly<Space> | undefined
  resolveObjectReference: (objectId: string, targetId: string) => string | undefined
  getPrefabs: () => DeepReadonly<GraphObject>[]
  hasInstanceDescendant: (id: string) => boolean
  getDescendantObjectIds: (id: string) => string[]
  getParentPrefabId: (id: string) => string
  getActiveCamera: (spaceId?: string) => DeepReadonly<GraphObject> | undefined
  getAllCameras: () => DeepReadonly<GraphObject>[]
  getCamerasForSpace: (spaceId?: string) => DeepReadonly<GraphObject>[]
}

const deriveScene = (scene: DeepReadonly<SceneGraph>): DerivedScene => {
  const parentToChildren: ParentToChildren = Object.values(scene.objects).reduce(
    (acc, object) => {
      const {id, parentId} = object
      if (parentId) {
        if (!acc[parentId]) {
          acc[parentId] = []
        }
        acc[parentId].push(id)
      }
      return acc
    },
    {} as Record<string, string[]>
  )

  return {
    getObject: (id: string) => deriveFullObject(scene, id),
    getChildren: (id: string) => getChildObjectIds(scene, id, parentToChildren),
    getTopLevelObjectIds: (
      (spaceId?: string) => getTopLevelObjectIds(scene, spaceId, parentToChildren)
    ),
    getSpaceObjects: (spaceId?: string) => getSpaceObjects(scene, spaceId, parentToChildren),
    getAllIncludedSpaces: (spaceId?: string) => getAllIncludedSpaces(scene, spaceId),
    getAllSceneObjects: () => getObjectsOrMergedInstances(scene, parentToChildren),
    deepSortObjects: (parentId?: string) => deepSortObjects(scene, parentId, parentToChildren),
    sortObjectIds: (ids: string[]) => sortObjectIds(scene, ids),
    resolveSpaceForObject: (id: string) => resolveSpaceForObject(scene, id),
    resolveObjectReference: (objectId: string, targetId: string) => (
      resolveObjectReference(scene, objectId, targetId)
    ),
    getPrefabs: () => getPrefabs(scene),
    hasInstanceDescendant: (id: string) => hasInstanceDescendant(scene, id, parentToChildren),
    getDescendantObjectIds: (id: string) => getDescendantObjectIds(scene, id, parentToChildren),
    getParentPrefabId: memoize((id: string) => getParentPrefabId(scene, id)),
    getActiveCamera: memoize((spaceId?: string) => getActiveCameraFromSceneGraph(scene, spaceId)),
    getAllCameras: () => getAllCamerasFromSceneGraph(scene),
    getCamerasForSpace: (spaceId?: string) => getCamerasForSpace(scene, spaceId),
  }
}

export {deriveScene, type DerivedScene}
