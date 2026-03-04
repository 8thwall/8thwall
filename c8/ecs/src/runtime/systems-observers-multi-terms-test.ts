// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, before, assert as chaiAssert} from '@nia/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'

describe('ECS Multi-term Observers Stress Test', () => {
  before(async () => {
    await ecs.ready()
  })

  it('trigger and run multi-term observers in different conditions', async () => {
    const window = {ecs}

    let currentMsg = ''
    const log = (...args: string[]) => {
      currentMsg = args.join(' ')
    }
    const callbackCounts = new Map()
    const multiCallbackCounts = new Map()

    const resetCounts = () => {
      callbackCounts.set('add', 0)
      callbackCounts.set('remove', 0)
      callbackCounts.set('changed', 0)
      multiCallbackCounts.set('add', 0)
      multiCallbackCounts.set('remove', 0)
      multiCallbackCounts.set('changed', 0)
    }

    resetCounts()

    const TestA = window.ecs.registerComponent({
      name: 'test-a',
      schema: {
        a: window.ecs.f32,
        b: window.ecs.string,
        c: window.ecs.boolean,
      },
    })

    const TestB = window.ecs.registerComponent({
      name: 'test-b',
      schema: {
        a: window.ecs.f32,
        b: window.ecs.string,
        c: window.ecs.boolean,
      },
    })

    const testQuery = window.ecs.defineQuery([TestA])

    const testQueryMultiple = window.ecs.defineQuery([TestA, TestB])

    const {enter, changed, exit} = window.ecs.lifecycleQueries(testQuery)
    const {
      enter: multiEnter,
      changed: multiChanged,
      exit: multiExit,
    } = window.ecs.lifecycleQueries(testQueryMultiple)

    const testEnter = () => {
      callbackCounts.set('add', (callbackCounts.get('add') || 0) + 1)
    }

    const testExit = () => {
      callbackCounts.set('remove', (callbackCounts.get('remove') || 0) + 1)
    }

    const testChange = () => {
      callbackCounts.set('changed', (callbackCounts.get('changed') || 0) + 1)
    }

    const testEnterMulti = () => {
      multiCallbackCounts.set('add', (multiCallbackCounts.get('add') || 0) + 1)
    }

    const testExitMulti = () => {
      multiCallbackCounts.set('remove', (multiCallbackCounts.get('remove') || 0) + 1)
    }

    const testChangeMulti = () => {
      multiCallbackCounts.set('changed', (multiCallbackCounts.get('changed') || 0) + 1)
    }

    window.ecs.registerBehavior((world) => {
      exit(world).forEach(testExit)
      enter(world).forEach(testEnter)
      changed(world).forEach(testChange)
    })

    window.ecs.registerBehavior((world) => {
      multiExit(world).forEach(testExitMulti)
      multiEnter(world).forEach(testEnterMulti)
      multiChanged(world).forEach(testChangeMulti)
    })

    const expectAdds = (expected: number, label: string, multi?: boolean) => {
      const actual = multi ? multiCallbackCounts.get('add') : callbackCounts.get('add')
      chaiAssert.equal(actual, expected, `'[adds]': ${label} on Test: ${currentMsg}`)
    }

    const expectRemoves = (expected: number, label: string, multi?: boolean) => {
      const actual = multi ? multiCallbackCounts.get('remove') : callbackCounts.get('remove')
      chaiAssert.equal(actual, expected, `'[removes]': ${label} on Test: ${currentMsg}`)
    }

    const expectChanged = (expected: number, label: string, multi?: boolean) => {
      const actual = multi ? multiCallbackCounts.get('changed') : callbackCounts.get('changed')
      chaiAssert.equal(actual, expected, `'[changed]': ${label} on Test: ${currentMsg}`)
    }

    const world = window.ecs.createWorld(...initThree())

    const eid = world.createEntity()

    // initial start
    world.tick()

    log('Doesn\'t have attributes, remove')
    TestA.remove(world, eid)
    world.tick()
    log('--- Single term')
    expectRemoves(0, 'remove')
    expectAdds(0, 'add')
    expectChanged(0, 'changed')
    log('--- Multi term')
    expectRemoves(0, 'remove', true)
    expectAdds(0, 'add', true)
    expectChanged(0, 'changed', true)
    resetCounts()

    log('Doesn\'t have attributes, set')
    TestA.set(world, eid)
    world.tick()
    log('--- Single term')
    expectRemoves(0, 'remove')
    expectAdds(1, 'add')
    expectChanged(0, 'changed')
    log('--- Multi term')
    expectRemoves(0, 'remove', true)
    expectAdds(0, 'add', true)
    expectChanged(0, 'changed', true)
    resetCounts()

    log('Multi Doesn\'t have all attributes, set remaining')
    TestB.set(world, eid)
    world.tick()
    log('--- Single term')
    expectRemoves(0, 'remove')
    expectAdds(0, 'add')
    expectChanged(0, 'changed')
    log('--- Multi term')
    expectRemoves(0, 'remove', true)
    expectAdds(1, 'add', true)
    expectChanged(0, 'changed', true)
    resetCounts()

    log('Multi: Has attributes, set TestB')
    TestB.set(world, eid)
    world.tick()
    log('--- Single term')
    expectRemoves(0, 'remove')
    expectAdds(0, 'add')
    expectChanged(0, 'changed')
    log('--- Multi term')
    expectRemoves(0, 'remove', true)
    expectAdds(0, 'add', true)
    expectChanged(1, 'changed', true)
    resetCounts()

    log('Multi: Has attributes, set TestA')
    TestA.set(world, eid)
    world.tick()
    log('--- Single term')
    expectRemoves(0, 'remove')
    expectAdds(0, 'add')
    expectChanged(1, 'changed')
    log('--- Multi term')
    expectRemoves(0, 'remove', true)
    expectAdds(0, 'add', true)
    expectChanged(1, 'changed', true)
    resetCounts()

    log('Multi: Remove TestB')
    TestB.remove(world, eid)
    world.tick()
    log('--- Single term')
    expectRemoves(0, 'remove')
    expectAdds(0, 'add')
    expectChanged(0, 'changed')
    log('--- Multi term')
    expectRemoves(1, 'remove', true)
    expectAdds(0, 'add', true)
    expectChanged(0, 'changed', true)
    resetCounts()

    log('Multi: Remove TestA 1')
    TestA.remove(world, eid)
    world.tick()
    log('--- Single term')
    expectRemoves(1, 'remove')
    expectAdds(0, 'add')
    expectChanged(0, 'changed')
    log('--- Multi term')
    expectRemoves(0, 'remove', true)
    expectAdds(0, 'add', true)
    expectChanged(0, 'changed', true)
    resetCounts()

    log('Multi has all attributes, Remove TestA')
    TestA.set(world, eid)
    TestB.set(world, eid)
    world.tick()
    resetCounts()

    TestA.remove(world, eid)
    world.tick()
    log('--- Single term')
    expectRemoves(1, 'remove')
    expectAdds(0, 'add')
    expectChanged(0, 'changed')
    log('--- Multi term')
    expectRemoves(1, 'remove', true)
    expectAdds(0, 'add', true)
    expectChanged(0, 'changed', true)
    resetCounts()

    log('Multi doesn\'t have attributes, set both')
    TestB.remove(world, eid)
    world.tick()
    resetCounts()
    TestA.set(world, eid)
    TestB.set(world, eid)
    world.tick()
    log('--- Single term')
    expectRemoves(0, 'remove')
    expectAdds(1, 'add')
    expectChanged(0, 'changed')
    log('--- Multi term')
    expectRemoves(0, 'remove', true)
    expectAdds(1, 'add', true)
    expectChanged(0, 'changed', true)
    resetCounts()

    log('Multi has all attributes, set both')
    resetCounts()
    TestA.set(world, eid)
    TestB.set(world, eid)
    world.tick()
    log('--- Multi term')
    expectRemoves(0, 'remove', true)
    expectAdds(0, 'add', true)
    expectChanged(1, 'changed', true)
    resetCounts()

    log('Multi has all attributes, set single')
    resetCounts()
    TestA.set(world, eid)
    world.tick()
    log('--- Multi term')
    expectRemoves(0, 'remove', true)
    expectAdds(0, 'add', true)
    expectChanged(1, 'changed', true)
    resetCounts()

    log('Multi has all attributes, remove set set single')
    resetCounts()
    TestA.remove(world, eid)
    TestA.set(world, eid)
    TestA.set(world, eid)
    world.tick()
    log('--- Multi term')
    expectRemoves(1, 'remove', true)
    expectAdds(1, 'add', true)
    expectChanged(0, 'changed', true)
    resetCounts()

    log('Multi has all attributes, remove set set remove remove set set single')
    resetCounts()
    TestA.remove(world, eid)
    TestA.set(world, eid)
    TestA.set(world, eid)
    TestA.remove(world, eid)
    TestA.remove(world, eid)
    TestA.set(world, eid)
    TestA.set(world, eid)
    world.tick()
    log('--- Multi term')
    expectRemoves(1, 'remove', true)
    expectAdds(1, 'add', true)
    expectChanged(0, 'changed', true)
    resetCounts()

    log('Multi has all attributes, remove set set remove remove single')
    resetCounts()
    TestA.remove(world, eid)
    TestA.set(world, eid)
    TestA.set(world, eid)
    TestA.remove(world, eid)
    TestA.remove(world, eid)
    world.tick()
    log('--- Multi term')
    expectRemoves(1, 'remove', true)
    expectAdds(0, 'add', true)
    expectChanged(0, 'changed', true)
    resetCounts()

    log('Multi doesn\'t have all attributes, remove remaining')
    resetCounts()
    TestB.remove(world, eid)
    world.tick()
    log('--- Multi term')
    expectRemoves(0, 'remove', true)
    expectAdds(0, 'add', true)
    expectChanged(0, 'changed', true)
    resetCounts()
  })
})
