import {vec3, quat, mat4} from './math/math'
import type {Mat4} from './math/math'

import {Position, Quaternion, Scale} from './components'
import type {World} from './world'
import type {Eid} from '../shared/schema'

const setScale = (world: World, eid: Eid, x: number, y: number, z: number) => {
  const scaleCursor = Scale.acquire(world, eid)
  scaleCursor.x = x
  scaleCursor.y = y
  scaleCursor.z = z
  Scale.commit(world, eid)
}

const setPosition = (world: World, eid: Eid, x: number, y: number, z: number) => {
  const positionCursor = Position.acquire(world, eid)
  positionCursor.x = x
  positionCursor.y = y
  positionCursor.z = z
  Position.commit(world, eid)
}

const setQuaternion = (world: World, eid: Eid, x: number, y: number, z: number, w: number) => {
  const quaternionCursor = Quaternion.acquire(world, eid)
  quaternionCursor.x = x
  quaternionCursor.y = y
  quaternionCursor.z = z
  quaternionCursor.w = w
  Quaternion.commit(world, eid)
}

const normalizeQuaternion = (world: World, eid: Eid) => {
  const quaternionCursor = Quaternion.acquire(world, eid)
  const mag = Math.sqrt(
    quaternionCursor.x ** 2 +
    quaternionCursor.y ** 2 +
    quaternionCursor.z ** 2 +
    quaternionCursor.w ** 2
  )

  quaternionCursor.x /= mag
  quaternionCursor.y /= mag
  quaternionCursor.z /= mag
  quaternionCursor.w /= mag
  Quaternion.commit(world, eid)
}

const tempTrs = {
  t: vec3.zero(),
  r: quat.zero(),
  s: vec3.zero(),
}

const getTransform = (world: World, eid: Eid, target: Mat4) => {
  const positionCursor = Position.get(world, eid)
  const scaleCursor = Scale.get(world, eid)
  const quaternionCursor = Quaternion.get(world, eid)
  tempTrs.t.setXyz(positionCursor.x, positionCursor.y, positionCursor.z)
  tempTrs.s.setXyz(scaleCursor.x, scaleCursor.y, scaleCursor.z)
  tempTrs.r.setXyzw(
    quaternionCursor.x, quaternionCursor.y, quaternionCursor.z, quaternionCursor.w
  )
  target.makeTrs(tempTrs.t, tempTrs.r, tempTrs.s)
}

const tempMatrix = mat4.i()
const getWorldTransform = (world: World, eid: Eid, target: Mat4) => {
  target.makeI()
  let currentEid = eid
  while (currentEid) {
    getTransform(world, currentEid, tempMatrix)
    target.setPremultiply(tempMatrix)
    currentEid = world.getParent(currentEid)
  }
}

const setTransform = (world: World, eid: Eid, matrix: Mat4) => {
  matrix.decomposeTrs(tempTrs)
  setPosition(world, eid, tempTrs.t.x, tempTrs.t.y, tempTrs.t.z)
  setScale(world, eid, tempTrs.s.x, tempTrs.s.y, tempTrs.s.z)
  setQuaternion(
    world, eid, tempTrs.r.x, tempTrs.r.y, tempTrs.r.z, tempTrs.r.w
  )
}

export {
  setScale,
  setPosition,
  setQuaternion,
  normalizeQuaternion,
  getTransform,
  getWorldTransform,
  setTransform,
}
