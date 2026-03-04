import type {Eid} from '../shared/schema'
import type {TransformManager} from './transform-manager-types'
import type {World} from './world'
import {
  Position as RootPosition, Scale as RootScale, Quaternion as RootQuaternion,
} from './components'

import {mat4, Mat4, vec3, Vec3Source, Vec3, quat, QuatSource, Quat} from './math/math'

const temp1Mat4 = mat4.i()
const temp2Mat4 = mat4.i()
const temp3Mat4 = mat4.i()

const temp1Vec3 = vec3.zero()
const temp2Vec3 = vec3.zero()
const temp3Vec3 = vec3.zero()

const temp1Quat = quat.zero()
const temp2Quat = quat.zero()

const temp1Trs = {
  t: vec3.zero(),
  r: quat.zero(),
  s: vec3.zero(),
}

const ZERO_VEC3 = vec3.zero()
const UP_VEC3 = vec3.up()

const createTransformManager = (world: World): TransformManager => {
  const Position = RootPosition.forWorld(world)
  const Scale = RootScale.forWorld(world)
  const Quaternion = RootQuaternion.forWorld(world)

  const getLocalPosition = (eid: Eid, out: Vec3 = vec3.zero()): Vec3 => {
    const position = Position.get(eid)
    return out.setFrom(position)
  }

  const getLocalTransform = (eid: Eid, out: Mat4 = mat4.i()): Mat4 => (
    out.makeTrs(Position.get(eid), Quaternion.get(eid), Scale.get(eid))
  )

  const getWorldTransform = (eid: Eid, _out?: Mat4): Mat4 => {
    const out = _out ? _out.makeI() : mat4.i()
    let currentEid = eid
    while (currentEid) {
      getLocalTransform(currentEid, temp1Mat4)
      out.setPremultiply(temp1Mat4)
      currentEid = world.getParent(currentEid)
    }
    return out
  }

  const getWorldPosition = (eid: Eid, out?: Vec3): Vec3 => {
    getWorldTransform(eid, temp2Mat4)
    return temp2Mat4.decomposeT(out)
  }

  const getWorldQuaternion = (eid: Eid, out: Quat = quat.zero()): Quat => {
    getWorldTransform(eid, temp2Mat4)
    return temp2Mat4.decomposeR(out)
  }

  const setLocalPosition = (eid: Eid, position: Vec3Source) => {
    world.setPosition(eid, position.x, position.y, position.z)
  }

  const setLocalQuaternion = (eid: Eid, quaternion: QuatSource) => {
    world.setQuaternion(eid, quaternion.x, quaternion.y, quaternion.z, quaternion.w)
  }

  const setLocalScale = (eid: Eid, scale: Vec3Source) => {
    world.setScale(eid, scale.x, scale.y, scale.z)
  }

  const setLocalTransform = (eid: Eid, transform: Mat4) => {
    transform.decomposeTrs(temp1Trs)
    setLocalPosition(eid, temp1Trs.t)
    setLocalQuaternion(eid, temp1Trs.r)
    setLocalScale(eid, temp1Trs.s)
  }

  const setWorldPosition = (eid: Eid, position: Vec3Source) => {
    getWorldTransform(world.getParent(eid), temp2Mat4)
    temp1Vec3.setFrom(position)
    temp2Mat4.setInv().timesVec(temp1Vec3, temp1Vec3)
    setLocalPosition(eid, temp1Vec3)
  }

  const setWorldQuaternion = (eid: Eid, quaternion: QuatSource) => {
    getWorldTransform(world.getParent(eid), temp2Mat4)
    temp2Mat4.setInv()
    temp3Mat4.makeR(quaternion)
    temp3Mat4.setPremultiply(temp2Mat4)
    temp3Mat4.decomposeR(temp1Quat)
    setLocalQuaternion(eid, temp1Quat)
  }

  const setWorldTransform = (eid: Eid, transform: Mat4) => {
    getWorldTransform(world.getParent(eid), temp2Mat4)
    temp2Mat4.setInv()
    temp2Mat4.setTimes(transform)
    setLocalTransform(eid, temp2Mat4)
  }

  const translateDirect = (eid: Eid, translation: Vec3Source) => {
    const position = Position.acquire(eid)
    position.x += translation.x
    position.y += translation.y
    position.z += translation.z
    Position.commit(eid)
  }

  const translateSelf = (eid: Eid, translation: Partial<Vec3Source>) => {
    temp1Vec3.setXyz(translation.x || 0, translation.y || 0, translation.z || 0)
    temp1Quat.setFrom(Quaternion.get(eid))
    temp1Quat.timesVec(temp1Vec3, temp1Vec3)
    translateDirect(eid, temp1Vec3)
  }

  const translateLocal = (eid: Eid, translation: Partial<Vec3Source>) => {
    temp1Vec3.setXyz(translation.x || 0, translation.y || 0, translation.z || 0)
    translateDirect(eid, temp1Vec3)
  }

  const translateWorld = (eid: Eid, translation: Partial<Vec3Source>) => {
    getWorldTransform(world.getParent(eid), temp2Mat4)
    temp2Mat4.setInv()
    temp1Vec3.setXyz(translation.x || 0, translation.y || 0, translation.z || 0)
    temp2Mat4.timesVec(temp1Vec3, temp1Vec3)
    translateDirect(eid, temp1Vec3)
  }

  const rotateSelf = (eid: Eid, rotation: QuatSource) => {
    const quaternion = Quaternion.acquire(eid)
    temp1Quat.setFrom(rotation)
    temp2Quat.setFrom(quaternion).setTimes(temp1Quat)
    quaternion.x = temp2Quat.x
    quaternion.y = temp2Quat.y
    quaternion.z = temp2Quat.z
    quaternion.w = temp2Quat.w
    Quaternion.commit(eid)
  }

  const rotateLocal = (eid: Eid, rotation: QuatSource) => {
    const quaternion = Quaternion.acquire(eid)
    temp1Quat.setFrom(quaternion)
    temp2Quat.setFrom(rotation).setTimes(temp1Quat)
    quaternion.x = temp2Quat.x
    quaternion.y = temp2Quat.y
    quaternion.z = temp2Quat.z
    quaternion.w = temp2Quat.w
    Quaternion.commit(eid)
  }

  const lookAtLocal = (eid: Eid, position: Vec3Source) => {
    temp2Vec3.setFrom(position)
    temp2Vec3.setMinus(Position.get(eid))
    temp2Vec3.setDivide(Scale.get(eid))
    temp1Quat.makeLookAt(ZERO_VEC3, temp2Vec3, UP_VEC3)
    setLocalQuaternion(eid, temp1Quat)
  }

  const lookAtWorld = (eid: Eid, position: Vec3Source) => {
    getWorldTransform(world.getParent(eid), temp3Mat4)
    temp3Mat4.setInv()
    temp3Mat4.timesVec(position, temp1Vec3)
    lookAtLocal(eid, temp1Vec3)
  }

  const lookAt = (eid: Eid, other: Eid) => {
    getWorldPosition(other, temp3Vec3)
    lookAtWorld(eid, temp3Vec3)
  }

  return {
    getLocalPosition,
    getLocalTransform,
    getWorldPosition,
    getWorldQuaternion,
    getWorldTransform,

    setLocalPosition,
    setLocalTransform,
    setWorldPosition,
    setWorldQuaternion,
    setWorldTransform,

    translateSelf,
    translateLocal,
    translateWorld,

    rotateSelf,
    rotateLocal,

    lookAt,
    lookAtLocal,
    lookAtWorld,
  }
}

export {
  createTransformManager,
}
