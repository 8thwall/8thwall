import type {
  BaseGraphObject, GraphObject, PrefabInstanceChildren, SceneGraph,
} from '@ecs/shared/scene-graph'
import {assert} from 'chai'

import {reparentObjects} from '../src/client/studio/reparent-object'
import {deriveScene} from '../src/client/studio/derive-scene'

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
  id: string, instanceOf: string, deletions?: {}, extra?: Partial<GraphObject>,
  children?: PrefabInstanceChildren
): GraphObject => ({
  id,
  name: '<unset>',
  position: [0, 0, 0],
  rotation: [0, 0, 0, 1],
  scale: [1, 1, 1],
  components: {},
  instanceData: {
    instanceOf,
    deletions: deletions || {},
    ...(children !== undefined ? {children} : {}),
  },
  ...extra,
})

describe('reparentObjects', () => {
  it('Reparents a normal object', () => {
    const object = makeObject('object')
    const newParent = makeObject('newParent')
    const oldScene: SceneGraph = {
      objects: {
        object,
        newParent,
      },
      spaces: {},
    }
    const derivedScene = deriveScene(oldScene)
    const newScene = reparentObjects(
      oldScene, derivedScene, ['object'], {position: 'within', from: 'newParent'}
    )
    assert.strictEqual(newScene.objects.object.parentId, 'newParent')
  })

  it('Reparents a prefab object', () => {
    const prefab = makePrefab('prefab')
    const newParent = makeObject('newParent')
    const oldScene: SceneGraph = {
      objects: {
        prefab,
        newParent,
      },
      spaces: {},
    }
    const derivedScene = deriveScene(oldScene)
    const newScene = reparentObjects(
      oldScene, derivedScene, ['prefab'], {position: 'within', from: 'newParent'}
    )
    assert.strictEqual(newScene.objects.prefab.parentId, 'newParent')
  })

  it('Reparents an instance object', () => {
    const prefab = makePrefab('prefab')
    const instance = makeInstance('instance', 'prefab')
    const newParent = makeObject('newParent')
    const oldScene: SceneGraph = {
      objects: {
        prefab,
        instance,
        newParent,
      },
      spaces: {},
    }
    const derivedScene = deriveScene(oldScene)
    const newScene = reparentObjects(
      oldScene, derivedScene, ['instance'], {position: 'within', from: 'newParent'}
    )
    assert.strictEqual(newScene.objects.instance.parentId, 'newParent')
  })

  it('Reparents an instance child object', () => {
    const prefab = makePrefab('prefab')
    const child = makeObject('child', {parentId: 'prefab'})
    const instance = makeInstance('instance', 'prefab', {}, {}, {
      'child': {id: 'child', deletions: {}},
    })
    const newParent = makeObject('newParent')
    const oldScene: SceneGraph = {
      objects: {
        prefab,
        child,
        instance,
        newParent,
      },
      spaces: {},
    }
    const derivedScene = deriveScene(oldScene)
    const newScene = reparentObjects(
      oldScene, derivedScene, ['instance/child'], {position: 'within', from: 'newParent'}
    )
    assert.strictEqual(
      newScene.objects.instance.instanceData?.children?.child?.parentId, 'newParent'
    )
  })

  it('inserts before an object', () => {
    const parent = makeObject('parent')
    const a = makeObject('a', {parentId: 'parent', order: 1})
    const b = makeObject('b', {parentId: 'parent', order: 2})
    const x = makeObject('x')
    const y = makeObject('y')
    const oldScene: SceneGraph = {
      objects: {
        parent,
        a,
        b,
        x,
        y,
      },
      spaces: {},
    }
    let derivedScene = deriveScene(oldScene)
    let newScene = reparentObjects(
      oldScene, derivedScene, ['x'], {position: 'before', from: 'a'}
    )
    derivedScene = deriveScene(newScene)
    newScene = reparentObjects(
      newScene, derivedScene, ['y'], {position: 'before', from: 'b'}
    )
    derivedScene = deriveScene(newScene)
    const children = derivedScene.sortObjectIds(derivedScene.getChildren('parent'))
    assert.deepEqual(children, ['x', 'a', 'y', 'b'])
  })

  it('inserts after an object', () => {
    const parent = makeObject('parent')
    const a = makeObject('a', {parentId: 'parent', order: 1})
    const b = makeObject('b', {parentId: 'parent', order: 2})
    const x = makeObject('x')
    const y = makeObject('y')
    const oldScene: SceneGraph = {
      objects: {
        parent,
        a,
        b,
        x,
        y,
      },
      spaces: {},
    }
    let derivedScene = deriveScene(oldScene)
    let newScene = reparentObjects(
      oldScene, derivedScene, ['x'], {position: 'after', from: 'a'}
    )
    derivedScene = deriveScene(newScene)
    newScene = reparentObjects(
      newScene, derivedScene, ['y'], {position: 'after', from: 'b'}
    )
    derivedScene = deriveScene(newScene)
    const children = derivedScene.sortObjectIds(derivedScene.getChildren('parent'))
    assert.deepEqual(children, ['a', 'x', 'b', 'y'])
  })
})
