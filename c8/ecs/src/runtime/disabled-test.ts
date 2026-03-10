// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, before, assert} from '@repo/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'

describe('Disabled behavior', () => {
  before(async () => {
    await ecs.ready()
  })

  it('should update disabled entities correctly', async () => {
    const callbackCounts = new Map()

    const resetCounts = () => {
      callbackCounts.set('add', 0)
      callbackCounts.set('remove', 0)
      callbackCounts.set('changed', 0)
    }

    resetCounts()

    const TestAttribute = ecs.registerComponent({
      name: 'test-attribute',
      schema: {
        a: ecs.f32,
        b: ecs.string,
        c: ecs.boolean,
      },
    })

    const testQuery = ecs.defineQuery([TestAttribute])

    const enter = ecs.enterQuery(testQuery)
    const changed = ecs.changedQuery(testQuery)
    const exit = ecs.exitQuery(testQuery)

    const testEnter = () => {
      callbackCounts.set('add', (callbackCounts.get('add') || 0) + 1)
    }

    const testExit = () => {
      callbackCounts.set('remove', (callbackCounts.get('remove') || 0) + 1)
    }

    const testChange = () => {
      callbackCounts.set('changed', (callbackCounts.get('changed') || 0) + 1)
    }

    const expectAdds = (expected: number, label: string, desc: string) => {
      const actual = callbackCounts.get('add') || 0
      const error = `Expected [adds] ${expected} ${label} events, got ${actual}`
      assert.equal(actual, expected, `${error}on test:${desc}`)
    }

    const expectRemoves = (expected: number, label: string, desc: string) => {
      const actual = callbackCounts.get('remove') || 0
      const error = `Expected [removes] ${expected} ${label} events, got ${actual}`
      assert.equal(actual, expected, `${error}on test:${desc}`)
    }

    const world = ecs.createWorld(...initThree())

    // @ts-ignore
    const expectSceneParent = (childObject, parentObject, label, desc: string) => {
      const actualParentUUID = childObject?.parent?.uuid
      const expectedParentUUID = parentObject?.uuid
      const error = `
        Expected [scene parent] ${label} to be ${expectedParentUUID}, got ${actualParentUUID}`
      assert.equal(actualParentUUID, expectedParentUUID, `${error}on test:${desc}`)
    }

    // @ts-ignore
    const countObjects = (scene) => {
      let count = 0

      scene.traverse(() => {
        count++
      })

      return count
    }

    const expectSceneObjects = (expected: number, label: string, desc: string) => {
      const actual = countObjects(world.three.scene)
      const error = `Expected [scene objects] ${label} count to be ${expected}, got ${actual}`
      assert.equal(actual, expected, `${error}on test:${desc}`)
    }

    const TestComponent = ecs.registerComponent({
      name: 'test',
      schema: {period: ecs.f32},
      add: () => {
        callbackCounts.set('add', (callbackCounts.get('add') || 0) + 1)
      },
      remove: () => {
        callbackCounts.set('remove', (callbackCounts.get('remove') || 0) + 1)
      },
    })

    const eid = world.createEntity()
    const eid2 = world.createEntity()

    const childEid = world.createEntity()
    const grandchildEid = world.createEntity()

    const resetToEnabled = () => {
      ecs.Disabled.remove(world, childEid)
      ecs.Disabled.remove(world, grandchildEid)
      ecs.Disabled.remove(world, eid)
      ecs.Disabled.remove(world, eid2)
    }

    ecs.registerBehavior((w) => {
      exit(w).forEach(testExit)
      enter(w).forEach(testEnter)
      changed(w).forEach(testChange)
    })

    let desc: string = ''
    world.tick()

    desc = 'Doesn\'t have component, remove'
    TestComponent.remove(world, eid)
    world.tick()
    expectRemoves(0, 'remove', desc)
    expectAdds(0, 'add', desc)
    resetCounts()

    desc = 'Doesn\'t have component, set'
    TestComponent.set(world, eid)
    TestComponent.set(world, eid2)
    world.tick()
    expectRemoves(0, 'remove', desc)
    expectAdds(2, 'add', desc)
    resetCounts()

    desc = 'Disable/enable an entity with the component'
    TestComponent.set(world, eid)
    world.tick()
    expectRemoves(0, 'remove', desc)
    expectAdds(0, 'add', desc)
    ecs.Disabled.set(world, eid)
    world.tick()
    expectRemoves(1, 'remove', desc)
    ecs.Disabled.remove(world, eid)
    world.tick()
    expectAdds(1, 'add', desc)
    resetCounts()

    desc = 'Disable/enable an entity with a child entity that both have the component'
    TestComponent.set(world, childEid)
    world.setParent(childEid, eid)
    world.tick()
    expectRemoves(0, 'remove', desc)
    expectAdds(1, 'add', desc)
    resetCounts()
    ecs.Disabled.set(world, eid)
    world.tick()
    expectRemoves(2, 'remove', desc)
    expectAdds(0, 'add', desc)
    resetCounts()
    ecs.Disabled.remove(world, eid)
    world.tick()
    expectRemoves(0, 'remove', desc)
    expectAdds(2, 'add', desc)
    resetCounts()

    desc = 'Disable/enable an entity with a child entity that only the child has the component'
    TestComponent.remove(world, eid)
    world.setParent(childEid, eid)
    world.tick()
    expectRemoves(1, 'remove', desc)
    expectAdds(0, 'add', desc)
    resetCounts()
    ecs.Disabled.set(world, eid)
    world.tick()
    expectRemoves(1, 'remove', desc)
    expectAdds(0, 'add', desc)
    resetCounts()
    ecs.Disabled.remove(world, eid)
    world.tick()
    expectRemoves(0, 'remove', desc)
    expectAdds(1, 'add', desc)
    resetCounts()

    desc = 'Disable/enable an entity with a nested child entities that all have the component'
    TestComponent.set(world, eid)
    TestComponent.set(world, grandchildEid)
    world.setParent(grandchildEid, childEid)
    world.setParent(childEid, eid)
    world.tick()
    expectRemoves(0, 'remove', desc)
    expectAdds(2, 'add', desc)
    resetCounts()
    ecs.Disabled.set(world, eid)
    world.tick()
    expectRemoves(3, 'remove', desc)
    expectAdds(0, 'add', desc)
    resetCounts()
    ecs.Disabled.remove(world, eid)
    world.tick()
    expectRemoves(0, 'remove', desc)
    expectAdds(3, 'add', desc)
    resetCounts()

    desc =
    'Disable/enable an entity with a child entity and a parent entity that both have the component'
    ecs.Disabled.set(world, childEid)
    world.tick()
    expectRemoves(2, 'remove', desc)
    expectAdds(0, 'add', desc)
    resetCounts()
    ecs.Disabled.remove(world, eid)
    world.tick()
    expectRemoves(0, 'remove', desc)
    expectAdds(0, 'add', desc)
    resetCounts()
    resetToEnabled()
    world.tick()
    resetCounts()

    desc = 'Child and grandchild should stay disabled after enabling parent if child is disabled'
    ecs.Disabled.set(world, eid)
    world.tick()
    expectRemoves(3, 'remove', desc)
    expectAdds(0, 'add', desc)
    resetCounts()
    ecs.Disabled.set(world, childEid)
    world.tick()
    expectRemoves(0, 'remove', desc)
    expectAdds(0, 'add', desc)
    resetCounts()
    ecs.Disabled.remove(world, eid)
    world.tick()
    expectRemoves(0, 'remove', desc)
    expectAdds(1, 'add', desc)
    resetCounts()
    resetToEnabled()
    world.tick()
    resetCounts()

    desc = 'Child should stay disabled after enabling child if the parent is disabled'
    ecs.Disabled.set(world, eid)
    ecs.Disabled.set(world, childEid)
    world.tick()
    expectRemoves(3, 'remove', desc)
    expectAdds(0, 'add', desc)
    resetCounts()
    ecs.Disabled.remove(world, childEid)
    world.tick()
    expectRemoves(0, 'remove', desc)
    expectAdds(0, 'add', desc)
    ecs.Disabled.remove(world, grandchildEid)
    world.tick()
    expectRemoves(0, 'remove', desc)
    expectAdds(0, 'add', desc)
    ecs.Disabled.remove(world, eid)
    world.tick()
    expectRemoves(0, 'remove', desc)
    expectAdds(3, 'add', desc)
    resetCounts()
    resetToEnabled()
    world.tick()

    desc = 'Child should be disabled after re-parenting to a disabled parent'
    ecs.Disabled.set(world, eid2)
    world.setParent(childEid, eid2)
    world.tick()
    expectRemoves(3, 'remove', desc)
    expectAdds(0, 'add', desc)
    resetCounts()
    world.setParent(childEid, eid)
    world.tick()
    expectRemoves(0, 'remove', desc)
    expectAdds(2, 'add', desc)
    resetCounts()
    resetToEnabled()
    world.tick()
    resetCounts()

    desc = 'Disabled child should stay disabled after re-parenting to an enabled parent'
    ecs.Disabled.set(world, childEid)
    world.tick()
    expectRemoves(2, 'remove', desc)
    expectAdds(0, 'add', desc)
    resetCounts()
    world.setParent(childEid, eid2)
    world.tick()
    expectRemoves(0, 'remove', desc)
    expectAdds(0, 'add', desc)
    resetToEnabled()
    world.tick()
    resetCounts()

    desc = 'Un-parent child out of disabled parent should re-enable them'
    world.setParent(childEid, eid)
    ecs.Disabled.set(world, eid)
    world.tick()
    expectRemoves(3, 'remove', desc)
    expectAdds(0, 'add', desc)
    resetCounts()
    world.setParent(childEid, BigInt(0))
    world.tick()
    expectRemoves(0, 'remove', desc)
    expectAdds(2, 'add', desc)
    resetToEnabled()
    world.tick()
    resetCounts()

    desc = 'threejs scene should only contain enabled entities'
    expectSceneObjects(5, 'initial scene objects', desc)
    world.setParent(childEid, eid)
    world.setParent(grandchildEid, childEid)
    ecs.Disabled.set(world, eid)
    world.tick()
    expectSceneObjects(2, 'scene objects after disabling an entity with 2 children', desc)
    ecs.Disabled.remove(world, eid)
    world.tick()
    expectSceneObjects(5, 'scene objects after enabling an entity with 2 children', desc)
    resetToEnabled()
    world.tick()
    resetCounts()

    desc =
    'disabling an child and re-enabling it should disable it\'s children in the threejs scene'
    expectSceneObjects(5, 'initial scene objects', desc)
    ecs.Disabled.set(world, childEid)
    world.tick()
    expectSceneObjects(3, 'scene objects after disabling a child entity with one child', desc)
    ecs.Disabled.remove(world, childEid)
    world.tick()
    expectSceneObjects(5, 'scene objects after enabling a child entity with one child', desc)

    resetToEnabled()
    world.tick()
    resetCounts()

    desc =
    'disabling an child and re-enabling it should re-parent it to it\'s parent in the threejs scene'
    expectSceneParent(
      world.three.entityToObject.get(childEid),
      world.three.entityToObject.get(eid),
      'initial parent',
      desc
    )
    ecs.Disabled.set(world, childEid)
    world.tick()
    expectSceneParent(
      world.three.entityToObject.get(childEid),
      undefined,
      'no parent after disabling',
      desc
    )
    ecs.Disabled.remove(world, childEid)
    world.tick()
    expectSceneParent(
      world.three.entityToObject.get(childEid),
      world.three.entityToObject.get(eid),
      'parent after enabling',
      desc
    )
    resetToEnabled()
    world.tick()
    resetCounts()

    desc = 'changing the parent of a disabled object should not add it to the scene'
    expectSceneObjects(5, 'initial scene objects', desc)
    ecs.Disabled.set(world, childEid)
    world.tick()
    expectSceneObjects(3, 'scene objects after disabling a child entity with one child', desc)
    world.setParent(childEid, eid2)
    world.tick()
    expectSceneObjects(3, 'scene objects after changing the parent of a disabled entity', desc)
    resetToEnabled()
    world.tick()
    resetCounts()

    desc = 'changing the parent of an object to a disabled object should remove it from the scene'
    world.setParent(childEid, eid)
    world.tick()
    expectSceneObjects(5, 'initial scene objects', desc)
    ecs.Disabled.set(world, eid2)
    world.tick()
    expectSceneObjects(4, 'scene objects after disabling a parent entity', desc)
    world.setParent(childEid, eid2)
    world.tick()
    expectSceneObjects(
      2,
      'scene objects after changing the parent of an entity to a disabled entity',
      desc
    )
    resetToEnabled()
    world.tick()
    resetCounts()

    desc = 'changing the parent of a object from a disabled object to a disabled object ' +
           'should parent the three object to the new parent'
    world.setParent(childEid, eid)
    ecs.Disabled.set(world, eid)
    world.tick()
    expectSceneParent(
      world.three.entityToObject.get(childEid),
      world.three.entityToObject.get(eid),
      'initial parent',
      desc
    )
    world.setParent(childEid, eid2)
    world.tick()
    expectSceneParent(
      world.three.entityToObject.get(childEid),
      world.three.entityToObject.get(eid2),
      'new parent',
      desc
    )
    resetToEnabled()
    world.tick()
    resetCounts()
  })
})
