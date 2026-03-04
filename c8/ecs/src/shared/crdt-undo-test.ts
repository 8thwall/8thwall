// @package(npm-ecs)
// @attr(externalize_npm = 1)
import {describe, it, assert} from '@nia/bzl/js/chai-js'
import type {DeepReadonly} from 'ts-essentials'

import {createEmptySceneDoc, loadSceneDoc} from './crdt'
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

const addObject = (
  prev: DeepReadonly<SceneGraph>, id: string, extra?: Partial<GraphObject>
): DeepReadonly<SceneGraph> => ({
  ...prev,
  objects: {
    ...prev.objects,
    [id]: object(id, extra),
  },
})

const updateObject = (
  prev: DeepReadonly<SceneGraph>, id: string, extra: Partial<GraphObject>
): DeepReadonly<SceneGraph> => ({
  ...prev,
  objects: {
    ...prev.objects,
    [id]: {
      ...prev.objects[id],
      ...extra,
    },
  },
})

describe('crdt-undo', () => {
  it('can undo and redo (one action)', () => {
    const doc = createEmptySceneDoc(ACTOR_ID)
    doc.update(prev => addObject(prev, 'object1'))
    doc.update(prev => addObject(prev, 'object2'))

    // user edit
    const beforeNameUpdateId = doc.getVersionId()
    doc.update(prev => updateObject(prev, 'object1', {name: 'name 1'}))
    const afterNameUpdateId = doc.getVersionId()

    // runtime updates
    doc.update(prev => updateObject(prev, 'object2', {position: [2, 2, 2]}))

    assert.deepEqual(doc.distill().objects, {
      object1: object('object1', {name: 'name 1'}),
      object2: object('object2', {position: [2, 2, 2]}),
    })

    const beforeNameUpdateView = doc.getView(beforeNameUpdateId)
    const afterNameUpdateView = doc.getView(afterNameUpdateId)

    // undo naming object 1
    doc.updateAt([afterNameUpdateId], () => beforeNameUpdateView)
    assert.deepEqual(doc.distill(), {
      objects: {
        object1: object('object1'),
        object2: object('object2', {position: [2, 2, 2]}),
      },
    })

    // continue with runtime updates
    doc.update(prev => updateObject(prev, 'object2', {position: [3, 3, 3]}))
    assert.deepEqual(doc.distill(), {
      objects: {
        object1: object('object1'),
        object2: object('object2', {position: [3, 3, 3]}),
      },
    })

    // redo naming object 1
    doc.updateAt([beforeNameUpdateId], () => afterNameUpdateView)
    assert.deepEqual(doc.distill(), {
      objects: {
        object1: object('object1', {name: 'name 1'}),
        object2: object('object2', {position: [3, 3, 3]}),
      },
    })
  })

  it('can undo and redo (multiple actions)', () => {
    /*
        updates:
        action0: init object1
        action1 - user: name: 'name 1'
        action2 - user: scale: [5, 5, 5]
        action3 - user: material: {type: 'basic', color: 'red'}
        action4 - runtime: position: [2, 2, 2]
        action5 - runtime: position: [3, 3, 3]
        action6 - runtime: material: {type: 'basic', color: 'blue'}
    */
    const doc = createEmptySceneDoc(ACTOR_ID)

    // action0 - init object1
    doc.update(prev => addObject(prev, 'object1', {material: {type: 'basic', color: 'white'}}))
    const action0 = doc.getVersionId()

    // user action1 - name: 'name 1'
    doc.update(prev => updateObject(prev, 'object1', {name: 'name 1'}))
    const action1 = doc.getVersionId()

    // user action2 - scale: [5, 5, 5]
    doc.update(prev => updateObject(prev, 'object1', {scale: [5, 5, 5]}))
    const action2 = doc.getVersionId()

    // user action3 - material: {type: 'basic', color: 'red'}
    doc.update(prev => updateObject(prev, 'object1', {material: {type: 'basic', color: 'red'}}))
    const action3 = doc.getVersionId()

    // runtime action4 - position: [2, 2, 2]
    doc.update(prev => updateObject(prev, 'object1', {position: [2, 2, 2]}))

    // runtime action5 - position: [3, 3, 3]
    doc.update(prev => updateObject(prev, 'object1', {position: [3, 3, 3]}))

    // runtime action6 - material: {type: 'basic', color: 'blue'}
    doc.update(prev => updateObject(prev, 'object1', {material: {type: 'basic', color: 'blue'}}))

    const finalState = {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          scale: [5, 5, 5],
          material: {type: 'basic', color: 'blue'},
          position: [3, 3, 3],
        }),
      },
    }

    assert.deepEqual(doc.distill(), finalState)

    // undo action 3
    const beforeAction3View = doc.getView(action2)
    doc.updateAt([action3], () => beforeAction3View)
    assert.deepEqual(doc.distill(), {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          scale: [5, 5, 5],
          position: [3, 3, 3],
          material: {type: 'basic', color: 'white'},
        }),
      },
    })

    // redo action 3
    const afterAction3View = doc.getView(action3)
    doc.updateAt([action2], () => afterAction3View)
    assert.deepEqual(doc.distill(), {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          scale: [5, 5, 5],
          position: [3, 3, 3],
          material: {type: 'basic', color: 'red'},
        }),
      },
    })

    // undo action 3 again
    doc.updateAt([action3], () => beforeAction3View)
    assert.deepEqual(doc.distill(), {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          scale: [5, 5, 5],
          position: [3, 3, 3],
          material: {type: 'basic', color: 'white'},
        }),
      },
    })

    // undo action 2
    const beforeAction2View = doc.getView(action1)
    doc.updateAt([action2], () => beforeAction2View)
    assert.deepEqual(doc.distill(), {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          material: {type: 'basic', color: 'white'},
          position: [3, 3, 3],
        }),
      },
    })

    // undo action 1
    const beforeAction1View = doc.getView(action0)
    doc.updateAt([action1], () => beforeAction1View)
    assert.deepEqual(doc.distill(), {
      objects: {
        object1: object('object1', {
          material: {type: 'basic', color: 'white'},
          position: [3, 3, 3],
        }),
      },
    })
  })

  it('can send undo/redo to another doc', () => {
    const debugDoc = createEmptySceneDoc('ed')
    let prevDoc

    // initial scene in edit mode
    debugDoc.update(prev => addObject(prev, 'object1', {
      geometry: {type: 'box', width: 1, height: 1, depth: 1},
      material: {type: 'basic', color: 'white'},
    }))
    debugDoc.update(prev => addObject(prev, 'object2'))

    // start runtime mode
    const syncDoc = loadSceneDoc(debugDoc.save(), 'de')

    // user action1 - name: 'name 1'
    const beforeAction1Id = debugDoc.getVersionId()
    prevDoc = debugDoc.raw()
    debugDoc.update(prev => updateObject(prev, 'object1', {name: 'name 1'}))
    const action1 = debugDoc.getVersionId()
    syncDoc.applyChanges(debugDoc.getChanges(prevDoc))

    // runtime update - object1 & object2 - position: [2, 2, 2]
    prevDoc = syncDoc.raw()
    syncDoc.update(prev => updateObject(prev, 'object1', {position: [2, 2, 2]}))
    syncDoc.update(prev => updateObject(prev, 'object2', {position: [2, 2, 2]}))

    debugDoc.applyChanges(syncDoc.getChanges(prevDoc))

    let currentState = {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          geometry: {type: 'box', width: 1, height: 1, depth: 1},
          material: {type: 'basic', color: 'white'},
          position: [2, 2, 2],
        }),
        object2: object('object2', {position: [2, 2, 2]}),
      },
    }
    assert.deepEqual(debugDoc.distill(), currentState)
    assert.deepEqual(syncDoc.distill(), currentState)

    // user action2 - object1 - material: {type: 'basic', color: 'red'}
    const beforeAction2Id = debugDoc.getVersionId()
    prevDoc = debugDoc.raw()
    debugDoc.update(prev => updateObject(
      prev, 'object1', {material: {type: 'basic', color: 'red'}}
    ))
    const action2 = debugDoc.getVersionId()
    syncDoc.applyChanges(debugDoc.getChanges(prevDoc))

    // runtime update - object1 - position: [3, 3, 3]
    prevDoc = syncDoc.raw()
    syncDoc.update(prev => updateObject(prev, 'object1', {position: [3, 3, 3]}))
    debugDoc.applyChanges(syncDoc.getChanges(prevDoc))

    currentState = {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          geometry: {type: 'box', width: 1, height: 1, depth: 1},
          material: {type: 'basic', color: 'red'},
          position: [3, 3, 3],
        }),
        object2: object('object2', {position: [2, 2, 2]}),
      },
    }
    assert.deepEqual(debugDoc.distill(), currentState)
    assert.deepEqual(syncDoc.distill(), currentState)

    // undo action2
    const beforeAction2View = debugDoc.getView(beforeAction2Id)
    prevDoc = debugDoc.raw()
    debugDoc.updateAt([action2], () => beforeAction2View)
    syncDoc.applyChanges(debugDoc.getChanges(prevDoc))

    const undoneAction2State = {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          geometry: {type: 'box', width: 1, height: 1, depth: 1},
          material: {type: 'basic', color: 'white'},
          position: [3, 3, 3],
        }),
        object2: object('object2', {position: [2, 2, 2]}),
      },
    }
    assert.deepEqual(debugDoc.distill(), undoneAction2State)
    assert.deepEqual(syncDoc.distill(), undoneAction2State)

    // runtime update - object2 - position: [3, 3, 3]
    prevDoc = syncDoc.raw()
    syncDoc.update(prev => updateObject(prev, 'object2', {position: [3, 3, 3]}))
    debugDoc.applyChanges(syncDoc.getChanges(prevDoc))

    // undo action1
    const beforeAction1View = debugDoc.getView(beforeAction1Id)
    prevDoc = debugDoc.raw()
    debugDoc.updateAt([action1], () => beforeAction1View)
    syncDoc.applyChanges(debugDoc.getChanges(prevDoc))

    const undoneAction1State = {
      objects: {
        object1: object('object1', {
          geometry: {type: 'box', width: 1, height: 1, depth: 1},
          material: {type: 'basic', color: 'white'},
          position: [3, 3, 3],
        }),
        object2: object('object2', {position: [3, 3, 3]}),
      },
    }
    assert.deepEqual(debugDoc.distill(), undoneAction1State)
    assert.deepEqual(syncDoc.distill(), undoneAction1State)

    // redo action1
    const afterAction1View = debugDoc.getView(action1)
    prevDoc = debugDoc.raw()
    debugDoc.updateAt([beforeAction1Id], () => afterAction1View)
    syncDoc.applyChanges(debugDoc.getChanges(prevDoc))

    const redoAction1State = {
      objects: {
        object1: object('object1', {
          name: 'name 1',
          geometry: {type: 'box', width: 1, height: 1, depth: 1},
          material: {type: 'basic', color: 'white'},
          position: [3, 3, 3],
        }),
        object2: object('object2', {position: [3, 3, 3]}),
      },
    }

    assert.deepEqual(debugDoc.distill(), redoAction1State)
    assert.deepEqual(syncDoc.distill(), redoAction1State)
  })

  it('can undo after clearing history', () => {
    const doc = createEmptySceneDoc(ACTOR_ID)
    doc.update(prev => addObject(prev, 'object1'))
    doc.update(prev => addObject(prev, 'object2'))

    doc.update(prev => updateObject(prev, 'object1', {name: 'name 1'}))
    const action1 = doc.getVersionId()

    doc.update(prev => updateObject(prev, 'object1', {position: [1, 1, 1]}))
    const action2 = doc.getVersionId()

    doc.update(prev => updateObject(prev, 'object1', {position: [2, 2, 2]}))
    const action3 = doc.getVersionId()

    // clear history
    const newDoc = createEmptySceneDoc(ACTOR_ID)
    newDoc.update(() => doc.distill())
    const newDocInitUpdate = newDoc.getVersionId()

    let currentState = {
      objects: {
        object1: object('object1', {name: 'name 1', position: [2, 2, 2]}),
        object2: object('object2'),
      },
    }
    assert.deepEqual(newDoc.distill(), currentState)
    assert.deepEqual(doc.distill(), currentState)

    newDoc.update(prev => updateObject(prev, 'object1', {scale: [5, 5, 5]}))
    currentState = {
      objects: {
        object1: object('object1', {name: 'name 1', position: [2, 2, 2], scale: [5, 5, 5]}),
        object2: object('object2'),
      },
    }
    assert.deepEqual(newDoc.distill(), currentState)

    // undo action3
    let docUndoneState = {
      objects: {
        object1: object('object1', {name: 'name 1', position: [1, 1, 1]}),
        object2: object('object2'),
      },
    }
    let newDocUndoneState = {
      objects: {
        object1: object('object1', {name: 'name 1', position: [1, 1, 1], scale: [5, 5, 5]}),
        object2: object('object2'),
      },
    }
    const beforeAction3View = doc.getView(action2)
    doc.updateAt([action3], () => beforeAction3View)
    newDoc.updateAt([newDocInitUpdate], () => doc.distill())
    assert.deepEqual(doc.distill(), docUndoneState)
    assert.deepEqual(newDoc.distill(), newDocUndoneState)

    // more new updates on newDoc
    newDoc.update(prev => updateObject(prev, 'object1', {position: [3, 3, 3]}))
    newDoc.update(prev => updateObject(prev, 'object2', {position: [3, 3, 3]}))

    // undo action2
    docUndoneState = {
      objects: {
        object1: object('object1', {name: 'name 1'}),
        object2: object('object2'),
      },
    }
    newDocUndoneState = {
      objects: {
        object1: object('object1', {name: 'name 1', scale: [5, 5, 5]}),
        object2: object('object2', {position: [3, 3, 3]}),
      },
    }
    const beforeAction2View = doc.getView(action1)
    doc.updateAt([action2], () => beforeAction2View)
    newDoc.updateAt([newDocInitUpdate], () => doc.distill())
    assert.deepEqual(doc.distill(), docUndoneState)
    assert.deepEqual(newDoc.distill(), newDocUndoneState)

    // redo action2
    const docRedoneState = {
      objects: {
        object1: object('object1', {name: 'name 1', position: [1, 1, 1]}),
        object2: object('object2'),
      },
    }
    const newDocRedoneState = {
      objects: {
        object1: object('object1', {name: 'name 1', position: [1, 1, 1], scale: [5, 5, 5]}),
        object2: object('object2', {position: [3, 3, 3]}),
      },
    }
    const afterAction2View = doc.getView(action2)
    doc.updateAt([action1], () => afterAction2View)
    newDoc.updateAt([newDocInitUpdate], () => doc.distill())
    assert.deepEqual(doc.distill(), docRedoneState)
    assert.deepEqual(newDoc.distill(), newDocRedoneState)
  })
})
