// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import {describe, it, beforeEach, afterEach, assert} from '@nia/bzl/js/chai-js'

import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'
import type {World} from './world'
import type {
  BaseGraphObject, GraphObject, PrefabInstanceChildren, PrefabInstanceDeletions, SceneGraph,
} from '../shared/scene-graph'
import type {Eid} from '../shared/schema'
import {assertPosition, assertQuaternion} from './transform-test-helpers'

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

const makeInstance = (
  id: string, instanceOf: string, deletions?: PrefabInstanceDeletions,
  overrides?: Partial<GraphObject>,
  children?: PrefabInstanceChildren
): GraphObject => ({
  id,
  name: '<unset>',
  position: [0, 0, 0],
  rotation: [0, 0, 0, 1],
  scale: [1, 1, 1],
  components: {},
  instanceData: {instanceOf, deletions: deletions || {}, children},
  ...overrides,
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

describe('Prefab instance test', () => {
  let world: World
  beforeEach(async () => {
    await ecs.ready()
    world = ecs.createWorld(...initThree())
  })
  afterEach(() => {
    world.destroy()
  })

  it('spawning in a scene with a prefab and an instance', () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
    })
    const instance = makeInstance('instance', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    // Verify prefab exists
    const prefabEid = sceneHandle.graphIdToPrefab.get('prefab')
    assert.isOk(prefabEid, 'Prefab entity exists')

    // Verify instance exists
    const instanceEid = sceneHandle.graphIdToEid.get('instance')
    assert.isOk(instanceEid, 'Instance entity exists')

    const instanceGeometry = ecs.BoxGeometry.get(world, instanceEid!)
    assert.deepEqual(instanceGeometry, {width: 1, height: 1, depth: 1})
  })

  it('updates to prefab propagate to instances', () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
    })
    const instance = makeInstance('instance', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const prefabEid = sceneHandle.graphIdToPrefab.get('prefab')!
    const instanceEid = sceneHandle.graphIdToEid.get('instance')!

    // Update prefab geometry
    ecs.BoxGeometry.set(world, prefabEid, {width: 2, height: 2, depth: 2})
    world.tick()

    const instanceGeometry = ecs.BoxGeometry.get(world, instanceEid)
    assert.deepEqual(instanceGeometry, {width: 2, height: 2, depth: 2})
  })

  it('updates to prefab children propagate to instance children', () => {
    const prefab = makePrefab('prefab', {components: {}})
    const prefabChild1 = makeObject('prefabChild1',
      {parentId: 'prefab', components: {}, geometry: {type: 'box', width: 1, height: 1, depth: 1}})
    const prefabChild2 = makeObject('prefabChild2', {})
    const instance = makeInstance('instance', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        prefabChild1,
        prefabChild2,
        instance,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const prefabChild1Eid = sceneHandle.graphIdToPrefab.get('prefabChild1')!
    const instanceEid = sceneHandle.graphIdToEid.get('instance')!
    const instanceChild1Eid = world.getInstanceEntity(instanceEid, prefabChild1Eid)

    // Update prefab child geometry
    ecs.BoxGeometry.set(world, prefabChild1Eid, {width: 2, height: 2, depth: 2})
    world.tick()

    const instanceGeometry = ecs.BoxGeometry.get(world, instanceChild1Eid)
    assert.deepEqual(instanceGeometry, {width: 2, height: 2, depth: 2})
  })

  it('debug updates to prefab children propagate to instance children', () => {
    const prefab = makePrefab('prefab', {components: {}})
    const prefabChild1 = makeObject('prefabChild1',
      {parentId: 'prefab', components: {}, geometry: {type: 'box', width: 1, height: 1, depth: 1}})
    const prefabChild2 = makeObject('prefabChild2', {})
    const instance = makeInstance('instance', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        prefabChild1,
        prefabChild2,
        instance,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const prefabChild1Eid = sceneHandle.graphIdToPrefab.get('prefabChild1')!
    const instanceEid = sceneHandle.graphIdToEid.get('instance')!
    const instanceChild1Eid = world.getInstanceEntity(instanceEid, prefabChild1Eid)

    // Debug update to prefab child
    sceneHandle.updateDebug({
      objects: {
        prefab,
        prefabChild2,
        instance,
        prefabChild1: makeObject('prefabChild1',
          {
            parentId: 'prefab',
            components: {},
            geometry: {type: 'sphere', radius: 3},
          }),
      },
      spaces: {},
    })

    world.tick()
    const instanceGeometry = ecs.SphereGeometry.get(world, instanceChild1Eid)
    assert.deepEqual(instanceGeometry, {radius: 3}, 'Instance child geometry updated correctly')
  })

  it('adding and removing a override in debug update adds and removes the component', () => {
    const prefab = makePrefab('prefab', {components: {}})
    const prefabChild1 = makeObject('prefabChild1',
      {parentId: 'prefab', components: {}, geometry: {type: 'box', width: 1, height: 1, depth: 1}})
    const prefabChild2 = makeObject('prefabChild2', {})
    const instance = makeInstance('instance', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        prefabChild1,
        prefabChild2,
        instance,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const instanceEid = sceneHandle.graphIdToEid.get('instance')!

    // Debug update to instance
    sceneHandle.updateDebug({
      objects: {
        prefab,
        prefabChild2,
        instance: makeInstance('instance', 'prefab', {}, {
          geometry: {type: 'sphere', radius: 3},
        }),
        prefabChild1,
      },
      spaces: {},
    })

    world.tick()
    const instanceGeometry = ecs.SphereGeometry.get(world, instanceEid)
    assert.deepEqual(instanceGeometry, {radius: 3}, 'Instance geometry updated correctly')

    // Debug update to remove the geometry override
    sceneHandle.updateDebug({
      objects: {
        prefab,
        prefabChild2,
        instance: makeInstance('instance', 'prefab', {}, {}),
        prefabChild1,
      },
      spaces: {},
    })

    world.tick()
    assert.isFalse(
      ecs.SphereGeometry.has(world, instanceEid), 'Instance geometry removed correctly'
    )
  })

  it('adding and removing a instance child override in debug update adds and removes the component',
    () => {
      const prefab = makePrefab('prefab', {components: {}})
      const prefabChild1 = makeObject('prefabChild1',
        {
          parentId: 'prefab',
          components: {},
          geometry: {type: 'box', width: 1, height: 1, depth: 1},
        })
      const prefabChild2 = makeObject('prefabChild2', {})
      const instance = makeInstance('instance', 'prefab', {}, {})

      const baseGraph: SceneGraph = {
        objects: {
          prefab,
          prefabChild1,
          prefabChild2,
          instance,
        },
        spaces: {},
      }

      const sceneHandle = world.loadScene(baseGraph)
      world.tick()

      const prefabChild1Eid = sceneHandle.graphIdToPrefab.get('prefabChild1')!
      const instanceEid = sceneHandle.graphIdToEid.get('instance')!
      const instanceChild1Eid = world.getInstanceEntity(instanceEid, prefabChild1Eid)

      // Debug update to instance child
      sceneHandle.updateDebug({
        objects: {
          prefab,
          prefabChild2,
          instance: makeInstance('instance', 'prefab', {}, {}, {
            'prefabChild1': {
              geometry: {type: 'sphere', radius: 3},
            },
          }),
          prefabChild1,
        },
        spaces: {},
      })

      world.tick()
      const instanceGeometry = ecs.SphereGeometry.get(world, instanceChild1Eid)
      assert.deepEqual(instanceGeometry, {radius: 3}, 'Instance child geometry updated correctly')

      // Debug update to remove the instance child geometry override
      sceneHandle.updateDebug({
        objects: {
          prefab,
          prefabChild2,
          instance: makeInstance('instance', 'prefab', {}, {}, {}),
          prefabChild1,
        },
        spaces: {},
      })

      world.tick()
      assert.isFalse(
        ecs.SphereGeometry.has(world, instanceChild1Eid),
        'Instance child geometry removed correctly'
      )
    })

  it('spawning instance with overrides', () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
    })
    const instance = makeInstance('instance', 'prefab', {}, {
      geometry: {type: 'sphere', radius: 1},
    })

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const instanceEid = sceneHandle.graphIdToEid.get('instance')!
    const instanceGeometry = ecs.SphereGeometry.get(world, instanceEid)
    assert.deepEqual(instanceGeometry, {radius: 1})
  })

  it('overriding instance component at runtime', () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
    })
    const instance1 = makeInstance('instance1', 'prefab', {}, {})
    const instance2 = makeInstance('instance2', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        instance1,
        instance2,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const instance1Eid = sceneHandle.graphIdToEid.get('instance1')!
    const instance2Eid = sceneHandle.graphIdToEid.get('instance2')!
    const prefabEid = sceneHandle.graphIdToPrefab.get('prefab')!

    // Override geometry on instance1
    ecs.BoxGeometry.set(world, instance1Eid, {width: 3, height: 3, depth: 3})
    world.tick()

    const prefabGeometry = ecs.BoxGeometry.get(world, prefabEid)
    assert.deepEqual(prefabGeometry, {width: 1, height: 1, depth: 1})

    const instance1Geometry = ecs.BoxGeometry.get(world, instance1Eid)
    assert.deepEqual(instance1Geometry, {width: 3, height: 3, depth: 3})

    const instance2Geometry = ecs.BoxGeometry.get(world, instance2Eid)
    assert.deepEqual(instance2Geometry, {width: 1, height: 1, depth: 1})
  })

  it('adding component to prefab propagates to instances', () => {
    const prefab = makePrefab('prefab')
    const instance = makeInstance('instance', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const prefabEid = sceneHandle.graphIdToPrefab.get('prefab')!
    const instanceEid = sceneHandle.graphIdToEid.get('instance')!

    // Add new component to prefab
    ecs.Material.set(world, prefabEid, {r: 255, g: 0, b: 0})
    world.tick()

    const instanceMaterial = ecs.Material.get(world, instanceEid)
    assert.strictEqual(instanceMaterial.r, 255)
    assert.strictEqual(instanceMaterial.g, 0)
    assert.strictEqual(instanceMaterial.b, 0)
  })

  it('adding component to instance does not affect other instances', () => {
    const prefab = makePrefab('prefab')
    const instance1 = makeInstance('instance1', 'prefab', {}, {})
    const instance2 = makeInstance('instance2', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        instance1,
        instance2,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const instance1Eid = sceneHandle.graphIdToEid.get('instance1')!
    const instance2Eid = sceneHandle.graphIdToEid.get('instance2')!

    // Add new component to instance1
    ecs.Material.set(world, instance1Eid, {r: 0, g: 255, b: 0})
    world.tick()

    const instance1Material = ecs.Material.get(world, instance1Eid)
    assert.strictEqual(instance1Material.r, 0)
    assert.strictEqual(instance1Material.g, 255)
    assert.strictEqual(instance1Material.b, 0)

    const instance2Material = ecs.Material.has(world, instance2Eid)
    assert.isNotOk(instance2Material)
  })

  it('removing a component from the source prefab removes it from all its instances', () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
    })
    const instance1 = makeInstance('instance1', 'prefab', {}, {})
    const instance2 = makeInstance('instance2', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        instance1,
        instance2,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const prefabEid = sceneHandle.graphIdToPrefab.get('prefab')!
    const instance1Eid = sceneHandle.graphIdToEid.get('instance1')!
    const instance2Eid = sceneHandle.graphIdToEid.get('instance2')!

    // Remove geometry from prefab
    ecs.BoxGeometry.remove(world, prefabEid)
    world.tick()

    // Verify prefab and all instances no longer have the geometry component
    assert.isNotOk(ecs.BoxGeometry.has(world, prefabEid), 'Prefab is deleted')
    assert.isNotOk(ecs.BoxGeometry.has(world, instance1Eid), 'Instance1 is deleted')
    assert.isNotOk(ecs.BoxGeometry.has(world, instance2Eid), 'Instance2 is deleted')
  })

  it('removing a scene and updating with the old scene graph works', () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
    })
    const instance1 = makeInstance('instance1', 'prefab', {}, {})
    const instance2 = makeInstance('instance2', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        instance1,
        instance2,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    // Remove the scene
    sceneHandle.remove()

    // Update with the old scene graph
    assert.doesNotThrow(() => {
      sceneHandle.updateDebug(baseGraph)
    }, 'Updating with old scene graph does not throw errors')

    // Verify new prefab and instances exist
    const newPrefabEid = sceneHandle.graphIdToPrefab.get('prefab')!
    const newInstance1Eid = sceneHandle.graphIdToEid.get('instance1')!
    const newInstance2Eid = sceneHandle.graphIdToEid.get('instance2')!
    assert.isOk(newPrefabEid, 'New prefab entity exists')
    assert.isOk(newInstance1Eid, 'New instance1 entity exists')
    assert.isOk(newInstance2Eid, 'New instance2 entity exists')
  })

  it('components can reference prefabs', () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
    })
    const instance1 = makeInstance('instance1', 'prefab', {}, {})
    const object = makeObject('object', {
      components: {
        'look-at': {
          'id': 'look-at',
          'name': 'look-at-animation',
          'parameters': {
            'target': {
              'type': 'entity',
              'id': 'prefab',
            },
          },
        },
      },
    })

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        instance1,
        object,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const objectEid = sceneHandle.graphIdToEid.get('object')!
    assert.isOk(objectEid, 'Object entity exists')

    const prefabEid = sceneHandle.graphIdToPrefab.get('prefab')!

    // Get the reference component and its schema
    const referenceComponent = ecs.LookAtAnimation.get(world, objectEid)
    assert.isOk(referenceComponent, 'Reference component exists')
    const {target} = referenceComponent
    assert.strictEqual(target, prefabEid, 'Target type is entity')
  })

  it('sceneHandle.remove clears graphIdToPrefab and graphIdToEidOrPrefab', () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
    })
    const instance = makeInstance('instance', 'prefab', {}, {})
    const normalObject = makeObject('normalObject')

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        instance,
        normalObject,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    // Sanity check: should have entries before remove
    assert.isAbove(sceneHandle.graphIdToPrefab.size, 0, 'graphIdToPrefab has entries before remove')
    assert.isAbove(
      sceneHandle._graphIdToEidOrPrefab.size, 0, 'graphIdToEidOrPrefab has entries before remove'
    )

    sceneHandle.remove()

    assert.strictEqual(sceneHandle.graphIdToPrefab.size, 0, 'graphIdToPrefab is empty after remove')
    assert.strictEqual(
      sceneHandle._graphIdToEidOrPrefab.size, 0, 'graphIdToEidOrPrefab is empty after remove'
    )
  })

  it(`prefab instance inheriting a component with an internal
    prefab reference is remapped to the corresponding instance`,
  () => {
    const prefab = makePrefab('prefab', {
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
      components: {
        'look-at': {
          'id': 'look-at',
          'name': 'look-at-animation',
          'parameters': {
            'target': {
              'type': 'entity',
              'id': 'prefab',
            },
          },
        },
      },
    })
    const instance1 = makeInstance('instance', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        instance1,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const instanceEid = sceneHandle.graphIdToEid.get('instance')!

    // Get the reference component and its schema
    const referenceComponent = ecs.LookAtAnimation.get(world, instanceEid)
    assert.isOk(referenceComponent, 'Reference component exists')
    const {target} = referenceComponent
    assert.strictEqual(target, instanceEid, 'Target type is entity')
  })

  it(`prefab instance inheriting a component with a reference to a prefab child is
    remapped to the corresponding instance child`,
  () => {
    const prefab = makePrefab('prefab', {
      components: {},
    })
    const prefabChild = makeObject('prefabChild', {
      parentId: 'prefab',
      components: {
        'look-at': {
          'id': 'look-at',
          'name': 'look-at-animation',
          'parameters': {
            'target': {
              'type': 'entity',
              'id': 'prefabChild',
            },
          },
        },
      },
    })
    const instance = makeInstance('instance', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        prefabChild,
        instance,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.setSceneHook(sceneHandle)
    world.tick()

    const instanceEid = sceneHandle.graphIdToEid.get('instance')!
    const prefabChildEid = sceneHandle.graphIdToPrefab.get('prefabChild')!

    const instanceChildEid = world.getInstanceEntity(instanceEid, prefabChildEid)

    const referenceComponent = ecs.LookAtAnimation.get(world, instanceChildEid)
    assert.isOk(referenceComponent, 'Reference component exists')
    const {target} = referenceComponent
    assert.strictEqual(target, instanceChildEid, 'Target is remapped to instance child')
  })

  it(`prefab instance child inheriting a component with
    a reference to the root prefab is remapped to the instance`,
  () => {
    const prefab = makePrefab('prefab', {components: {}})
    const prefabChild = makeObject('prefabChild', {
      parentId: 'prefab',
      components: {
        'look-at': {
          'id': 'look-at',
          'name': 'look-at-animation',
          'parameters': {
            'target': {
              'type': 'entity',
              'id': 'prefab',
            },
          },
        },
      },
    })
    const instance = makeInstance('instance', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        prefabChild,
        instance,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const instanceEid = sceneHandle.graphIdToEid.get('instance')!
    const prefabChildEid = sceneHandle.graphIdToPrefab.get('prefabChild')!
    const instanceChildEid = world.getInstanceEntity(instanceEid, prefabChildEid)

    const referenceComponent = ecs.LookAtAnimation.get(world, instanceChildEid)
    assert.isOk(referenceComponent, 'Reference component exists')
    const {target} = referenceComponent
    assert.strictEqual(target, instanceEid, 'Target is remapped to instance')
  })

  it(`prefab instance child inheriting a component with a reference to another prefab child is
    remapped to the corresponding instance child`,
  () => {
    const prefab = makePrefab('prefab', {components: {}})
    const prefabChild1 = makeObject('prefabChild1', {parentId: 'prefab', components: {}})
    const prefabChild2 = makeObject('prefabChild2', {
      parentId: 'prefab',
      components: {
        'look-at': {
          'id': 'look-at',
          'name': 'look-at-animation',
          'parameters': {
            'target': {
              'type': 'entity',
              'id': 'prefabChild1',
            },
          },
        },
      },
    })
    const instance = makeInstance('instance', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        prefabChild1,
        prefabChild2,
        instance,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const instanceEid = sceneHandle.graphIdToEid.get('instance')!
    const prefabChild1Eid = sceneHandle.graphIdToPrefab.get('prefabChild1')!
    const instanceChild1Eid = world.getInstanceEntity(instanceEid, prefabChild1Eid)

    const prefabChild2Eid = sceneHandle.graphIdToPrefab.get('prefabChild2')!
    const instanceChild2Eid = world.getInstanceEntity(instanceEid, prefabChild2Eid)

    const referenceComponent = ecs.LookAtAnimation.get(world, instanceChild2Eid)
    assert.isOk(referenceComponent, 'Reference component exists')
    const {target} = referenceComponent
    assert.strictEqual(
      target, instanceChild1Eid, 'Target is remapped to corresponding instance child'
    )
  })

  it('non-prefab instance entity reference is not remapped', () => {
    const prefab = makePrefab('prefab', {components: {}})
    const object = makeObject('object', {
      components: {
        'look-at': {
          'id': 'look-at',
          'name': 'look-at-animation',
          'parameters': {
            'target': {
              'type': 'entity',
              'id': 'prefab',
            },
          },
        },
      },
    })

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        object,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const objectEid = sceneHandle.graphIdToEid.get('object')!
    const prefabEid = sceneHandle.graphIdToPrefab.get('prefab')!
    const referenceComponent = ecs.LookAtAnimation.get(world, objectEid)

    assert.isOk(referenceComponent, 'Reference component exists')
    const {target} = referenceComponent
    assert.strictEqual(target, prefabEid, 'Reference is not remapped')
  })

  it('object with a reference to an unrelated entity is not remapped', () => {
    const unrelated = makeObject('unrelated', {components: {}})

    const instance = makeObject('instance', {
      components: {
        'look-at': {
          'id': 'look-at',
          'name': 'look-at-animation',
          'parameters': {
            'target': {
              'type': 'entity',
              'id': 'unrelated',
            },
          },
        },
      },
    })

    const baseGraph: SceneGraph = {
      objects: {
        unrelated,
        instance,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const instanceEid = sceneHandle.graphIdToEid.get('instance')!
    const unrelatedEid = sceneHandle.graphIdToEid.get('unrelated')!
    const referenceComponent = ecs.LookAtAnimation.get(world, instanceEid)

    assert.isOk(referenceComponent, 'Reference component exists')
    const {target} = referenceComponent

    assert.strictEqual(target, unrelatedEid, 'Reference to unrelated entity is not remapped')
  })

  it('system eid mapping works as expected', () => {
    const prefab = makePrefab('prefab', {
      components: {},
    })
    const prefabChild = makeObject('prefabChild', {
      parentId: 'prefab',
      components: {
        'look-at': {
          'id': 'look-at',
          'name': 'look-at-animation',
          'parameters': {
            'target': {
              'type': 'entity',
              'id': 'prefabChild',
            },
          },
        },
      },
    })
    const instance = makeInstance('instance', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        prefabChild,
        instance,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const prefabEid = sceneHandle.graphIdToPrefab.get('prefab')!
    const prefabChildEid = sceneHandle.graphIdToPrefab.get('prefabChild')!
    const instanceEid = sceneHandle.graphIdToEid.get('instance')!

    // Define a system that runs on LookAtAnimation and checks the target remapping
    let systemRan = false
    const system = ecs.defineSystem([ecs.LookAtAnimation], (w: World, eid: Eid, [lookAt]) => {
      systemRan = true
      if (eid !== prefabEid) {
        return
      }
      const instanceChildEid = w.getInstanceEntity(instanceEid, prefabChildEid)
      assert.strictEqual(eid, instanceChildEid, 'System runs on instance child')
      assert.strictEqual(lookAt.target, instanceChildEid, 'Remapped target is correct')
    })

    ecs.registerBehavior(system)
    world.tick()
    assert.isTrue(systemRan, 'System ran and checked remapping')
  })

  it('prefab instance with a reference to an unrelated entity is not remapped', () => {
    const prefab = makePrefab('prefab', {})
    const unrelated = makeObject('unrelated', {components: {}})
    const instance = makeInstance('instance', 'prefab', {}, {
      components: {
        'look-at': {
          'id': 'look-at',
          'name': 'look-at-animation',
          'parameters': {
            'target': {
              'type': 'entity',
              'id': 'unrelated',
            },
          },
        },
      },
    })

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        unrelated,
        instance,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const instanceEid = sceneHandle.graphIdToEid.get('instance')!
    const unrelatedEid = sceneHandle.graphIdToEid.get('unrelated')!
    const referenceComponent = ecs.LookAtAnimation.get(world, instanceEid)

    assert.isOk(referenceComponent, 'Reference component exists')
    const {target} = referenceComponent

    assert.strictEqual(target, unrelatedEid, 'Reference to unrelated entity is not remapped')
  })

  it('prefab children are not added to world.allEntities or graphIdToEid', () => {
    const prefab = makePrefab('prefab', {components: {}})
    const prefabChild1 = makeObject('prefabChild1', {parentId: 'prefab', components: {}})
    const prefabChild2 = makeObject('prefabChild2', {
      parentId: 'prefab',
      components: {
        'look-at': {
          'id': 'look-at',
          'name': 'look-at-animation',
          'parameters': {
            'target': {
              'type': 'entity',
              'id': 'prefabChild1',
            },
          },
        },
      },
    })
    const instance = makeInstance('instance', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        prefabChild1,
        prefabChild2,
        instance,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    world.tick()

    const prefabChild1Eid = sceneHandle.graphIdToPrefab.get('prefabChild1')!
    const prefabChild2Eid = sceneHandle.graphIdToPrefab.get('prefabChild2')!

    assert.isFalse(
      world.allEntities.has(prefabChild1Eid), 'Prefab child 1 is not in allEntities'
    )
    assert.isFalse(
      world.allEntities.has(prefabChild2Eid), 'Prefab child 2 is not in allEntities'
    )

    assert.isFalse(
      sceneHandle.graphIdToEid.has('prefabChild1'), 'Prefab child 1 is not in graphIdToEid'
    )
    assert.isFalse(
      sceneHandle.graphIdToEid.has('prefabChild2'), 'Prefab child 2 is not in graphIdToEid'
    )
  })

  it('prefab children are correctly parented', () => {
    const prefab = makePrefab('prefab', {components: {}})
    const prefabChild1 = makeObject('prefabChild1', {parentId: 'prefab', components: {}})
    const nestedPrefabChild = makeObject(
      'nestedPrefabChild', {parentId: 'prefabChild1', components: {}}
    )
    const instance = makeInstance('instance', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        prefabChild1,
        nestedPrefabChild,
        instance,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
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

  it('instance child inherits position/rotation from prefab child', () => {
    const prefab = makePrefab('prefab', {})
    const prefabChild = makeObject('prefabChild', {
      parentId: 'prefab',
      position: [1, 2, 3],
      rotation: [0, 0, 0, 1],
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
      collider: {
        geometry: {type: 'box', width: 1, height: 1, depth: 1},
        mass: 0,
      },
    })
    const instance = makeInstance('instance', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        prefabChild,
        instance,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    for (let i = 0; i < 10; i++) {
      world.tick((i + 1) * 10)
    }

    const instanceEid = sceneHandle.graphIdToEid.get('instance')!
    const prefabChildEid = sceneHandle.graphIdToPrefab.get('prefabChild')!
    const instanceChildEid = world.getInstanceEntity(instanceEid, prefabChildEid)

    assert.strictEqual(ecs.Collider.get(world, instanceChildEid).type, ecs.ColliderType.Static)

    assert.isTrue(ecs.Position.has(world, instanceChildEid), 'Instance has position component')
    assert.isTrue(ecs.Quaternion.has(world, instanceChildEid), 'Instance has quaternion component')

    const initialInstanceChildY = ecs.Position.get(world, instanceChildEid).y
    assert.strictEqual(
      initialInstanceChildY,
      2,
      'Instance child y position matches prefab child y position'
    )

    ecs.Position.set(world, prefabChildEid, {x: 10, y: 20, z: 30})
    ecs.Quaternion.set(world, prefabChildEid, {x: 1, y: 0, z: 0, w: 0})
    world.tick()

    assertPosition(world, instanceChildEid, {x: 10, y: 20, z: 30},
      'Instance child position updated to match prefab child after tick')
    assertQuaternion(world, instanceChildEid, {x: 1, y: 0, z: 0, w: 0},
      'Instance child quaternion updated to match prefab child after tick')
  })

  it(`instance child inheriting position/rotation from prefab child
    removes inheritance when getting dynamic physics updates`, () => {
    const prefab = makePrefab('prefab', {})
    const prefabChild = makeObject('prefabChild', {
      parentId: 'prefab',
      position: [1, 2, 3],
      rotation: [0, 0, 0, 1],
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
      collider: {
        geometry: {type: 'box', width: 1, height: 1, depth: 1},
        mass: 1,
      },
    })
    const instance = makeInstance('instance', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        prefabChild,
        instance,
      },
      spaces: {},
    }

    const sceneHandle = world.loadScene(baseGraph)
    for (let i = 0; i < 10; i++) {
      world.tick((i + 1) * 10)
    }

    const instanceEid = sceneHandle.graphIdToEid.get('instance')!
    const prefabChildEid = sceneHandle.graphIdToPrefab.get('prefabChild')!
    const instanceChildEid = world.getInstanceEntity(instanceEid, prefabChildEid)

    assert.strictEqual(ecs.Collider.get(world, instanceChildEid).type, ecs.ColliderType.Dynamic)

    assert.isTrue(ecs.Position.has(world, instanceEid),
      'Instance has position component after becoming dynamic')
    assert.isTrue(ecs.Quaternion.has(world, instanceEid),
      'Instance has quaternion component after becoming dynamic')

    const postUpdateInstanceChildY = ecs.Position.get(world, instanceChildEid).y

    assert.notEqual(
      postUpdateInstanceChildY,
      2,
      'Instance child y position value falls after breaking inheritance'
    )

    assertPosition(world, prefabChildEid, {x: 1, y: 2, z: 3},
      'Prefab child position unchanged')

    ecs.Position.set(world, prefabChildEid, {x: 10, y: 20, z: 30})
    ecs.Quaternion.set(world, prefabChildEid, {x: 1, y: 0, z: 0, w: 0})
    world.tick()

    assertPosition(world, instanceChildEid, {x: 1, y: postUpdateInstanceChildY, z: 3},
      'Instance child x position unchanged after prefab modification (inheritance broken)')

    assertQuaternion(world, instanceChildEid, {x: 0, y: 0, z: 0, w: 1},
      'Instance quaternion unchanged after prefab modification (inheritance broken)')
  })
})
