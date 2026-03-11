import {describe, it} from 'mocha'
import {assert} from 'chai'

import type {GraphObject, SceneGraph} from '@ecs/shared/scene-graph'

import type {DeepReadonly} from 'ts-essentials'

import type {IdMapper} from '../src/client/studio/id-generation'
import {duplicateSpace} from '../src/client/studio/configuration/duplicate-space'

const makeMockIdStream = (prefix: string): IdMapper => ({
  fromId: (id: string) => `${prefix}${id}`,
})

const makeObject = (
  id: string, name: string, parentId?: string
): GraphObject => ({
  parentId,
  id,
  name,
  components: {},
})

describe('duplicateSpace', () => {
  it('does nothing if the space does not exist', () => {
    const oldScene: DeepReadonly<SceneGraph> = {
      objects: {},
      spaces: {},
    }
    const spaceId = 'example'
    const ids = makeMockIdStream('new')
    const newScene = duplicateSpace(oldScene, spaceId, ids)
    assert.strictEqual(newScene, oldScene)
  })

  it('can duplicate an empty space', () => {
    const oldScene: DeepReadonly<SceneGraph> = {
      objects: {},
      spaces: {
        A: {
          id: 'A',
          name: 'My Name',
          activeCamera: null,
        },
      },
    }
    const ids = makeMockIdStream('new')
    const newScene = duplicateSpace(oldScene, 'A', ids)
    assert.deepStrictEqual(newScene, {
      objects: {},
      spaces: {
        A: {
          id: 'A',
          name: 'My Name',
          activeCamera: null,
        },
        newA: {
          id: 'newA',
          name: 'My Name (1)',
          activeCamera: null,
        },
      },
    })
  })

  it('can duplicate a space with an active camera', () => {
    const oldScene: DeepReadonly<SceneGraph> = {
      objects: {
        Camera: makeObject('Camera', 'Camera', 'A'),
      },
      spaces: {
        A: {
          id: 'A',
          name: 'My Name',
          activeCamera: 'Camera',
        },
      },
    }
    const ids = makeMockIdStream('new')
    const newScene = duplicateSpace(oldScene, 'A', ids)
    assert.deepStrictEqual(newScene, {
      objects: {
        Camera: makeObject('Camera', 'Camera', 'A'),
        newCamera: makeObject('newCamera', 'Camera', 'newA'),
      },
      spaces: {
        A: {
          id: 'A',
          name: 'My Name',
          activeCamera: 'Camera',
        },
        newA: {
          id: 'newA',
          name: 'My Name (1)',
          activeCamera: 'newCamera',
        },
      },
    })
  })
})
