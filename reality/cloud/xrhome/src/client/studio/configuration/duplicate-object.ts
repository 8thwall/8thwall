import type {DeepReadonly} from 'ts-essentials'
import type {SceneGraph} from '@ecs/shared/scene-graph'
import {resolveSpaceForObject} from '@ecs/shared/object-hierarchy'

import type {IdMapper} from '../id-generation'
import {cloneObject} from './clone-object'
import {updateSceneEntityRefs} from './update-scene-entity-refs'
import type {DerivedScene} from '../derive-scene'
import {reparentObjects} from '../reparent-object'

type DuplicateObjectParams = {
  scene: DeepReadonly<SceneGraph>
  derivedScene: DerivedScene
  rootObjectId: string
  spaceId: string | undefined
  oldToNewEid: Map<string, string>
  ids: IdMapper
  parentId: string
  reorderSeed: number
}

const duplicateObject = ({
  scene, derivedScene, rootObjectId, spaceId, oldToNewEid, ids, parentId, reorderSeed,
}: DuplicateObjectParams) => {
  let newScene = {...scene, objects: {...scene.objects}}

  const iterateForDuplication = (
    objectId: string, newParentId?: string
  ) => {
    // Note(Dale): We want the unmerged object for duplication
    const targetObject = scene.objects[objectId]
    if (!targetObject) {
      return
    }
    const newObjectId = ids.fromId(objectId)
    const newObjectParentId = newParentId || targetObject.parentId

    const newObject = cloneObject(newScene, targetObject, spaceId, newObjectId, newObjectParentId)
    newScene.objects[newObject.id] = newObject
    oldToNewEid.set(objectId, newObjectId)

    if (newObjectParentId === targetObject.parentId) {
      newScene = reparentObjects(
        newScene, derivedScene, [newObjectId], {position: 'after', from: objectId}, reorderSeed
      )
    }

    Object.values(scene.objects).forEach((object) => {
      if (object.parentId === objectId) {
        iterateForDuplication(object.id, newObjectId)
      }
    })
  }
  iterateForDuplication(rootObjectId, parentId)
  return newScene
}

const canDuplicateObjects = (
  derivedScene: DerivedScene, objectIds: DeepReadonly<string[]>
): Boolean => !objectIds.every(obj => !!derivedScene.getObject(obj)?.location)

type DuplicateObjectsParams = {
  scene: DeepReadonly<SceneGraph>
  derivedScene: DerivedScene
  objectIds: DeepReadonly<string[]>
  ids: IdMapper
  newParentId?: string
  onObjectsSkipped?: () => void
  spaceId?: string
  reorderSeed?: number
}

const duplicateObjects = ({
  scene, derivedScene, objectIds, ids, newParentId, onObjectsSkipped, spaceId,
  reorderSeed = Math.random(),
}: DuplicateObjectsParams) => {
  const existingPoiIds = Object.values(scene.objects)
    .map(obj => obj.location?.poiId).filter(Boolean)
  let skippedDuplicate = false

  let newScene = scene
  const oldToNewEid = new Map<string, string>()

  objectIds.filter((id) => {
    let parentId = derivedScene.getObject(id)?.parentId
    while (parentId) {
      if (objectIds.includes(parentId)) {
        return false
      }
      parentId = derivedScene.getObject(parentId)?.parentId
    }
    return true
  }).forEach((id) => {
    if (existingPoiIds.includes(derivedScene.getObject(id)?.location?.poiId)) {
      skippedDuplicate = true
      return
    }
    const newSpaceId = spaceId || resolveSpaceForObject(scene, id)?.id

    newScene = duplicateObject({
      scene: newScene,
      derivedScene,
      rootObjectId: id,
      spaceId: newSpaceId,
      oldToNewEid,
      ids,
      parentId: newParentId,
      reorderSeed,
    })
  })

  oldToNewEid.forEach((rootId) => {
    newScene = updateSceneEntityRefs(newScene, rootId, oldToNewEid)
  })

  if (skippedDuplicate) {
    onObjectsSkipped?.()
  }

  return newScene
}

export {
  duplicateObjects,
  canDuplicateObjects,
}
