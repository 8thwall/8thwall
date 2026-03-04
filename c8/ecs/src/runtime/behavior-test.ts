// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, before, assert} from '@nia/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'

describe('behavior', () => {
  before(async () => {
    await ecs.ready()
  })

  it('Can add and remove behaviors', async () => {
    const window = {ecs}

    const runTest = () => {
      const world = window.ecs.createWorld(...initThree())

      // initial start
      world.tick()

      const seenBehaviors: string[] = []

      const expectBehaviors = (expected: any[], label: string) => {
        // @ts-ignore
        assert.deepEqual(expected, seenBehaviors, `${label} on Test:`)
        seenBehaviors.length = 0
      }

      const behaviorA = () => {
        seenBehaviors.push('A')
      }

      const behaviorB = () => {
        seenBehaviors.push('B')
      }

      const behaviorC = () => {
        seenBehaviors.push('C')
      }

      ecs.registerBehavior(behaviorA)
      world.tick()
      expectBehaviors(['A'], 'Just A')

      ecs.registerBehavior(behaviorB)
      world.tick()
      expectBehaviors(['A', 'B'], 'A and B')

      ecs.registerBehavior(behaviorC)
      world.tick()
      expectBehaviors(['A', 'B', 'C'], 'A, B, and C')

      const nonBehavior = () => { }
      ecs.unregisterBehavior(nonBehavior)
      world.tick()
      expectBehaviors(['A', 'B', 'C'], 'No-op')

      ecs.unregisterBehavior(behaviorA)
      world.tick()
      expectBehaviors(['B', 'C'], 'No more A')

      ecs.registerBehavior(behaviorA)
      world.tick()
      expectBehaviors(['B', 'C', 'A'], 'A after')
    }

    runTest()
  })
})
