import type {DeepReadonly} from 'ts-essentials'

import type {
  BaseGraphObject, GraphObject, InstanceData, SceneGraph, Space,
} from './scene-graph'

const descopeObjectId = (id: string): string[] => id.split('/')

const scopeObjectIds = (ids: string[]): string => ids.join('/')

const isScopedObjectId = (id: string) => id.includes('/')

const getInstanceAndScopedId = (id: string) => {
  if (!isScopedObjectId(id)) {
    throw new Error(`getInstanceAndScopedId called with non-scoped id: ${id}`)
  }

  const ids = descopeObjectId(id)
  const instanceId = ids[0]
  const instanceScopedId = ids.slice(1).join('/')
  return [instanceId, instanceScopedId]
}

const getIdOrInstanceId = (id: string) => {
  if (id && isScopedObjectId(id)) {
    const [instanceId] = descopeObjectId(id)
    return instanceId
  }
  return id
}

const scopedObjectExists = (scene: DeepReadonly<SceneGraph>, id: string) => {
  if (!isScopedObjectId(id)) {
    return false
  }
  const [instanceId, sourceId] = descopeObjectId(id)
  return scene.objects[instanceId] && scene.objects[sourceId] &&
    // eslint-disable-next-line @typescript-eslint/no-use-before-define
    getParentPrefabId(scene, sourceId) === scene.objects[instanceId].instanceData?.instanceOf
}

const isValidSpaceId = (scene: DeepReadonly<SceneGraph>, id: string) => (
  !!scene.spaces && !!scene.spaces[id]
)

const hasParent = (scene: DeepReadonly<SceneGraph>, id: string) => {
  const object = scene.objects[id]
  if (!object) {
    return false
  }
  let {parentId} = object

  if (parentId && isScopedObjectId(parentId)) {
    if (!scopedObjectExists(scene, parentId)) {
      return false
    }
    const [instanceId] = descopeObjectId(parentId)
    const instance = scene.objects[instanceId]
    parentId = instance.parentId
  }

  return !!parentId && (!!scene.objects[parentId] || isValidSpaceId(scene, parentId))
}

const isPrefab = (object: DeepReadonly<GraphObject>) => !!object.prefab

const isPrefabInstance = (
  object: DeepReadonly<GraphObject>
): object is DeepReadonly<GraphObject & {instanceData: InstanceData}> => !!object.instanceData

const getParentInstanceId = (scene: DeepReadonly<SceneGraph>, id: string) => {
  if (!isScopedObjectId(id)) {
    return scene.objects[id] && isPrefabInstance(scene.objects[id]) ? id : undefined
  }

  return getIdOrInstanceId(id)
}

const getSourceObjectId = (
  scene: DeepReadonly<SceneGraph>, objectId: string
): string | undefined => (isScopedObjectId(objectId)
  ? descopeObjectId(objectId).at(-1)
  : scene.objects[objectId]?.instanceData?.instanceOf)

const getParentPrefabId = (scene: DeepReadonly<SceneGraph>, initialId: string) => {
  let id = initialId

  while (id) {
    const object = scene.objects[getIdOrInstanceId(id)]
    if (object && isPrefab(object)) {
      return object.id
    }
    if (object && object.parentId) {
      id = scene.objects[object.parentId]?.id
    } else {
      return undefined
    }
  }

  return undefined
}

const isBaseGraphObject = (
  object: DeepReadonly<GraphObject>
): object is DeepReadonly<BaseGraphObject> => (
  !isPrefabInstance(object)
)

const isMissingPrefab = (scene: DeepReadonly<SceneGraph>, id: string) => {
  const object = scene.objects[id]
  if (!object) {
    return false
  }
  if (!isPrefabInstance(object)) {
    return false
  }
  return !scene.objects[object.instanceData.instanceOf]
}

const resolveSpaceForObject = (
  scene: DeepReadonly<SceneGraph>, objectId: string
): DeepReadonly<Space> | undefined => {
  const idOrInstanceId = getIdOrInstanceId(objectId)
  let parentId = scene.objects[idOrInstanceId]?.parentId
  while (parentId) {
    const space = scene.spaces?.[parentId]
    if (space) {
      return space
    }
    parentId = scene.objects[parentId]?.parentId
  }
  return undefined
}

