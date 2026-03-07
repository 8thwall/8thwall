// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, before, assert as chaiAssert} from '@nia/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'

describe('entity deletion test', () => {
  before(async () => {
    await ecs.ready()
  })
  it('should correctly delete entities', async () => {
    const window = {ecs}
    const assert = (statement: boolean, label: string) => {
      chaiAssert.isTrue(statement, label)
    }

    {
      const world = window.ecs.createWorld(...initThree())
      assert(world.allEntities.size === 0, 'initializes with 0 entities')
      const entity = world.createEntity()
      assert(world.allEntities.size === 1, 'creates an entity')
      world.deleteEntity(entity)
      assert(world.allEntities.size === 0, 'deletes an entity')
      world.destroy()
    }

    {
      const world = window.ecs.createWorld(...initThree())
      const parent = world.createEntity()
      const child = world.createEntity()
      world.setParent(child, parent)
      assert(world.allEntities.size === 2, 'creates 2 entities')
      world.deleteEntity(parent)
      assert(world.allEntities.size === 0, 'child is deleted by deleting parent')
      world.destroy()
    }

    {
      const world = window.ecs.createWorld(...initThree())
      const parent = world.createEntity()
      const child = world.createEntity()
      const grandchild = world.createEntity()
      world.createEntity()
      world.setParent(child, parent)
      world.setParent(grandchild, child)
      assert(world.allEntities.size === 4, 'creates 4 entities')
      ecs.Disabled.set(world, parent)
      world.deleteEntity(parent)
      assert(world.allEntities.size === 1, 'children are deleted by deleting disabled parent')
      world.destroy()
    }

    {
      const world1 = window.ecs.createWorld(...initThree())
      const world1Parent = world1.createEntity()
      const world1Child = world1.createEntity()
      world1.setParent(world1Child, world1Parent)

      const world2 = window.ecs.createWorld(...initThree())
      const world2Parent = world2.createEntity()
      const world2Child = world2.createEntity()
      const world2GrandChild = world2.createEntity()
      world2.setParent(world2Child, world2Parent)
      world2.setParent(world2GrandChild, world2Child)

      assert(world1.allEntities.size === 2, 'world 1 has 2 entities')
      assert(world2.allEntities.size === 3, 'world 2 has 3 entities')

      world1.deleteEntity(world1Parent)
      assert(world1.allEntities.size === 0, 'world 1 has 0 entities')
      assert(world2.allEntities.size === 3, 'world 2 has 3 entities')

      world2.deleteEntity(world2Child)
      assert(world1.allEntities.size === 0, 'world 1 has 0 entities')
      assert(world2.allEntities.size === 1, 'world 2 has 1 entity')
      assert(world2.allEntities.has(world2Parent), 'world 2 last eid is parent')
      world1.destroy()
      world2.destroy()
    }
  })
})
