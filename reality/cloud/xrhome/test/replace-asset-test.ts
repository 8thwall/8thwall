import {describe, it} from 'mocha'
import {assert} from 'chai'
import type {SceneGraph} from '@ecs/shared/scene-graph'

import {replaceAssetInScene} from '../src/client/studio/replace-asset'

describe('replaceAssetInScene', () => {
  it('works for all types of assets', () => {
    const initialScene: SceneGraph = {
      objects: {
        object1: {
          id: 'object1',
          position: [0, 0, 0],
          rotation: [0, 0, 0, 1],
          scale: [1, 1, 1],
          material: {
            type: 'basic',
            color: '#FFFFFF',
            normalMap: {
              type: 'asset',
              asset: 'assets/old-path.png',
            },
          },
          ui: {
            font: {
              type: 'asset',
              asset: 'assets/old-path.png',
            },
          },
          components: {
            1: {
              id: '1',
              name: 'customComponent',
              parameters: {
                myAsset: 'assets/old-path.png',  // Assets in custom components are just strings
              },
            },
          },
        },
      },
      spaces: {
        space1: {
          id: 'space1',
          name: 'Space 1',
          sky: {
            type: 'image',
            src: {
              type: 'asset',
              asset: 'assets/old-path.png',
            },
          },
          reflections: {
            type: 'asset',
            asset: 'assets/old-path.png',
          },
        },
      },
    }

    const expectedScene: SceneGraph = {
      objects: {
        object1: {
          id: 'object1',
          position: [0, 0, 0],
          rotation: [0, 0, 0, 1],
          scale: [1, 1, 1],
          material: {
            type: 'basic',
            color: '#FFFFFF',
            normalMap: {
              type: 'asset',
              asset: 'assets/new-path.png',
            },
          },
          ui: {
            font: {
              type: 'asset',
              asset: 'assets/new-path.png',
            },
          },
          components: {
            1: {
              id: '1',
              name: 'customComponent',
              parameters: {
                myAsset: 'assets/new-path.png',
              },
            },
          },
        },
      },
      spaces: {
        space1: {
          id: 'space1',
          name: 'Space 1',
          sky: {
            type: 'image',
            src: {
              type: 'asset',
              asset: 'assets/new-path.png',
            },
          },
          reflections: {
            type: 'asset',
            asset: 'assets/new-path.png',
          },
        },
      },
    }

    const newScene = replaceAssetInScene(initialScene, 'assets/old-path.png', 'assets/new-path.png')
    assert.deepStrictEqual(newScene, expectedScene)
  })

  it('does not modify the scene if there are no matches', () => {
    const initialScene: SceneGraph = {
      objects: {
        object1: {
          id: 'object1',
          position: [0, 0, 0],
          rotation: [0, 0, 0, 1],
          scale: [1, 1, 1],
          material: {
            type: 'basic',
            color: '#FFFFFF',
            normalMap: {
              type: 'asset',
              asset: 'assets/old-path.png',
            },
          },
          components: {
            1: {
              id: '1',
              name: 'customComponent',
              parameters: {
                myAsset: 'assets/old-path.png',
              },
            },
          },
        },
      },
      spaces: {
        space1: {
          id: 'space1',
          name: 'Space 1',
          sky: {
            type: 'image',
            src: {
              type: 'asset',
              asset: 'assets/old-path.png',
            },
          },
          reflections: {
            type: 'asset',
            asset: 'assets/old-path.png',
          },
        },
      },
    }

    const newScene = replaceAssetInScene(
      initialScene,
      'assets/other-path.png',
      'assets/new-path.png'
    )
    assert.strictEqual(newScene, initialScene)
  })
})
