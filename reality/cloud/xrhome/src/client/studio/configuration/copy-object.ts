import {v4 as uuid} from 'uuid'
import type {DeepReadonly} from 'ts-essentials'
import type {GraphObject, SceneGraph} from '@ecs/shared/scene-graph'

import type {StudioStateContext} from '../studio-state-context'
import {updateSceneEntityRefs} from './update-scene-entity-refs'
import {cloneObject} from './clone-object'
import {deriveScene, type DerivedScene} from '../derive-scene'
import {reparentObjects} from '../reparent-object'

const copyObjects = (
  stateCtx: DeepReadonly<StudioStateContext>,
  scene: DeepReadonly<SceneGraph>,
  derivedScene: DerivedScene,
  ids: DeepReadonly<string[]>
) => {
  const idsToCopy = new Set(ids)
  const iterateForCopy = (objectId: string) => {
    Object.values(scene.objects).forEach((sceneObject) => {
      if (sceneObject.parentId === objectId) {
        idsToCopy.add(sceneObject.id)
        iterateForCopy(sceneObject.id)
      }
    })
  }

  ids.forEach((objectId) => { iterateForCopy(objectId) })

  const newClipboard = Array.from(idsToCopy)
    // Note(Dale): We want to copy the unmerged object
    .map(objectId => scene.objects[objectId])
    .filter(Boolean)

  if (newClipboard.length) {
    stateCtx.update(state => ({
      ...state,
      clipboard: {
        type: 'objects',
        objects: newClipboard,
      },
    }))
  }
}

const pasteObjects = (
  scene: DeepReadonly<SceneGraph>,
  objects: DeepReadonly<GraphObject[]>,
  spaceId: string | undefined,
  selectedId: string | undefined,
  newIds?: string[],
  onObjectsSkipped?: () => void,
  reorderSeed = Math.random()
) => {
  let newScene = {...scene, objects: {...scene.objects}}
  const newObjectIds = newIds || objects.map(() => uuid())
  const selectedParentId = scene.objects[selectedId]?.parentId
  const topLevelParentId = selectedParentId || spaceId
  const oldToNewEid = new Map<string, string>()

  const existingPoiIds = Object.values(scene.objects)
    .map(obj => obj.location?.poiId).filter(Boolean)
  let skippedPaste = false

  const topLevelClipboardIds: string[] = []

  objects.forEach((objToPaste, i) => {
    if (!objToPaste) {
      return
    }
    if (existingPoiIds.includes(objToPaste.location?.poiId)) {
      skippedPaste = true
      return
    }
    // if object was parented within the clipboard, paste it under the new one.
    // otherwise, paste it as a sibling of the selected object
    const parentObjIdx = objects.findIndex(obj => obj.id === objToPaste.parentId)
    const isTopLevel = parentObjIdx === -1
    const newParentId = !isTopLevel ? newObjectIds[parentObjIdx] : undefined
    const newObject = cloneObject(newScene, objToPaste, spaceId, newObjectIds[i], newParentId)
    newObject.order = i
    newScene.objects[newObject.id] = newObject
    oldToNewEid.set(objToPaste.id, newObject.id)
    if (isTopLevel) {
      topLevelClipboardIds.push(newObject.id)
    }
  })

  oldToNewEid.forEach((rootId) => {
    newScene = updateSceneEntityRefs(newScene, rootId, oldToNewEid)
  })

  const position = selectedId && selectedParentId
    ? {position: 'after' as const, from: selectedId}
    : {position: 'within' as const, from: topLevelParentId}
  const derivedScene = deriveScene(newScene)
  newScene = reparentObjects(newScene, derivedScene, topLevelClipboardIds, position, reorderSeed)

  if (skippedPaste) {
    onObjectsSkipped?.()
  }

  return newScene
}

export {
  copyObjects,
  pasteObjects,
}
