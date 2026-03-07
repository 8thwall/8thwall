// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, before, assert} from '@nia/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'

describe('independent observer test', () => {
  before(async () => {
    await ecs.ready()
  })

  it('Can run independent components', async () => {
    const window = {ecs}

    let currentMsg = ''
    const log = (...args: string[]) => {
      currentMsg = ''
      currentMsg = args.join(' ')
    }

    const callbackCounts = new Map()

    const resetCounts = () => {
      callbackCounts.set('add', 0)
      callbackCounts.set('remove', 0)
      callbackCounts.set('changed', 0)
    }

    resetCounts()

    const TestAttribute = window.ecs.registerComponent({
      name: 'test-attribute',
      schema: {
        a: window.ecs.f32,
        b: window.ecs.string,
        c: window.ecs.boolean,
      },
    })

    const testQuery = window.ecs.defineQuery([TestAttribute])

    const enter = window.ecs.enterQuery(testQuery)
    const changed = window.ecs.changedQuery(testQuery)
    const exit = window.ecs.exitQuery(testQuery)

    const testEnter = () => {
      callbackCounts.set('add', (callbackCounts.get('add') || 0) + 1)
    }

    const testExit = () => {
      callbackCounts.set('remove', (callbackCounts.get('remove') || 0) + 1)
    }

    const testChange = () => {
      callbackCounts.set('changed', (callbackCounts.get('changed') || 0) + 1)
    }

    window.ecs.registerBehavior((world) => {
      exit(world).forEach(testExit)
      enter(world).forEach(testEnter)
      changed(world).forEach(testChange)
    })

    const expectAdds = (expected: number, label: string) => {
      const actual = callbackCounts.get('add') || 0
      assert.equal(actual, expected,
        `'[adds]', ${label}, (${actual}, expected: ${expected})`)
    }

    const expectRemoves = (expected: number, label: string) => {
      const actual = callbackCounts.get('remove') || 0
      assert.equal(actual, expected,
        `'[removes]', ${label}, (${actual}, expected: ${expected}: ${currentMsg})`)
    }

    const expectChanged = (expected: number, label: string) => {
      const actual = callbackCounts.get('changed') || 0
      assert.equal(actual, expected,
        `'[changed]', ${label} (${actual}, expected: ${expected}): ${currentMsg}`)
    }

    const world = window.ecs.createWorld(...initThree())

    const eid = world.createEntity()

    // initial start
    world.tick()

    log('Doesn\'t have attribute, remove')
    TestAttribute.remove(world, eid)
    world.tick()
    expectRemoves(0, 'remove')
    expectAdds(0, 'add')
    expectChanged(0, 'changed')
    resetCounts()

    log('Doesn\'t have attribute, set')
    TestAttribute.set(world, eid)
    world.tick()
    expectRemoves(0, 'remove')
    expectAdds(1, 'add')
    expectChanged(1, 'changed')
    resetCounts()

    log('Has attribute, set')
    TestAttribute.set(world, eid)
    world.tick()
    expectRemoves(0, 'remove')
    expectAdds(0, 'add')
    expectChanged(1, 'changed')
    resetCounts()

    log('Has attribute, remove')
    TestAttribute.remove(world, eid)
    world.tick()
    expectRemoves(1, 'remove')
    expectAdds(0, 'add')
    expectChanged(0, 'changed')
    resetCounts()

    log('set * 3')
    TestAttribute.set(world, eid)
    TestAttribute.set(world, eid)
    TestAttribute.set(world, eid)
    world.tick()
    expectRemoves(0, 'remove')
    expectAdds(1, 'add')
    expectChanged(1, 'changed')
    resetCounts()

    log('Has attribute, set * 3')
    TestAttribute.set(world, eid)
    TestAttribute.set(world, eid)
    TestAttribute.set(world, eid)
    world.tick()
    expectRemoves(0, 'remove')
    expectAdds(0, 'add')
    expectChanged(1, 'changed')
    resetCounts()

    log('remove * 3')
    TestAttribute.remove(world, eid)
    TestAttribute.remove(world, eid)
    TestAttribute.remove(world, eid)
    world.tick()
    expectRemoves(1, 'remove')
    expectAdds(0, 'add')
    expectChanged(0, 'changed')
    resetCounts()

    log('Doesn\'t have attribute, set, remove')
    TestAttribute.set(world, eid)
    TestAttribute.remove(world, eid)
    world.tick()
    expectRemoves(1, 'remove')
    expectAdds(1, 'add')
    expectChanged(1, 'changed')
    resetCounts()

    log('Has attribute, set, remove')
    TestAttribute.set(world, eid)
    world.tick()
    resetCounts()
    TestAttribute.set(world, eid)
    TestAttribute.remove(world, eid)
    world.tick()
    expectRemoves(1, 'remove')
    expectAdds(0, 'add')
    expectChanged(1, 'changed')
    resetCounts()

    log('Doesn\'t have attribute, remove, set')
    TestAttribute.remove(world, eid)
    TestAttribute.set(world, eid)
    world.tick()
    expectRemoves(0, 'remove')
    expectAdds(1, 'add')
    expectChanged(1, 'changed')
    resetCounts()

    log('Has attribute, remove, set')
    TestAttribute.remove(world, eid)
    TestAttribute.set(world, eid)
    world.tick()
    expectRemoves(1, 'remove')
    expectAdds(1, 'add')
    expectChanged(1, 'changed')
    resetCounts()

    log('Has attribute, remove, set, set')
    TestAttribute.remove(world, eid)
    TestAttribute.set(world, eid)
    world.tick()
    expectRemoves(1, 'remove')
    expectAdds(1, 'add')
    expectChanged(1, 'changed')
    resetCounts()

    log('Doesn\'t have attribute, set, remove, set, remove, set')
    TestAttribute.remove(world, eid)
    world.tick()
    resetCounts()

    TestAttribute.set(world, eid)
    TestAttribute.remove(world, eid)
    TestAttribute.set(world, eid)
    TestAttribute.remove(world, eid)
    TestAttribute.set(world, eid)
    world.tick()
    expectRemoves(1, 'remove')
    expectAdds(1, 'add')
    expectChanged(1, 'changed')
    resetCounts()

    log('Has attribute, remove, set, remove, set, remove')
    TestAttribute.remove(world, eid)
    TestAttribute.set(world, eid)
    TestAttribute.remove(world, eid)
    TestAttribute.set(world, eid)
    TestAttribute.remove(world, eid)
    world.tick()
    expectRemoves(1, 'remove')
    expectAdds(1, 'add')
    expectChanged(1, 'changed')
    resetCounts()

    log('Has attribute, remove, remove, set, set, remove, set, set, set')
    TestAttribute.set(world, eid)
    world.tick()
    resetCounts()

    TestAttribute.remove(world, eid)
    TestAttribute.remove(world, eid)
    TestAttribute.set(world, eid)
    TestAttribute.set(world, eid)
    TestAttribute.remove(world, eid)
    TestAttribute.set(world, eid)
    TestAttribute.set(world, eid)
    TestAttribute.set(world, eid)
    world.tick()
    expectRemoves(1, 'remove')
    expectAdds(1, 'add')
    expectChanged(1, 'changed')
  })
})
