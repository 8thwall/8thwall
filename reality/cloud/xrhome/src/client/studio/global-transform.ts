import type {GraphObject} from '@ecs/shared/scene-graph'
import type {DeepReadonly} from 'ts-essentials'

import {mat4, quat, vec3} from '@ecs/runtime/math/math'
import type {Mat4, Vec3} from '@ecs/runtime/math/math'

import type {DerivedScene} from './derive-scene'

const calculateLocalTransform = (o: DeepReadonly<GraphObject>): Mat4 => mat4.trs(
  vec3.xyz(...o.position), quat.xyzw(...o.rotation), vec3.xyz(...o.scale)
)

// Recursively build the global transform of an object by multiplying its local transform with the
// global transform of its parent.
const calculateGlobalTransform = (derivedScene: DerivedScene, objectId: string): Mat4 => {
  const o = derivedScene.getObject(objectId)
  if (o) {
    const transform = calculateGlobalTransform(derivedScene, o.parentId)
    const localTransform = calculateLocalTransform(o)
    transform.setTimes(localTransform)
    return transform
  } else {
    return mat4.i()
  }
}

const calculateGlobalPosition = (derivedScene: DerivedScene, objectId: string): Vec3 => {
  const o = derivedScene.getObject(objectId)
  if (o) {
    const transform = calculateGlobalTransform(derivedScene, o.parentId)
    const localTransform = calculateLocalTransform(o)
    transform.setTimes(localTransform)
    return transform.decomposeT()
  } else {
    return vec3.zero()
  }
}

export {
  calculateGlobalTransform,
  calculateGlobalPosition,
}
