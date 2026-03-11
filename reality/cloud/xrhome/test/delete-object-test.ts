// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

import type {
  BaseGraphObject, GraphObject, PrefabInstanceChildren, SceneGraph,
} from '@ecs/shared/scene-graph'
import {assert} from 'chai'

import {deleteObjects} from '../src/client/studio/configuration/delete-object'

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
  id: string, instanceOf: string, deletions?: {}, overrides?: Partial<GraphObject>,
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

describe('deleteObject', () => {
  it('Deletes prefab children correctly when a scoped ID is passed', () => {
    const prefab = makePrefab('prefab')
    const child = makeObject('child', {parentId: 'prefab'})
    const instance = makeInstance('instance', 'prefab', {}, {}, {})
    const graph: SceneGraph = {
      objects: {
        prefab,
        child,
        instance,
      },
      spaces: {},
    }

    const updatedGraph = deleteObjects(graph, ['instance/child'])

    assert.deepEqual(updatedGraph.objects.instance, {
      ...instance,
      instanceData: {
        ...instance.instanceData,
        children: {
          'child': {
            deleted: true,
          },
        },
      },
    })
  })
})
