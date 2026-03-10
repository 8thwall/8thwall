// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, before, assert as chaiAssert} from '@repo/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'
import type {Eid} from '../shared/schema'

describe('lifeCycle Tests', () => {
  before(async () => {
    await ecs.ready()
  })
  it('can ', async () => {
    const window = {ecs}

    let currentMsg = ''
    const log = (...args: string[]) => {
      currentMsg = args.join(' ')
    }

    let attributeCounter = 0
    const callbackCounts = new Map()

    const resetCounts = () => {
      callbackCounts.set('add', 0)
      callbackCounts.set('remove', 0)
      callbackCounts.set('changed', 0)
    }

    resetCounts()

    const createTestAttribute = () => window.ecs.registerComponent({
      name: `attr-${attributeCounter++}`,
      schema: {
        value: window.ecs.f32,
      },
    })

    const testEnter = () => {
      callbackCounts.set('add', (callbackCounts.get('add') || 0) + 1)
    }

    const testExit = () => {
      callbackCounts.set('remove', (callbackCounts.get('remove') || 0) + 1)
    }

    const testChange = () => {
      callbackCounts.set('changed', (callbackCounts.get('changed') || 0) + 1)
    }

    const expectAdds = (expected: number, label: string) => {
      const actual = callbackCounts.get('add') || 0
      chaiAssert.equal(actual, expected, `'[adds]': ${label} on Test: ${currentMsg}`)
    }

    const expectRemoves = (expected: number, label: string) => {
      const actual = callbackCounts.get('remove') || 0
      chaiAssert.equal(actual, expected, `'[removes]': ${label} on Test: ${currentMsg}`)
    }

    const expectChanged = (expected: number, label: string) => {
      const actual = callbackCounts.get('changed') || 0
      chaiAssert.equal(actual, expected, `'[changed]': ${label} on Test: ${currentMsg}`)
    }

    const assert = (statement: boolean, label: string) => {
      chaiAssert(statement, label + currentMsg)
    }

    {
      log('Flush on exit')
      let enterIds: Eid[] = []
      let exitIds: Eid[] = []
      let changedIds: Eid[] = []

      const TestAttribute = createTestAttribute()
      const testQuery = window.ecs.defineQuery([TestAttribute])
      const {enter, changed, exit} = window.ecs.lifecycleQueries(testQuery)
      const world = window.ecs.createWorld(...initThree())
      const eid1 = world.createEntity()
      const eid2 = world.createEntity()

      // initial start
      exit(world)
      enter(world)
      changed(world)

      log(`Set eid1 ${eid1} and eid2 ${eid2}`)
      TestAttribute.set(world, eid1)
      TestAttribute.set(world, eid2)

      log('enter, changed')
      assert(enter(world).length === 0, 'enter(world).length === 0')
      assert(changed(world).length === 0, 'changed(world).length === 0')

      log('exit, enter, changed')
      assert(exit(world).length === 0, 'exit(world).length === 0')
      enterIds = enter(world)
      assert(enterIds.length === 2, 'enter(world).length === 2')
      assert(enterIds.includes(eid1), `enter(world) includes eid: ${eid1}`)
      assert(enterIds.includes(eid2), `enter(world) includes eid: ${eid2}`)
      assert(changed(world).length === 0, 'changed(world).length === 0')

      log('enter, changed')
      enterIds = enter(world)
      assert(enterIds.length === 2, 'enter(world).length === 2')
      assert(enterIds.includes(eid1), `enter(world) includes eid: ${eid1}`)
      assert(enterIds.includes(eid2), `enter(world) includes eid: ${eid2}`)
      assert(changed(world).length === 0, 'changed(world).length === 0')

      log(`Remove from ${eid2}`)
      TestAttribute.remove(world, eid2)
      log('exit, enter, changed')
      exitIds = exit(world)
      assert(exitIds.length === 1, 'exit(world).length === 1')
      assert(exitIds.includes(eid2), `exit(world) includes eid: ${eid2}`)
      assert(enter(world).length === 0, 'enter(world).length === 0')
      assert(changed(world).length === 0, 'changed(world).length === 0')

      log(`Set ${eid1}`)
      TestAttribute.set(world, eid1)
      log('changed')
      assert(changed(world).length === 0, 'changed(world).length === 0')
      log('exit, changed')
      assert(exit(world).length === 0, 'exit(world).length === 0')
      changedIds = changed(world)
      assert(changedIds.length === 1, 'changed(world).length === 1')
      assert(changedIds.includes(eid1), `changed(world) includes eid: ${eid1}`)

      world.destroy()
    }

    {
      log('Flush on enter')
      let enterIds: Eid[] = []
      let exitIds: Eid[] = []
      let changedIds: Eid[] = []

      const TestAttribute = createTestAttribute()
      const testQuery = window.ecs.defineQuery([TestAttribute])
      const {enter, changed, exit} = window.ecs.lifecycleQueries(testQuery)
      const world = window.ecs.createWorld(...initThree())
      const eid1 = world.createEntity()
      const eid2 = world.createEntity()

      // initial start
      enter(world)
      exit(world)
      changed(world)

      log(`Set eid1 ${eid1} and eid2 ${eid2}`)
      TestAttribute.set(world, eid1)
      TestAttribute.set(world, eid2)

      log('enter, exit, changed')
      enterIds = enter(world)
      assert(enterIds.length === 2, 'enter(world).length === 2')
      assert(enterIds.includes(eid1), `enter(world) includes eid: ${eid1}`)
      assert(enterIds.includes(eid2), `enter(world) includes eid: ${eid2}`)
      assert(exit(world).length === 0, 'exit(world).length === 0')
      assert(changed(world).length === 0, 'changed(world).length === 0')

      log(`Remove from ${eid1}`)
      TestAttribute.remove(world, eid1)
      log('exit, changed')
      assert(exit(world).length === 0, 'exit(world).length === 0')
      assert(changed(world).length === 0, 'changed(world).length === 0')
      log('enter, exit, changed')
      assert(enter(world).length === 0, 'enter(world).length === 0')
      exitIds = exit(world)
      assert(exitIds.length === 1, 'exit(world).length === 1')
      assert(exitIds.includes(eid1), `exit(world) includes eid: ${eid1}`)
      assert(changed(world).length === 0, 'changed(world).length === 0')

      log(`Set ${eid2}`)
      TestAttribute.set(world, eid2)
      log('exit, changed')
      exitIds = exit(world)
      assert(exitIds.length === 1, 'exit(world).length === 1')
      assert(exitIds.includes(eid1), `exit(world) includes eid: ${eid1}`)
      assert(changed(world).length === 0, 'changed.length === 0')
      log('enter, exit, changed')
      assert(enter(world).length === 0, 'enter(world).length === 0')
      changedIds = changed(world)
      assert(exit(world).length === 0, 'exit(world).length === 0')
      assert(changedIds.length === 1, 'changed(world).length === 1')
      assert(changedIds.includes(eid2), `changed(world) includes eid: ${eid2}`)

      world.destroy()
    }

    {
      log('Calls exit and enter on tick, all lifecycle event entities flush each cycle')
      const TestAttribute = createTestAttribute()
      const testQuery = window.ecs.defineQuery([TestAttribute])

      const {enter, changed, exit} = window.ecs.lifecycleQueries(testQuery)
      // @ts-ignore
      const behavior = (world) => {
        exit(world).forEach(testExit)
        enter(world).forEach(testEnter)
      }

      window.ecs.registerBehavior(behavior)

      const world = window.ecs.createWorld(...initThree())
      const eid1 = world.createEntity()
      const eid2 = world.createEntity()
      world.tick()

      log(`Set eid1 ${eid1} and eid2 ${eid2}, tick`)
      TestAttribute.set(world, eid1)
      TestAttribute.set(world, eid2)
      world.tick()
      expectRemoves(0, 'remove')
      expectAdds(2, 'add')
      expectChanged(0, 'changed')
      assert(changed(world).length === 0, 'changed(world).length === 0')
      resetCounts()

      log(`Set eid1 ${eid1}, tick, skip changed, set eid2 ${eid2}, tick`)
      TestAttribute.set(world, eid1)
      world.tick()
      TestAttribute.set(world, eid2)
      world.tick()
      expectRemoves(0, 'remove')
      expectAdds(0, 'add')
      expectChanged(0, 'changed')
      const changedIds = changed(world)
      assert(changedIds.length === 1, 'changed(world).length === 1')
      assert(changedIds.includes(eid2), `changed(world) includes eid: ${eid2}`)
      resetCounts()

      log(`Set eid1 ${eid1} and eid2 ${eid2}, tick, skip changed, tick`)
      TestAttribute.set(world, eid1)
      TestAttribute.set(world, eid2)
      world.tick()
      world.tick()
      expectRemoves(0, 'remove')
      expectAdds(0, 'add')
      expectChanged(0, 'changed')
      assert(changed(world).length === 0, 'changed(world).length === 0')
      resetCounts()

      world.destroy()
      window.ecs.unregisterBehavior(behavior)
    }

    {
      log('Changed called on tick, all lifecycle event entities flush each cycle')
      const TestAttribute = createTestAttribute()
      const testQuery = window.ecs.defineQuery([TestAttribute])

      const {enter, changed, exit} = window.ecs.lifecycleQueries(testQuery)
      // @ts-ignore
      const behavior = (w) => {
        changed(w).forEach(testChange)
      }

      window.ecs.registerBehavior(behavior)

      const world = window.ecs.createWorld(...initThree())
      const eid1 = world.createEntity()
      const eid2 = world.createEntity()
      world.tick()
      resetCounts()

      log(`Set eid1 ${eid1} and eid2 ${eid2}, tick`)
      TestAttribute.set(world, eid1)
      TestAttribute.set(world, eid2)
      world.tick()
      expectRemoves(0, 'remove')
      expectAdds(0, 'add')
      expectChanged(0, 'changed')
      assert(exit(world).length === 0, 'exit(world).length === 0')
      assert(enter(world).length === 2, 'enter(world).length === 2')
      resetCounts()

      log(`Set eid1 ${eid1}, tick, set eid2 ${eid2}, tick`)
      TestAttribute.set(world, eid1)
      world.tick()
      TestAttribute.set(world, eid2)
      world.tick()
      expectRemoves(0, 'remove')
      expectAdds(0, 'add')
      expectChanged(2, 'changed')
      assert(exit(world).length === 0, 'exit(world).length === 0')
      assert(enter(world).length === 0, 'enter(world).length === 0')
      resetCounts()

      log(`Remove eid1 ${eid1}, tick, skip enter and exit, remove eid2 ${eid2}, tick`)
      TestAttribute.remove(world, eid1)
      world.tick()
      TestAttribute.remove(world, eid2)
      world.tick()
      expectRemoves(0, 'remove')
      expectAdds(0, 'add')
      expectChanged(0, 'changed')
      const exitIds = exit(world)
      assert(exitIds.length === 1, 'exit(world).length === 1')
      assert(exitIds.includes(eid2), `exit(world) includes eid: ${eid2}`)
      resetCounts()

      log(`Set eid1 ${eid1}, tick, skip enter and exit, set eid2 ${eid2}, tick`)
      TestAttribute.set(world, eid1)
      world.tick()
      TestAttribute.set(world, eid2)
      world.tick()
      expectRemoves(0, 'remove')
      expectAdds(0, 'add')
      expectChanged(0, 'changed')
      const enterIds = enter(world)
      assert(enterIds.length === 1, 'enter(world).length === 1')
      assert(enterIds.includes(eid2), `enter(world) includes eid: ${eid2}`)
      resetCounts()

      world.destroy()
      window.ecs.unregisterBehavior(behavior)
    }
  })
})
