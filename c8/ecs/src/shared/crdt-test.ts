// @package(npm-ecs)
// @attr(externalize_npm = 1)
import {describe, it, assert} from '@nia/bzl/js/chai-js'
import type {DeepReadonly} from 'ts-essentials'

import * as Automerge from '@nia/c8/ecs/src/shared/automerge'

import {
  createEmptySceneDoc, loadSceneDoc, fixStringDuplication, Json, SceneDoc,
} from './crdt'
import type {
  BaseGraphObject,
  GraphComponent, GraphObject, Material, SceneGraph, Space,
} from './scene-graph'

const ACTOR_ID = '6666'

const object = (id: string, extra?: Partial<GraphObject>): GraphObject => ({
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

const space = (id: string, extra?: Partial<Space>): Space => ({
  id,
  name: id,
  ...extra,
})

const component = (id: string, parameters?: {}): GraphComponent => ({
  id,
  name: `${id}-name`,
  parameters: parameters || {},
})

describe('crdt', () => {
  it('Can create a wrapped document', () => {
    const doc = createEmptySceneDoc(ACTOR_ID)
    assert.deepEqual(doc.distill(), {
      objects: {},
    })
  })

  it('Can update activeCamera', () => {
    const doc = createEmptySceneDoc(ACTOR_ID)
    doc.update(prev => ({
      ...prev,
      activeCamera: 'camera1',
    }))
    assert.deepEqual(doc.distill(), {
      activeCamera: 'camera1',
      objects: {},
    })
    doc.update(prev => ({
      ...prev,
      activeCamera: 'camera2',
    }))
    assert.deepEqual(doc.distill(), {
      activeCamera: 'camera2',
      objects: {},
    })
  })

  it('Can set activeCamera to undefined', () => {
    const doc = createEmptySceneDoc(ACTOR_ID)
    doc.update(prev => ({
      ...prev,
      activeCamera: 'camera1',
    }))
    assert.deepEqual(doc.distill(), {
      activeCamera: 'camera1',
      objects: {},
    })
    doc.update(prev => ({
      ...prev,
      activeCamera: undefined,
    }))
    assert.deepEqual(doc.distill(), {
      objects: {},
    })
  })

  it('Can update entrySpaceId', () => {
    const doc = createEmptySceneDoc(ACTOR_ID)
    doc.update(prev => ({
      ...prev,
      entrySpaceId: 'space1',
    }))
    assert.deepEqual(doc.distill(), {
      entrySpaceId: 'space1',
      objects: {},
    })
    doc.update(prev => ({
      ...prev,
      entrySpaceId: 'camera2',
    }))
    assert.deepEqual(doc.distill(), {
      entrySpaceId: 'camera2',
      objects: {},
    })
  })

  it('Can set entrySpaceId to undefined', () => {
    const doc = createEmptySceneDoc(ACTOR_ID)
    doc.update(prev => ({
      ...prev,
      entrySpaceId: 'space1',
    }))
    assert.deepEqual(doc.distill(), {
      entrySpaceId: 'space1',
      objects: {},
    })
    doc.update(prev => ({
      ...prev,
      entrySpaceId: undefined,
    }))
    assert.deepEqual(doc.distill(), {
      objects: {},
    })
  })

  it('Can make update an object', () => {
    const doc1 = createEmptySceneDoc(ACTOR_ID)
    doc1.update(prev => ({
      ...prev,
      activeCamera: 'camera1',
      objects: {...prev.objects, object1: object('object1')},
    }))

    assert.deepEqual(doc1.distill(), {
      activeCamera: 'camera1',
      objects: {
        object1: object('object1'),
      },
    })

    doc1.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object1: {
          ...prev.objects.object1,
          name: 'name 1',
        },
      },
    }))

    assert.deepEqual(doc1.distill(), {
      activeCamera: 'camera1',
      objects: {
        object1: object('object1', {name: 'name 1'}),
      },
    })
  })

  it('Noop updates do not create a new doc', () => {
    const doc1 = createEmptySceneDoc(ACTOR_ID)
    const original = doc1.raw()
    doc1.update(prev => prev)
    assert.strictEqual(doc1.raw(), original)
    assert.lengthOf(doc1.getChanges(original), 0)
  })

  it('Exact replacement does not create a new doc', () => {
    const doc1 = createEmptySceneDoc(ACTOR_ID)

    const completeObject: Required<BaseGraphObject> = {
      id: 'object1',
      name: '<unset>',
      hidden: false,
      disabled: true,
      prefab: true,
      ephemeral: false,
      parentId: 'other',
      persistent: true,
      order: 1,
      position: [0, 0, 0],
      rotation: [0, 0, 0, 1],
      scale: [1, 1, 1],
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
      material: {type: 'basic', color: '#ffffff'},
      gltfModel: {src: {type: 'url', url: '/example.gltf'}},
      collider: {mass: 3},
      audio: {src: {type: 'url', url: '/example.mp3'}},
      videoControls: {
      },
      ui: {image: {type: 'url', url: '/example.png'}},
      shadow: {receiveShadow: false},
      light: {type: 'directional', color: '#ffffff'},
      camera: {type: 'perspective'},
      components: {
        component1: {
          id: 'component1',
          name: 'component1-name',
          parameters: {valueA: 1, src: {type: 'entity', id: 'object2'}},
        },
      },
      splat: {
        src: {type: 'url', url: '/example.spz'},
      },
      face: {
        id: 3,
        addAttachmentState: false,
      },
      map: {
        latitude: 90,
        radius: 30,
        longitude: 180,
        spawnLocations: true,
        useGps: true,
      },
      mapTheme: {
      },
      mapPoint: {
        latitude: 90,
        longitude: 180,
        minScale: 3,
        meters: 3,
      },
      location: {
        name: 'something',
        poiId: 'poi',
        lat: 0,
        lng: 0,
        title: '',
        anchorNodeId: '',
        anchorSpaceId: '',
        imageUrl: '',
        anchorPayload: '',
      },
      imageTarget: {
        name: 'something',
      },
    }

    const completeSpace: Required<Space> = {
      id: 'space1',
      name: 'space1',
      activeCamera: 'camera1',
      reflections: {type: 'asset', asset: 'file.png'},
      sky: {type: 'none'},
      fog: {type: 'none'},
      includedSpaces: [],
    }

    const scene: Required<SceneGraph> = {
      activeCamera: 'camera1',
      entrySpaceId: 'space1',
      reflections: {type: 'asset', asset: 'file.png'},
      sky: {type: 'none'},
      inputs: {'map1': []},
      activeMap: 'map1',
      objects: {
        object1: completeObject,
      },
      spaces: {
        space1: completeSpace,
      },
      runtimeVersion: {
        type: 'version',
        level: 'patch',
        major: 1,
        minor: 0,
        patch: 0,
      },
    }

    doc1.update(() => scene)
    const original = doc1.raw()
    const distilled = doc1.distill()
    assert.deepStrictEqual(original, distilled)
    assert.deepStrictEqual(distilled, scene)

    doc1.update(() => scene)
    assert.lengthOf(doc1.getChanges(original), 0)
    assert.strictEqual(doc1.raw(), original)
  })

  it('Can update both activeCamera and an object', () => {
    const doc1 = createEmptySceneDoc(ACTOR_ID)
    doc1.update(prev => ({
      ...prev,
      activeCamera: 'camera1',
      objects: {...prev.objects, object1: object('object1')},
    }))

    doc1.update(prev => ({
      ...prev,
      activeCamera: 'camera2',
      objects: {
        ...prev.objects,
        object1: {
          ...prev.objects.object1,
          disabled: true,
        },
      },
    }))

    assert.deepEqual(doc1.distill(), {
      activeCamera: 'camera2',
      objects: {
        object1: object('object1', {disabled: true}),
      },
    })
  })

  it('Can merge updates to the same object', () => {
    const doc1 = createEmptySceneDoc(ACTOR_ID)
    doc1.update(prev => ({
      ...prev,
      objects: {...prev.objects, object1: object('object1')},
    }))

    const doc2 = doc1.clone('')

    doc1.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object1: {
          ...prev.objects.object1,
          name: 'name 1',
          material: {type: 'basic', color: '#ffffff'},
        },
      },
    }))

    doc2.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object1: {
          ...prev.objects.object1,
          geometry: {type: 'sphere', radius: 1},
        },
      },
    }))

    assert.deepEqual(doc1.distill(), {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          material: {type: 'basic', color: '#ffffff'},
        }),
      },
    })

    assert.deepEqual(doc2.distill(), {
      objects: {
        object1: object('object1', {
          geometry: {type: 'sphere', radius: 1},
        }),
      },
    })

    doc1.merge(doc2)

    assert.deepEqual(doc1.distill(), {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          material: {type: 'basic', color: '#ffffff'},
          geometry: {type: 'sphere', radius: 1},
        }),
      },
    })
  })

  it('Can add a space', () => {
    const doc1 = createEmptySceneDoc(ACTOR_ID)
    doc1.update(prev => ({
      ...prev,
      objects: {...prev.objects, object1: object('object1')},
      spaces: {space1: space('space1')},
    }))
    assert.deepEqual(doc1.distill(), {
      objects: {
        object1: object('object1'),
      },
      spaces: {
        space1: space('space1'),
      },
    })
  })

  it('Can modify a space', () => {
    const doc1 = createEmptySceneDoc(ACTOR_ID)
    doc1.update(prev => ({
      ...prev,
      objects: {...prev.objects, object1: object('object1')},
      spaces: {space1: space('space1')},
    }))

    doc1.update(prev => ({
      ...prev,
      spaces: {
        ...prev.spaces,
        space1: {
          ...prev.spaces!.space1,
          name: 'new name',
        },
      },
    }))

    assert.deepEqual(doc1.distill(), {
      objects: {
        object1: object('object1'),
      },
      spaces: {
        space1: space('space1', {name: 'new name'}),
      },
    })
  })

  it('Can update active camera and sky in space', () => {
    const doc1 = createEmptySceneDoc(ACTOR_ID)
    doc1.update(prev => ({
      ...prev,
      objects: {...prev.objects, camera1: object('camera1')},
      spaces: {space1: space('space1')},
    }))

    // update active camera in space
    doc1.update(prev => ({
      ...prev,
      spaces: {
        ...prev.spaces,
        space1: {
          ...prev.spaces!.space1,
          activeCamera: 'camera1',
        },
      },
    }))

    assert.deepEqual(doc1.distill(), {
      objects: {
        camera1: object('camera1'),
      },
      spaces: {
        space1: space('space1', {activeCamera: 'camera1'}),
      },
    })

    // update sky in space
    doc1.update(prev => ({
      ...prev,
      spaces: {
        ...prev.spaces,
        space1: {
          ...prev.spaces!.space1,
          sky: {type: 'color', color: '#000000'},
        },
      },
    }))

    assert.deepEqual(doc1.distill(), {
      objects: {
        camera1: object('camera1'),
      },
      spaces: {
        space1: space('space1', {
          activeCamera: 'camera1',
          sky: {type: 'color', color: '#000000'},
        }),
      },
    })

    // remove active camera in space
    doc1.update(prev => ({
      ...prev,
      spaces: {
        ...prev.spaces,
        space1: {
          ...prev.spaces!.space1,
          activeCamera: undefined,
        },
      },
    }))

    assert.deepEqual(doc1.distill(), {
      objects: {
        camera1: object('camera1'),
      },
      spaces: {
        space1: space('space1', {
          sky: {type: 'color', color: '#000000'},
        }),
      },
    })

    // remove sky in space
    doc1.update(prev => ({
      ...prev,
      spaces: {
        ...prev.spaces,
        space1: space('space1'),
      },
    }))

    assert.deepEqual(doc1.distill(), {
      objects: {
        camera1: object('camera1'),
      },
      spaces: {
        space1: space('space1'),
      },
    })
  })

  it('Can update reflections in a spaceless scene', () => {
    const doc1 = createEmptySceneDoc(ACTOR_ID)
    doc1.update(prev => ({
      ...prev,
    }))

    // update reflections
    doc1.update(prev => ({
      ...prev,
      reflections: {type: 'asset', asset: 'file.png'},
    }))

    assert.deepEqual(doc1.distill(), {
      objects: {},
      reflections: {type: 'asset', asset: 'file.png'},
    })

    // remove reflections
    doc1.update(prev => ({
      ...prev,
      reflections: undefined,
    }))

    // can delete reflections
    assert.deepEqual(doc1.distill(), {
      objects: {},
    })
  })

  it('Can update reflections in a space', () => {
    const doc1 = createEmptySceneDoc(ACTOR_ID)
    doc1.update(prev => ({
      ...prev,
      spaces: {space1: space('space1')},
    }))

    // update reflections in space
    doc1.update(prev => ({
      ...prev,
      spaces: {
        ...prev.spaces,
        space1: {
          ...prev.spaces!.space1,
          reflections: {type: 'asset', asset: 'file.png'},
        },
      },
    }))

    assert.deepEqual(doc1.distill(), {
      objects: {},
      spaces: {
        space1: space('space1', {
          reflections: {type: 'asset', asset: 'file.png'},
        }),
      },
    })

    // remove reflections
    doc1.update(prev => ({
      ...prev,
      spaces: {
        ...prev.spaces,
        space1: {
          ...prev.spaces!.space1,
          reflections: undefined,
        },
      },
    }))

    // can delete reflections
    assert.deepEqual(doc1.distill(), {
      objects: {},
      spaces: {
        space1: space('space1'),
      },
    })
  })

  it('Can delete a space', () => {
    const doc1 = createEmptySceneDoc(ACTOR_ID)
    doc1.update(prev => ({
      ...prev,
      objects: {...prev.objects, object1: object('object1')},
      spaces: {space1: space('space1')},
    }))

    doc1.update(prev => ({
      ...prev,
      spaces: {},
    }))

    assert.deepEqual(doc1.distill(), {
      objects: {
        object1: object('object1'),
      },
      spaces: {},
    })
  })

  it('Can delete all spaces', () => {
    const doc1 = createEmptySceneDoc(ACTOR_ID)
    doc1.update(prev => ({
      ...prev,
      objects: {...prev.objects, object1: object('object1')},
      spaces: {space1: space('space1')},
    }))

    doc1.update(prev => ({
      ...prev,
      spaces: undefined,
    }))

    assert.deepEqual(doc1.distill(), {
      objects: {
        object1: object('object1'),
      },
    })
  })

  it('Can add a second space', () => {
    const doc1 = createEmptySceneDoc(ACTOR_ID)
    doc1.update(prev => ({
      ...prev,
      objects: {...prev.objects, object1: object('object1')},
      spaces: {space1: space('space1')},
    }))

    doc1.update(prev => ({
      ...prev,
      spaces: {
        ...prev.spaces,
        space2: space('space2'),
      },
    }))

    assert.deepEqual(doc1.distill(), {
      objects: {
        object1: object('object1'),
      },
      spaces: {
        space1: space('space1'),
        space2: space('space2'),
      },
    })
  })

  it('Can update included spaces', () => {
    const doc1 = createEmptySceneDoc(ACTOR_ID)
    doc1.update(prev => ({
      ...prev,
      objects: {...prev.objects, object1: object('object1')},
      spaces: {space1: space('space1'), space2: space('space2')},
    }))

    // include space2 in space1
    doc1.update(prev => ({
      ...prev,
      spaces: {
        ...prev.spaces,
        space1: space('space1', {includedSpaces: ['space2']}),
      },
    }))

    assert.deepEqual(doc1.distill(), {
      objects: {
        object1: object('object1'),
      },
      spaces: {
        space1: space('space1', {includedSpaces: ['space2']}),
        space2: space('space2'),
      },
    })

    // remove space2 from included spaces
    doc1.update(prev => ({
      ...prev,
      spaces: {
        ...prev.spaces,
        space1: space('space1'),
      },
    }))

    assert.deepEqual(doc1.distill(), {
      objects: {
        object1: object('object1'),
      },
      spaces: {
        space1: space('space1'),
        space2: space('space2'),
      },
    })
  })

  type Mutation = (state: DeepReadonly<SceneGraph>) => DeepReadonly<SceneGraph>
  type FinalState = DeepReadonly<SceneGraph>
  type DualMutation = (
    [Mutation | null, Mutation | null] |
    [Mutation | null, Mutation | null, FinalState]
  )

  const testMergeObjectMutation = (
    label: string,
    mutations: DualMutation[]
  ) => {
    it(label, () => {
      const doc1 = createEmptySceneDoc(ACTOR_ID)
      doc1.update(prev => ({
        ...prev,
        objects: {...prev.objects, object1: object('object1')},
      }))

      // Using a different actor ID as a tie breaker to avoid nondeterministic behavior
      const doc2 = loadSceneDoc(doc1.save(), `${ACTOR_ID}22`)

      mutations.forEach(([mut1, mut2, final], i) => {
        const count = `#${i + 1}`
        const originalDoc1 = doc1.raw()
        const originalDoc2 = doc2.raw()
        if (mut1) {
          doc1.update(mut1)
        }
        if (mut2) {
          doc2.update(mut2)
        }

        doc2.applyChanges(doc1.getChanges(originalDoc1))
        doc1.applyChanges(doc2.getChanges(originalDoc2))

        if (final) {
          assert.deepStrictEqual(doc1.distill(), final, `doc1 should match final ${count}`)
          assert.deepStrictEqual(doc2.distill(), final, `doc2 should match final ${count}`)
        } else {
          assert.deepStrictEqual(
            doc1.distill(),
            doc2.distill(),
            `doc1 and doc2 should be equal ${count}`
          )
        }
      })
    })
  }

  {
    type ObjectUpdate = (
      ((object: DeepReadonly<GraphObject>) => DeepReadonly<GraphObject>)
     | Partial<GraphObject>
    )

    const updateObject = (update: ObjectUpdate): Mutation => prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object1: {
          ...prev.objects.object1,
          ...(typeof update === 'function' ? update(prev.objects.object1) : update),
        },
      },
    })

    const objectState = (extra?: Partial<GraphObject>): FinalState => ({
      objects: {
        object1: object('object1', extra),
      },
    })

    testMergeObjectMutation('Empty state', [
      [null, null, objectState()],
    ])

    testMergeObjectMutation('Can merge hidden', [
      [
        updateObject({hidden: true}),
        updateObject({hidden: false}),
        objectState({hidden: false}),
      ],
    ])

    testMergeObjectMutation('Can merge prefab', [
      [
        updateObject({prefab: true}),
        updateObject({prefab: undefined}),
        objectState({prefab: true}),
      ],
    ])

    testMergeObjectMutation('Can merge instanceData', [
      [
        updateObject({instanceData: {instanceOf: 'object2', deletions: {}}}),
        updateObject({instanceData: {instanceOf: 'object3', deletions: {geometry: true}}}),
        objectState({instanceData: {instanceOf: 'object3', deletions: {geometry: true}}}),
      ],
    ])

    testMergeObjectMutation('Can merge name', [
      [updateObject({name: 'my-name'}), null],
      [
        updateObject({name: 'my-name2'}),
        updateObject({name: 'my-name3'}),
        objectState({name: 'my-name3'}),
      ],
    ])
    testMergeObjectMutation('Can merge edits make to different sides of name', [
      [updateObject({name: 'my-name'}), null],
      [
        updateObject({name: '2my-name'}),
        updateObject({name: 'my-name3'}),
        objectState({name: 'my-name3'}),
      ],
    ])

    const fullMaterial: Material = {
      type: 'basic',
      color: 'red',
      textureSrc: {type: 'asset', asset: 'file.png'},
    }

    testMergeObjectMutation('Can merge material updates', [
      [
        updateObject(o => ({...o, material: {type: 'basic', color: 'yellow'}})),
        updateObject({material: {type: 'basic', color: 'red'}}),
        objectState({material: {type: 'basic', color: 'red'}}),
      ],
      [
        updateObject({material: fullMaterial}),
        null,
        objectState({material: fullMaterial}),
      ],
    ])

    testMergeObjectMutation('Can merge collider updates', [
      [
        updateObject({collider: {mass: 10}}),
        null,
        objectState({collider: {mass: 10}}),
      ],
      [
        null,
        updateObject({collider: {friction: 12}}),
        objectState({collider: {friction: 12}}),
      ],
      [
        updateObject({collider: {mass: 45, friction: 12}}),
        updateObject({collider: {friction: 15}}),
        objectState({collider: {mass: 45, friction: 15}}),
      ],
    ])

    testMergeObjectMutation('Can merge component updates', [
      [
        updateObject({components: {component1: component('component1')}}),
        null],
      [
        updateObject({components: {component1: component('component1', {valueA: 12})}}),
        updateObject({components: {component1: component('component1', {valueB: 42})}}),
        objectState({components: {component1: component('component1', {valueA: 12, valueB: 42})}}),
      ],
    ])

    testMergeObjectMutation('Can merge update to arrays of different sizes', [
      [
        updateObject({components: {component1: component('component1', {array: [1, 2, 3]})}}),
        null],
      [
        updateObject({components: {component1: component('component1', {array: [1, 2, 3, 4]})}}),
        null,
        objectState({components: {component1: component('component1', {array: [1, 2, 3, 4]})}}),
      ],
      [
        updateObject({components: {component1: component('component1', {array: [1, 2]})}}),
        null,
        objectState({components: {component1: component('component1', {array: [1, 2]})}}),
      ],
    ])
  }

  it('Can set a parentId then back to undefined', () => {
    const doc = createEmptySceneDoc(ACTOR_ID)

    doc.update(d => ({
      ...d,
      activeCamera: 'object1',
      objects: {
        object1: object('object1'),
        object2: object('object2'),
      },
    }))

    doc.update(d => ({
      ...d,
      objects: {
        ...d.objects,
        object2: {...d.objects.object2, parentId: 'object1'},
      },
    }))

    assert.deepEqual(doc.distill(), {
      activeCamera: 'object1',
      objects: {
        object1: object('object1'),
        object2: object('object2', {parentId: 'object1'}),
      },
    })

    doc.update(d => ({
      ...d,
      objects: {
        ...d.objects,
        object2: {...d.objects.object2, parentId: undefined},
      },
    }))

    assert.deepEqual(doc.distill(), {
      activeCamera: 'object1',
      objects: {
        object1: object('object1'),
        object2: object('object2'),
      },
    })
  })

  it('Can restore a saved doc', () => {
    const doc1 = createEmptySceneDoc(ACTOR_ID)
    doc1.update(prev => ({
      ...prev,
      activeCamera: 'camera1',
      objects: {...prev.objects, object1: object('object1')},
    }))

    const doc2 = loadSceneDoc(doc1.save(), ACTOR_ID)

    assert.deepEqual(doc1.distill(), doc2.distill())
  })

  it('Can export and apply changes between docs', () => {
    const doc1 = createEmptySceneDoc(ACTOR_ID)
    doc1.update(prev => ({
      ...prev,
      objects: {...prev.objects, object1: object('object1')},
    }))

    const doc2 = doc1.clone('')

    doc1.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object1: {
          ...prev.objects.object1,
          name: 'name 1',
          material: {type: 'basic', color: '#ffffff'},
        },
      },
    }))

    const diff = doc1.getChanges(doc2.raw())

    doc2.applyChanges(diff)

    assert.deepEqual(doc1.distill(), doc2.distill())
  })

  it('Does not connect the document to live object references', () => {
    const doc = createEmptySceneDoc(ACTOR_ID)
    const myObject = object('object1')
    doc.update(prev => ({
      ...prev,
      objects: {...prev.objects, object1: myObject},
    }))

    myObject.name = 'changed direct'

    assert.strictEqual(doc.distill().objects.object1.name, '<unset>')

    doc.rawUpdate((d) => {
      d.objects.object1.name = 'changed in rawUpdate'
    })

    assert.strictEqual(doc.distill().objects.object1.name, 'changed in rawUpdate')
    assert.strictEqual(myObject.name, 'changed direct')
  })

  it('Can return version ID for a document', () => {
    const doc = createEmptySceneDoc(ACTOR_ID)
    assert.strictEqual(
      '9343576c711a4bfa57c75338c3c1becc21d70ac5d9775c23e5a4cba8838fa113',
      doc.getVersionId()
    )

    doc.update(prev => ({
      ...prev,
      activeCamera: 'camera1',
      objects: {...prev.objects, object1: object('object1')},
    }))

    assert.strictEqual(
      '3ccf9c7cf30e064c32965a2335f5f51b42d60fa6bc41bed572fa343a8a1095ea',
      doc.getVersionId()
    )
  })

  it('Can reset back to an earlier version', () => {
    const doc = createEmptySceneDoc(ACTOR_ID)
    doc.update(prev => ({
      ...prev,
      activeCamera: 'camera1',
      objects: {...prev.objects, object1: object('object1', {name: 'initial'})},
    }))

    const initialVersion = doc.getVersionId()

    doc.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object1: {
          ...prev.objects.object1,
          name: 'modified',
        },
      },
    }))

    assert.strictEqual(doc.distill().objects.object1.name, 'modified')
    const revertedDoc = doc.clone('')
    revertedDoc.resetToVersionId(initialVersion)
    assert.strictEqual(revertedDoc.distill().objects.object1.name, 'initial')
    revertedDoc.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object1: {
          ...prev.objects.object1,
          name: 'modified again',
        },
      },
    }))
    assert.strictEqual(revertedDoc.distill().objects.object1.name, 'modified again')
  })

  it('creates new references only if objects were mutated', () => {
    const doc = createEmptySceneDoc(ACTOR_ID)
    doc.update(prev => ({
      ...prev,
      objects: {...prev.objects, object1: object('object1'), object2: object('object2')},
    }))

    const object1Before = doc.raw().objects.object1
    const object2Before = doc.raw().objects.object2

    doc.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object1: {
          ...prev.objects.object1,
          name: 'name 1',
        },
      },
    }))

    const object1After = doc.raw().objects.object1
    const object2After = doc.raw().objects.object2

    assert.notStrictEqual(object1Before, object1After)
    assert.strictEqual(object2Before, object2After)
  })

  it('Strips undefined from object property updates', () => {
    const doc1 = createEmptySceneDoc(ACTOR_ID)
    const doc2 = doc1.clone('')

    const object1 = object('object1')

    doc1.update(prev => ({
      ...prev,
      activeCamera: 'camera1',
      objects: {
        ...prev.objects,
        object1: {
          ...object1,
          shadow: undefined,
          material: {type: 'basic', color: '#ffffff', roughness: undefined},
        },
      },
    }))

    assert.deepEqual(doc1.distill().objects, {
      object1: {
        ...object1,
        material: {type: 'basic', color: '#ffffff'},
      },
    })

    doc1.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object2: {
          ...object('object2'),
          audio: null,
        },
      },
    }))

    doc2.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object2: {
          ...object('object2'),
          audio: undefined,
        },
      },
    }))

    assert.equal(doc1.raw().objects.object2.audio, null)
    assert.equal(doc2.raw().objects.object2.audio, undefined)

    assert.isTrue(Object.keys(doc1.raw().objects.object2).includes('audio'))
    assert.isFalse(Object.keys(doc2.raw().objects.object2).includes('audio'))
  })

  it('Strips undefined from component property updates', () => {
    const doc = createEmptySceneDoc(ACTOR_ID)

    const object1 = object('object1')
    const component1 = component('component1', {valueA: 1, valueB: undefined})

    doc.update(prev => ({
      ...prev,
      activeCamera: 'camera1',
      objects: {
        ...prev.objects,
        object1: {
          ...object1,
          components: {'component1': component1},
        },
      },
    }))

    assert.isFalse(Object.keys(doc.raw().objects.object1.components.component1).includes('valueB'))
  })

  it('can skip transform updates', () => {
    const doc = createEmptySceneDoc(ACTOR_ID)
    doc.update(prev => ({
      ...prev,
      objects: {...prev.objects, object1: object('object1', {position: [1, 2, 3]})},
    }))

    doc.updateWithoutTransform(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object1: {
          ...prev.objects.object1,
          position: [4, 5, 6],
        },
      },
    }))

    assert.deepEqual(doc.distill().objects.object1, object('object1', {position: [1, 2, 3]}))
  })

  it('Can resolve to good state after bad merge wth compacted doc', () => {
    const originalDoc = createEmptySceneDoc(ACTOR_ID)
    originalDoc.update(prev => ({
      ...prev,
      objects: {...prev.objects, object1: object('object1')},
    }))

    const doc1 = originalDoc.clone('')
    const doc2 = originalDoc.clone('')

    // add object3 to doc2
    doc2.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object3: object('object3'),
      },
    }))

    // update object1 in doc1
    doc1.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object1: {
          ...prev.objects.object1,
          name: 'name 1',
          material: {type: 'basic', color: '#ffffff'},
        },
      },
    }))

    // add object2 to doc1
    doc1.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object2: object('object2'),
      },
    }))

    // update object2 in doc1
    doc1.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object2: {
          ...prev.objects.object2,
          name: 'name 2',
        },
      },
    }))

    const finalState = {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          material: {type: 'basic', color: '#ffffff'},
        }),
        object2: object('object2', {name: 'name 2'}),
        object3: object('object3'),
      },
    }

    // can merge before compacting
    const merged = doc2.clone('')
    merged.merge(doc1)
    assert.deepEqual(merged.distill(), finalState)

    // doc1: can merge original doc with compacted doc
    const compacted1 = originalDoc.clone('')
    compacted1.update(() => doc1.distill())

    // doc2: can merge original doc with compacted doc
    const compacted2 = originalDoc.clone('')
    compacted2.update(() => doc2.distill())

    // BAD MERGE
    compacted2.merge(merged)
    compacted1.merge(compacted2)  // would have duplicated strings without fixStringDuplication

    assert.deepEqual(compacted1.raw(), finalState)
    assert.deepEqual(compacted1.distill(), finalState)
    assert.deepEqual(compacted2.raw(), finalState)
    assert.deepEqual(compacted2.distill(), finalState)

    // can keep making changes after merge
    compacted1.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object1: {
          ...prev.objects.object1,
          name: 'name 1',
          material: {type: 'basic', color: '#000000'},
        },
      },
    }))

    const finalStateAfterChange = {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          material: {type: 'basic', color: '#000000'},
        }),
        object2: object('object2', {name: 'name 2'}),
        object3: object('object3'),
      },
    }

    assert.deepEqual(compacted1.distill(), finalStateAfterChange)
  })

  it('Can sync and merge from remote with local changes', () => {
    const GIT_ACTION_ACTOR_ID = 'cc'
    const forkPoint = createEmptySceneDoc('')
    const defaultScene = {
      box: object('box1', {geometry: {type: 'box', width: 1, height: 1, depth: 1}}),
      light1: object('light1', {name: 'ambient light', light: {type: 'ambient'}}),
      light2: object('light2', {name: 'directional light', light: {type: 'directional'}}),
    }

    let remote = forkPoint.save()
    const main = forkPoint.clone('')
    const client1 = forkPoint.clone('')
    const client2 = forkPoint.clone('')

    // add default scene to main
    main.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        ...defaultScene,
      },
    }))

    // add object2 to main
    main.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object2: object('object2'),
      },
    }))

    // update object2 in main
    main.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object2: {
          ...prev.objects.object2,
          name: 'name 2',
          material: {type: 'basic', color: '#111111'},
        },
      },
    }))

    // main landed changes
    const mainRemote = forkPoint.clone('')
    mainRemote.update(() => main.distill())
    remote = mainRemote.save()

    // add default scene to client1
    client1.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        ...defaultScene,
      },
    }))

    // add object3 to client1
    client1.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object3: object('object3'),
      },
    }))

    // save client 1 changes to repo
    const client1Remote = forkPoint.clone('')
    client1Remote.update(() => client1.distill())

    // client1 syncs with main
    const client1Merged = loadSceneDoc(client1Remote.save(), GIT_ACTION_ACTOR_ID)
    client1Merged.merge(loadSceneDoc(remote, GIT_ACTION_ACTOR_ID))

    const client1Expected = {
      objects: {
        ...defaultScene,
        object2: object('object2', {
          name: 'name 2',
          material: {type: 'basic', color: '#111111'},
        }),
        object3: object('object3'),
      },
    }
    assert.deepEqual(client1Merged.distill(), client1Expected)

    // client1 landed changes
    remote = client1Merged.save()

    // add default scene to client2
    client2.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        ...defaultScene,
      },
    }))

    // client2 add new changes
    client2.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object4: object('object4'),
      },
    }))

    // save client 2 changes to repo
    const client2Remote = forkPoint.clone('')
    client2Remote.update(() => client2.distill())

    // client2 syncs with remote
    const client2Merged = loadSceneDoc(client2Remote.save(), GIT_ACTION_ACTOR_ID)
    client2Merged.merge(loadSceneDoc(remote, GIT_ACTION_ACTOR_ID))

    const client2Expected = {
      objects: {
        ...defaultScene,
        object2: object('object2', {
          name: 'name 2',
          material: {type: 'basic', color: '#111111'},
        }),
        object3: object('object3'),
        object4: object('object4'),
      },
    }

    assert.deepEqual(client2Merged.distill(), client2Expected)
  })

  const getActorId = (doc: SceneDoc) => Automerge.getActorId(doc.raw())
  it('Can create scene doc with random actor ID', () => {
    const emptyDocWithActorId = createEmptySceneDoc(ACTOR_ID)
    const emptyDocWithEmptyActorId = createEmptySceneDoc('')

    const loadedDocWithEmptyActorId1 = loadSceneDoc(emptyDocWithActorId.save(), '')
    const loadedDocWithEmptyActorId2 = loadSceneDoc(emptyDocWithActorId.save(), '')
    const loadedDocWithValidActorId = loadSceneDoc(emptyDocWithActorId.save(), '1111')

    assert.equal(getActorId(emptyDocWithActorId), ACTOR_ID)
    assert.equal(getActorId(loadedDocWithValidActorId), '1111')

    assert.notEqual(getActorId(emptyDocWithEmptyActorId), '')
    assert.notEqual(getActorId(loadedDocWithEmptyActorId1), '')
    assert.notEqual(getActorId(loadedDocWithEmptyActorId1), getActorId(loadedDocWithEmptyActorId2))
  })

  it('Can merge scene doc with random actor ID', () => {
    const doc = createEmptySceneDoc('')
    doc.update(prev => ({
      ...prev,
      objects: {...prev.objects, object1: object('object1')},
    }))

    const yours = loadSceneDoc(doc.save(), '')
    const theirs = loadSceneDoc(doc.save(), '')
    assert.notEqual(getActorId(yours), getActorId(theirs))
    assert.deepEqual(yours.distill(), theirs.distill())

    yours.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object1: {
          ...prev.objects.object1,
          name: 'name 1',
          material: {type: 'basic', color: 'red'},
        },
      },
    }))

    theirs.update(prev => ({
      ...prev,
      objects: {
        ...prev.objects,
        object2: object('object2', {name: 'name 2'}),
      },
    }))
    yours.merge(theirs)

    const mergedScene = {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          material: {type: 'basic', color: 'red'},
        }),
        object2: object('object2', {name: 'name 2'}),
      },
    }
    assert.deepEqual(yours.distill(), mergedScene)

    doc.merge(yours)
    assert.deepEqual(doc.distill(), mergedScene)
  })
})

