import type {BaseGraphObject, SceneGraph} from '@ecs/shared/scene-graph'
import type {DeepReadonly} from 'ts-essentials'

import {quat, vec3} from '@ecs/runtime/math/math'

import {
  objectExists, descopeObjectId, getObjectParentId, isScopedObjectId, getParentInstanceId,
} from '@ecs/shared/object-hierarchy'

import type {SceneContext} from './scene-context'
import type {StudioStateContext} from './studio-state-context'
import {calculateGlobalTransform} from './global-transform'
import type {DerivedScene} from './derive-scene'
import {sortObjectIds} from './derive-scene-operations'

const canSetParentTo = (
  scene: DeepReadonly<SceneGraph>,
  derivedScene: DerivedScene,
  targetId: string,
  newParentId: string
) => {
  const targetSourceInstance = getParentInstanceId(scene, targetId)
  const targetPrefab = derivedScene.getParentPrefabId(targetId)
  const parentSourceInstance = getParentInstanceId(scene, newParentId)
  const parentPrefab = derivedScene.getParentPrefabId(newParentId)

  const prefabExchange = targetPrefab && targetPrefab !== parentPrefab
  const isTargetRootPrefab = targetId === targetPrefab

  return !(
    (isScopedObjectId(targetId) && targetSourceInstance !== parentSourceInstance) ||
    (targetSourceInstance && parentPrefab) ||
    (prefabExchange && isTargetRootPrefab)
  )
}

type Placement = {position: 'before' | 'after' | 'within', from: string} | undefined

const getParentId = (placement: Placement, derivedScene: DerivedScene) => (
  placement?.position === 'before' || placement?.position === 'after'
    ? derivedScene.getObject(placement.from)?.parentId
    : placement?.from
)

// NOTE(christoph): randomSeed is used to make sure we rarely have a tiebreaker problem with order.
// Every time you drag objects around, the resulting orderings are slightly different
// than if anyone else was moving objects at the same time. That should mostly avoid
// two objects ever having the same order. In the case where we're trying to drop an object between
// two siblings with the same order, we'll re-sort all the siblings to make sure the new object
// has room to fit in.
const getNewOrder = (
  derivedScene: DerivedScene,
  objectIds: string[],
  placement: Placement,
  randomSeed: number
): Map<string, number> => {
  const newOrderMap = new Map<string, number>()
  const newParentId = getParentId(placement, derivedScene)

  const existingSiblings = newParentId
    ? derivedScene.getChildren(newParentId)
    : derivedScene.getTopLevelObjectIds()

  if (existingSiblings.length === 0) {
    return newOrderMap
  }

  let nextSiblingIndex = -1
  let prevSiblingIndex = -1
  switch (placement?.position) {
    case 'before':
      nextSiblingIndex = existingSiblings.indexOf(placement.from)
      if (nextSiblingIndex > -1) {
        prevSiblingIndex = nextSiblingIndex - 1
      }
      break
    case 'after':
      prevSiblingIndex = existingSiblings.indexOf(placement.from)
      if (prevSiblingIndex > -1 && prevSiblingIndex < existingSiblings.length - 1) {
        nextSiblingIndex = prevSiblingIndex + 1
      }
      break
    case 'within':
    default:
      prevSiblingIndex = existingSiblings.length - 1
  }

  let prevOrder: number | undefined
  let nextOrder: number | undefined

  if (prevSiblingIndex >= 0) {
    const prevSiblingId = existingSiblings[prevSiblingIndex]
    prevOrder = derivedScene.getObject(prevSiblingId)?.order
  }

  if (nextSiblingIndex >= 0) {
    const nextSiblingId = existingSiblings[nextSiblingIndex]
    nextOrder = derivedScene.getObject(nextSiblingId)?.order
  }

  const needsBackfill = (prevSiblingIndex >= 0 && prevOrder === undefined) ||
                        (nextSiblingIndex >= 0 && nextOrder === undefined) ||
                        (prevOrder === nextOrder)  // Ensure there is room between prev/next

  if (needsBackfill) {
    existingSiblings.forEach((siblingId, index) => {
      newOrderMap.set(siblingId, index + randomSeed)
    })
    prevOrder = prevSiblingIndex >= 0 ? prevSiblingIndex + randomSeed : undefined
    nextOrder = nextSiblingIndex >= 0 ? nextSiblingIndex + randomSeed : undefined
  }

  if (typeof prevOrder === 'number' && typeof nextOrder === 'number') {
    // If the random seed is 0, go 25% towards next,
    // If it's 0.5, go 50% towards next
    // If it's 1, go 75% towards next
    // stepSize is the proportion of the way through the objectId list we are
    objectIds.forEach((objectId, index) => {
      const stepSize = (index + 1) / objectIds.length
      const proportion = 0.25 + (randomSeed * stepSize) / 2
      newOrderMap.set(objectId, prevOrder * (1 - proportion) + nextOrder * proportion)
    })
  } else if (typeof prevOrder === 'number') {
    objectIds.forEach((objectId, index) => {
      const stepSize = (index + 1) / objectIds.length
      newOrderMap.set(objectId, prevOrder + 1 + randomSeed * stepSize)
    })
  } else if (typeof nextOrder === 'number') {
    objectIds.forEach((objectId, index) => {
      const stepSize = (objectIds.length - index + 1) / objectIds.length
      newOrderMap.set(objectId, nextOrder - 1 - randomSeed * stepSize)
    })
  }

  return newOrderMap
}

