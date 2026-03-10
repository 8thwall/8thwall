// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, before, beforeEach, afterEach, assert} from '@repo/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'
import type {World} from './world'
import type {StateMachineDefiner} from './state-machine'

describe('State Machine', () => {
  let world: World

  before(async () => {
    await ecs.ready()
  })

  beforeEach(() => {
    world = ecs.createWorld(...initThree())
  })

  afterEach(() => {
    world.destroy()
  })

  it('Can create a state machine with expected definer arguments', async () => {
    const entity = world.getEntity(world.createEntity())

    let args: Parameters<StateMachineDefiner>[number] | null = null
    ecs.createStateMachine(world, entity.eid, (_args) => {
      args = _args

      ecs.defineState('initial').initial()
    })

    assert.deepStrictEqual(args, {
      world,
      entity,
      eid: entity.eid,
    })
  })
})
