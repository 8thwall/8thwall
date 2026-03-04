// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)
import {describe, it, assert, beforeEach, afterEach} from '@nia/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'

import type {World} from './world'

const makeEntity = (world: World) => world.getEntity(world.createEntity())

describe('Entity Impl', () => {
  let world: World

  beforeEach(async () => {
    await ecs.ready()
    world = ecs.createWorld(...initThree())
  })

  afterEach(() => {
    world.destroy()
  })

  describe('eid', () => {
    it('is set', () => {
      const entity = makeEntity(world)
      assert.isOk(entity.eid)
    })

    it('is unique', () => {
      const entity1 = makeEntity(world)
      const entity2 = makeEntity(world)
      assert.notEqual(entity1.eid, entity2.eid)
    })
  })

  describe('get', () => {
    it('returns expected properties', () => {
      const entity = makeEntity(world)
      ecs.Position.set(world, entity.eid, {x: 1, y: 2, z: 3})
      assert.deepStrictEqual(entity.get(ecs.Position), {x: 1, y: 2, z: 3})
    })

    it('returns a unique object per access', () => {
      const entity = makeEntity(world)
      const position1 = entity.get(ecs.Position)
      ecs.Position.set(world, entity.eid, {x: 1, y: 2, z: 3})
      const position2 = entity.get(ecs.Position)
      assert.deepStrictEqual(position1, {x: 0, y: 0, z: 0})
      assert.deepStrictEqual(position2, {x: 1, y: 2, z: 3})
    })

    it('returns different data per entity', () => {
      const entity1 = makeEntity(world)
      const entity2 = makeEntity(world)
      ecs.Position.set(world, entity1.eid, {x: 1, y: 2, z: 3})
      ecs.Position.set(world, entity2.eid, {x: 4, y: 5, z: 6})
      assert.notStrictEqual(entity1.get(ecs.Position), entity2.get(ecs.Position))
    })
  })

  describe('has', () => {
    it('returns true for existing component', () => {
      const entity = makeEntity(world)
      assert.isTrue(entity.has(ecs.Position))
    })

    it('returns false for non-existing component', () => {
      const entity = makeEntity(world)
      assert.isFalse(entity.has(ecs.Hidden))
    })
  })

  describe('set', () => {
    it('sets component data', () => {
      const entity = makeEntity(world)
      entity.set(ecs.Position, {x: 4, y: 5, z: 6})
      assert.deepStrictEqual(ecs.Position.get(world, entity.eid), {x: 4, y: 5, z: 6})
    })
  })

  describe('remove', () => {
    it('removes components', () => {
      const entity = makeEntity(world)
      ecs.Audio.set(world, entity.eid, {url: 'test.mp3'})
      assert.isTrue(ecs.Audio.has(world, entity.eid))
      entity.remove(ecs.Audio)
      assert.isFalse(ecs.Audio.has(world, entity.eid))
    })
  })

  describe('reset', () => {
    it('resets components', () => {
      const entity = makeEntity(world)
      ecs.Audio.set(world, entity.eid, {url: 'test.mp3'})
      assert.isTrue(ecs.Audio.has(world, entity.eid))
      entity.reset(ecs.Audio)
      assert.isTrue(ecs.Audio.has(world, entity.eid))
      assert.strictEqual(ecs.Audio.get(world, entity.eid).url, '')
    })
  })

  describe('hide', () => {
    it('hides the entity', () => {
      const entity = makeEntity(world)
      entity.hide()
      assert.isTrue(ecs.Hidden.has(world, entity.eid))
    })
  })

  describe('show', () => {
    it('shows the entity', () => {
      const entity = makeEntity(world)
      entity.hide()
      assert.isTrue(ecs.Hidden.has(world, entity.eid))
      entity.show()
      assert.isFalse(ecs.Hidden.has(world, entity.eid))
    })
  })

  describe('isHidden', () => {
    it('returns true if hidden', () => {
      const entity = makeEntity(world)
      ecs.Hidden.reset(world, entity.eid)
      assert.isTrue(entity.isHidden())
    })

    it('returns false if not hidden', () => {
      const entity = makeEntity(world)
      assert.isFalse(entity.isHidden())
    })
  })

  describe('disable', () => {
    it('disables the entity', () => {
      const entity = makeEntity(world)
      entity.disable()
      assert.isTrue(ecs.Disabled.has(world, entity.eid))
    })
  })

  describe('enable', () => {
    it('enables the entity', () => {
      const entity = makeEntity(world)
      entity.disable()
      assert.isTrue(ecs.Disabled.has(world, entity.eid))
      entity.enable()
      assert.isFalse(ecs.Disabled.has(world, entity.eid))
    })
  })

  describe('isDisabled', () => {
    it('returns true if disabled', () => {
      const entity = makeEntity(world)
      ecs.Disabled.reset(world, entity.eid)
      assert.isTrue(entity.isDisabled())
    })

    it('returns false if not disabled', () => {
      const entity = makeEntity(world)
      assert.isFalse(entity.isDisabled())
    })
  })

  describe('delete', () => {
    it('deletes the entity', () => {
      const entity = makeEntity(world)
      entity.delete()
      assert.isTrue(entity.isDeleted())
    })
  })

  describe('isDeleted', () => {
    it('returns true if deleted', () => {
      const entity = makeEntity(world)
      entity.delete()
      assert.isTrue(entity.isDeleted())
    })

    it('returns false if not deleted', () => {
      const entity = makeEntity(world)
      assert.isFalse(entity.isDeleted())
    })
  })

  describe('setParent', () => {
    it('sets the parent entity', () => {
      const parentEid = world.createEntity()
      const childEntity = makeEntity(world)
      childEntity.setParent(parentEid)
      assert.equal(world.getParent(childEntity.eid), parentEid)
    })

    it('accepts Entity references', () => {
      const parent = makeEntity(world)
      const child = makeEntity(world)
      child.setParent(parent)
      assert.equal(world.getParent(child.eid), parent.eid)
    })

    it('accepts null or undefined', () => {
      const childEntity = makeEntity(world)
      childEntity.setParent(1n)
      childEntity.setParent(null)
      assert.equal(world.getParent(childEntity.eid), 0n)

      childEntity.setParent(1n)
      childEntity.setParent(undefined)
      assert.equal(world.getParent(childEntity.eid), 0n)
    })
  })

  describe('getChildren', () => {
    it('returns child entities', () => {
      const entity = makeEntity(world)
      const child = makeEntity(world)
      world.setParent(child.eid, entity.eid)
      const children = entity.getChildren()
      assert.strictEqual(children.length, 1)
      assert.strictEqual(children[0], child)
    })
  })

  describe('getParent', () => {
    it('returns the parent entity', () => {
      const entity = makeEntity(world)
      const parentEid = world.createEntity()
      world.setParent(entity.eid, parentEid)
      const parent = world.getEntity(parentEid)
      assert.strictEqual(entity.getParent(), parent)
    })

    it('returns null if no parent', () => {
      const entity = makeEntity(world)
      assert.isNull(entity.getParent())
    })
  })

  describe('addChild', () => {
    it('adds a child entity', () => {
      const entity = makeEntity(world)
      const childEid = world.createEntity()
      entity.addChild(childEid)
      assert.strictEqual(world.getParent(childEid), entity.eid)
    })
    it('accepts Entity references', () => {
      const parent = makeEntity(world)
      const child = makeEntity(world)
      parent.addChild(child)
      assert.strictEqual(world.getParent(child.eid), parent.eid)
    })
  })
})
