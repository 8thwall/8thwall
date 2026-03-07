import {describe, it, before, assert, beforeEach} from '@nia/bzl/js/chai-js'
// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'

describe('Event behavior', () => {
  let world: ecs.World
  before(async () => {
    await ecs.ready()
  })

  beforeEach(() => {
    if (world) {
      world.destroy()
    }
    world = ecs.createWorld(...initThree())
  })

  it('Can listen to all dispatched events on globalId', () => {
    const eid = world.createEntity()

    let globalCounter = 0
    let childCounter = 0
    world.events.addListener(world.events.globalId, 'test-event', () => {
      globalCounter++
    })

    world.events.addListener(eid, 'test-event', () => {
      childCounter++
    })

    world.events.dispatch(world.events.globalId, 'test-event')
    world.events.dispatch(eid, 'test-event')

    world.tick()
    assert.equal(globalCounter, 2)
    assert.equal(childCounter, 1)
  })

  it('Can listen to different events', () => {
    const eid = world.createEntity()

    let appleCounter = 0
    let bananaCounter = 0

    world.events.addListener(eid, 'apple', () => {
      appleCounter++
    })

    world.events.addListener(eid, 'banana', () => {
      bananaCounter++
    })

    world.events.dispatch(eid, 'apple')
    world.events.dispatch(eid, 'banana')
    world.events.dispatch(eid, 'banana')
    world.events.dispatch(eid, 'banana')

    world.tick()
    assert.equal(appleCounter, 1)
    assert.equal(bananaCounter, 3)
  })

  it('Can listen to multiple events in a row', () => {
    const eid = world.createEntity()

    let appleCounter = 0

    world.events.addListener(eid, 'apple', () => {
      appleCounter++
    })

    world.events.dispatch(eid, 'apple')

    world.tick()
    assert.equal(appleCounter, 1)

    world.events.dispatch(eid, 'banana')
    world.events.dispatch(eid, 'apple')
    world.tick()
    assert.equal(appleCounter, 2)
  })

  it('Can have multiple listeners for the same event', () => {
    const eid = world.createEntity()

    let bananaCounter = 0

    world.events.addListener(eid, 'banana', () => {
      bananaCounter++
    })

    world.events.addListener(eid, 'banana', () => {
      bananaCounter++
    })

    world.events.dispatch(eid, 'banana')
    world.events.dispatch(eid, 'banana')
    world.events.dispatch(eid, 'banana')

    world.tick()

    assert.equal(bananaCounter, 6)
  })

  it('Can clean up listeners', () => {
    const eid = world.createEntity()

    let bananaCounter = 0

    const bananaHandler = () => {
      bananaCounter++
    }

    world.events.addListener(eid, 'banana', bananaHandler)

    world.events.dispatch(eid, 'banana')
    world.events.dispatch(eid, 'banana')
    world.events.dispatch(eid, 'banana')

    world.tick()

    world.events.removeListener(eid, 'banana', bananaHandler)

    world.events.dispatch(eid, 'banana')

    world.tick()

    assert.equal(bananaCounter, 3)
  })

  it('Root eid from get parent works as global', () => {
    const eid = world.createEntity()
    const rootId = world.getParent(eid)

    let bananaCounter = 0

    const bananaHandler = () => {
      bananaCounter++
    }

    world.events.addListener(rootId, 'banana', bananaHandler)

    world.events.dispatch(eid, 'banana')
    world.events.dispatch(eid, 'banana')
    world.events.dispatch(eid, 'banana')

    world.tick()

    assert.equal(bananaCounter, 3)
  })

  it('dispatches physics events on the same frame as they occur', () => {
    const entity1 = world.getEntity(world.createEntity())
    const entity2 = world.getEntity(world.createEntity())

    let collisionEnterCount = 0
    let collisionExitCount = 0

    world.events.addListener(entity1.eid, ecs.physics.COLLISION_START_EVENT, () => {
      collisionEnterCount++
    })
    world.events.addListener(entity1.eid, ecs.physics.COLLISION_END_EVENT, () => {
      collisionExitCount++
    })

    let time = 10

    world.tick(time += 10)
    assert.equal(collisionEnterCount, 0, 'no collisions yet')

    entity1.setLocalPosition({x: 0, y: 0, z: 0})
    entity2.setLocalPosition({x: 0, y: 0, z: 1})

    entity1.set(ecs.Collider, {
      shape: ecs.ColliderShape.Sphere,
      radius: 1,
      mass: 1,
      type: ecs.ColliderType.Dynamic,
      eventOnly: true,
    })

    entity2.set(ecs.Collider, {
      shape: ecs.ColliderShape.Box,
      radius: 1,
      mass: 1,
      type: ecs.ColliderType.Dynamic,
      eventOnly: true,
    })

    world.tick(time += 10)
    assert.equal(collisionEnterCount, 0, 'too far initially')

    entity2.setLocalPosition({x: 0, y: 0, z: 0.5})
    world.tick(time += 10)
    assert.equal(collisionEnterCount, 1, 'collision dispatched immediately on move')

    entity2.setLocalPosition({x: 0, y: 0, z: 3})
    world.tick(time += 10)
    assert.equal(collisionExitCount, 1, 'exit dispatched immediately on move away')

    entity2.setLocalPosition({x: 0, y: 0, z: 0.5})
    world.tick(time += 10)
    assert.equal(collisionEnterCount, 2, 'collision dispatched immediately on move back')

    const entity3 = world.getEntity(world.createEntity())
    entity3.set(ecs.Collider, {
      shape: ecs.ColliderShape.Box,
      radius: 1,
      mass: 1,
      type: ecs.ColliderType.Dynamic,
      eventOnly: true,
    })

    world.tick(time += 10)
    assert.equal(collisionEnterCount, 3, 'collision dispatched immediately on new entity')
  })

  it('dispatches physics update event only when enabled', () => {
    let updateCount = 0
    world.events.addListener(world.events.globalId, ecs.physics.UPDATE_EVENT, () => {
      updateCount++
    })

    ecs.physics.disable(world)

    let time = 10

    world.tick(time += 10)
    assert.equal(updateCount, 0)
    world.tick(time += 10)
    assert.equal(updateCount, 0)

    ecs.physics.enable(world)

    world.tick(time += 10)
    assert.equal(updateCount, 1, 'first update event after enabling physics')
    world.tick(time += 10)
    assert.equal(updateCount, 2, 'second update event after enabling physics')
  })
})
