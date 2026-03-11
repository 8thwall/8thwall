import type {BaseGraphObject, GraphObject, SceneGraph} from '@ecs/shared/scene-graph'

import {
  descopeObjectId, isBaseGraphObject, isScopedObjectId, scopedObjectExists,
} from '@ecs/shared/object-hierarchy'

import type {DeepReadonly} from 'ts-essentials'

import type {MutateCallback} from './scene-context'
import {getObjectOrMergedInstance} from './derive-scene-operations'

const KEYS_TO_EXCLUDE = ['id', 'parentId', 'prefab', 'name', 'components']

const valueExists = (
  key: string, object: DeepReadonly<GraphObject>
) => key in object && object[key] !== undefined && object[key] !== null

const componentExists = (
  key: string, object: Record<string, object>
) => key in object && object[key] !== undefined && object[key] !== null

const updateScopedObject = (
  oldScene: DeepReadonly<SceneGraph>,
  id: string,
  cb: MutateCallback<BaseGraphObject>
): DeepReadonly<SceneGraph> => {
  const objects = {...oldScene.objects}
  const [instanceId, sourceId] = descopeObjectId(id)
  const oldMerged = getObjectOrMergedInstance(oldScene, id)
  const newMerged = cb(oldMerged)

  const oldInstance = objects[instanceId]

  const children = {...oldInstance.instanceData.children}

  Object.keys(newMerged).forEach((key) => {
    if (newMerged[key] !== oldMerged[key]) {
      if (children[sourceId]) {
        children[sourceId][key] = newMerged[key]
      } else {
        children[sourceId] = {
          id: sourceId,
          deletions: {},
          [key]: newMerged[key],
        }
      }
    }
  })

  // Handle deletions for children
  const deletionsToAdd = Object.keys(oldMerged).filter(
    key => !KEYS_TO_EXCLUDE.includes(key) &&
      !valueExists(key, newMerged) && valueExists(key, oldMerged) &&
      !children[sourceId]?.deletions[key]
  )

  // Components
  const componentDeletionsToAdd: string[] = []
  Object.keys(oldMerged.components).forEach((componentName) => {
    if (
      !componentExists(componentName, newMerged.components) &&
      !children[sourceId]?.deletions?.components?.[componentName]
    ) {
      componentDeletionsToAdd.push(componentName)
    }
  })

  const deletionsToRemove = Object.keys(newMerged).filter(
    key => !KEYS_TO_EXCLUDE.includes(key) &&
      valueExists(key, newMerged) && !valueExists(key, oldMerged) &&
      children[sourceId]?.deletions[key]
  )

  // Components: Remove newly added components
  const componentDeletionsToRemove: string[] = []
  Object.keys(newMerged.components).forEach((componentName) => {
    if (
      componentExists(componentName, newMerged.components) &&
      !componentExists(componentName, oldMerged.components) &&
      children[sourceId]?.deletions?.components?.[componentName]
    ) {
      componentDeletionsToRemove.push(componentName)
    }
  })

  if (deletionsToAdd.length || deletionsToRemove.length ||
     componentDeletionsToAdd.length || componentDeletionsToRemove.length) {
    const updatedDeletions = {...(children[sourceId]?.deletions || {})}
    deletionsToAdd.forEach((key) => { updatedDeletions[key] = true })
    componentDeletionsToAdd.forEach((key) => {
      updatedDeletions.components = {
        ...(updatedDeletions.components || {}),
        [key]: true,
      }
    })

    deletionsToRemove.forEach((key) => { delete updatedDeletions[key] })
    const components = {...updatedDeletions.components || {}}
    componentDeletionsToRemove.forEach((key) => {
      delete components[key]
    })
    if (componentDeletionsToRemove.length) {
      updatedDeletions.components = components
    }
    children[sourceId] = {
      ...children[sourceId],
      deletions: updatedDeletions,
    }
  }

  const newInstance = {
    ...oldInstance,
    instanceData: {
      ...oldInstance.instanceData,
      children,
    },
  }

  return {
    ...oldScene,
    objects: {
      ...objects,
      [instanceId]: newInstance,
    },
  }
}

