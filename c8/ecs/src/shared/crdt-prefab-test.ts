// @package(npm-ecs)
// @attr(externalize_npm = 1)
import {describe, it, assert} from '@repo/bzl/js/chai-js'
import type {DeepReadonly} from 'ts-essentials'

import {
  createEmptySceneDoc, loadSceneDoc,
} from './crdt'
import type {GraphObject, SceneGraph} from './scene-graph'

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

describe('crdt', () => {
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

    testMergeObjectMutation('Can merge instanceData.deletions', [
      [updateObject({instanceData: {instanceOf: 'object2', deletions: {}, children: {}}}), null],
      [
        updateObject({instanceData: {instanceOf: 'object2', deletions: {material: true}}}),
        updateObject({instanceData: {instanceOf: 'object2', deletions: {geometry: true}}}),
        objectState({
          instanceData: {
            instanceOf: 'object2',
            deletions: {material: true, geometry: true},
            children: {},
          },
        }),
      ],
    ])

    testMergeObjectMutation('Can merge separate instanceData.children', [
      [updateObject({instanceData: {instanceOf: 'object2', deletions: {}, children: {}}}), null],
      [
        updateObject({
          instanceData: {
            instanceOf: 'object2',
            deletions: {},
            children: {
              object3: {hidden: true},
            },
          },
        }),
        updateObject({
          instanceData: {
            instanceOf: 'object2',
            deletions: {},
            children: {
              object4: {hidden: true},
            },
          },
        }),
        objectState({
          instanceData: {
            instanceOf: 'object2',
            deletions: {},
            children: {
              object3: {hidden: true},
              object4: {hidden: true},
            },
          },
        }),
      ],
    ])
  }
})
