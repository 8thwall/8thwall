// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, before, assert} from '@nia/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'
import type {World} from './world'

describe('Component Schema', () => {
  let world: World
  before(async () => {
    await ecs.ready()
    world = ecs.createWorld(...initThree())
  })

  it('has the correct behavior for empty component', async () => {
    const EmptyComponent = ecs.registerComponent({
      name: 'EmptyComponent',
    })

    const entity = world.createEntity()
    assert.isFalse(EmptyComponent.has(world, entity))
    EmptyComponent.reset(world, entity)
    assert.isTrue(EmptyComponent.has(world, entity))
    assert.isEmpty(Object.keys(EmptyComponent.get(world, entity)))
  })

  it('can create and read a normal component', async () => {
    const NormalComponent = ecs.registerComponent({
      name: 'NormalComponent',
      schema: {
        v1: ecs.string,
        v2: ecs.f32,
        v3: ecs.boolean,
        v4: ecs.ui8,
        v5: ecs.eid,
      },
    })

    const entity = world.createEntity()
    assert.isFalse(NormalComponent.has(world, entity))
    NormalComponent.reset(world, entity)
    assert.isTrue(NormalComponent.has(world, entity))
    const data = NormalComponent.get(world, entity)
    assert.deepStrictEqual(data, {
      v1: '',
      v2: 0,
      v3: false,
      v4: 0,
      v5: BigInt(0),
    })
  })

  it('can create and read a component with defaults', async () => {
    const ComponentWithDefaults = ecs.registerComponent({
      name: 'ComponentWithDefaults',
      schema: {
        v1: ecs.string,
        v2: ecs.f32,
        v3: ecs.boolean,
        v4: ecs.ui8,
        v5: ecs.eid,
      },
      schemaDefaults: {
        v1: 'default string',
        v2: 42.0,
        v3: true,
        v4: 255,
      },
    })

    const entity = world.createEntity()

    assert.isFalse(ComponentWithDefaults.has(world, entity))
    ComponentWithDefaults.reset(world, entity)
    assert.isTrue(ComponentWithDefaults.has(world, entity))
    const data = ComponentWithDefaults.get(world, entity)
    assert.deepStrictEqual(data, {
      v1: 'default string',
      v2: 42.0,
      v3: true,
      v4: 255,
      v5: BigInt(0),
    })
  })

  it('can create and read a component with inline defaults', async () => {
    const ComponentWithInlineDefaults = ecs.registerComponent({
      name: 'ComponentWithInlineDefaults',
      schema: {
        v1: [ecs.string, 'default string'],
        v2: [ecs.f32, 42.0],
        v3: [ecs.boolean, true],
        v4: [ecs.ui8, 255],
        v5: ecs.eid,
      },
    })

    const entity = world.createEntity()

    assert.isFalse(ComponentWithInlineDefaults.has(world, entity))
    ComponentWithInlineDefaults.reset(world, entity)
    assert.isTrue(ComponentWithInlineDefaults.has(world, entity))
    const data = ComponentWithInlineDefaults.get(world, entity)
    assert.deepStrictEqual(data, {
      v1: 'default string',
      v2: 42.0,
      v3: true,
      v4: 255,
      v5: BigInt(0),
    })
  })

  it('can create and read a component with custom data defaults', async () => {
    let data

    const ComponentWithCustomDataDefaults = ecs.registerComponent({
      name: 'ComponentWithCustomDataDefaults',
      schema: {},
      data: {
        v1: [ecs.string, 'custom default string'],
        v2: [ecs.f32, 100.0],
        v3: [ecs.boolean, true],
        v4: [ecs.ui8, 128],
      },
      tick: (_, component) => {
        data = {...component.data}
      },
    })
    world.tick()

    const entity = world.createEntity()

    assert.isFalse(ComponentWithCustomDataDefaults.has(world, entity))
    ComponentWithCustomDataDefaults.reset(world, entity)
    assert.isTrue(ComponentWithCustomDataDefaults.has(world, entity))
    world.tick()
    assert.deepStrictEqual(data, {
      v1: 'custom default string',
      v2: 100.0,
      v3: true,
      v4: 128,
    })
  })
})
