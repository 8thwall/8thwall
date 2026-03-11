import type {DeepReadonly} from 'ts-essentials'

import type {BaseGraphObject, GraphObject, SceneGraph} from '@ecs/shared/scene-graph'
import {
  getParentPrefabId, descopeObjectId, isPrefabInstance, scopeObjectIds, isScopedObjectId, hasParent,
  isPrefab, isBaseGraphObject, getParentInstanceId, objectExists,
} from '@ecs/shared/object-hierarchy'
import memoize from 'memoizee'

type ParentToChildren = DeepReadonly<Record<string, string[]>> | undefined

const getMergedInstanceChild = (
  scene: DeepReadonly<SceneGraph>, id: string
): DeepReadonly<BaseGraphObject> => {
  const [instanceId, sourceId] = descopeObjectId(id)

  const instanceObject = scene.objects[instanceId]
  const baseChildObject = scene.objects[sourceId]

  if (!isBaseGraphObject(baseChildObject)) {
    throw new Error(`Child object is not a base graph object: ${id}`)
  }

  if (!isPrefabInstance(instanceObject)) {
    throw new Error(`Instance object is a base graph object: ${id}`)
  }

  if (!baseChildObject.parentId) {
    throw new Error(`Child source object has no parent: ${id}`)
  }

  const parentId = baseChildObject.parentId === instanceObject.instanceData?.instanceOf
    ? instanceId
    : scopeObjectIds([instanceId, baseChildObject.parentId])

  const {
    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    id: baseId, parentId: baseParentId, prefab, ...baseChildRest
  } = baseChildObject

  if (!instanceObject.instanceData?.children || !instanceObject.instanceData.children[sourceId]) {
    return {id, parentId, ...baseChildRest}
  }

  const {
    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    deleted, deletions = {}, id: childId = undefined, ...childOverrides
  } = instanceObject.instanceData.children[sourceId] || {}

  const mergedChildObject = {
    id,
    parentId,
    ...baseChildRest,
  }

  // Apply overrides
  Object.entries(childOverrides).forEach(([key, value]) => {
    if (key !== 'components' && value !== null && value !== undefined) {
      (mergedChildObject as any)[key] = value
    }
  })

  if (childOverrides.components) {
    mergedChildObject.components = {
      ...baseChildObject.components,
      ...childOverrides.components,
    }
  }

  // Apply deletions
  Object.keys(deletions || {}).forEach((key) => {
    if (key !== 'components') {
      delete mergedChildObject[key as keyof typeof mergedChildObject]
    }
  })

  // Apply component deletions
  const components = {...mergedChildObject.components}
  const componentDeletions = deletions?.components || {}
  Object.keys(componentDeletions).forEach((key) => {
    if (key in components) {
      delete components[key as keyof typeof components]
    }
  })

  mergedChildObject.components = components
  return mergedChildObject
}

const getObjectOrMergedInstance = memoize((
  scene: DeepReadonly<SceneGraph>, id: string
): DeepReadonly<BaseGraphObject> => {
  if (isScopedObjectId(id)) {
    return getMergedInstanceChild(scene, id)
  }

  const instance = scene.objects[id]

  if (isBaseGraphObject(instance)) {
    return instance
  }

  const {instanceData, components: instanceComponents, ...instanceRest} = instance
  const {deletions, instanceOf} = instanceData!

  if (!scene.objects[instanceOf]) {
    return {
      components: instanceComponents,
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      geometry: null,
      material: null,
      ...instanceRest,
    }
  }

  const {
    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    id: prefabId, parentId, prefab: prefabFlag, components: prefabComponents, name, ...prefabRest
  } = getObjectOrMergedInstance(scene, instanceOf)

  const res = {
    id,
    name: instanceRest.name,
    components: {...prefabComponents, ...instanceComponents},
    ...prefabRest,
  }

  Object.keys(deletions).forEach((key) => {
    if (key !== 'components') {
      delete res[key as keyof typeof res]
    }
  })
  const componentDeletions = deletions?.components || {}
  Object.keys(componentDeletions).forEach((key) => {
    if (key in res.components) {
      delete res.components[key as keyof typeof res.components]
    }
  })

  Object.entries(instanceRest).forEach(([key, value]) => {
    if (value !== null && value !== undefined) {
      (res as any)[key] = value
    }
  })

  return res
})

