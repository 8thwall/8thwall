// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, beforeEach, afterEach, assert} from '@nia/bzl/js/chai-js'

import {initThree} from '../test-env'
import {ecs} from '../test-runtime-lib'
import type {World} from '../world'

describe('Transform Components', () => {
  let world: World
  beforeEach(async () => {
    await ecs.ready()
    world = ecs.createWorld(...initThree())
  })
  afterEach(() => {
    world.destroy()
  })

  describe('Position', () => {
    it('Defaults to 0,0,0', () => {
      const eid = world.createEntity()
      ecs.Position.reset(world, eid)
      assert.deepEqual(ecs.Position.get(world, eid), {x: 0, y: 0, z: 0})
    })
  })

  describe('Quaternion', () => {
    it('Defaults to 0,0,0,1', () => {
      const eid = world.createEntity()
      ecs.Quaternion.reset(world, eid)
      assert.deepEqual(ecs.Quaternion.get(world, eid), ecs.math.quat.zero())
    })
  })

  describe('Scale', () => {
    it('Defaults to 1,1,1', () => {
      const eid = world.createEntity()
      ecs.Scale.reset(world, eid)
      assert.deepEqual(ecs.Scale.get(world, eid), {x: 1, y: 1, z: 1})
    })
  })
})
