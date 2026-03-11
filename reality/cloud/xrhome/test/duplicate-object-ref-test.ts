import {assert} from 'chai'

import type {BaseGraphObject, EntityReference, GraphObject} from '@ecs/shared/scene-graph'

import {duplicateObjects} from '../src/client/studio/configuration/duplicate-object'
import type {IdMapper} from '../src/client/studio/id-generation'
import {deriveScene} from '../src/client/studio/derive-scene'

const NULL_ID = '00000000-0000-0000-0000-000000000000'
const REORDER_SEED = 0

const makeMockIdStream = (prefix: string): IdMapper => ({
  fromId: (id: string) => `${prefix}${id}`,
})

const makeObject = (
  id: string, name: string, parentId?: string, order?: number, targetId?: string
): GraphObject => ({
  parentId: parentId || NULL_ID,
  id,
  ...(order !== undefined ? {order} : {}),
  components: {
    [NULL_ID]: {
      id: NULL_ID,
      name: 'name',
      parameters: {
        target: {
          type: 'entity',
          id: targetId || NULL_ID,
        },
      },
    },
  },
  name,
})

type MapAttributes = Pick<BaseGraphObject, 'map' | 'mapPoint'> & {
  pretendNewThing: { targetEntity: EntityReference }
};

const mapAttributes = (targetId: string): MapAttributes => ({
  mapPoint: {
    targetEntity: {
      type: 'entity',
      id: targetId,
    },
    latitude: 0,
    longitude: 0,
    meters: 0,
    minScale: 0,
  },
  pretendNewThing: {
    targetEntity: {
      type: 'entity',
      id: targetId,
    },
  },
  map: {
    latitude: 0,
    longitude: 0,
    targetEntity: {
      type: 'entity',
      id: targetId,
    },
    radius: 0,
    spawnLocations: false,
    useGps: false,
  },
})

