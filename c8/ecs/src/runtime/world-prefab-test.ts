// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, beforeEach, afterEach, assert} from '@repo/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'
import type {World} from './world'
import type {
  BaseGraphObject, GraphObject, SceneGraph,
} from '../shared/scene-graph'
import type {Eid} from '../shared/schema'

const makePrefab = (id: string, extra?: Partial<GraphObject>): BaseGraphObject => ({
  id,
  name: '<unset>',
  position: [0, 0, 0],
  rotation: [0, 0, 0, 1],
  scale: [1, 1, 1],
  geometry: null,
  material: null,
  prefab: true,
  components: {},
  ...extra,
})

const makeObject = (id: string, extra?: Partial<GraphObject>): GraphObject => ({
  id,
  name: '<unset>',
  position: [0, 0, 0],
  rotation: [0, 0, 0, 1],
  scale: [1, 1, 1],
  geometry: null,
  material: null,
  components: {},
  ...extra,
})

describe('world prefab api test', () => {
  let world: World
  beforeEach(async () => {
    await ecs.ready()
    world = ecs.createWorld(...initThree())
  })
  afterEach(() => {
    world.destroy()
  })

  it('spawning a new instance', () => {
    const prefab = makePrefab('prefab', {
      name: 'prefabName',
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
    })

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.setSceneHook(sceneHandle)

    world.tick()

    // Verify instance exists
    const instanceEid = world.createEntity('prefabName')
    assert.isOk(instanceEid, 'Instance entity exists')

    const instanceGeometry = ecs.BoxGeometry.get(world, instanceEid!)
    assert.deepEqual(instanceGeometry, {width: 1, height: 1, depth: 1})
  })

  it('spawning a new instance by eid', () => {
    const prefab = makePrefab('prefab', {
      name: 'prefabName',
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
    })

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.setSceneHook(sceneHandle)

    world.tick()

    const prefabEid = sceneHandle.graphIdToPrefab.get('prefab')!

    // Verify instance exists
    const instanceEid = world.createEntity(prefabEid)
    assert.isOk(instanceEid, 'Instance entity exists')

    const instanceGeometry = ecs.BoxGeometry.get(world, instanceEid!)
    assert.deepEqual(instanceGeometry, {width: 1, height: 1, depth: 1})
  })

  it('spawning a new instance with a child', () => {
    const prefab = makePrefab('prefab', {
      name: 'prefabName',
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
    })
    const child = makeObject('child', {
      name: 'childName',
      geometry: {type: 'sphere', radius: 1},
      parentId: 'prefab',
    })

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        child,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.setSceneHook(sceneHandle)

    world.tick()

    // Note: a bit of a hack to get prefab child, but users can put it in schema
    const prefabEid = sceneHandle.graphIdToPrefab.get('prefab')
    let childEid: Eid = 0n

    for (const c of world.getChildren(prefabEid!)) {
      childEid = c
    }

    // Verify instance exists
    const instanceEid = world.createEntity('prefabName')
    assert.isOk(instanceEid, 'Instance entity exists')

    const instanceGeometry = ecs.BoxGeometry.get(world, instanceEid!)
    assert.deepEqual(instanceGeometry, {width: 1, height: 1, depth: 1})

    // Verify child exists
    const childInstanceEid = world.getInstanceEntity(instanceEid, childEid)
    assert.isOk(childInstanceEid, 'Child instance entity exists')

    const childGeometry = ecs.SphereGeometry.get(world, childEid)
    assert.deepEqual(childGeometry, {radius: 1})

    assert.notEqual(Number(childInstanceEid), Number(instanceEid),
      'Child instance entity is not the same as root')
  })

  it('updates to prefab propagate to instances', () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 3, height: 3, depth: 3},
      name: 'prefabName',
    })

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.setSceneHook(sceneHandle)

    world.tick()

    // Create two instances
    const instanceEid1 = world.createEntity('prefabName')
    const instanceEid2 = world.createEntity('prefabName')
    world.tick()

    // Update prefab geometry
    const prefabEid = sceneHandle.graphIdToPrefab.get('prefab')!
    ecs.BoxGeometry.set(world, prefabEid, {width: 2, height: 2, depth: 2})
    world.tick()

    // Verify instance geometries
    const instanceGeometry1 = ecs.BoxGeometry.get(world, instanceEid1)
    assert.deepEqual(instanceGeometry1, {width: 2, height: 2, depth: 2})

    const instanceGeometry2 = ecs.BoxGeometry.get(world, instanceEid2)
    assert.deepEqual(instanceGeometry2, {width: 2, height: 2, depth: 2})
  })

  it('add overrides to an instance created at runtime', () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 3, height: 3, depth: 3},
      name: 'prefabName',
    })

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.setSceneHook(sceneHandle)

    world.tick()

    const instanceEid = world.createEntity('prefabName')

    ecs.BoxGeometry.set(world, instanceEid, {width: 3, height: 3, depth: 3})
    world.tick()

    const instanceGeometry = ecs.BoxGeometry.get(world, instanceEid)
    assert.deepEqual(instanceGeometry, {width: 3, height: 3, depth: 3})
  })

  it('add overrides to a child of an instance created at runtime', () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 3, height: 3, depth: 3},
      name: 'prefabName',
    })

    const child = makeObject('child', {
      geometry: {type: 'sphere', radius: 1},
      parentId: 'prefab',
    })

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        child,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.setSceneHook(sceneHandle)

    world.tick()

    const instanceEid = world.createEntity('prefabName')
    assert.isOk(instanceEid, 'Instance entity exists')

    const childEid = sceneHandle.graphIdToPrefab.get('child')!

    // Verify child exists
    const childInstanceEid = world.getInstanceEntity(instanceEid, childEid)
    ecs.SphereGeometry.set(world, childInstanceEid, {radius: 2})

    const childGeometry = ecs.SphereGeometry.get(world, childInstanceEid)
    assert.deepEqual(childGeometry, {radius: 2})
  })

  it('deleting the prefab child deletes the instance child', () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 3, height: 3, depth: 3},
      name: 'prefabName',
    })

    const child = makeObject('child', {
      geometry: {type: 'sphere', radius: 1},
      parentId: 'prefab',
    })

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        child,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.setSceneHook(sceneHandle)

    world.tick()

    const instanceEid = world.createEntity('prefabName')

    const childEid = sceneHandle.graphIdToPrefab.get('child')!

    // Verify child exists
    const childInstanceEid = world.getInstanceEntity(instanceEid, childEid)
    assert.isOk(childInstanceEid, 'Child instance entity exists')

    // Delete the prefab child
    world.deleteEntity(childEid)
    world.tick()

    // Verify the instance child is deleted
    assert.isNotOk(world.getInstanceEntity(instanceEid, childEid), 'Child instance entity deleted')
  })

  it('deleting the prefab deletes the instance and instance children', () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 3, height: 3, depth: 3},
      name: 'prefabName',
    })

    const child = makeObject('child', {
      geometry: {type: 'sphere', radius: 1},
      parentId: 'prefab',
    })

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        child,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.setSceneHook(sceneHandle)

    world.tick()

    const instanceEid = world.createEntity('prefabName')

    const prefabEid = sceneHandle.graphIdToPrefab.get('prefab')!
    const childEid = sceneHandle.graphIdToPrefab.get('child')!

    // Delete the prefab
    world.deleteEntity(prefabEid)
    world.tick()

    // Verify the prefab child no longer has data
    assert.isNotOk(ecs.BoxGeometry.has(world, prefabEid), 'Prefab entity deleted')

    // Verify the instance eid no longer has data
    assert.isNotOk(ecs.BoxGeometry.has(world, instanceEid), 'Instance entity deleted')

    // Verify the instance child is deleted
    assert.isNotOk(world.getInstanceEntity(instanceEid, childEid), 'Child instance entity deleted')
  })

  it('should return itself if given the prefab itself as the second argument', () => {
    const prefab = makePrefab('prefab', {
      name: 'prefabName',
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
    })

    const child = makeObject('child', {
      geometry: {type: 'sphere', radius: 1},
      parentId: 'prefab',
    })

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        child,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.setSceneHook(sceneHandle)

    world.tick()

    const prefabEid = sceneHandle.graphIdToPrefab.get('prefab')!
    const prefabChildEid = sceneHandle.graphIdToPrefab.get('child')!
    const instanceEid = world.createEntity('prefabName')

    // Verify that passing the prefab itself as the second argument returns itself
    const resultEid = world.getInstanceEntity(instanceEid, prefabEid)
    const resultChildEid = world.getInstanceEntity(instanceEid, prefabChildEid)
    assert.isOk(resultEid, 'Instance entity exists')
    assert.equal(Number(resultEid), Number(instanceEid), 'Should return the instance itself')
    assert.notEqual(Number(resultEid), Number(resultChildEid),
      'Should not return the same eid for root & child')
  })

  it('prefab children are correctly parented', () => {
    const prefab = makePrefab('prefab', {components: {}})
    const prefabChild1 = makeObject('prefabChild1', {parentId: 'prefab', components: {}})
    const nestedPrefabChild = makeObject(
      'nestedPrefabChild', {parentId: 'prefabChild1', components: {}}
    )

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        prefabChild1,
        nestedPrefabChild,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.setSceneHook(sceneHandle)

    world.tick()
    const prefabChild1Eid = sceneHandle.graphIdToPrefab.get('prefabChild1')!
    const nestedPrefabChildEid = sceneHandle.graphIdToPrefab.get('nestedPrefabChild')!

    assert.equal(
      world.getParent(prefabChild1Eid), sceneHandle.graphIdToPrefab.get('prefab'),
      'Prefab child 1 is parented to prefab'
    )

    assert.equal(
      world.getParent(nestedPrefabChildEid), sceneHandle.graphIdToPrefab.get('prefabChild1'),
      'Nested prefab child is parented to prefab child 1'
    )
  })
})
