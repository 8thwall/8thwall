// @package(npm-ecs)
// @attr(esnext = 1)
// @attr(externalize_npm = 1)
import {describe, it, beforeEach, afterEach} from '@repo/bzl/js/chai-js'

import {initThree} from './test-env'
import type {World} from './world'
import {ecs} from './test-runtime-lib'
import {assertMat4Equal, assertQuatEqual, assertVec3Equal} from './transform-test-helpers'

const makeEntity = (world: World) => {
  const eid = world.createEntity()
  return world.getEntity(eid)
}

describe('Entity Impl', () => {
  let world: World

  beforeEach(async () => {
    await ecs.ready()
    world = ecs.createWorld(...initThree())
  })

  afterEach(() => {
    world.destroy()
  })

  describe('translateSelf', () => {
    it('translates the entity in self space', () => {
      const entity = makeEntity(world)
      entity.set(ecs.Quaternion, ecs.math.quat.xDegrees(-45))
      entity.translateSelf({z: 4 * Math.sqrt(2)})
      assertVec3Equal(entity.get(ecs.Position), {x: 0, y: 4, z: 4})
    })
  })

  describe('translateLocal', () => {
    it('translates the entity in local space', () => {
      const entity = makeEntity(world)
      entity.set(ecs.Quaternion, ecs.math.quat.xDegrees(45))
      entity.translateLocal({x: 1, y: 2, z: 3})
      assertVec3Equal(entity.get(ecs.Position), {x: 1, y: 2, z: 3})
    })
  })

  describe('translateWorld', () => {
    it('translates the entity in world space', () => {
      const parent = makeEntity(world)
      parent.set(ecs.Quaternion, ecs.math.quat.yDegrees(180))
      const entity = makeEntity(world)
      parent.addChild(entity.eid)
      entity.translateWorld({z: 3})
      assertVec3Equal(entity.get(ecs.Position), {x: 0, y: 0, z: -3})
    })
  })

  describe('rotateSelf', () => {
    it('rotates the entity in self space', () => {
      const entity = makeEntity(world)
      entity.set(ecs.Quaternion, ecs.math.quat.xDegrees(45))
      entity.rotateSelf(ecs.math.quat.xDegrees(45))
      assertQuatEqual(entity.get(ecs.Quaternion), ecs.math.quat.xDegrees(90))
    })
  })

  describe('rotateLocal', () => {
    it('rotates the entity in local space', () => {
      const entity = makeEntity(world)
      entity.set(ecs.Quaternion, ecs.math.quat.xDegrees(45))
      entity.rotateLocal(ecs.math.quat.xDegrees(45))
      assertQuatEqual(entity.get(ecs.Quaternion), ecs.math.quat.xDegrees(90))
    })
  })

  describe('lookAt', () => {
    it('looks at another entity', () => {
      const entity = makeEntity(world)
      const targetEntity = makeEntity(world)
      ecs.Position.set(world, targetEntity.eid, {x: 0, y: 3, z: 3})
      entity.lookAt(targetEntity.eid)
      assertQuatEqual(entity.get(ecs.Quaternion), ecs.math.quat.xDegrees(-45))
    })

    it('works for Entity object', () => {
      const entity = makeEntity(world)
      const targetEntity = makeEntity(world)
      targetEntity.set(ecs.Position, {x: 0, y: 3, z: 3})
      entity.lookAt(targetEntity)
      assertQuatEqual(entity.get(ecs.Quaternion), ecs.math.quat.xDegrees(-45))
    })
  })

  describe('lookAtLocal', () => {
    it('looks at a position in local space', () => {
      const entity = makeEntity(world)
      entity.set(ecs.Position, {x: 0, y: 0, z: 3})
      entity.lookAtLocal({x: 0, y: 3, z: 3})
      assertQuatEqual(entity.get(ecs.Quaternion), ecs.math.quat.xDegrees(-90))
    })
  })

  describe('lookAtWorld', () => {
    it('looks at a position in world space', () => {
      const parent = makeEntity(world)
      parent.set(ecs.Position, {x: 0, y: 0, z: 3})
      const entity = makeEntity(world)
      parent.addChild(entity.eid)
      entity.lookAtWorld({x: 0, y: 3, z: 3})
      assertQuatEqual(entity.get(ecs.Quaternion), ecs.math.quat.xDegrees(-90))
    })
  })

  describe('getLocalPosition', () => {
    it('returns the local position of the entity', () => {
      const entity = makeEntity(world)
      entity.set(ecs.Position, {x: 1, y: 2, z: 3})
      assertVec3Equal(entity.getLocalPosition(), {x: 1, y: 2, z: 3})
    })
  })

  describe('getLocalTransform', () => {
    it('returns the local transform of the entity', () => {
      const entity = makeEntity(world)
      entity.set(ecs.Position, {x: 1, y: 2, z: 3})
      entity.set(ecs.Quaternion, ecs.math.quat.xDegrees(45))
      entity.set(ecs.Scale, {x: 2, y: 2, z: 2})

      const expectedTransform = ecs.math.mat4.trs(
        {x: 1, y: 2, z: 3},
        ecs.math.quat.xDegrees(45),
        {x: 2, y: 2, z: 2}
      )

      assertMat4Equal(entity.getLocalTransform(), expectedTransform)
    })
  })

  describe('getWorldPosition', () => {
    it('returns the world position of the entity', () => {
      const parent = makeEntity(world)
      parent.set(ecs.Position, {x: 1, y: 2, z: 3})
      parent.set(ecs.Scale, {x: 2, y: 2, z: 2})
      const child = makeEntity(world)
      parent.addChild(child.eid)
      child.set(ecs.Position, {x: 4, y: 5, z: 6})
      assertVec3Equal(child.getWorldPosition(), {x: 9, y: 12, z: 15})
    })
  })

  describe('getWorldTransform', () => {
    it('returns the world transform of the entity', () => {
      const parent = makeEntity(world)
      parent.set(ecs.Position, {x: 1, y: 2, z: 3})
      parent.set(ecs.Scale, {x: 2, y: 2, z: 2})
      const child = makeEntity(world)
      parent.addChild(child.eid)
      child.set(ecs.Quaternion, ecs.math.quat.xDegrees(45))
      child.set(ecs.Position, {x: 4, y: 5, z: 6})

      const expectedTransform = ecs.math.mat4.trs(
        {x: 9, y: 12, z: 15},
        ecs.math.quat.xDegrees(45),
        {x: 2, y: 2, z: 2}
      )

      assertMat4Equal(child.getWorldTransform(), expectedTransform)
    })
  })

  describe('setLocalPosition', () => {
    it('sets the local position of the entity', () => {
      const entity = makeEntity(world)
      entity.setLocalPosition({x: 1, y: 2, z: 3})
      assertVec3Equal(entity.get(ecs.Position), {x: 1, y: 2, z: 3})
    })
  })

  describe('setLocalTransform', () => {
    it('sets the local transform of the entity', () => {
      const parent = makeEntity(world)
      parent.set(ecs.Position, {x: 1, y: 2, z: 3})
      parent.set(ecs.Scale, {x: 2, y: 2, z: 2})
      const entity = makeEntity(world)
      parent.addChild(entity.eid)

      const transform = ecs.math.mat4.trs(
        {x: 3, y: 4, z: 5},
        ecs.math.quat.xDegrees(45),
        {x: 4, y: 4, z: 4}
      )

      entity.setLocalTransform(transform)
      assertVec3Equal(entity.get(ecs.Position), {x: 3, y: 4, z: 5})
      assertQuatEqual(entity.get(ecs.Quaternion), ecs.math.quat.xDegrees(45))
      assertVec3Equal(entity.get(ecs.Scale), {x: 4, y: 4, z: 4})
    })
  })

  describe('setWorldPosition', () => {
    it('sets the world position of the entity', () => {
      const parent = makeEntity(world)
      parent.set(ecs.Position, {x: 1, y: 2, z: 3})
      parent.set(ecs.Scale, {x: 2, y: 2, z: 2})
      const entity = makeEntity(world)
      parent.addChild(entity.eid)
      entity.setWorldPosition({x: 3, y: 4, z: 5})
      assertVec3Equal(entity.get(ecs.Position), {x: 1, y: 1, z: 1})
    })
  })

  describe('setWorldTransform', () => {
    it('sets the world transform of the entity', () => {
      const parent = makeEntity(world)
      parent.set(ecs.Position, {x: 1, y: 2, z: 3})
      parent.set(ecs.Scale, {x: 2, y: 2, z: 2})
      const entity = makeEntity(world)
      parent.addChild(entity.eid)

      const transform = ecs.math.mat4.trs(
        {x: 3, y: 4, z: 5},
        ecs.math.quat.xDegrees(45),
        {x: 4, y: 4, z: 4}
      )

      entity.setWorldTransform(transform)
      assertVec3Equal(entity.get(ecs.Position), {x: 1, y: 1, z: 1})
      assertQuatEqual(entity.get(ecs.Quaternion), ecs.math.quat.xDegrees(45))
      assertVec3Equal(entity.get(ecs.Scale), {x: 2, y: 2, z: 2})
    })
  })
})