const resolveSpaceIdForObjectOrSpace = (
  sceneGraph: DeepReadonly<SceneGraph>, id: string | undefined
) => {
  if (!id) {
    return undefined
  }

  if (isValidSpaceId(sceneGraph, id)) {
    return id
  }

  return resolveSpaceForObject(sceneGraph, id)?.id
}

const resolveSpaceId = (sceneGraph: DeepReadonly<SceneGraph>, spaceIdOrName: string) => {
  if (!sceneGraph.spaces || !spaceIdOrName) {
    return undefined
  }
  if (sceneGraph.spaces[spaceIdOrName]) {
    return spaceIdOrName
  }

  const nameMatches = Object.values(sceneGraph.spaces).filter(space => space.name === spaceIdOrName)

  if (nameMatches.length === 0) {
    throw new Error(`No space found with id or name: ${spaceIdOrName}`)
  } else if (nameMatches.length > 1) {
    throw new Error(`Multiple spaces found with name: ${spaceIdOrName}`)
  } else {
    return nameMatches[0].id
  }
}

const getObjectParentId = (
  scene: DeepReadonly<SceneGraph>,
  objectId: string
): string | undefined => {
  if (isScopedObjectId(objectId)) {
    const [instanceId, sourceId] = descopeObjectId(objectId)
    const instance = scene.objects[instanceId]
    const source = scene.objects[sourceId]
    if (!instance || !instance.instanceData || !source) {
      return undefined
    }
    if (instance.instanceData.children?.[sourceId]?.deleted) {
      return undefined
    }
    const sourceParent = source.parentId
    let scopedSourceParent: string | undefined
    if (sourceParent === instance.instanceData.instanceOf) {
      scopedSourceParent = instanceId
    } else {
      scopedSourceParent = sourceParent ? scopeObjectIds([instanceId, sourceParent]) : undefined
    }
    const overrideParent = instance.instanceData?.children?.[sourceId]?.parentId
    return overrideParent || scopedSourceParent
  }
  return scene.objects[objectId]?.parentId
}

const isDescendantOf = (scene: DeepReadonly<SceneGraph>, id: string, parentId: string) => {
  if (id === parentId) {
    return false
  }

  let currentId: string | undefined = id

  while (currentId) {
    if (currentId === parentId) {
      return true
    }
    currentId = getObjectParentId(scene, currentId)
  }

  return false
}

const getIncludedSpacesRecursive = (
  scene: DeepReadonly<SceneGraph>, spaceId: string, visited: Set<string>, rootSpaceId: string
): Set<string> => {
  if (visited.has(spaceId)) {
    return new Set()
  }

  visited.add(spaceId)

  const includedSpaces = scene.spaces?.[spaceId]?.includedSpaces || []
  const allIncludedSpaces = new Set(includedSpaces)

  includedSpaces.forEach((id) => {
    getIncludedSpacesRecursive(scene, id, visited, rootSpaceId)
      .forEach(subId => subId !== rootSpaceId && allIncludedSpaces.add(subId))
  })

  return allIncludedSpaces
}

const getIncludedSpaces = (
  scene: DeepReadonly<SceneGraph>, spaceId: string | undefined
): string[] => {
  if (!spaceId) {
    return []
  }

  const allIncludedSpaces = getIncludedSpacesRecursive(scene, spaceId, new Set<string>(), spaceId)

  return Array.from(allIncludedSpaces)
}

const getAllIncludedSpaces = (
  scene: DeepReadonly<SceneGraph>, spaceId: string | undefined
): string[] => (
  spaceId
    ? [spaceId, ...getIncludedSpaces(scene, spaceId)]
    : []
)

const getPrefabs = (scene: DeepReadonly<SceneGraph>) => (
  Object.values(scene.objects).filter(obj => obj.prefab)
)

const objectExists = (scene: DeepReadonly<SceneGraph>, id: string) => (
  id && (!!scene.objects[id] || scopedObjectExists(scene, id))
)

export {
  resolveSpaceForObject,
  resolveSpaceIdForObjectOrSpace,
  resolveSpaceId,
  getIncludedSpaces,
  getInstanceAndScopedId,
  getAllIncludedSpaces,
  isPrefab,
  isPrefabInstance,
  getParentPrefabId,
  getParentInstanceId,
  getSourceObjectId,
  isMissingPrefab,
  isBaseGraphObject,
  getPrefabs,
  descopeObjectId,
  isScopedObjectId,
  scopeObjectIds,
  scopedObjectExists,
  objectExists,
  hasParent,
  getObjectParentId,
  isDescendantOf,
}
