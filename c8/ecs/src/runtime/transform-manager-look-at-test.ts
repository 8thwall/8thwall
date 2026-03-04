// @package(npm-ecs)
// @attr(esnext = 1)
// @attr(externalize_npm = 1)
import {describe, it, beforeEach, afterEach} from '@nia/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'
import type {World} from './world'
import type {TransformManager} from './transform-manager-types'
import {assertPosition, assertQuaternion, assertScale} from './transform-test-helpers'

describe('Transform Manager Look At Functions', () => {
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

  describe('lookAt', () => {
    it('should point the positive z towards the target', () => {
      const eid = world.createEntity()
      const targetEid = world.createEntity()
      ecs.Position.set(world, targetEid, {x: 0, y: 0, z: 3})
      transform.lookAt(eid, targetEid)
      assertQuaternion(world, eid, {x: 0, y: 0, z: 0, w: 1})
    })

    it('handles angles', () => {
      const eid = world.createEntity()
      const targetEid = world.createEntity()
      ecs.Position.set(world, targetEid, {x: 0, y: 30, z: 30})
      transform.lookAt(eid, targetEid)
      assertQuaternion(world, eid, ecs.math.quat.xDegrees(-45))
    })

    it('takes own position into account', () => {
      const eid = world.createEntity()
      const targetEid = world.createEntity()
      ecs.Position.set(world, targetEid, {x: 0, y: 30, z: 30})
      ecs.Position.set(world, eid, {x: 0, y: 0, z: 30})
      transform.lookAt(eid, targetEid)
      assertQuaternion(world, eid, ecs.math.quat.xDegrees(-90))
    })

    it('does not reset position/scale', () => {
      const eid = world.createEntity()
      const targetEid = world.createEntity()
      ecs.Position.set(world, eid, {x: 0, y: 0, z: 3})
      ecs.Scale.set(world, eid, {x: 2, y: 2, z: 2})
      transform.lookAt(eid, targetEid)
      assertQuaternion(world, eid, ecs.math.quat.yDegrees(180))
      assertPosition(world, eid, {x: 0, y: 0, z: 3})
      assertScale(world, eid, {x: 2, y: 2, z: 2})
    })

    it('works when eid has a parent transform', () => {
      const parent = world.createEntity()
      const child = world.createEntity()
      const targetEid = world.createEntity()
      world.setParent(child, parent)
      ecs.Scale.set(world, parent, {x: 100, y: 100, z: 100})
      ecs.Position.set(world, child, {x: 0, y: 0, z: 3})
      ecs.Position.set(world, targetEid, {x: 0, y: 300, z: 300})
      transform.lookAt(child, targetEid)
      assertQuaternion(world, child, ecs.math.quat.xDegrees(-90))
    })

    it('works when target has a parent transform', () => {
      const eid = world.createEntity()
      const parent = world.createEntity()
      const targetEid = world.createEntity()
      world.setParent(targetEid, parent)
      ecs.Quaternion.set(world, parent, ecs.math.quat.yDegrees(90))
      ecs.Position.set(world, parent, {x: 0, y: 0, z: 100})
      ecs.Position.set(world, targetEid, {x: 0, y: 0, z: 100})
      transform.lookAt(eid, targetEid)
      assertQuaternion(world, eid, ecs.math.quat.yDegrees(45))
    })
  })

  describe('lookAtLocal', () => {
    it('should point the positive z towards the target', () => {
      const eid = world.createEntity()
      transform.lookAtLocal(eid, {x: 0, y: 0, z: 3})
      assertQuaternion(world, eid, {x: 0, y: 0, z: 0, w: 1})
    })

    it('handles angles', () => {
      const eid = world.createEntity()
      transform.lookAtLocal(eid, {x: 0, y: 30, z: 30})
      assertQuaternion(world, eid, ecs.math.quat.xDegrees(-45))
    })

    it('takes own position into account', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 0, y: 0, z: 3})
      transform.lookAtLocal(eid, {x: 0, y: 0, z: 0})
      assertQuaternion(world, eid, ecs.math.quat.yDegrees(180))
    })

    it('does not reset position/scale', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 0, y: 0, z: 3})
      ecs.Scale.set(world, eid, {x: 2, y: 2, z: 2})
      transform.lookAtLocal(eid, {x: 0, y: 0, z: 0})
      assertQuaternion(world, eid, ecs.math.quat.yDegrees(180))
      assertPosition(world, eid, {x: 0, y: 0, z: 3})
      assertScale(world, eid, {x: 2, y: 2, z: 2})
    })

    it('works when eid has a parent transform', () => {
      const parent = world.createEntity()
      const child = world.createEntity()
      world.setParent(child, parent)
      ecs.Scale.set(world, parent, {x: 100, y: 100, z: 100})
      ecs.Position.set(world, child, {x: 0, y: 0, z: 3})
      transform.lookAtLocal(child, {x: 0, y: 300, z: 300})
      assertQuaternion(world, child, ecs.math.quat.xDegrees(-45))
    })

    it('handles non-uniform scale', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 100, y: 0, z: 0})
      ecs.Scale.set(world, eid, {x: 1, y: 1, z: 2})
      transform.lookAtLocal(eid, {x: 100, y: 15, z: 30})
      assertQuaternion(world, eid, ecs.math.quat.xDegrees(-45))
    })
  })

  describe('lookAtWorld', () => {
    it('should point the positive z towards the target', () => {
      const eid = world.createEntity()
      transform.lookAtWorld(eid, {x: 0, y: 0, z: 3})
      assertQuaternion(world, eid, {x: 0, y: 0, z: 0, w: 1})
    })

    it('handles angles', () => {
      const eid = world.createEntity()
      transform.lookAtWorld(eid, {x: 0, y: 30, z: 30})
      assertQuaternion(world, eid, ecs.math.quat.xDegrees(-45))
    })

    it('takes own position into account', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 0, y: 0, z: 3})
      transform.lookAtWorld(eid, {x: 0, y: 0, z: 0})
      assertQuaternion(world, eid, ecs.math.quat.yDegrees(180))
    })

    it('does not reset position/scale', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 0, y: 0, z: 3})
      ecs.Scale.set(world, eid, {x: 2, y: 2, z: 2})
      transform.lookAtWorld(eid, {x: 0, y: 0, z: 0})
      assertQuaternion(world, eid, ecs.math.quat.yDegrees(180))
      assertPosition(world, eid, {x: 0, y: 0, z: 3})
      assertScale(world, eid, {x: 2, y: 2, z: 2})
    })

    it('works when eid has a parent transform', () => {
      const parent = world.createEntity()
      const child = world.createEntity()
      world.setParent(child, parent)
      ecs.Scale.set(world, parent, {x: 100, y: 100, z: 100})
      ecs.Position.set(world, child, {x: 0, y: 0, z: 3})
      transform.lookAtWorld(child, {x: 0, y: 300, z: 300})
      assertQuaternion(world, child, ecs.math.quat.xDegrees(-90))
    })

    it('handles non-uniform scale on the parent', () => {
      const parent = world.createEntity()
      const child = world.createEntity()
      world.setParent(child, parent)
      ecs.Position.set(world, parent, {x: 100, y: 0, z: 0})
      ecs.Scale.set(world, parent, {x: 1, y: 1, z: 2})
      transform.lookAtWorld(child, {x: 100, y: 15, z: 30})
      assertQuaternion(world, child, ecs.math.quat.xDegrees(-45))
    })

    it('handles non-uniform scale on the child', () => {
      const parent = world.createEntity()
      const child = world.createEntity()
      world.setParent(child, parent)
      ecs.Position.set(world, parent, {x: 100, y: 0, z: 0})
      ecs.Scale.set(world, child, {x: 1, y: 1, z: 2})
      transform.lookAtWorld(child, {x: 100, y: 15, z: 30})
      assertQuaternion(world, child, ecs.math.quat.xDegrees(-45))
    })

    it('handles non-uniform scale on parent and child', () => {
      const parent = world.createEntity()
      const child = world.createEntity()
      world.setParent(child, parent)
      ecs.Position.set(world, parent, {x: 100, y: 0, z: 0})
      ecs.Scale.set(world, parent, {x: 1, y: 1, z: 2})
      ecs.Scale.set(world, child, {x: 1, y: 2, z: 1})
      transform.lookAtWorld(child, {x: 100, y: 30, z: 30})
      assertQuaternion(world, child, ecs.math.quat.xDegrees(-45))
    })
  })
})
