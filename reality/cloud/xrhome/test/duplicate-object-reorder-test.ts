import {assert} from 'chai'
import type {GraphObject} from '@ecs/shared/scene-graph'

import {duplicateObjects} from '../src/client/studio/configuration/duplicate-object'
import type {IdMapper} from '../src/client/studio/id-generation'
import {deriveScene} from '../src/client/studio/derive-scene'

const NULL_ID = '00000000-0000-0000-0000-000000000000'

const makeMockIdStream = (prefix: string): IdMapper => ({
  fromId: (id: string) => `${prefix}${id}`,
})

const makeObject = (id: string, parentId = NULL_ID, order?: number): GraphObject => ({
  id, parentId, order, components: {},
})

describe('duplicateObjects', () => {
  it('should place duplicates directly after their source', () => {
    const scene = {
      objects: {
        P: makeObject('P'),
        A: makeObject('A', 'P', 1),
        B: makeObject('B', 'P', 2),
        C: makeObject('C', 'P', 3),
        D: makeObject('D', 'P', 4),
      },
    }

    const derivedScene = deriveScene(scene)
    const stream = makeMockIdStream('new')
    const newScene = duplicateObjects({
      scene, derivedScene, objectIds: ['A', 'C', 'D'], ids: stream, reorderSeed: 0.5,
    })

    const newDerivedScene = deriveScene(newScene)
    const newChildren = newDerivedScene.getChildren('P')

    const expectedChildren = ['A', 'newA', 'B', 'C', 'newC', 'D', 'newD']
    assert.deepStrictEqual(newChildren, expectedChildren)

    const orderValues = newChildren.map(id => newDerivedScene.getObject(id).order)
    const noRepeats = [...new Set(orderValues)]
    assert.lengthOf(noRepeats, noRepeats.length, 'order values must be unique')
  })
})
