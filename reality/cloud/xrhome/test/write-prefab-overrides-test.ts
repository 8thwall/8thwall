import type {
  BaseGraphObject,
  GraphObject, PrefabInstanceChildren, SceneGraph,
} from '@ecs/shared/scene-graph'
import {assert} from 'chai'

import {applyPrefabOverridesToRoot} from '../src/client/studio/configuration/prefab'

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
  id: string, instanceOf: string, deletions?: {}, overrides?: Partial<GraphObject>,
  children?: PrefabInstanceChildren
): GraphObject => ({
  id,
  name: '<unset>',
  position: [0, 0, 0],
  rotation: [0, 0, 0, 1],
  scale: [1, 1, 1],
  components: {},
  instanceData: {
    instanceOf,
    deletions:
     deletions || {},
    ...(children ? {children} : {}),
  },
  ...overrides,
})

describe('applyPrefabOverridesToRoot', () => {
  it('writes material override from instance to prefab and clears override', () => {
    const prefab = makePrefab('prefab', {
      material: {type: 'basic', color: '#ffffff'},
    })
    const instance = makeInstance('instance', 'prefab', {}, {
      material: {type: 'basic', color: '#ff0000'},
    })
    const graph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }

    const newScene = applyPrefabOverridesToRoot(graph, 'instance', ['material'])

    // Prefab should now have the overridden material
    assert.deepEqual(newScene.objects.prefab.material, {type: 'basic', color: '#ff0000'})

    // Instance should no longer have the material override
    assert.isUndefined(newScene.objects.instance.material)
  })

  it('writes component override from instance to prefab and clears override', () => {
    const testComponent = {
      id: 'testComponent',
      name: 'Test Component',
      parameters: {value: 'original'},
    }
    const overriddenComponent = {
      id: 'testComponent',
      name: 'Test Component',
      parameters: {value: 'overridden'},
    }

    const prefab = makePrefab('prefab', {components: {testComponent}})
    const instance = makeInstance('instance', 'prefab', {}, {
      components: {testComponent: overriddenComponent},
    })
    const graph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }

    const newScene = applyPrefabOverridesToRoot(graph, 'instance', ['testComponent'], true)

    // Prefab should now have the overridden component
    assert.deepEqual(
      newScene.objects.prefab.components.testComponent.parameters,
      {value: 'overridden'}
    )

    // Instance should no longer have the component override
    assert.isUndefined(newScene.objects.instance.components.testComponent)
  })

  it('handles instance child overrides', () => {
    const prefab = makePrefab('prefab', {material: {type: 'basic', color: '#ffffff'}})
    const prefabChild = makeObject('prefabChild', {
      parentId: 'prefab',
      material: {type: 'basic', color: '#00ff00'},
    })
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      prefabChild: {
        material: {type: 'basic', color: '#ff0000'},
      },
    })
    const graph: SceneGraph = {
      objects: {
        prefab,
        prefabChild,
        instance,
      },
      spaces: {},
    }

    const newScene = applyPrefabOverridesToRoot(graph, 'instance/prefabChild', ['material'])

    // PrefabChild should now have the overridden material
    assert.deepEqual(newScene.objects.prefabChild.material, {type: 'basic', color: '#ff0000'})

    // Instance child should no longer have the material override
    assert.isUndefined(newScene.objects.instance.instanceData?.children?.prefabChild?.material)
  })

  it('throws error when object is not a prefab instance', () => {
    const regularObject: GraphObject = {
      id: 'regularObject',
      name: 'Regular Object',
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      geometry: null,
      material: null,
      components: {},
    }
    const graph: SceneGraph = {
      objects: {
        regularObject,
      },
      spaces: {},
    }

    assert.throws(() => {
      applyPrefabOverridesToRoot(graph, 'regularObject', ['material'])
    }, 'Object regularObject is not a prefab instance')
  })

  it('throws error when prefab instance not found', () => {
    const graph: SceneGraph = {
      objects: {},
      spaces: {},
    }

    assert.throws(() => {
      applyPrefabOverridesToRoot(graph, 'nonexistent', ['material'])
    }, 'Prefab instance nonexistent not found')
  })
})
