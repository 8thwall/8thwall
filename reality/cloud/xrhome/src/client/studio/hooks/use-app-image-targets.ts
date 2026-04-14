import type {GraphObject} from '@ecs/shared/scene-graph'
import type {DeepReadonly} from 'ts-essentials'

import {quat, vec3} from '@ecs/runtime/math/math'

import {calculateGlobalTransform} from '../global-transform'
import type {DerivedScene} from '../derive-scene'

const getImageTargetRotation = (
  derivedScene: DerivedScene, name: string | undefined
): DeepReadonly<GraphObject['rotation']> | undefined => {
  if (!name) return undefined
  const obj = derivedScene.getAllSceneObjects().find(
    o => o.imageTarget?.name === name
  )

  if (!obj) return undefined

  const transform = calculateGlobalTransform(derivedScene, obj.id)
  const rotation = quat.zero()
  transform.decomposeTrs({t: vec3.zero(), r: rotation, s: vec3.zero()})
  return [rotation.x, rotation.y, rotation.z, rotation.w]
}

export {
  getImageTargetRotation,
}
