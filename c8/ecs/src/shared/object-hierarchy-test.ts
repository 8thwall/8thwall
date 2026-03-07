// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)
import {describe, it, assert} from '@nia/bzl/js/chai-js'

import type {BaseGraphObject, GraphObject, PrefabInstanceChildren, SceneGraph} from './scene-graph'
import {getParentInstanceId, isDescendantOf} from './object-hierarchy'

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

const makeInstance = (
  id: string, instanceOf: string, overrides?: Partial<GraphObject>, deletions?: {},
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

describe('isDescendantOf', () => {
  it('Objects are not descendants of themselves', () => {
    const prefab = makePrefab('prefab')
    const graph: SceneGraph = {
      objects: {
        prefab,
      },
      spaces: {},
    }
    assert.isFalse(isDescendantOf(graph, 'prefab', 'prefab'))
  })

  it('Works properly for both scoped and regular object IDs', () => {
    const prefab = makePrefab('prefab')
    const child1 = makeObject('child1', {parentId: 'prefab'})
    const child2 = makeObject('child2', {parentId: 'child1'})
    const instance = makeInstance('instance', 'prefab', {}, {})
    const graph: SceneGraph = {
      objects: {
        prefab,
        child1,
        child2,
        instance,
      },
      spaces: {},
    }

    // Regular object IDs
    assert.isTrue(isDescendantOf(graph, 'child2', 'prefab'))
    assert.isTrue(isDescendantOf(graph, 'child2', 'child1'))
    assert.isFalse(isDescendantOf(graph, 'child1', 'child2'))

    // Scoped object IDs
    assert.isTrue(isDescendantOf(graph, 'instance/child2', 'instance'))
    assert.isTrue(isDescendantOf(graph, 'instance/child2', 'instance/child1'))
    assert.isFalse(isDescendantOf(graph, 'instance/child1', 'instance/child2'))
    assert.isFalse(isDescendantOf(graph, 'instance', 'instance'))

    // Additional checks
    assert.isFalse(isDescendantOf(graph, 'instance', 'prefab'))
    assert.isFalse(isDescendantOf(graph, 'instance/child2', 'prefab'))
  })

  it('Handles deleted children and nested deleted children in an instance', () => {
    const prefab = makePrefab('prefab')
    const child1 = makeObject('child1', {parentId: 'prefab'})
    const child2 = makeObject('child2', {parentId: 'child1'})
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      child1: {deleted: true},
    })
    const graph: SceneGraph = {
      objects: {
        prefab,
        child1,
        child2,
        instance,
      },
      spaces: {},
    }

    assert.isFalse(isDescendantOf(graph, 'instance/child1', 'instance'))
    assert.isFalse(isDescendantOf(graph, 'instance/child2', 'instance'))
  })

  it('Handles children that don\'t exist', () => {
    const prefab = makePrefab('prefab')
    const instance = makeInstance('instance', 'prefab', {}, {}, {})
    const graph: SceneGraph = {
      objects: {
        prefab,
        instance,
      },
      spaces: {},
    }

    assert.isFalse(isDescendantOf(graph, 'instance/child1', 'instance'))
  })
})

describe('getParentInstanceId', () => {
  it('should return instanceOf for regular object IDs with instanceData', () => {
    const graph: SceneGraph = {
      objects: {
        'prefab': makePrefab('prefab'),
        'object': makeInstance('object', 'prefab'),
      },
      spaces: {},
    }

    assert.equal('object', getParentInstanceId(graph, 'object'))
  })

  it('should return undefined for regular object IDs without instanceData', () => {
    const graph: SceneGraph = {
      objects: {
        'object': makeObject('object'),
        'prefab': makePrefab('prefab'),
      },
      spaces: {},
    }

    assert.isUndefined(getParentInstanceId(graph, 'object'))
  })

  it('should return the instance part for scoped IDs', () => {
    const graph: SceneGraph = {
      objects: {
        'object': makeInstance('object', 'prefab'),
        'prefab': makePrefab('prefab'),
        'prefab/child': makeObject('prefab/child', {parentId: 'prefab'}),
      },
      spaces: {},
    }

    assert.equal('object', getParentInstanceId(graph, 'object/child'))
  })
})