// Create a copy of the object with the new parent id while preserving the object's
// global position, rotation and scale.
const reparentObjects = (
  oldScene: DeepReadonly<SceneGraph>,
  derivedScene: DerivedScene,
  objectIds: string[],
  placement: Placement,
  randomSeed: number = Math.random()  // For reproducible unit tests
): DeepReadonly<SceneGraph> => {
  // Remove non-existent objectIds
  const persistentObjectIds = objectIds.filter(
    id => objectExists(oldScene, id)
  )

  if (persistentObjectIds.length === 0) {
    return oldScene
  }

  // Sort objectIds by their current order, selection order is not guaranteed to be correct
  derivedScene.sortObjectIds(persistentObjectIds)
  const newOrder = getNewOrder(derivedScene, persistentObjectIds, placement, randomSeed)

  const newScene = {
    ...oldScene,
    objects: {...oldScene.objects},
  }

  newOrder.forEach((order, id) => {
    if (isScopedObjectId(id)) {
      const [instanceId, sourceId] = descopeObjectId(id)
      const instance = newScene.objects[instanceId]
      const children = instance.instanceData?.children || {}

      newScene.objects[instanceId] = {
        ...instance,
        instanceData: {
          ...instance.instanceData,
          children: {
            ...children,
            [sourceId]: {
              ...children[sourceId],
              order,
            },
          },
        },
      }
    } else {
      newScene.objects[id] = {
        ...oldScene.objects[id],
        order,
      }
    }
  })

  const newParentId = getParentId(placement, derivedScene)

  // If we're not changing parents, just return the updated orders
  if (persistentObjectIds.every(id => (getObjectParentId(oldScene, id) === newParentId))) {
    return newScene
  }

  // Else change parents and preserve global position, rotation and scale
  persistentObjectIds.forEach((objectId) => {
    // Build the global transform of the object (its position, rotation and scale in world space)
    const globalTransform = calculateGlobalTransform(derivedScene, objectId)

    // Build the new parent's global transform and invert it so we can get the new local transform
    const inverseNewParentTransform = calculateGlobalTransform(derivedScene, newParentId).inv()

    const newLocalTransform = inverseNewParentTransform.clone().setTimes(globalTransform)

    // Decompose back to local position, rotation and scale
    const newPosition = vec3.zero()
    const newRotation = quat.zero()
    const newScale = vec3.zero()
    newLocalTransform.decomposeTrs({t: newPosition, r: newRotation, s: newScale})

    const updatedObjectData: Pick<BaseGraphObject, 'position' | 'rotation' |'scale' | 'parentId'> =
      {
        parentId: newParentId,
        position: [newPosition.x, newPosition.y, newPosition.z],
        rotation: [newRotation.x, newRotation.y, newRotation.z, newRotation.w],
        scale: [newScale.x, newScale.y, newScale.z],
      }

    if (isScopedObjectId(objectId)) {
      const [instanceId, sourceId] = descopeObjectId(objectId)
      const instance = newScene.objects[instanceId]
      const children = instance.instanceData?.children || {}

      newScene.objects[instanceId] = {
        ...instance,
        instanceData: {
          ...instance.instanceData,
          children: {
            ...children,
            [sourceId]: {
              ...children[sourceId],
              ...updatedObjectData,
            },
          },
        },
      }
    } else {
      newScene.objects[objectId] = {
        ...newScene.objects[objectId],
        // Build the new object: a copy of the original object with the new parent id and the new
        // local position, rotation and scale.
        ...updatedObjectData,
      }
    }
  })

  return newScene
}

