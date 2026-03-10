// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, before, assert as chaiAssert} from '@repo/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'

describe('ECS Component Stress Test', () => {
  before(async () => {
    await ecs.ready()
  })

  it('trigger and run observers in different conditions', async () => {
    const window = {ecs}

    let currentMsg = ''
    const log = (...args: string[]) => {
      currentMsg = args.join(' ')
    }

    const callbackCounts = new Map()

    const resetCounts = () => {
      callbackCounts.set('add', 0)
      callbackCounts.set('remove', 0)
    }

    const expectAdds = (expected: number, label: string) => {
      const actual = callbackCounts.get('add') || 0
      chaiAssert.equal(actual, expected, `'[adds]': ${label} on Test: ${currentMsg}`)
    }

    const expectRemoves = (expected: number, label: string) => {
      const actual = callbackCounts.get('remove') || 0
      chaiAssert.equal(actual, expected, `'[removes]': ${label} on Test: ${currentMsg}`)
    }

    resetCounts()
    const TestComponent = window.ecs.registerComponent({
      name: 'test',
      schema: {period: ecs.f32},
      add: () => {
        callbackCounts.set('add', (callbackCounts.get('add') || 0) + 1)
      },
      remove: () => {
        callbackCounts.set('remove', (callbackCounts.get('remove') || 0) + 1)
      },
    })

    const world = window.ecs.createWorld(...initThree())

    const eid = world.createEntity()

    // initial start
    world.tick()

    log('Doesn\'t have component, remove')
    TestComponent.remove(world, eid)
    world.tick()
    expectRemoves(0, 'remove')
    expectAdds(0, 'add')
    resetCounts()

    log('Doesn\'t have component, set')
    TestComponent.set(world, eid)
    world.tick()
    expectRemoves(0, 'remove')
    expectAdds(1, 'add')
    resetCounts()

    log('Has component, set')
    TestComponent.set(world, eid)
    world.tick()
    expectRemoves(0, 'remove')
    expectAdds(0, 'add')
    resetCounts()

    log('Has component, remove')
    TestComponent.remove(world, eid)
    world.tick()
    expectRemoves(1, 'remove')
    expectAdds(0, 'add')
    resetCounts()

    log('set * 3')
    TestComponent.set(world, eid)
    TestComponent.set(world, eid)
    TestComponent.set(world, eid)
    world.tick()
    expectRemoves(0, 'remove')
    expectAdds(1, 'add')
    resetCounts()

    log('remove * 3')
    TestComponent.remove(world, eid)
    TestComponent.remove(world, eid)
    TestComponent.remove(world, eid)
    world.tick()
    expectRemoves(1, 'remove')
    expectAdds(0, 'add')
    resetCounts()

    log('Doesn\'t have component, set, remove')
    TestComponent.set(world, eid)
    TestComponent.remove(world, eid)
    world.tick()
    expectRemoves(0, 'remove')
    expectAdds(0, 'add')
    resetCounts()

    log('Has component, set, remove')
    TestComponent.set(world, eid)
    world.tick()
    resetCounts()
    TestComponent.set(world, eid)
    TestComponent.remove(world, eid)
    world.tick()
    expectRemoves(1, 'remove')
    expectAdds(0, 'add')
    resetCounts()

    log('Doesn\'t have component, remove, set')
    TestComponent.remove(world, eid)
    TestComponent.set(world, eid)
    world.tick()
    expectRemoves(0, 'remove')
    expectAdds(1, 'add')
    resetCounts()

    log('Has component, remove, set')
    TestComponent.remove(world, eid)
    TestComponent.set(world, eid)
    world.tick()
    expectRemoves(1, 'remove')
    expectAdds(1, 'add')
    resetCounts()

    log('Doesn\'t have component, set, remove, set, remove, set')
    TestComponent.remove(world, eid)
    world.tick()
    resetCounts()

    TestComponent.set(world, eid)
    TestComponent.remove(world, eid)
    TestComponent.set(world, eid)
    TestComponent.remove(world, eid)
    TestComponent.set(world, eid)
    world.tick()
    expectRemoves(0, 'remove')
    expectAdds(1, 'add')
    resetCounts()

    log('Has component, remove, set, remove, set, remove')
    TestComponent.remove(world, eid)
    TestComponent.set(world, eid)
    TestComponent.remove(world, eid)
    TestComponent.set(world, eid)
    TestComponent.remove(world, eid)
    world.tick()
    expectRemoves(1, 'remove')
    expectAdds(0, 'add')
  })
})