const deriveFullObject = (scene: DeepReadonly<SceneGraph>, id: string) => (
  objectExists(scene, id) ? getObjectOrMergedInstance(scene, id) : undefined
)

const resolveObjectReference = (
  scene: DeepReadonly<SceneGraph>, objectId: string, targetId: string
) => {
  // TODO: Handle nested prefabs
  if (!objectId || !targetId) {
    return undefined
  }

  const parentInstanceId = getParentInstanceId(scene, objectId)
  if (!parentInstanceId) {
    return targetId
  }

  const target = deriveFullObject(scene, targetId)
  if (!target) {
    return undefined
  }

  const targetPrefabId = getParentPrefabId(scene, target.id)
  if (targetPrefabId !== scene.objects[parentInstanceId].instanceData!.instanceOf) {
    return targetId
  }

  return isPrefab(target)
    ? parentInstanceId
    : scopeObjectIds([parentInstanceId, target.id])
}

const filterValidChildren = (
  scene: DeepReadonly<SceneGraph>, instance: DeepReadonly<GraphObject>, prefabId: string,
  parentId: string, sourceId?: string
) => (e: string) => {
  const childParentId = instance.instanceData?.children?.[e]?.parentId
  return (
    childParentId === parentId ||
    (!childParentId && scene.objects[e]?.parentId === (sourceId || prefabId))
  ) && !instance.instanceData?.children?.[e]?.deleted
}

const getInstanceChildren = (
  scene: DeepReadonly<SceneGraph>, instanceId: string, parentToChildren: ParentToChildren
) => {
  const instance = scene.objects[instanceId]
  if (!isPrefabInstance(instance)) {
    return []
  }

  const prefabId = instance.instanceData.instanceOf
  // eslint-disable-next-line @typescript-eslint/no-use-before-define
  return getDescendantObjectIds(scene, prefabId, parentToChildren)
    .filter(filterValidChildren(scene, instance, prefabId, instanceId))
    .map(e => scopeObjectIds([instanceId, e]))
}

const getScopedObjectChildren = (
  scene: DeepReadonly<SceneGraph>, parentId: string, parentToChildren: ParentToChildren
) => {
  // TODO(Dale): Handle nested instances
  const [instanceId, sourceId] = descopeObjectId(parentId)
  const instance = scene.objects[instanceId]
  const prefabId = instance.instanceData?.instanceOf
  if (!prefabId) {
    return []
  }
  if (!isPrefabInstance(instance)) {
    return []
  }
  // eslint-disable-next-line @typescript-eslint/no-use-before-define
  return getDescendantObjectIds(scene, prefabId, parentToChildren)
    .filter(filterValidChildren(scene, instance, prefabId, parentId, sourceId))
    .map(e => scopeObjectIds([instanceId, e]))
}

const sortObjectIds = memoize((scene: DeepReadonly<SceneGraph>, ids: string[]) => (
  // TODO(Dale): Get order and ephemeral without merging the instance
  ids.sort((leftId, rightId) => {
    // eslint-disable-next-line @typescript-eslint/no-use-before-define
    const left = getObjectOrMergedInstance(scene, leftId)
    // eslint-disable-next-line @typescript-eslint/no-use-before-define
    const right = getObjectOrMergedInstance(scene, rightId)
    if (!!left.ephemeral !== !!right.ephemeral) {
      return left.ephemeral ? 1 : -1
    }
    return (left.order ?? 0) - (right.order ?? 0)
  })
))

const getChildObjectIds = memoize((
  scene: DeepReadonly<SceneGraph>, parentId: string, parentToChildren: ParentToChildren
) => {
  const childIds = parentToChildren
    ? [...parentToChildren[parentId] || []]
    : Object.keys(scene.objects).filter(e => scene.objects[e].parentId === parentId)
  const parent = scene.objects[parentId]
  if (parent && isPrefabInstance(parent)) {
    childIds.push(...getInstanceChildren(scene, parentId, parentToChildren))
  } else if (isScopedObjectId(parentId)) {
    childIds.push(...getScopedObjectChildren(scene, parentId, parentToChildren))
  }
  return sortObjectIds(scene, childIds)
})

