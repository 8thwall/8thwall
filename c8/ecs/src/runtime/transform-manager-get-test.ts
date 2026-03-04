// @package(npm-ecs)
// @attr(esnext = 1)
// @attr(externalize_npm = 1)
import {describe, it, beforeEach, afterEach, assert} from '@nia/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'
import type {World} from './world'
import {assertMat4Equal, assertQuatEqual, assertVec3Equal} from './transform-test-helpers'
import type {TransformManager} from './transform-manager-types'

describe('Transform Manager Get Functions', () => {
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

  describe('getLocalPosition', () => {
    it('works for root object', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 1, y: 2, z: 3})
      const actual = transform.getLocalPosition(eid)
      const expected = ecs.math.vec3.xyz(1, 2, 3)
      assertVec3Equal(actual, expected)
    })
    it('uses optional out variable', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 1, y: 2, z: 3})
      const out = ecs.math.vec3.zero()
      const actual = transform.getLocalPosition(eid, out)
      assert.strictEqual(actual, out)
      assertVec3Equal(actual, ecs.math.vec3.xyz(1, 2, 3))
    })
    it('works for child object when parent has transform', () => {
      const parent = world.createEntity()
      const child = world.createEntity()
      world.setParent(child, parent)
      ecs.Position.set(world, parent, {x: 100, y: 100, z: 100})
      ecs.Position.set(world, child, {x: 1, y: 2, z: 3})
      const actual = transform.getLocalPosition(child)
      const expected = ecs.math.vec3.xyz(1, 2, 3)
      assertVec3Equal(actual, expected)
    })
  })

  describe('getLocalTransform', () => {
    it('works for root object', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 1, y: 2, z: 3})
      ecs.Quaternion.set(world, eid, ecs.math.quat.yDegrees(30))
      ecs.Scale.set(world, eid, {x: 4, y: 5, z: 6})
      const actual = transform.getLocalTransform(eid)
      const expected = ecs.math.mat4.trs(
        ecs.math.vec3.xyz(1, 2, 3),
        ecs.math.quat.yDegrees(30),
        ecs.math.vec3.xyz(4, 5, 6)
      )
      assertMat4Equal(actual, expected)
    })
    it('uses optional out variable', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 1, y: 2, z: 3})
      ecs.Quaternion.set(world, eid, ecs.math.quat.yDegrees(30))
      ecs.Scale.set(world, eid, {x: 4, y: 5, z: 6})
      const out = ecs.math.mat4.i()
      const actual = transform.getLocalTransform(eid, out)
      assert.strictEqual(actual, out)
      const expected = ecs.math.mat4.trs(
        ecs.math.vec3.xyz(1, 2, 3),
        ecs.math.quat.yDegrees(30),
        ecs.math.vec3.xyz(4, 5, 6)
      )
      assertMat4Equal(actual, expected)
    })
    it('works for child object when parent has transform', () => {
      const parent = world.createEntity()
      const child = world.createEntity()
      world.setParent(child, parent)

      ecs.Position.set(world, parent, {x: 100, y: 100, z: 100})
      ecs.Quaternion.set(world, parent, ecs.math.quat.yDegrees(90))
      ecs.Scale.set(world, parent, {x: 10, y: 10, z: 10})

      ecs.Position.set(world, child, {x: 1, y: 2, z: 3})
      ecs.Quaternion.set(world, child, ecs.math.quat.yDegrees(30))
      ecs.Scale.set(world, child, {x: 4, y: 5, z: 6})
      const actual = transform.getLocalTransform(child)
      const expected = ecs.math.mat4.trs(
        ecs.math.vec3.xyz(1, 2, 3),
        ecs.math.quat.yDegrees(30),
        ecs.math.vec3.xyz(4, 5, 6)
      )
      assertMat4Equal(actual, expected)
    })
  })

  describe('getWorldPosition', () => {
    it('works for root object', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 1, y: 2, z: 3})
      const actual = transform.getWorldPosition(eid)
      const expected = ecs.math.vec3.xyz(1, 2, 3)
      assertVec3Equal(actual, expected)
    })
    it('uses optional out variable', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 1, y: 2, z: 3})
      const out = ecs.math.vec3.zero()
      const actual = transform.getWorldPosition(eid, out)
      assert.strictEqual(actual, out)
      const expected = ecs.math.vec3.xyz(1, 2, 3)
      assertVec3Equal(actual, expected)
    })
    it('works for child object when parent has transform', () => {
      const parent = world.createEntity()
      const child = world.createEntity()
      world.setParent(child, parent)
      ecs.Position.set(world, parent, {x: 100, y: 100, z: 100})
      ecs.Position.set(world, child, {x: 1, y: 2, z: 3})
      const actual = transform.getWorldPosition(child)
      const expected = ecs.math.vec3.xyz(101, 102, 103)
      assertVec3Equal(actual, expected)
    })
  })

  describe('getWorldTransform', () => {
    it('works for root object', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 1, y: 2, z: 3})
      ecs.Quaternion.set(world, eid, ecs.math.quat.yDegrees(30))
      ecs.Scale.set(world, eid, {x: 4, y: 5, z: 6})
      const actual = transform.getWorldTransform(eid)
      const expected = ecs.math.mat4.trs(
        ecs.math.vec3.xyz(1, 2, 3),
        ecs.math.quat.yDegrees(30),
        ecs.math.vec3.xyz(4, 5, 6)
      )
      assertMat4Equal(actual, expected)
    })
    it('uses optional out variable', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 1, y: 2, z: 3})
      ecs.Quaternion.set(world, eid, ecs.math.quat.yDegrees(30))
      ecs.Scale.set(world, eid, {x: 4, y: 5, z: 6})
      const out = ecs.math.mat4.i()
      const actual = transform.getWorldTransform(eid, out)
      assert.strictEqual(actual, out)
      const expected = ecs.math.mat4.trs(
        ecs.math.vec3.xyz(1, 2, 3),
        ecs.math.quat.yDegrees(30),
        ecs.math.vec3.xyz(4, 5, 6)
      )
      assertMat4Equal(actual, expected)
    })
    it('works for child object when parent has transform', () => {
      const parent = world.createEntity()
      const child = world.createEntity()
      world.setParent(child, parent)

      ecs.Position.set(world, parent, {x: 100, y: 100, z: 100})
      ecs.Quaternion.set(world, parent, ecs.math.quat.yDegrees(90))
      ecs.Scale.set(world, parent, {x: 10, y: 10, z: 10})

      ecs.Position.set(world, child, {x: 1, y: 2, z: 3})
      ecs.Quaternion.set(world, child, ecs.math.quat.yDegrees(30))
      ecs.Scale.set(world, child, {x: 4, y: 5, z: 6})

      const actual = transform.getWorldTransform(child)

      const expected = ecs.math.mat4.trs(
        // the local offsets are scaled by the parent scale and rotated
        ecs.math.vec3.xyz(130, 120, 90),
        ecs.math.quat.yDegrees(120),
        ecs.math.vec3.xyz(40, 50, 60)
      )
      assertMat4Equal(actual, expected)
    })
  })

  describe('getWorldQuaternion', () => {
    it('works for root object', () => {
      const eid = world.createEntity()
      ecs.Quaternion.set(world, eid, ecs.math.quat.yDegrees(30))
      const actual = transform.getWorldQuaternion(eid)
      const expected = ecs.math.quat.yDegrees(30)
      assertQuatEqual(actual, expected)
    })
    it('uses optional out variable', () => {
      const eid = world.createEntity()
      ecs.Quaternion.set(world, eid, ecs.math.quat.yDegrees(30))
      const out = ecs.math.quat.zero()
      const actual = transform.getWorldQuaternion(eid, out)
      assert.strictEqual(actual, out)
      const expected = ecs.math.quat.yDegrees(30)
      assertQuatEqual(actual, expected)
    })
    it('works for child object when parent has transform', () => {
      const parent = world.createEntity()
      const child = world.createEntity()
      world.setParent(child, parent)
      ecs.Quaternion.set(world, parent, ecs.math.quat.yDegrees(90))
      ecs.Quaternion.set(world, child, ecs.math.quat.yDegrees(30))
      const actual = transform.getWorldQuaternion(child)
      const expected = ecs.math.quat.yDegrees(120)
      assertQuatEqual(actual, expected)
    })
  })
})
