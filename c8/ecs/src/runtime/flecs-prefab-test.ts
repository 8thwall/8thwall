// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, assert, beforeEach, afterEach, before} from '@nia/bzl/js/chai-js'

import './test-env'
import {asm, asmReady} from './asm'
import type {Eid} from '../shared/schema'
import type {World} from './world'
import {createCursor} from './cursor'

describe('Prefab instance test', () => {
  let world: number
  before(async () => {
    await asmReady()
  })
  beforeEach(async () => {
    await asmReady()
    world = asm.createWorld()
  })
  afterEach(() => {
    asm.deleteWorld(world)
  })

  // Reusable function to assign data
  const assignData = (cursor: any, eid: Eid, component: number, data: {}) => {
    const mutIndex = asm.entityStartMutComponent(world, eid, component)
    cursor._ptr = mutIndex
    Object.assign(cursor, data)
    asm.entityEndMutComponent(world, eid, component, true)
  }

  // Reusable function to retrieve data
  const retrieveData = (cursor: any, eid: Eid, component: number, property: string) => {
    const ptr = asm.entityGetComponent(world, eid, component)
    cursor._ptr = ptr
    cursor._index = 0
    cursor._eid = eid
    return cursor[property]
  }

  it('can create a prefab', () => {
    const prefabEid = asm.createPrefab(world)
    const prefabChildEid = asm.createPrefabChild(world, prefabEid)
    const prefabChild2Eid = asm.createPrefabChild(world, prefabEid)
    const prefabChild3Eid = asm.createPrefabChild(world, prefabEid)

    asm.entitySetParent(world, prefabChildEid, prefabEid)
    asm.entitySetParent(world, prefabChild2Eid, prefabEid)
    asm.entitySetParent(world, prefabChild3Eid, prefabChild2Eid)

    assert.isOk(prefabEid)
    assert.isOk(prefabChildEid)
    assert.isOk(prefabChild2Eid)
    assert.isOk(prefabChild3Eid)
  })

  it('can get instance child entities', () => {
    const prefabEid = asm.createPrefab(world)
    const prefabChildEid = asm.createPrefabChild(world, prefabEid)
    const prefabChild2Eid = asm.createPrefabChild(world, prefabEid)
    const prefabChild3Eid = asm.createPrefabChild(world, prefabEid)

    asm.entitySetParent(world, prefabChildEid, prefabEid)
    asm.entitySetParent(world, prefabChild2Eid, prefabEid)
    asm.entitySetParent(world, prefabChild3Eid, prefabChild2Eid)

    const instanceEid = asm.spawnPrefab(world, prefabEid)
    const instanceChildEid = asm.getInstanceChild(world, instanceEid, prefabChildEid)
    const instanceChild2Eid = asm.getInstanceChild(world, instanceEid, prefabChild2Eid)
    const instanceChild3Eid = asm.getInstanceChild(world, instanceEid, prefabChild3Eid)

    assert.isOk(instanceChildEid)
    assert.isOk(instanceChild2Eid)
    assert.isOk(instanceChild3Eid)
    assert.isOk(instanceChild3Eid)
  })

  it('prefab instances inherit component data from prefabs', () => {
    const prefabEid = asm.createPrefab(world)

    const component1 = asm.createComponent(world, 4, 4)
    asm.entityAddComponent(world, prefabEid, component1)

    const cursor = createCursor({} as World, [['property', 'f32', 0]])

    assignData(cursor, prefabEid, component1, {property: 50})

    const instanceEid = asm.spawnPrefab(world, prefabEid)

    assignData(cursor, prefabEid, component1, {property: 75})

    const prefabData = retrieveData(cursor, prefabEid, component1, 'property')
    const instanceData = retrieveData(cursor, instanceEid, component1, 'property')

    assert.strictEqual(prefabData, 75)
    assert.strictEqual(instanceData, 75)
  })

  it('prefab instances children inherit component data from prefab children', () => {
    const prefabEid = asm.createPrefab(world)

    const component1 = asm.createComponent(world, 4, 4)
    const cursor = createCursor({} as World, [['property', 'f32', 0]])

    const prefabChildEid = asm.createPrefabChild(world, prefabEid)
    asm.entitySetParent(world, prefabChildEid, prefabEid)

    asm.entityAddComponent(world, prefabChildEid, component1)
    assignData(cursor, prefabChildEid, component1, {property: 75})

    const instanceEid = asm.spawnPrefab(world, prefabEid)

    assignData(cursor, prefabChildEid, component1, {property: 100})
    const instanceChildEid = asm.getInstanceChild(world, instanceEid, prefabChildEid)
    const instanceChildData = retrieveData(cursor, instanceChildEid, component1, 'property')
    const prefabChildData = retrieveData(cursor, prefabChildEid, component1, 'property')

    assert.strictEqual(prefabChildData, 100)
    assert.strictEqual(instanceChildData, 100)
  })

  it('prefab instances can override prefab component data', () => {
    const prefabEid = asm.createPrefab(world)

    const component1 = asm.createComponent(world, 4, 4)
    asm.entityAddComponent(world, prefabEid, component1)

    const cursor = createCursor({} as World, [['property', 'f32', 0]])

    assignData(cursor, prefabEid, component1, {property: 50})

    const instanceEid = asm.spawnPrefab(world, prefabEid)
    assignData(cursor, instanceEid, component1, {property: 75})

    const instance2Eid = asm.spawnPrefab(world, prefabEid)
    assignData(cursor, instance2Eid, component1, {property: 100})

    const prefabData = retrieveData(cursor, prefabEid, component1, 'property')
    const instanceData = retrieveData(cursor, instanceEid, component1, 'property')
    const instance2Data = retrieveData(cursor, instance2Eid, component1, 'property')

    assert.strictEqual(prefabData, 50)
    assert.strictEqual(instanceData, 75)
    assert.strictEqual(instance2Data, 100)
  })

  it('prefab instances can have components not on the prefab', () => {
    const prefabEid = asm.createPrefab(world)

    const component1 = asm.createComponent(world, 4, 4)
    asm.entityAddComponent(world, prefabEid, component1)

    const cursor = createCursor({} as World, [['property', 'f32', 0]])
    assignData(cursor, prefabEid, component1, {property: 50})

    const instanceEid = asm.spawnPrefab(world, prefabEid)

    const component2 = asm.createComponent(world, 4, 4)
    asm.entityAddComponent(world, instanceEid, component2)

    assignData(cursor, instanceEid, component2, {property: 75})

    const instanceData = retrieveData(cursor, instanceEid, component2, 'property')

    assert.strictEqual(instanceData, 75)
  })

  it('removing a component from the prefab removes it from the instance', () => {
    const prefabEid = asm.createPrefab(world)

    const component1 = asm.createComponent(world, 4, 4)
    asm.entityAddComponent(world, prefabEid, component1)

    const cursor = createCursor({} as World, [['property', 'f32', 0]])
    assignData(cursor, prefabEid, component1, {property: 50})

    const instanceEid = asm.spawnPrefab(world, prefabEid)

    asm.entityRemoveComponent(world, prefabEid, component1)

    const prefabHasComponent = asm.entityHasComponent(world, prefabEid, component1)
    const instanceHasComponent = asm.entityHasComponent(world, instanceEid, component1)

    assert.isNotOk(prefabHasComponent)
    assert.isNotOk(instanceHasComponent)
  })

  it('instance has component inherited from prefab', () => {
    const prefabEid = asm.createPrefab(world)

    const component1 = asm.createComponent(world, 4, 4)
    asm.entityAddComponent(world, prefabEid, component1)

    const instanceEid = asm.spawnPrefab(world, prefabEid)

    const instanceHasComponent = asm.entityHasComponent(world, instanceEid, component1)

    assert.isOk(instanceHasComponent)
  })
})