const getDescendantObjectIds = (
  scene: DeepReadonly<SceneGraph>, parentId: string, parentToChildren: ParentToChildren
) => {
  const res: string[] = []

  const collectDescendants = (id: string) => {
    getChildObjectIds(scene, id, parentToChildren).forEach((childId) => {
      res.push(childId)
      collectDescendants(childId)
    })
  }

  collectDescendants(parentId)

  return sortObjectIds(scene, res)
}

const getTopLevelObjectIds = (
  scene: DeepReadonly<SceneGraph>, spaceId: string | undefined,
  parentToChildren: ParentToChildren
) => {
  if (spaceId) {
    return getChildObjectIds(scene, spaceId, parentToChildren)
  }
  return sortObjectIds(
    scene,
    Object.keys(scene.objects)
      .filter(e => !hasParent(scene, e) && !isPrefab(scene.objects[e]))
  )
}

const deepSortObjects = (
  scene: DeepReadonly<SceneGraph>, parentId: string | undefined,
  parentToChildren: ParentToChildren
) => {
  const res: string[] = []

  const sortAndFlatten = (id: string) => {
    res.push(id)
    const sortedChildIds = getChildObjectIds(scene, id, parentToChildren)
    sortedChildIds.forEach(sortAndFlatten)
  }

  const topLevelObjectIds = parentId
    ? getChildObjectIds(scene, parentId, parentToChildren)
    : getTopLevelObjectIds(scene, undefined, parentToChildren)

  topLevelObjectIds.forEach(id => sortAndFlatten(id))

  return res
}

const getSpaceObjects = (
  scene: DeepReadonly<SceneGraph>, spaceId: string | undefined,
  parentToChildren: ParentToChildren
) => (
  // eslint-disable-next-line @typescript-eslint/no-use-before-define
  deepSortObjects(scene, spaceId, parentToChildren).map(id => deriveFullObject(scene, id))
    .filter(Boolean) as DeepReadonly<BaseGraphObject>[]
)

const getObjectsOrMergedInstances = (
  scene: DeepReadonly<SceneGraph>,
  parentToChildren: ParentToChildren
): DeepReadonly<BaseGraphObject>[] => {
  const objectIds = Object.keys(scene.objects)
  const objects: DeepReadonly<BaseGraphObject>[] = []

  const collectMergedInstances = (id: string) => {
    if (!id) {
      return
    }
    const object = getObjectOrMergedInstance(scene, id)
    objects.push(object)
    const childIds = getChildObjectIds(scene, id, parentToChildren)
    childIds.forEach((childId) => {
      // prevent over counting baseGraphObjects
      if (!scene.objects[childId]) {
        collectMergedInstances(childId)
      }
    })
  }

  objectIds.forEach((id) => {
    const object = scene.objects[id]
    if (object && isBaseGraphObject(object)) {
      objects.push(object)
    } else if (object && isPrefabInstance(object)) {
      collectMergedInstances(id)
    }
  })

  return objects
}

const hasInstanceDescendant = (
  scene: DeepReadonly<SceneGraph>, objectId: string, parentToChildren: ParentToChildren
): boolean => {
  if (!objectExists(scene, objectId)) {
    return false
  }
  const object = scene.objects[objectId]
  if (object && isPrefabInstance(object)) {
    return true
  }

  return getChildObjectIds(scene, objectId, parentToChildren)
    .some(childId => hasInstanceDescendant(scene, childId, parentToChildren))
}

const getAllCamerasFromSceneGraph = (
  sceneGraph: DeepReadonly<SceneGraph>
): DeepReadonly<GraphObject>[] => Object.values(sceneGraph.objects)
  .filter(obj => !!obj.camera)

const getCamerasForSpace = (
  sceneGraph: DeepReadonly<SceneGraph>, spaceId: string | undefined
): DeepReadonly<GraphObject>[] => getSpaceObjects(sceneGraph, spaceId, undefined)
  .filter(obj => !!obj.camera)

export {
  getObjectOrMergedInstance,
  deriveFullObject,
  getChildObjectIds,
  getTopLevelObjectIds,
  getSpaceObjects,
  getObjectsOrMergedInstances,
  deepSortObjects,
  sortObjectIds,
  resolveObjectReference,
  hasInstanceDescendant,
  getDescendantObjectIds,
  getAllCamerasFromSceneGraph,
  getCamerasForSpace,
}

export type {
  ParentToChildren,
}
