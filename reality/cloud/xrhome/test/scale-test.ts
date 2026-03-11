import type {
  GraphObject, SceneGraph,
} from '@ecs/shared/scene-graph'

import {assert} from 'chai'

import {calculateDescalingFactor} from '../src/client/studio/scale'

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
  id: string, instanceOf: string, overrides?: Partial<GraphObject>
): GraphObject => ({
  id,
  name: '<unset>',
  components: {},
  instanceData: {instanceOf, deletions: {}, children: {}},
  ...overrides,
})

describe('scaling test', () => {
  it('should calculate factor in a simple scene with a simple hierarchy', () => {
    const initialScene: SceneGraph = {
      objects: {
        A: makeObject('A', {scale: [20, 100, 15], position: [1, 2, 3]}),
        B: makeObject('B', {position: [4, 5, 6], parentId: 'A'}),
      },
      spaces: {},
    }
    const factor = calculateDescalingFactor('B', initialScene)
    assert.deepEqual(factor, [20, 100, 15])
  })

  it('should not error out on a simple scene', () => {
    const initialScene: SceneGraph = {
      objects: {
        A: makeObject('A', {position: [1, 2, 3]}),
        B: makeObject('B', {position: [4, 5, 6], parentId: 'A'}),
      },
      spaces: {},
    }
    const factor = calculateDescalingFactor('A', initialScene)
    assert.deepEqual(factor, [1, 1, 1])
  })

  it('should not error out if object has an invalid parentId', () => {
    const initialScene: SceneGraph = {
      objects: {
        A: makeObject('A', {position: [1, 2, 3], parentId: 'C'}),
        B: makeObject('B', {position: [4, 5, 6], parentId: 'A'}),
      },
      spaces: {},
    }
    const factor = calculateDescalingFactor('A', initialScene)
    assert.deepEqual(factor, [1, 1, 1])
  })

  it('should not error out if the scene doesnt have a space set', () => {
    const initialScene: SceneGraph = {
      objects: {
        A: makeObject('A', {position: [1, 2, 3], parentId: 'C'}),
        B: makeObject('B', {position: [4, 5, 6], parentId: 'A'}),
      },
    }
    const factor = calculateDescalingFactor('A', initialScene)
    assert.isOk(factor)
  })

  it('honors inherited scale from parent objects', () => {
    const initialScene: SceneGraph = {
      objects: {
        A: makeObject('A', {scale: [2, 3, 4], position: [1, 2, 3]}),
        B: makeObject('B', {position: [4, 5, 6], parentId: 'A'}),
      },
      spaces: {},
    }
    const factor = calculateDescalingFactor('B', initialScene)
    assert.deepEqual(factor, [2, 3, 4])
  })

  it('inherits scale from prefab source', () => {
    const initialScene: SceneGraph = {
      objects: {
        A: makeObject('A', {scale: [2, 3, 4], prefab: true}),
        B: makeInstance('B', 'A'),
      },
      spaces: {},
    }
    const factor = calculateDescalingFactor('B', initialScene)
    assert.deepEqual(factor, [2, 3, 4])
  })

  it('can handle multiple levels of hierarchy', () => {
    const initialScene: SceneGraph = {
      objects: {
        A: makeObject('A', {scale: [2, 3, 4], position: [1, 2, 3]}),
        B: makeObject('B', {scale: [2, 3, 4], parentId: 'A'}),
        C: makeObject('C', {position: [7, 8, 9], parentId: 'B'}),
      },
      spaces: {},
    }
    const factor = calculateDescalingFactor('C', initialScene)
    assert.deepEqual(factor, [4, 9, 16])
  })

  it('works for prefab children', () => {
    const initialScene: SceneGraph = {
      objects: {
        'PREFAB': makeObject('PREFAB', {scale: [2, 3, 4], prefab: true}),
        'PREFAB_CHILD': makeObject('PREFAB_CHILD', {scale: [2, 3, 4], parentId: 'PREFAB'}),
      },
    }
    const factor = calculateDescalingFactor('PREFAB_CHILD', initialScene)
    assert.deepEqual(factor, [4, 9, 16])
  })

  it('works for scoped IDs', () => {
    const initialScene: SceneGraph = {
      objects: {
        'PREFAB': makeObject('PREFAB', {scale: [2, 3, 4], prefab: true}),
        'PREFAB_CHILD': makeObject('PREFAB_CHILD', {scale: [2, 3, 4], parentId: 'PREFAB'}),
        'INSTANCE': makeInstance('INSTANCE', 'PREFAB', {
          parentId: 'NOTHING',
        }),
        'OTHER': makeObject('INSTANCE', {scale: [2, 2, 2], parentId: 'INSTANCE/PREFAB_CHILD'}),
      },
    }
    const factor = calculateDescalingFactor('INSTANCE/PREFAB_CHILD', initialScene)
    assert.deepEqual(factor, [4, 9, 16])
  })

  it('works for children of scoped IDs', () => {
    const initialScene: SceneGraph = {
      objects: {
        'PREFAB': makeObject('PREFAB', {scale: [2, 3, 4], prefab: true}),
        'PREFAB_CHILD': makeObject('PREFAB_CHILD', {scale: [2, 3, 4], parentId: 'PREFAB'}),
        'INSTANCE': makeInstance('INSTANCE', 'PREFAB', {
          parentId: 'NOTHING',
        }),
        'OTHER': makeObject('INSTANCE', {scale: [2, 2, 2], parentId: 'INSTANCE/PREFAB_CHILD'}),
      },
    }
    const factor = calculateDescalingFactor('OTHER', initialScene)
    assert.deepEqual(factor, [8, 18, 32])
  })
})
