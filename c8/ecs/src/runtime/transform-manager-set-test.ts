// @package(npm-ecs)
// @attr(esnext = 1)
// @attr(externalize_npm = 1)
import {describe, it, beforeEach, afterEach} from '@nia/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'
import type {World} from './world'
import type {TransformManager} from './transform-manager-types'
import {
  assertPosition, assertQuaternion, assertScale,
} from './transform-test-helpers'

describe('Transform Manager Set Functions', () => {
  let world: World
  let transform: TransformManager

  beforeEach(async () => {
    await ecs.ready()
    world = ecs.createWorld(...initThree())
    transform = world.transform
  })

  afterEach(() => {
    world.destroy()
  })

  describe('setLocalPosition', () => {
    it('works for root object', () => {
      const eid = world.createEntity()
      transform.setLocalPosition(eid, {x: 1, y: 2, z: 3})
      assertPosition(world, eid, ecs.math.vec3.xyz(1, 2, 3))
    })
    it('works for child object when parent has transform', () => {
      const parent = world.createEntity()
      const child = world.createEntity()
      world.setParent(child, parent)
      transform.setLocalPosition(parent, {x: 100, y: 100, z: 100})
      transform.setLocalPosition(child, {x: 1, y: 2, z: 3})
      assertPosition(world, child, {x: 1, y: 2, z: 3})
    })
  })

  describe('setLocalTransform', () => {
    it('works for root object', () => {
      const eid = world.createEntity()
      transform.setLocalTransform(eid, ecs.math.mat4.trs(
        ecs.math.vec3.xyz(1, 2, 3),
        ecs.math.quat.xDegrees(45),
        ecs.math.vec3.xyz(2, 2, 2)
      ))
      assertPosition(world, eid, ecs.math.vec3.xyz(1, 2, 3))
      assertQuaternion(world, eid, ecs.math.quat.xDegrees(45))
      assertScale(world, eid, ecs.math.vec3.xyz(2, 2, 2))
    })
    it('works for child object when parent has transform', () => {
      const parent = world.createEntity()
      const child = world.createEntity()
      world.setParent(child, parent)
      transform.setLocalTransform(parent, ecs.math.mat4.trs(
        ecs.math.vec3.xyz(100, 100, 100),
        ecs.math.quat.zero(),
        ecs.math.vec3.xyz(2, 2, 2)
      ))
      transform.setLocalTransform(child, ecs.math.mat4.trs(
        ecs.math.vec3.xyz(1, 2, 3),
        ecs.math.quat.zero(),
        ecs.math.vec3.xyz(2, 2, 2)
      ))
      assertPosition(world, child, ecs.math.vec3.xyz(1, 2, 3))
      assertQuaternion(world, child, ecs.math.quat.zero())
      assertScale(world, child, ecs.math.vec3.xyz(2, 2, 2))
    })
  })

  describe('setWorldPosition', () => {
    it('works for root object', () => {
      const eid = world.createEntity()
      transform.setWorldPosition(eid, {x: 1, y: 2, z: 3})
      assertPosition(world, eid, ecs.math.vec3.xyz(1, 2, 3))
    })
    it('works for child object when parent has transform', () => {
      const parent = world.createEntity()
      const child = world.createEntity()
      world.setParent(child, parent)
      ecs.Position.set(world, parent, {x: 100, y: 100, z: 100})
      ecs.Quaternion.set(world, parent, ecs.math.quat.yDegrees(90))
      transform.setWorldPosition(child, {x: 101, y: 102, z: 103})
      assertPosition(world, child, {x: -3, y: 2, z: 1})
    })
    it('does not modify scale or rotation', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 100, y: 100, z: 100})
      ecs.Quaternion.set(world, eid, ecs.math.quat.yDegrees(45))
      ecs.Scale.set(world, eid, {x: 10, y: 10, z: 10})
      transform.setWorldPosition(eid, {x: 10, y: 20, z: 30})
      assertPosition(world, eid, ecs.math.vec3.xyz(10, 20, 30))
      assertQuaternion(world, eid, ecs.math.quat.yDegrees(45))
      assertScale(world, eid, ecs.math.vec3.scale(10))
    })
  })

  describe('setWorldTransform', () => {
    it('works for root object', () => {
      const eid = world.createEntity()
      transform.setWorldTransform(eid, ecs.math.mat4.trs(
        ecs.math.vec3.xyz(1, 2, 3),
        ecs.math.quat.xDegrees(45),
        ecs.math.vec3.xyz(2, 2, 2)
      ))
      assertPosition(world, eid, ecs.math.vec3.xyz(1, 2, 3))
      assertQuaternion(world, eid, ecs.math.quat.xDegrees(45))
      assertScale(world, eid, ecs.math.vec3.xyz(2, 2, 2))
    })
    it('works for child object when parent has transform', () => {
      const parent = world.createEntity()
      const child = world.createEntity()
      world.setParent(child, parent)
      ecs.Position.set(world, parent, {x: 11, y: 12, z: 13})
      ecs.Quaternion.set(world, parent, ecs.math.quat.yDegrees(180))
      ecs.Scale.set(world, parent, {x: 10, y: 10, z: 10})

      transform.setWorldTransform(child, ecs.math.mat4.trs(
        ecs.math.vec3.xyz(21, 22, 23),
        ecs.math.quat.xDegrees(-45),
        ecs.math.vec3.xyz(20, 20, 20)
      ))

      assertPosition(world, child, ecs.math.vec3.xyz(-1, 1, -1))
      assertQuaternion(world, child,
        ecs.math.quat.yDegrees(180).times(ecs.math.quat.xDegrees(-45)))
      assertScale(world, child, ecs.math.vec3.xyz(2, 2, 2))
    })
  })

  describe('setWorldQuaternion', () => {
    it('sets the world quaternion of an entity', () => {
      const eid = world.createEntity()

      transform.setWorldQuaternion(eid, ecs.math.quat.yDegrees(90))

      assertQuaternion(world, eid, ecs.math.quat.yDegrees(90))
    })

    it('handles parents with rotation', () => {
      const parentEid = world.createEntity()
      const childEid = world.createEntity()
      world.setParent(childEid, parentEid)
      ecs.Quaternion.set(world, parentEid, ecs.math.quat.yDegrees(45))

      transform.setWorldQuaternion(childEid, ecs.math.quat.yDegrees(90))

      assertQuaternion(world, childEid, ecs.math.quat.yDegrees(45))
    })

    it('preserves world position/scale', () => {
      const parentEid = world.createEntity()
      const childEid = world.createEntity()
      world.setParent(childEid, parentEid)
      ecs.Quaternion.set(world, parentEid, ecs.math.quat.yDegrees(45))
      ecs.Position.set(world, childEid, {x: 1, y: 2, z: 3})
      ecs.Scale.set(world, childEid, {x: 2, y: 2, z: 2})

      transform.setWorldQuaternion(childEid, ecs.math.quat.yDegrees(90))

      assertQuaternion(world, childEid, ecs.math.quat.yDegrees(45))
      assertPosition(world, childEid, {x: 1, y: 2, z: 3})
      assertScale(world, childEid, {x: 2, y: 2, z: 2})
    })
  })
})
