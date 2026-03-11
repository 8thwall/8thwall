import type {DeepReadonly} from 'ts-essentials'

import type {GraphObject, SceneGraph} from '@ecs/shared/scene-graph'
import {
  descopeObjectId, isPrefab, isPrefabInstance, isScopedObjectId, scopedObjectExists,
} from '@ecs/shared/object-hierarchy'

import {DerivedScene, deriveScene} from '../derive-scene'

const deleteScopedObject = (newObjects: Record<string, DeepReadonly<GraphObject>>, id: string) => {
  const [instanceId, sourceId] = descopeObjectId(id)
  const instance = newObjects[instanceId]
  if (!instance) {
    return
  }
  if (!isPrefabInstance(instance)) {
    return
  }
  const children = {...instance.instanceData.children}
  children[sourceId] = {
    deleted: true,
  }

  newObjects[instanceId] = {
    ...instance,
    instanceData: {
      ...instance.instanceData,
      children,
    },
  }
}

const deleteObject = (
  scene: DeepReadonly<SceneGraph>, derivedScene: DerivedScene, rootObjectId: string
) => {
  const idsToDelete = new Set([rootObjectId])

  derivedScene.getDescendantObjectIds(rootObjectId)
    .forEach((descendantId) => {
      idsToDelete.add(descendantId)
    })

  const newObjects = {...scene.objects}
  idsToDelete.forEach((id) => {
    if (isScopedObjectId(id)) {
      deleteScopedObject(newObjects, id)
    } else {
      delete newObjects[id]
    }
  })

  return {...scene, objects: newObjects}
}

const deleteObjects = (
  scene: DeepReadonly<SceneGraph>, ids: DeepReadonly<string[]>, derivedScene = deriveScene(scene)
) => {
  let newScene = scene
  ids.forEach((id) => {
    if (newScene.objects[id] || scopedObjectExists(newScene, id)) {
      newScene = deleteObject(newScene, derivedScene, id)
    }
  })
  return newScene
}

const deleteSpace = (
  scene: DeepReadonly<SceneGraph>, spaceId: string
) => {
  const derivedScene = deriveScene(scene)
  const spaceObjects = derivedScene.getChildren(spaceId)
  const newScene = {
    ...deleteObjects(scene, spaceObjects, derivedScene),
    spaces: {...scene.spaces},
  }
  delete newScene.spaces[spaceId]

  newScene.spaces = Object.keys(newScene.spaces).reduce((acc, id) => {
    const space = newScene.spaces[id]
    acc[id] = {
      ...space,
      includedSpaces: space.includedSpaces?.filter(includedId => includedId !== spaceId),
    }
    return acc
  }, {})

  if (newScene.entrySpaceId === spaceId) {
    const spaces = Object.keys(newScene.spaces)
    newScene.entrySpaceId = spaces.length > 0 ? spaces[0] : undefined
  }
  return newScene
}

const deleteAllPrefabInstances = (scene: DeepReadonly<SceneGraph>, prefabId: string) => {
  const instances: string[] = []
  Object.values(scene.objects).forEach((object) => {
    if (object.instanceData?.instanceOf === prefabId) {
      instances.push(object.id)
    }
  })
  return deleteObjects(scene, instances)
}

const clearPrefab = (scene: DeepReadonly<SceneGraph>, id: string):
DeepReadonly<SceneGraph> => {
  if (!isPrefab(scene.objects[id])) {
    throw new Error(`Object ${id} is not a prefab`)
  }

  const oldDerivedScene = deriveScene(scene)
  const objectsToDelete = oldDerivedScene.getDescendantObjectIds(id).filter(objectId => !isScopedObjectId(objectId))

  const newScene = deleteObjects(scene, objectsToDelete)

  return {
    ...newScene,
    objects: {
      ...newScene.objects,
      [id]: newScene.objects[id],
    },
  }
}

export {
  deleteObjects,
  deleteSpace,
  deleteAllPrefabInstances,
  clearPrefab,
}