const doesContainCircularDependency = (
  derivedScene: DerivedScene,
  objectId: string,
  newParentId: string | undefined
): boolean => {
  let parentId: string | undefined = newParentId
  while (parentId) {
    if (parentId === objectId) {
      return true
    }
    parentId = derivedScene.getObject(parentId)?.parentId
  }
  return false
}

const getRootSelectionIds = (
  derivedScene: DerivedScene,
  selectedIds: DeepReadonly<string[]>
) => {
  const notRootIds = new Set<string>()
  return selectedIds.filter((id) => {
    let parentId = derivedScene.getObject(id)?.parentId
    while (parentId) {
      if (notRootIds.has(parentId)) {
        return false
      }
      if (selectedIds.includes(parentId)) {
        notRootIds.add(id)
        return false
      }
      parentId = derivedScene.getObject(parentId)?.parentId
    }
    return true
  })
}

const handleObjectReorder = (
  ctx: SceneContext,
  stateCtx: StudioStateContext,
  derivedScene: DerivedScene,
  objectId: string,
  placement: Placement
): void => {
  const newParentId = getParentId(placement, derivedScene)
  // Reorder all selected objects or just the dragged object
  if (stateCtx.state.selectedIds.includes(objectId)) {
    const selectedIds = getRootSelectionIds(derivedScene, stateCtx.state.selectedIds)
    const isCircularDependency = selectedIds.some(
      id => doesContainCircularDependency(derivedScene, id, newParentId)
    )
    if (isCircularDependency) {
      return
    }

    // If any of the objects are scoped outside of it's parent instance, we need to return
    const invalidSelection = selectedIds.some((id) => {
      if (!isScopedObjectId(id)) {
        return false
      }
      if (!canSetParentTo(ctx.scene, derivedScene, id, newParentId)) {
        return true
      }
      return false
    })

    if (invalidSelection) {
      return
    }

    ctx.updateScene(
      oldScene => reparentObjects(
        oldScene,
        derivedScene,
        selectedIds,
        placement
      )
    )
  } else {
    if (doesContainCircularDependency(derivedScene, objectId, newParentId) ||
        !canSetParentTo(ctx.scene, derivedScene, objectId, newParentId)
    ) {
      return
    }
    ctx.updateScene(oldScene => reparentObjects(
      oldScene,
      derivedScene,
      [objectId],
      placement
    ))
  }
}

const getNextChildOrder = (scene: DeepReadonly<SceneGraph>, parentId: string) => {
  const siblingIds = Object.values(scene.objects)
    .filter(o => o.parentId === parentId)
    .map(o => o.id)
  const sortedSiblings = sortObjectIds(scene, siblingIds)
  const lastOrder = scene.objects[sortedSiblings[sortedSiblings.length - 1]]?.order ?? 0
  return lastOrder + 1 + Math.random()
}

export {
  handleObjectReorder,
  reparentObjects,
  getNextChildOrder,
}
