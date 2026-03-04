// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, before, beforeEach, afterEach, assert} from '@nia/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'
import type {World} from './world'

describe('createWorld', () => {
  let world: World

  before(async () => {
    await ecs.ready()
  })

  beforeEach(() => {
    world = ecs.createWorld(...initThree())
  })

  afterEach(() => {
    world.destroy()
  })

  it('Can create a world', async () => {
    assert.typeOf(world, 'object')
  })

  it('Can create an entity within a world', async () => {
    const entity = world.createEntity()
    assert.typeOf(entity, 'bigint')
    assert.notEqual(entity, 0n)
  })

  it('Can tick the world', async () => {
    const entity = world.createEntity()
    assert.typeOf(entity, 'bigint')
    world.tick()
  })

  describe('getEntity', () => {
    it('Returns an Entity object', () => {
      const eid = world.createEntity()
      const entity = world.getEntity(eid)
      assert.isObject(entity)
      assert.equal(entity.eid, eid)
    })

    it('Caches Entity objects on demand', () => {
      assert.equal(world.eidToEntity.size, 0)
      const eid = world.createEntity()
      assert.equal(world.eidToEntity.size, 0)
      const entity = world.getEntity(eid)
      assert.equal(world.eidToEntity.size, 1)
      assert.isObject(entity)
      assert.equal(entity.eid, eid)

      const sameEntity = world.getEntity(eid)
      assert.equal(world.eidToEntity.size, 1)
      assert.strictEqual(entity, sameEntity)

      world.deleteEntity(eid)
      assert.equal(world.eidToEntity.size, 0)

      world.destroy()
    })

    it('Throws errors on deleted entities', () => {
      const eid = world.createEntity()
      const entity = world.getEntity(eid)
      assert.isObject(entity)
      assert.equal(entity.eid, eid)

      world.deleteEntity(eid)
      assert.throws(() => {
        world.getEntity(eid)
      }, /does not exist/)
    })

    it('Throws errors on invalid entities', () => {
      assert.throws(() => {
        world.getEntity(0n)
      }, 'was not provided')

      assert.throws(() => {
        // @ts-ignore
        world.getEntity(null)
      }, 'was not provided')

      assert.throws(() => {
        world.getEntity(123n)
      }, 'does not exist')
    })
  })
})