describe('Deduplicate strings after bad merge', () => {
  it('can dedupe strings in scenes with same objects', () => {
    const yours = {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          material: {type: 'basic', color: '#ffffff'},
        }),
        object2: object('object2', {name: 'name 2'}),
        object3: object('object3', {name: 'name 3'}),
      },
    } as any as Json

    const theirs = {
      objects: {
        object1: object('object1', {
          name: 'name 1',
        }),
        object2: object('object2', {name: 'name 2', position: [2, 2, 2]}),
        object3: object('object3'),
      },
    } as any as Json

    const badMerged = {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          material: {type: 'basic', color: '#ffffff#ffffff'},
        }),
        object2: object('object2', {name: 'name 2name 2', position: [2, 2, 2]}),
        object3: object('object3', {name: 'name 3name 3'}),
      },
    } as any as Json

    const fixed = fixStringDuplication(badMerged, yours, theirs)

    const finalState = {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          material: {type: 'basic', color: '#ffffff'},
        }),
        object2: object('object2', {name: 'name 2', position: [2, 2, 2]}),
        object3: object('object3', {name: 'name 3'}),
      },
    }

    assert.deepEqual(fixed, finalState)
  })

  it('can dedupe strings in scenes with different objects', () => {
    const yours = {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          material: {type: 'basic', color: '#ffffff'},
        }),
        object2: object('object2', {name: 'name 2', position: [2, 2, 2]}),
      },
    } as any as Json

    const theirs = {
      objects: {
        object1: object('object1', {
          name: 'name 1',
        }),
        object3: object('object3', {name: 'name 3'}),
      },
    } as any as Json

    const badMerged = {
      objects: {
        object1: object('object1', {
          name: 'name 1name 1',
          material: {type: 'basic', color: '#ffffff#ffffff'},
        }),
        object2: object('object2object2', {name: 'name 2name 2', position: [2, 2, 2]}),
        object3: object('object3object3', {name: 'name 3name 3'}),
      },
    } as any as Json

    const fixed = fixStringDuplication(badMerged, yours, theirs)

    const finalState = {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          material: {type: 'basic', color: '#ffffff'},
        }),
        object2: object('object2', {name: 'name 2', position: [2, 2, 2]}),
        object3: object('object3', {name: 'name 3'}),
      },
    }

    assert.deepEqual(fixed, finalState)
  })

  it('can dedupe strings when one side has no objects', () => {
    const yours = {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          material: {type: 'basic', color: '#ffffff'},
        }),
        object2: object('object2', {name: 'name 2', position: [2, 2, 2]}),
      },
    }

    const theirs = {
      objects: {},
    }
    const badMerged = {
      objects: {
        object1: object('object1', {
          name: 'name 1name 1',
          material: {type: 'basic', color: '#ffffff#ffffff'},
        }),
        object2: object('object2object2', {name: 'name 2name 2', position: [2, 2, 2]}),
      },
    }

    const fixed = fixStringDuplication(badMerged, yours, theirs)

    const finalState = {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          material: {type: 'basic', color: '#ffffff'},
        }),
        object2: object('object2', {name: 'name 2', position: [2, 2, 2]}),
      },
    }

    assert.deepEqual(fixed, finalState)
  })

  it('can fix bad string merges', () => {
    const yours = {
      id: 'object1',
      name: 'myBox',
      type: 'box',
      color: 'red',
      url: 'https://example.com',
      properties: ['aaa', 'bbbb'],
    } as any as Json

    const theirs = {
      id: 'object1',
      name: 'sphere1',
      type: 'sphere',
      color: 'white',
      material: 'basic',
      properties: ['cc'],
    } as any as Json

    const badMerged = {
      id: 'object1object1',
      name: 'myBoxsphere1',
      type: 'sphere',
      color: 'redite',
      url: 'https://example.comhttps://example.com',
      material: 'basicbasic',
      properties: ['aaacc', 'bbbb'],
    } as any as Json

    const fixed = fixStringDuplication(badMerged, yours, theirs)

    assert.equal(fixed.id, 'object1')
    assert.ok(fixed.name === 'myBox' || fixed.name === 'sphere1')
    assert.equal(fixed.type, 'sphere')
    assert.ok(fixed.color === 'red' || fixed.color === 'white')
    assert.equal(fixed.url, 'https://example.com')
    assert.equal(fixed.material, 'basic')
    assert.includeMembers(fixed.properties, ['bbbb'])
    assert.ok(fixed.properties.includes('aaa') || fixed.properties.includes('cc'))
  })
})
