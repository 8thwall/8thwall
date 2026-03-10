// @package(npm-ecs)
// @attr(esnext = 1)
// @attr(externalize_npm = 1)
import {describe, it, beforeEach, afterEach} from '@repo/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'
import type {World} from './world'
import {assertQuaternion} from './transform-test-helpers'
import type {TransformManager} from './transform-manager-types'

describe('Transform Manager Rotate Functions', () => {
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

  describe('rotateSelf', () => {
    it('rotates an object', () => {
      const eid = world.createEntity()
      transform.rotateSelf(eid, ecs.math.quat.yDegrees(90))
      const expected = ecs.math.quat.yDegrees(90)
      assertQuaternion(world, eid, expected)
    })
    it('rotates an object that already had some rotation', () => {
      const eid = world.createEntity()
      ecs.Quaternion.set(world, eid, ecs.math.quat.yDegrees(90))
      transform.rotateSelf(eid, ecs.math.quat.yDegrees(90))
      const expected = ecs.math.quat.yDegrees(180)
      assertQuaternion(world, eid, expected)
    })
    it('rotates an object from within its own space', () => {
      const eid = world.createEntity()
      ecs.Quaternion.set(world, eid, ecs.math.quat.yDegrees(45))
      transform.rotateSelf(eid, ecs.math.quat.xDegrees(-10))
      const expected = ecs.math.quat.yDegrees(45).times(ecs.math.quat.xDegrees(-10))
      assertQuaternion(world, eid, expected)
    })
  })

  describe('rotateLocal', () => {
    it('rotates an object', () => {
      const eid = world.createEntity()
      transform.rotateLocal(eid, ecs.math.quat.yDegrees(90))
      const expected = ecs.math.quat.yDegrees(90)
      assertQuaternion(world, eid, expected)
    })
    it('rotates an object that already had some rotation', () => {
      const eid = world.createEntity()
      ecs.Quaternion.set(world, eid, ecs.math.quat.yDegrees(90))
      transform.rotateLocal(eid, ecs.math.quat.yDegrees(90))
      const expected = ecs.math.quat.yDegrees(180)
      assertQuaternion(world, eid, expected)
    })
    it('rotates an object from outside its own space', () => {
      const eid = world.createEntity()
      ecs.Quaternion.set(world, eid, ecs.math.quat.yDegrees(45))
      transform.rotateLocal(eid, ecs.math.quat.xDegrees(-10))
      const expected = ecs.math.quat.xDegrees(-10).times(ecs.math.quat.yDegrees(45))
      assertQuaternion(world, eid, expected)
    })
  })
})