const updatePrefabInstanceObject = (
  oldScene: DeepReadonly<SceneGraph>,
  id: string,
  cb: MutateCallback<BaseGraphObject>
): DeepReadonly<GraphObject> => {
  const oldObject = oldScene.objects[id]
  const prefab = oldScene.objects[oldObject.instanceData.instanceOf]
  const oldMerged = getObjectOrMergedInstance(oldScene, id)
  const newMerged = cb(oldMerged)

  const newObject = {...oldObject}

  // Apply changes from newMerged to newObject for properties that have changed
  Object.keys(newMerged).forEach((key) => {
    if (newMerged[key] !== oldMerged[key]) {
      newObject[key] = newMerged[key]
    }
  })

  // Add deletions for properties that are deleted and are in the prefab
  const deletionsToAdd = Object.keys(oldMerged).filter(
    key => !KEYS_TO_EXCLUDE.includes(key) &&
      !valueExists(key, newObject) && !valueExists(key, newMerged) && valueExists(key, prefab) &&
      !oldObject.instanceData.deletions[key]
  )

  // Components
  const componentDeletionsToAdd: string[] = []
  Object.keys(oldMerged.components).forEach((componentName) => {
    if (
      componentExists(componentName, prefab.components) &&
      !componentExists(componentName, newMerged.components) &&
      !oldObject.instanceData?.deletions?.components?.[componentName]
    ) {
      componentDeletionsToAdd.push(componentName)
    }
  })

  // Remove newly added properties that are in the prefab from deletions
  const deletionsToRemove = Object.keys(newObject).filter(
    key => !KEYS_TO_EXCLUDE.includes(key) &&
      !valueExists(key, oldObject) && valueExists(key, newMerged) && valueExists(key, prefab) &&
      oldObject.instanceData.deletions[key]
  )

  // Components: Remove newly added components
  const componentDeletionsToRemove: string[] = []
  Object.keys(newObject.components).forEach((componentName) => {
    if (
      !componentExists(componentName, oldObject.components) &&
      componentExists(componentName, newMerged.components) &&
      componentExists(componentName, prefab.components) &&
      oldObject.instanceData?.deletions?.components?.[componentName]
    ) {
      componentDeletionsToRemove.push(componentName)
    }
  })

  if (deletionsToAdd.length || deletionsToRemove.length ||
    componentDeletionsToAdd.length || componentDeletionsToRemove.length) {
    const updatedDeletions = {...oldObject.instanceData.deletions}
    deletionsToAdd.forEach((key) => { updatedDeletions[key] = true })
    componentDeletionsToAdd.forEach((key) => {
      updatedDeletions.components = {...updatedDeletions.components, [key]: true}
    })
    deletionsToRemove.forEach((key) => { delete updatedDeletions[key] })
    const components = {...updatedDeletions.components || {}}
    componentDeletionsToRemove.forEach((key) => {
      delete components[key]
    })
    if (componentDeletionsToRemove.length) {
      updatedDeletions.components = components
    }
    newObject.instanceData = {
      ...oldObject.instanceData,
      deletions: updatedDeletions,
    }
  }

  return newObject
}

const updateObject = (
  updateScene: (cb: MutateCallback<SceneGraph>) => void,
  id: string,
  cb: MutateCallback<BaseGraphObject>
) => {
  updateScene((oldScene) => {
    if (isScopedObjectId(id)) {
      return updateScopedObject(oldScene, id, cb)
    }

    const oldObject = oldScene.objects[id]
    if (!oldObject && !scopedObjectExists(oldScene, id)) {
      return oldScene
    }
    const newScene = {...oldScene, objects: {...oldScene.objects}}

    if (!isBaseGraphObject(oldObject)) {
      newScene.objects[id] = updatePrefabInstanceObject(oldScene, id, cb)
    } else {
      newScene.objects[id] = cb(oldObject)
    }
    return newScene
  })
}

export {
  updateObject,
}
