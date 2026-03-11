import type {GraphObject} from '@ecs/shared/scene-graph'
import type {DeepReadonly} from 'ts-essentials'

import type {DerivedScene} from '../derive-scene'
import {useDerivedScene} from '../derived-scene-context'

const getRootAttributeIdFromScene = (
  derivedScene: DerivedScene, id: string,
  condition: keyof GraphObject | ((obj: DeepReadonly<GraphObject>) => boolean)
) => {
  if (!id) {
    return null
  }

  const object = derivedScene.getObject(id)

  if (!object) {
    return undefined
  }

  let parentId = object?.parentId
  let root = (typeof condition === 'function' ? condition(object) : object[condition] !== undefined)
    ? id
    : undefined
  while (parentId) {
    const obj = derivedScene.getObject(parentId)
    if (!obj) {
      return root
    }
    if (typeof condition === 'function' ? condition(obj) : obj[condition] !== undefined) {
      root = parentId
    }
    parentId = obj?.parentId
  }
  return root
}

/**
 * Returns the root object id that satisfies the condition.
 * @param id The object id.
 * @param condition The attribute key or a function to evaluate.
 * @returns an id if any parent satisfies the condition.
 * @example
 * const isRoot = useRootAttributeId(id, obj => obj.mui === 'someValue')
 */
const useRootAttributeId = (
  id: string, condition: keyof GraphObject | ((obj: DeepReadonly<GraphObject>) => boolean)
) => {
  const derivedScene = useDerivedScene()
  return getRootAttributeIdFromScene(derivedScene, id, condition)
}

export {useRootAttributeId, getRootAttributeIdFromScene}
