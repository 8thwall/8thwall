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
import THREE from './three'

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

describe('Component callbacks test', () => {
  let world: World

  const addCalls: Eid[] = []
  const removeCalls: Eid[] = []

  const assertCallbacks = (expectedAdds: Eid[], expectedRemoves: Eid[], message: string) => {
    if (expectedAdds.length) {
      assert.deepEqual(addCalls, expectedAdds, `${message}: Add calls`)
    } else {
      assert.isEmpty(addCalls, `${message}: Add calls should be empty`)
    }
    if (expectedRemoves.length) {
      assert.deepEqual(removeCalls, expectedRemoves, `${message}: Remove calls`)
    } else {
      assert.isEmpty(removeCalls, `${message}: Remove calls should be empty`)
    }

    addCalls.length = 0
    removeCalls.length = 0
  }

  beforeEach(async () => {
    await ecs.ready()
    world = ecs.createWorld(...initThree())
    addCalls.length = 0
    removeCalls.length = 0
  })
  afterEach(() => {
    world.destroy()
  })

  it('normal object gets single set of callbacks', () => {
    ecs.registerComponent({
      name: 'test1',
      add: (_, {eid}) => {
        addCalls.push(eid)
      },
      remove: (_, {eid}) => {
        removeCalls.push(eid)
      },
    })

    const normal = makeObject('normal', {

      components: {
        someId: {
          id: 'someId',
          name: 'test1',
          parameters: {},
        },
      },
    })

    const baseGraph: SceneGraph = {
      objects: {
        normal,
      },
      spaces: {},
    }

    assertCallbacks([], [], 'Before loading scene')

    const scene = world.loadScene(baseGraph)
    const normalEid = scene.graphIdToEid.get('normal')!

    assertCallbacks([normalEid], [], 'After loading scene')
    world.tick()
    assertCallbacks([], [], 'After tick, nothing changes')
    scene.remove()
    assertCallbacks([], [], 'After removing scene, nothing updates yet')
    world.tick()
    assertCallbacks([], [normalEid], 'After tick, remove called')
  })

  it('prefab instance gets single set of callbacks', () => {
    ecs.registerComponent({
      name: 'test2',
      add: (_, {eid}) => {
        addCalls.push(eid)
      },
      remove: (_, {eid}) => {
        removeCalls.push(eid)
      },
    })

    const prefab = makePrefab('prefab', {
      components: {
        someId: {
          id: 'someId',
          name: 'test2',
          parameters: {},
        },
      },
    })

    const instance = makeInstance('instance', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }

    assertCallbacks([], [], 'Before loading scene')

    const scene = world.loadScene(baseGraph)
    const instanceEid = scene.graphIdToEid.get('instance')!

    assertCallbacks([instanceEid], [], 'After loading scene')
    world.tick()
    assertCallbacks([], [], 'After tick, nothing changes')
    scene.remove()
    assertCallbacks([], [], 'After removing scene, nothing updates yet')
    world.tick()
    assertCallbacks([], [instanceEid], 'After tick, remove called')
  })

  it('prefab instance gets single set of callbacks if world is constructed after register', () => {
    ecs.registerComponent({
      name: 'test3',
      add: (_, {eid}) => {
        addCalls.push(eid)
      },
      remove: (_, {eid}) => {
        removeCalls.push(eid)
      },
    })

    world.destroy()
    world = ecs.createWorld(...initThree())

    const prefab = makePrefab('prefab', {
      components: {
        someId: {
          id: 'someId',
          name: 'test3',
          parameters: {},
        },
      },
    })

    const instance = makeInstance('instance', 'prefab', {}, {})

    const baseGraph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }

    assertCallbacks([], [], 'Before loading scene')

    const scene = world.loadScene(baseGraph)
    const instanceEid = scene.graphIdToEid.get('instance')!

    assertCallbacks([instanceEid], [], 'After loading scene')
    world.tick()
    assertCallbacks([], [], 'After tick, nothing changes')
    scene.remove()
    assertCallbacks([], [], 'After removing scene, nothing updates yet')
    world.tick()
    assertCallbacks([], [instanceEid], 'After tick, remove called')
  })

  it('Only creates one light object after mutating light on prefab instance', () => {
    const source = makePrefab('source', {
      light: {
        type: 'point',
        color: '#ffffff',
        intensity: 1,
      },
    })

    const instance = makeInstance('instance', 'source')

    const baseGraph: SceneGraph = {
      objects: {
        source,
        instance,
      },
    }

    const scene = world.loadScene(baseGraph)

    const instanceEid = scene.graphIdToEid.get('instance')!
    assert.isOk(instanceEid)
    const instanceObject = world.three.entityToObject.get(instanceEid)!
    assert.isOk(instanceObject)

    assert.isTrue(ecs.Light.has(world, instanceEid), 'Instance should have Light component')

    assert.strictEqual(instanceObject.children.length, 1, 'After spawn')
    const lightObject = instanceObject.children[0]
    if (!(lightObject instanceof THREE.PointLight)) {
      throw new Error('Child of instance should be a PointLight')
    }

    world.tick()

    assert.deepEqual(instanceObject.children, [lightObject], 'After tick, no change')

    ecs.Light.set(world, instanceEid, {intensity: 2})

    world.tick()

    assert.strictEqual(instanceObject.children.length, 1, 'Length after update')
    assert.deepEqual(instanceObject.children, [lightObject], 'Identity after update')
    assert.strictEqual(ecs.Light.get(world, instanceEid).intensity, 2, 'ECS component intensity')
    assert.strictEqual(lightObject.intensity, 2 * Math.PI, 'THREE.js intensity (Math.PI factor)')
  })
})