describe('update-object-references', () => {
  it('should update object references', () => {
    // A -> B -> D, A -> C
    const scene = {
      objects: {
        A: makeObject('A', 'A'),
        B: makeObject('B', 'B', 'A', 0, 'C'),
        C: makeObject('C', 'C', 'A', 1, 'A'),
        D: makeObject('D', 'D', 'B', undefined, 'A'),
      },
    }

    const stream = makeMockIdStream('new')

    const derivedScene = deriveScene(scene)
    const newScene = duplicateObjects({
      scene,
      derivedScene,
      objectIds: ['A'],
      ids: stream,
      reorderSeed: REORDER_SEED,
    })

    const expectedScene = {
      objects: {
        A: makeObject('A', 'A', NULL_ID, 0),
        B: makeObject('B', 'B', 'A', 0, 'C'),
        C: makeObject('C', 'C', 'A', 1, 'A'),
        D: makeObject('D', 'D', 'B', undefined, 'A'),
        newA: makeObject('newA', 'A (1)', NULL_ID, 1),
        newB: makeObject('newB', 'B (1)', 'newA', 0, 'newC'),
        newC: makeObject('newC', 'C (1)', 'newA', 1, 'newA'),
        newD: makeObject('newD', 'D (1)', 'newB', undefined, 'newA'),
      },
    }

    assert.deepStrictEqual(newScene, expectedScene)
  })

  it('does not modify the input', () => {
    const SCENE_GRAPH_A = {
      objects: {
        A: makeObject('A', 'A'),
      },
    }
    const originalScene = JSON.parse(JSON.stringify(SCENE_GRAPH_A))

    const derivedScene = deriveScene(SCENE_GRAPH_A)
    const newScene1 = duplicateObjects({
      scene: SCENE_GRAPH_A,
      derivedScene,
      objectIds: ['A'],
      ids: makeMockIdStream('new'),
      reorderSeed: REORDER_SEED,
    })
    const newScene2 = duplicateObjects({
      scene: SCENE_GRAPH_A,
      derivedScene,
      objectIds: ['A'],
      ids: makeMockIdStream('new'),
      reorderSeed: REORDER_SEED,
    })
    assert.deepStrictEqual(newScene1, newScene2)
    assert.deepStrictEqual(SCENE_GRAPH_A, originalScene)
  })

  it('should update object references when the target is a middle child', () => {
    // A -> B -> D, A -> C
    const scene = {
      objects: {
        A: makeObject('A', 'A'),
        B: makeObject('B', 'B', 'A', 0, 'C'),
        C: makeObject('C', 'C', 'A', 1, 'A'),
        D: makeObject('D', 'D', 'B', undefined, 'A'),
      },
    }
    const derivedScene = deriveScene(scene)
    const newScene = duplicateObjects({
      scene,
      derivedScene,
      objectIds: ['B'],
      ids: makeMockIdStream('new'),
      reorderSeed: REORDER_SEED,
    })

    // test that new objects are created and have the correct references
    const expectedScene = {
      objects: {
        A: makeObject('A', 'A'),
        B: makeObject('B', 'B', 'A', 0, 'C'),
        C: makeObject('C', 'C', 'A', 1, 'A'),
        D: makeObject('D', 'D', 'B', undefined, 'A'),
        newB: makeObject('newB', 'B (1)', 'A', 0.25, 'C'),
        newD: makeObject('newD', 'D (1)', 'newB', undefined, 'A'),
      },
    }

    assert.deepStrictEqual(newScene, expectedScene)
  })

  it('update multi object selection', () => {
    // A -> C,  B -> D
    const scene = {
      objects: {
        A: makeObject('A', 'A'),
        B: makeObject('B', 'B'),
        C: makeObject('C', 'C', 'A', undefined, 'B'),
        D: makeObject('D', 'D', 'B', undefined, 'C'),
      },
    }
    const derivedScene = deriveScene(scene)
    const newScene = duplicateObjects({
      scene,
      derivedScene,
      objectIds: ['A', 'B'],
      ids: makeMockIdStream('new'),
      reorderSeed: REORDER_SEED,
    })

    const expectedScene = {
      objects: {
        A: makeObject('A', 'A', NULL_ID, 0),
        B: makeObject('B', 'B', NULL_ID, 1),
        C: makeObject('C', 'C', 'A', undefined, 'B'),
        D: makeObject('D', 'D', 'B', undefined, 'C'),
        newA: makeObject('newA', 'A (1)', NULL_ID, 0.25, NULL_ID),
        newB: makeObject('newB', 'B (1)', NULL_ID, 2, NULL_ID),
        newC: makeObject('newC', 'C (1)', 'newA', undefined, 'newB'),
        newD: makeObject('newD', 'D (1)', 'newB', undefined, 'newC'),
      },
    }

    assert.deepStrictEqual(newScene, expectedScene)
  })

  it('can update Map and Map point references', () => {
    // A -> B, with map entity reference
    const scene = {
      objects: {
        A: makeObject('A', 'A'),
        B: {
          ...makeObject('B', 'B', 'A'),
          ...mapAttributes('A'),
        },
      },
    }
    const derivedScene = deriveScene(scene)
    const newScene = duplicateObjects({
      scene,
      derivedScene,
      objectIds: ['A'],
      ids: makeMockIdStream('new'),
      reorderSeed: REORDER_SEED,
    })

    const expectedScene = {
      objects: {
        A: makeObject('A', 'A', NULL_ID, 0),
        B: {
          ...makeObject('B', 'B', 'A'),
          ...mapAttributes('A'),
        },
        newA: makeObject('newA', 'A (1)', NULL_ID, 1),
        newB: {
          ...makeObject('newB', 'B (1)', 'newA'),
          ...mapAttributes('newA'),
        },
      },
    }

    assert.deepStrictEqual(newScene, expectedScene)
  })

  it('should not duplicate objects with ancestors in the selection', () => {
    // A -> B, B -> C
    const scene = {
      objects: {
        A: makeObject('A', 'A', NULL_ID, undefined, 'C'),
        B: makeObject('B', 'B', 'A', undefined, 'A'),
        C: makeObject('C', 'C', 'B', undefined, 'A'),
        D: makeObject('D', 'D', NULL_ID, undefined, NULL_ID),
      },
    }
    const derivedScene = deriveScene(scene)
    const newScene = duplicateObjects({
      scene,
      derivedScene,
      objectIds: ['A', 'B', 'C', 'D'],
      ids: makeMockIdStream('new'),
      reorderSeed: REORDER_SEED,
    })

    const expectedScene = {
      objects: {
        A: makeObject('A', 'A', NULL_ID, 0, 'C'),
        B: makeObject('B', 'B', 'A', undefined, 'A'),
        C: makeObject('C', 'C', 'B', undefined, 'A'),
        D: makeObject('D', 'D', NULL_ID, 1, NULL_ID),
        newA: makeObject('newA', 'A (1)', NULL_ID, 0.25, 'newC'),
        newB: makeObject('newB', 'B (1)', 'newA', undefined, 'newA'),
        newC: makeObject('newC', 'C (1)', 'newB', undefined, 'newA'),
        newD: makeObject('newD', 'D (1)', NULL_ID, 2, NULL_ID),
      },
    }

    assert.deepStrictEqual(newScene, expectedScene)
  })
})
