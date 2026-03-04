// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, assert, beforeEach, before} from '@nia/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'

const ROOT_EID = 0n

describe('getChildren behavior', () => {
  let world: ecs.World

  const children = (eid: ecs.Eid) => Array.from(world.getChildren(eid))

  before(async () => {
    await ecs.ready()
  })

  beforeEach(async () => {
    await ecs.ready()
    if (world) {
      world.destroy()
    }
    world = ecs.createWorld(...initThree())
  })
  it('Can list all children adding incrementally', () => {
    assert.deepEqual(children(ROOT_EID), [])
    const child1 = world.createEntity()
    assert.deepEqual(children(ROOT_EID), [child1])
    const child2 = world.createEntity()
    assert.deepEqual(children(ROOT_EID), [child1, child2])
    const child3 = world.createEntity()
    assert.deepEqual(children(ROOT_EID), [child1, child2, child3])
    const child4 = world.createEntity()
    assert.deepEqual(children(ROOT_EID), [child1, child2, child3, child4])
  })
  it('Can list all children removing incrementally', () => {
    assert.deepEqual(children(ROOT_EID), [])
    const child1 = world.createEntity()
    const child2 = world.createEntity()
    const child3 = world.createEntity()
    const child4 = world.createEntity()
    assert.deepEqual(children(ROOT_EID), [child1, child2, child3, child4])
    world.deleteEntity(child4)
    assert.deepEqual(children(ROOT_EID), [child1, child2, child3])
    world.deleteEntity(child3)
    assert.deepEqual(children(ROOT_EID), [child1, child2])
    world.deleteEntity(child2)
    assert.deepEqual(children(ROOT_EID), [child1])
    world.deleteEntity(child1)
    assert.deepEqual(children(ROOT_EID), [])
  })
  it('Can enumerate all children recursively', () => {
    const allChildren: ecs.Eid[] = []
    const child1 = world.createEntity()
    const child2 = world.createEntity()
    const child3 = world.createEntity()
    const child4 = world.createEntity()
    world.setParent(child4, child3)
    const enumerate = (eid: ecs.Eid) => {
      for (const child of world.getChildren(eid)) {
        allChildren.push(child)
        enumerate(child)
      }
    }
    enumerate(ROOT_EID)
    assert.equal(allChildren.length, 4)
    assert.deepEqual(allChildren, [child1, child2, child3, child4])
  })

  it('Defining a component does not add a child', () => {
    const TestComponent = ecs.registerComponent({
      name: 'TestComponent',
      schema: {
        value: ecs.string,
      },
    })

    TestComponent.forWorld(world)
    assert.deepEqual(children(ROOT_EID), [])
  })

  it('Returns children of a prefab', () => {
    const scene = world.loadScene({
      objects: {
        prefab: {
          id: 'prefab',
          prefab: true,
          position: [0, 0, 0],
          rotation: [0, 0, 0, 1],
          scale: [1, 1, 1],
          components: {},
        },
        child: {
          id: 'child',
          parentId: 'prefab',
          position: [0, 0, 0],
          rotation: [0, 0, 0, 1],
          scale: [1, 1, 1],
          components: {},
        },
      },
    })

    assert.deepEqual(children(ROOT_EID), [])

    assert.deepEqual(children(scene.graphIdToPrefab.get('prefab')!), [
      scene.graphIdToPrefab.get('child')!,
    ])
  })
})
