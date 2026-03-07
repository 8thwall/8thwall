// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, before, assert as chaiAssert} from '@nia/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'

describe('cursor test', () => {
  before(async () => {
    await ecs.ready()
  })

  it('should reflect the state of the data accurately and fail when it has to', async () => {
    const window = {ecs}

    let currentMsg = ''
    const log = (...args: string[]) => {
      currentMsg = args.join(' ')
    }

    let componentCounter = 0

    const createSchema = (count: number) => {
      const schema = {}
      for (let i = 0; i < count; i++) {
        // @ts-ignore
        schema[`field${i}`] = window.ecs.f32
      }
      return schema
    }

    // @ts-ignore
    const createTestComponent = schema => window.ecs.registerComponent({
      name: `test-${componentCounter++}`,
      schema,
    })

    const assert = (statement: boolean, label: string) => {
      chaiAssert(statement, `${label}, ${currentMsg}`)
    }

    {
      const TestA = createTestComponent(createSchema(5))

      const world = window.ecs.createWorld(...initThree())
      const eid = world.createEntity()

      let errorMsg = ''

      try {
        TestA.cursor(world, eid)
      } catch (e) {
        const error = e as Error
        errorMsg = error.toString()
      }

      assert(!!errorMsg, 'Error thrown when trying to get cursor for entity without component')
      errorMsg = ''

      try {
        TestA.acquire(world, eid)
      } catch (e) {
        const error = e as Error
        errorMsg = error.toString()
      }
      assert(!!errorMsg,
        'Error thrown when trying to acquire cursor for entity without component')
      errorMsg = ''

      try {
        TestA.mutate(world, eid, () => false)
      } catch (e) {
        const error = e as Error
        errorMsg = error.toString()
      }
      assert(!!errorMsg, 'Error thrown when trying to mutate cursor for entity without component')
    }

    {
      const TestB = createTestComponent(createSchema(5))

      const world = window.ecs.createWorld(...initThree())
      const eid = world.createEntity()

      const componentValues = {field0: 1, field1: 45, field2: 87, field3: 16, field4: 5}

      // @ts-ignore
      const fieldsMatch = cursor => !Object.keys(componentValues).some((key) => {
        // @ts-ignore
        const value = componentValues[key]
        return value !== cursor[key]
      })

      TestB.set(world, eid, componentValues)

      let cursorB

      try {
        cursorB = TestB.cursor(world, eid)
      } catch (e) {
        const error = e as Error
        log('Error:', error.toString())
      }

      assert(fieldsMatch(cursorB), 'Cursor values match component values')

      try {
        cursorB = TestB.acquire(world, eid)
      } catch (e) {
        const error = e as Error
        log('Error:', error.toString())
      }

      TestB.commit(world, eid)

      assert(fieldsMatch(cursorB), 'Acquired cursor values match component values')

      let match = false
      try {
        TestB.mutate(world, eid, (cursor) => {
          match = fieldsMatch(cursor)
          return true
        })
      } catch (e) {
        const error = e as Error
        log('Error:', error.toString())
      }

      assert(match, 'Mutate cursor initial values match component values')
    }
  })
})
