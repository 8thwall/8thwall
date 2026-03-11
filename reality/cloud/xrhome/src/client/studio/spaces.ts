import {v4 as uuid} from 'uuid'
import type {DeepReadonly} from 'ts-essentials'
import type {SceneGraph} from '@ecs/shared/scene-graph'
import type {TFunction} from 'react-i18next'

import {makePrimitive, makeCamera} from './make-object'
import {deriveScene, type DerivedScene} from './derive-scene'
import type {StudioStateContext} from './studio-state-context'

const switchSpace = (stateCtx: StudioStateContext, derivedScene: DerivedScene, spaceId: string) => {
  if (spaceId === stateCtx.state.activeSpace) {
    return
  }
  stateCtx.update(s => ({...s, activeSpace: spaceId}))
  const selected = stateCtx.state.selectedIds
  const spacelessSelected = selected.filter(s => !derivedScene.resolveSpaceForObject(s))
  stateCtx.setSelection(...spacelessSelected)
}

const convertFromSpaceless = (
  scene: DeepReadonly<SceneGraph>, spaceName: string, id: string
) => {
  const newScene = {...scene, spaces: {...scene.spaces}}
  newScene.spaces[id] = {id, name: spaceName}
  const derivedScene = deriveScene(scene)
  const objectsWithoutSpace = derivedScene.getTopLevelObjectIds()
  if (objectsWithoutSpace.length) {
    const newObjects = {...scene.objects}
    objectsWithoutSpace.forEach((objectId) => {
      newObjects[objectId] = {...newObjects[objectId], parentId: id}
    })
    newScene.objects = newObjects
    newScene.entrySpaceId = id
    newScene.spaces[id] = {
      ...newScene.spaces[id],
      activeCamera: scene?.activeCamera,
      sky: scene?.sky,
    }
    delete newScene.activeCamera
    delete newScene.sky
  }
  return newScene
}

const createDefaultSpace = (
  scene: DeepReadonly<SceneGraph>, spaceName: string, id: string, t: TFunction
) => {
  const newScene = {...scene, spaces: {...scene.spaces}}
  newScene.spaces[id] = {id, name: spaceName}
  const directionalLight = makePrimitive(id, 'directional')
  directionalLight.name = t('new_primitive_button.option.light.directional')
  directionalLight.order = Math.random()
  const ambientLight = makePrimitive(id, 'ambient')
  ambientLight.name = t('new_primitive_button.option.light.ambient')
  ambientLight.order = Math.random() + 1
  const camera = makeCamera(id, t('new_primitive_button.option.camera'))
  camera.order = Math.random() + 2
  newScene.spaces[id] = {
    ...newScene.spaces[id],
    activeCamera: camera.id,
  }
  newScene.objects = {
    ...scene.objects,
    [directionalLight.id]: directionalLight,
    [ambientLight.id]: ambientLight,
    [camera.id]: camera,
  }
  return newScene
}

const addSpace = (
  oldScene: DeepReadonly<SceneGraph>, newSpaceName: string, t: TFunction, newSpaceId?: string
) => {
  const id = newSpaceId ?? uuid()
  if (!oldScene.spaces) {
    return convertFromSpaceless(oldScene, newSpaceName, id)
  } else {
    return createDefaultSpace(oldScene, newSpaceName, id, t)
  }
}

const renameSpace = (scene: DeepReadonly<SceneGraph>, spaceId: string, name: string) => {
  const newScene = {...scene, spaces: {...scene.spaces}}
  newScene.spaces[spaceId] = {...newScene.spaces[spaceId], name}
  return newScene
}

export {
  switchSpace,
  addSpace,
  renameSpace,
}
