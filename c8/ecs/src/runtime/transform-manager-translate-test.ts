// @package(npm-ecs)
// @attr(esnext = 1)
// @attr(externalize_npm = 1)
import {describe, it, beforeEach, afterEach} from '@repo/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'
import type {World} from './world'
import {assertPosition} from './transform-test-helpers'
import type {TransformManager} from './transform-manager-types'

describe('Transform Manager Translate Functions', () => {
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

  describe('translateSelf', () => {
    it('moves an object', () => {
      const eid = world.createEntity()
      transform.translateSelf(eid, {x: 1, y: 2, z: 3})
      const expected = ecs.math.vec3.xyz(1, 2, 3)
      assertPosition(world, eid, expected)
    })

    it('accepts partials', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 1, y: 2, z: 3})
      transform.translateSelf(eid, {x: 100})
      const expected = ecs.math.vec3.xyz(101, 2, 3)
      assertPosition(world, eid, expected)
    })

    it('uses local rotation of the object', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 1, y: 2, z: 3})
      ecs.Quaternion.set(world, eid, ecs.math.quat.yDegrees(180))
      transform.translateSelf(eid, {z: 100})
      const expected = ecs.math.vec3.xyz(1, 2, -97)
      assertPosition(world, eid, expected)
    })
  })

  describe('translateLocal', () => {
    it('moves an object', () => {
      const eid = world.createEntity()
      transform.translateLocal(eid, {x: 1, y: 2, z: 3})
      const expected = ecs.math.vec3.xyz(1, 2, 3)
      assertPosition(world, eid, expected)
    })

    it('accepts partials', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 1, y: 2, z: 3})
      transform.translateLocal(eid, {x: 100})
      const expected = ecs.math.vec3.xyz(101, 2, 3)
      assertPosition(world, eid, expected)
    })

    it('is not affected by local rotation of the object', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 1, y: 2, z: 3})
      ecs.Quaternion.set(world, eid, ecs.math.quat.yDegrees(180))
      transform.translateLocal(eid, {z: 100})
      const expected = ecs.math.vec3.xyz(1, 2, 103)
      assertPosition(world, eid, expected)
    })
  })

  describe('translateWorld', () => {
    it('moves an object', () => {
      const eid = world.createEntity()
      transform.translateWorld(eid, {x: 1, y: 2, z: 3})
      const expected = ecs.math.vec3.xyz(1, 2, 3)
      assertPosition(world, eid, expected)
    })

    it('accepts partials', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 1, y: 2, z: 3})
      transform.translateWorld(eid, {x: 100})
      const expected = ecs.math.vec3.xyz(101, 2, 3)
      assertPosition(world, eid, expected)
    })

    it('is not affected by local rotation of the object', () => {
      const eid = world.createEntity()
      ecs.Position.set(world, eid, {x: 1, y: 2, z: 3})
      ecs.Quaternion.set(world, eid, ecs.math.quat.yDegrees(180))
      transform.translateWorld(eid, {z: 100})
      const expected = ecs.math.vec3.xyz(1, 2, 103)
      assertPosition(world, eid, expected)
    })

    it('is affected by rotation of the parent', () => {
      const parent = world.createEntity()
      const child = world.createEntity()
      world.setParent(child, parent)
      ecs.Quaternion.set(world, parent, ecs.math.quat.yDegrees(180))
      transform.translateWorld(child, {z: 100})
      const expected = ecs.math.vec3.xyz(0, 0, -100)
      assertPosition(world, child, expected)
    })

    it('is affected by the scale of the parent', () => {
      const parent = world.createEntity()
      const child = world.createEntity()
      world.setParent(child, parent)
      ecs.Scale.set(world, parent, ecs.math.vec3.scale(10))
      transform.translateWorld(child, {z: 100})
      const expected = ecs.math.vec3.xyz(0, 0, 10)
      assertPosition(world, child, expected)
    })
  })
})
