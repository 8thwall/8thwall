// @package(npm-ecs)
// @attr(externalize_npm = 1)
// @attr(esnext = 1)

/* eslint-disable @typescript-eslint/no-unused-vars */
/* eslint-disable no-param-reassign */
/* eslint-disable max-len */

import type {BaseGraphObject} from '../shared/scene-graph'
import {initThree} from './test-env'
import {ecs} from './test-runtime-lib'

const bigIntPrinter = value => `${value}n`

const callbackCounts = new Map()

const checkEmoji = '✅'
const xEmoji = '❌'

const jsonReplacer = (_, value) => (
  (typeof value === 'bigint') ? bigIntPrinter(value) : value
)

const log = (...args) => {
  const text = args
    .map(e => ((typeof e === 'string') ? e : JSON.stringify(e, jsonReplacer)))
    .join(' ')

  // eslint-disable-next-line no-console
  console.log(text)
}

const object = (id: string, extra = {}): BaseGraphObject => ({
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

const updateObject = (prev, id, extra = {}) => ({
  ...prev,
  objects: {
    ...prev.objects,
    [id]: {
      ...prev.objects[id],
      ...extra,
    },
  },
})

const deleteObject = (prev, id) => {
  const newObjects = {...prev.objects}
  delete newObjects[id]
  return {
    ...prev,
    objects: newObjects,
  }
}

const space = (id, extra = {}) => ({
  id,
  name: id,
  ...extra,
})

const updateSpace = (prev, id, extra = {}) => ({
  ...prev,
  spaces: {
    ...prev.spaces,
    [id]: {
      ...prev.spaces[id],
      ...extra,
    },
  },
})

const component = (id, name, parameters = {}) => ({
  id,
  name,
  parameters: parameters || {},
})

const countObjects = (scene) => {
  let count = 0

  scene.traverse((o) => {
    if (o.userData.eid) {
      count++
    }
  })

  return count
}

const expectSceneObjects = (world, expected, label) => {
  const actual = countObjects(world.three.scene)
  if (actual === expected) {
    log(checkEmoji, label, `(${actual})`)
  } else {
    log(xEmoji, '[scene objects]', label, `(${actual}, expected: ${expected})`)
  }
}

const expectSameEid = (eid1, eid2, label) => {
  if (eid1 === eid2) {
    log(checkEmoji, label, `(${eid2})`)
  } else {
    log(xEmoji, '[eid]', label, `(expected: ${eid1}, actual: ${eid2})`)
  }
}

ecs.registerComponent({
  name: 'test',
  schema: {period: ecs.f32},
  add: () => {
    callbackCounts.set('add', (callbackCounts.get('add') || 0) + 1)
  },
  remove: () => {
    callbackCounts.set('remove', (callbackCounts.get('remove') || 0) + 1)
  },
})

const objectWithTestComponent = (id, extra = {}) => {
  const componentId = `${id}-test`
  return object(id, {
    components: {
      [componentId]: component(componentId, 'test', {period: 1}),
    },
    ...extra,
  })
}

const addObjectWithTestComponent = (prev, id, extra = {}) => ({
  ...prev,
  objects: {
    ...prev.objects,
    [id]: objectWithTestComponent(id, extra),
  },
})

const resetCounts = () => {
  callbackCounts.set('add', 0)
  callbackCounts.set('remove', 0)
}

const expectAdds = (expected, label) => {
  const actual = callbackCounts.get('add') || 0
  if (actual === expected) {
    log(checkEmoji, label, `(${actual})`)
  } else {
    log(xEmoji, '[adds]', label, `(${actual}, expected: ${expected})`)
  }
}

const expectRemoves = (expected, label) => {
  const actual = callbackCounts.get('remove') || 0
  if (actual === expected) {
    log(checkEmoji, label, `(${actual})`)
  } else {
    log(xEmoji, '[removes]', label, `(${actual}, expected: ${expected})`)
  }
}

// check if given objectIds match the spawned objects in scene handle
const expectSpawned = (sceneHandle, objectIds, label) => {
  const sortedObjectIds = objectIds.sort()
  const sortedGraphIds = Array.from(sceneHandle.graphIdToEid.keys()).sort()
  if (JSON.stringify(sortedObjectIds) === JSON.stringify(sortedGraphIds)) {
    log(checkEmoji, label, `(${objectIds})`)
  } else {
    log(xEmoji, '[spawned]', label, `(expected: ${objectIds}, actual: ${sortedGraphIds})`)
  }
}

const deepEqual = (obj1, obj2) => {
  if (obj1 === obj2) return true
  if (typeof obj1 !== 'object' || typeof obj2 !== 'object' || obj1 === null || obj2 === null) return false

  const keys1 = Object.keys(obj1)
  const keys2 = Object.keys(obj2)

  if (keys1.length !== keys2.length) return false

  for (const key of keys1) {
    if (!keys2.includes(key) || !deepEqual(obj1[key], obj2[key])) return false
  }

  return true
}

const expectEqual = (expected, actual, label) => {
  if (deepEqual(expected, actual)) {
    log(checkEmoji, label, `(${actual})`)
  } else {
    log(xEmoji, label, `(expected: ${expected}, actual: ${actual})`)
  }
}

const expectNotEqual = (expected, actual, label) => {
  if (!deepEqual(expected, actual)) {
    log(checkEmoji, label)
  } else {
    log(xEmoji, label, `(expected/actual: ${actual})`)
  }
}

const updateGraphs = (sceneHandle, debugGraph, baseGraph, callback) => {
  debugGraph = callback(debugGraph)
  baseGraph = callback(baseGraph)
  sceneHandle.updateBaseObjects(baseGraph.objects)
  sceneHandle.updateDebug(debugGraph)
  return {debugGraph, baseGraph}
}

const runLoadSpaceTest = () => {
  const world = ecs.createWorld(...initThree())

  log('LOAD SPACE TEST')

  log('\nGraph set up: ')
  log('Space A: objectA1, objectA2, includes space B')
  log('Space B: objectB1')
  log('Space C: objectC1, includes space B')
  log('Space D: objectD1')

  log('Entry space: A')

  const sceneGraph = {
    objects: {
      objectA1: objectWithTestComponent('objectA1', {parentId: 'spaceA'}),
      objectA2: objectWithTestComponent('objectA2', {parentId: 'objectA1'}),
      objectB1: objectWithTestComponent('objectB1', {parentId: 'spaceB'}),
      objectC1: objectWithTestComponent('objectC1', {parentId: 'spaceC'}),
      objectD1: objectWithTestComponent('objectD1', {parentId: 'spaceD'}),
    },
    spaces: {
      spaceA: space('spaceA', {includedSpaces: ['spaceB']}),
      spaceB: space('spaceB'),
      spaceC: space('spaceC', {includedSpaces: ['spaceB']}),
      spaceD: space('spaceD'),
    },
    entrySpaceId: 'spaceA',
  }
  resetCounts()
  const sceneHandle = world.loadScene(sceneGraph)

  world.setSceneHook(sceneHandle)

  let a1Eid, a2Eid, b1Eid, c1Eid, d1Eid

  const setEids = () => {
    a1Eid = sceneHandle.graphIdToEid.get('objectA1')
    a2Eid = sceneHandle.graphIdToEid.get('objectA2')
    b1Eid = sceneHandle.graphIdToEid.get('objectB1')
    c1Eid = sceneHandle.graphIdToEid.get('objectC1')
    d1Eid = sceneHandle.graphIdToEid.get('objectD1')
  }

  /* TEST INITIAL SPAWN ON WORLD START */

  world.tick()
  setEids()

  log('\nCan load entry space that also has included spaces: A & B')
  expectSceneObjects(world, 3, 'initial scene objects')
  expectAdds(3, 'initial add callbacks')
  expectSpawned(sceneHandle, ['objectA1', 'objectA2', 'objectB1'], 'initial spawned for A and B')
  resetCounts()

  /* TEST LOAD SPACE - NO DEBUG EDIT */

  // can switch to a previously included space: A(B), A -> B
  //    despawn A and B, spawn B
  log('\nCan switch to a previously included space: A -> B')
  let b1EidBefore = sceneHandle.graphIdToEid.get('objectB1')
  sceneHandle.loadSpace('spaceB')
  world.tick()
  let b1EidAfter = sceneHandle.graphIdToEid.get('objectB1')
  expectRemoves(3, 'despawn A and B')
  expectAdds(1, 'spawn B')
  expectSceneObjects(world, 1, 'space B objects')
  expectSpawned(sceneHandle, ['objectB1'], 'spawned for B')
  expectNotEqual(b1EidBefore, b1EidAfter, 'B1 eid changes after switching to space B')
  resetCounts()

  // can switch to a parent space: B -> A
  //   spawn A, should not respawn B
  log('\nCan switch to a parent space: B -> A')
  b1EidBefore = sceneHandle.graphIdToEid.get('objectB1')
  sceneHandle.loadSpace('spaceA')
  world.tick()
  b1EidAfter = sceneHandle.graphIdToEid.get('objectB1')
  expectRemoves(0, 'despawn B')
  expectAdds(2, 'spawn A')
  expectSceneObjects(world, 3, 'space A objects')
  expectSpawned(sceneHandle, ['objectA1', 'objectA2', 'objectB1'], 'spawned for A and B')
  expectSameEid(b1EidBefore, b1EidAfter, 'B1 eid stays after switching to space A')
  resetCounts()

  // can switch space with shared included spaces: A -> C
  //    despawn A, spawn C, should not respawn B
  log('\nCan switch space with shared included spaces: A -> C')
  b1EidBefore = sceneHandle.graphIdToEid.get('objectB1')
  sceneHandle.loadSpace('spaceC')
  world.tick()
  b1EidAfter = sceneHandle.graphIdToEid.get('objectB1')
  expectRemoves(2, 'despawn A')
  expectAdds(1, 'spawn C')
  expectSceneObjects(world, 2, 'space C objects')
  expectSpawned(sceneHandle, ['objectC1', 'objectB1'], 'spawned for C and B')
  expectSameEid(b1EidBefore, b1EidAfter, 'B1 eid stays after switching to space C')
  resetCounts()

  // can switch space: C -> D
  //    despawn B and C, spawn D
  log('\nCan switch space: C -> D')
  sceneHandle.loadSpace('spaceD')
  world.tick()
  expectRemoves(2, 'despawn B and C')
  expectAdds(1, 'spawn D')
  expectSceneObjects(world, 1, 'space D objects')
  expectSpawned(sceneHandle, ['objectD1'], 'spawned for D')
  resetCounts()

  // can switch to space with included spaces: D -> A
  //    despawn D, spawn A and B
  log('\nCan switch to space with included spaces: D -> A')
  sceneHandle.loadSpace('spaceA')
  world.tick()
  expectRemoves(1, 'despawn D')
  expectAdds(3, 'spawn A and B')
  expectSceneObjects(world, 3, 'space A objects')
  expectSpawned(sceneHandle, ['objectA1', 'objectA2', 'objectB1'], 'spawned for A and B')
  resetCounts()

  // runtime updates will be ignored after switching spaces
  //   update object A1 position and object B1 position
  //   switch to space C, then switch back to space A
  //   expect object A1 to reset, but B1 to remain in the updated position
  log('\nRuntime updates will be ignored after switching spaces')
  log('Update object A1 position and object B1 position, switch from A to C, then back to A')
  log('Expect object A1 to reset, but B1 to remain in the updated position')
  setEids()
  ecs.Position.set(world, a1Eid, {x: 5, y: 5, z: 5})
  ecs.Position.set(world, b1Eid, {x: 4, y: 4, z: 4})
  world.tick()
  sceneHandle.loadSpace('spaceC')
  world.tick()
  setEids()
  const b1Position = ecs.Position.get(world, b1Eid)
  expectEqual([4, 4, 4], [b1Position.x, b1Position.y, b1Position.z], 'B1 position stays after switching to space C')
  sceneHandle.loadSpace('spaceA')
  world.tick()
  setEids()
  const a1Position = ecs.Position.get(world, a1Eid)
  expectEqual([0, 0, 0], [a1Position.x, a1Position.y, a1Position.z], 'A1 position resets after switching to space A')
  const b1Position2 = ecs.Position.get(world, b1Eid)
  expectEqual([4, 4, 4], [b1Position2.x, b1Position2.y, b1Position2.z], 'B1 position stays after switching to space A')

  log('\n====================\n')
  world.destroy()
}

/* TEST UPDATE DEBUG WITH DEBUG EDIT */
const runUpdateDebugTestWithSaveEdits = () => {
  const world = ecs.createWorld(...initThree())

  log('UPDATE DEBUG TEST WITH SAVE EDITS ON')

  log('\nGraph set up: ')
  log('Space A: objectA1, objectA2, includes space B')
  log('Space B: objectB1')
  log('Space C: objectC1, includes space B')
  log('Space D: objectD1')

  log('Entry space: A')

  let baseGraph = {
    objects: {
      objectA1: objectWithTestComponent('objectA1', {parentId: 'spaceA'}),
      objectA2: objectWithTestComponent('objectA2', {parentId: 'objectA1'}),
      objectB1: objectWithTestComponent('objectB1', {parentId: 'spaceB'}),
      objectC1: objectWithTestComponent('objectC1', {parentId: 'spaceC'}),
      objectD1: objectWithTestComponent('objectD1', {parentId: 'spaceD'}),
    },
    spaces: {
      spaceA: space('spaceA', {includedSpaces: ['spaceB']}),
      spaceB: space('spaceB'),
      spaceC: space('spaceC', {includedSpaces: ['spaceB']}),
      spaceD: space('spaceD'),
    },
    entrySpaceId: 'spaceA',
  }

  let debugGraph = JSON.parse(JSON.stringify(baseGraph))

  resetCounts()
  const sceneHandle = world.loadScene(debugGraph)
  world.tick()

  /* TEST INITIAL SPAWN ON WORLD START */

  log('\nCan load entry space: A')
  expectSceneObjects(world, 3, 'initial scene objects')
  expectAdds(3, 'initial add callbacks')
  resetCounts()

  /* TEST UPDATE DEBUG */

  // OBJECT UPDATES

  // can add new object to the active space
  log('\nCan add new objects to active space and included space')
  ;({debugGraph, baseGraph} = updateGraphs(sceneHandle, debugGraph, baseGraph, (graph) => {
    graph = addObjectWithTestComponent(graph, 'objectA3', {parentId: 'spaceA'})
    graph = addObjectWithTestComponent(graph, 'objectB2', {parentId: 'objectB1'})
    return graph
  }))
  world.tick()
  expectAdds(2, 'add callback for adding A3 and B2')
  expectRemoves(0, 'remove callback for all objects')
  expectSceneObjects(world, 5, 'scene objects after add')
  expectSpawned(sceneHandle, ['objectA1', 'objectA2', 'objectB1', 'objectA3', 'objectB2'], 'spawned for A, B')
  resetCounts()

  // can edit properties of object in active space
  log('\nCan edit properties of objects in active space and included space')
  ;({debugGraph, baseGraph} = updateGraphs(sceneHandle, debugGraph, baseGraph, (graph) => {
    graph = updateObject(graph, 'objectA1', {position: [1, 1, 1]})
    graph = updateObject(graph, 'objectB2', {geometry: {type: 'sphere', radius: 5}})
    return graph
  }))
  world.tick()
  expectAdds(0, 'add callback for updating A1 and B2')
  expectRemoves(0, 'remove callback for updating A1 and B2')
  expectSceneObjects(world, 5, 'scene objects after edit')
  let a1Position = ecs.Position.get(world, sceneHandle.graphIdToEid.get('objectA1'))
  expectEqual([1, 1, 1], [a1Position.x, a1Position.y, a1Position.z], 'A1 position updated')
  const b2Geometry = ecs.SphereGeometry.get(world, sceneHandle.graphIdToEid.get('objectB2'))
  expectEqual({type: 'sphere', radius: 5}, b2Geometry, 'B2 geometry updated')
  resetCounts()

  // can delete object in active space
  log('\nCan delete objects in active space and included space')
  ;({debugGraph, baseGraph} = updateGraphs(sceneHandle, debugGraph, baseGraph, (graph) => {
    graph = deleteObject(graph, 'objectA3')
    graph = deleteObject(graph, 'objectB2')
    return graph
  }))
  world.tick()
  expectAdds(0, 'add callback for removing A3 and B2')
  expectRemoves(2, 'remove callback for removing A3 and B2')
  expectSceneObjects(world, 3, 'scene objects after delete')
  expectSpawned(sceneHandle, ['objectA1', 'objectA2', 'objectB1'], 'spawned for A and B')
  resetCounts()

  // add/edit/delete object in non-active space will be ignored,
  // but will be applied when switching to that space
  log('\nCan update objects in non-active space, and switch space to see changes')
  ;({debugGraph, baseGraph} = updateGraphs(sceneHandle, debugGraph, baseGraph, (graph) => {
    graph = updateObject(graph, 'objectC1', {position: [2, 2, 2]})
    graph = addObjectWithTestComponent(graph, 'objectC2', {parentId: 'objectC1'})
    return graph
  }))
  world.tick()
  expectAdds(0, 'add callback for updating C1 and adding C2')
  expectRemoves(0, 'remove callback for updating C1 and adding C2')
  expectSceneObjects(world, 3, 'scene objects after non-active space update')
  log('Switch to space C')
  sceneHandle.loadSpace('spaceC')
  world.tick()
  expectAdds(2, 'add callback for loading space C')
  expectRemoves(2, 'remove callback for despawning space A')
  expectSceneObjects(world, 3, 'scene objects after switching to space C')
  const c1Position = ecs.Position.get(world, sceneHandle.graphIdToEid.get('objectC1'))
  expectEqual([2, 2, 2], [c1Position.x, c1Position.y, c1Position.z], 'C1 position initialized with user edit')
  resetCounts()

  // SPACE UPDATE

  // can add a new included space
  log('\nCan add a new included space and new object: add objectD2, update space C to include space D')
  ;({debugGraph, baseGraph} = updateGraphs(sceneHandle, debugGraph, baseGraph, (graph) => {
    graph = addObjectWithTestComponent(graph, 'objectD2', {parentId: 'spaceD'})
    graph = updateSpace(graph, 'spaceC', {includedSpaces: ['spaceB', 'spaceD']})
    return graph
  }))
  world.tick()
  expectAdds(2, 'add callback for including space D in space C')
  expectRemoves(0, 'remove callback for including space D in space C')
  expectSceneObjects(world, 5, 'scene objects after adding included space')
  expectSpawned(sceneHandle, ['objectC1', 'objectC2', 'objectD1', 'objectD2', 'objectB1'], 'spawn for C, D, and B')
  resetCounts()

  // can remove an included space
  log('\nCan remove an included space: update space C to remove space B and D')
  ;({debugGraph, baseGraph} = updateGraphs(sceneHandle, debugGraph, baseGraph, (graph) => {
    graph = updateSpace(graph, 'spaceC', {includedSpaces: []})
    return graph
  }))
  world.tick()
  expectAdds(0, 'add callback for removing space B and D from space C')
  expectRemoves(3, 'remove callback for removing space B and D from space C')
  expectSceneObjects(world, 2, 'scene objects after removing included space')
  expectSpawned(sceneHandle, ['objectC1', 'objectC2'], 'spawn for C')
  resetCounts()

  // set the scene back to initial state
  log('\nReset space C to include space B ')
  ;({debugGraph, baseGraph} = updateGraphs(sceneHandle, debugGraph, baseGraph, (graph) => {
    graph = updateSpace(graph, 'spaceC', {includedSpaces: ['spaceB']})
    return graph
  }))
  world.tick()
  resetCounts()

  /* TEST UPDATE DEBUG + SWITCH SPACE */

  // objects should spawned with last user edit after switching spaces
  log('\nObjects should spawn with last user edit after switching spaces')
  log('Switch back to space A, expect A1 position to be [1, 1, 1]')
  sceneHandle.loadSpace('spaceA')
  world.tick()
  a1Position = ecs.Position.get(world, sceneHandle.graphIdToEid.get('objectA1'))
  expectEqual([1, 1, 1], [a1Position.x, a1Position.y, a1Position.z], 'A1 position stays after switching to space A')
  resetCounts()

  // can reparent object to another object in an included space, and switching to the other
  // space with same included space will not despawn it
  log('\nCan reparent object to another object in an included space')
  ;({debugGraph, baseGraph} = updateGraphs(sceneHandle, debugGraph, baseGraph, (graph) => {
    graph = updateObject(graph, 'objectA2', {parentId: 'objectB1'})
    return graph
  }))
  world.tick()
  expectAdds(0, 'add callback for reparenting A2 to B1')
  expectRemoves(0, 'remove callback for reparenting A2 to B1')
  expectSceneObjects(world, 3, 'scene objects after reparenting')
  resetCounts()

  log('Switching to the other space with same included space will not despawn it')
  sceneHandle.loadSpace('spaceC')
  world.tick()
  expectAdds(2, 'add callback for switching to space C')
  expectRemoves(1, 'remove callback for despawning space A')
  expectSceneObjects(world, 4, 'scene objects after switching to space C')
  resetCounts()

  log('=======================')
  world.destroy()
}

const runPersistenceTestWithSaveEdits = () => {
  const world = ecs.createWorld(...initThree())

  log('\nPERSISTENCE TEST WITH SAVE EDITS ON')

  log('\nGraph set up: ')
  log('Space A: objectA1 (persistent), objectA2')
  log('Space B: objectB1')

  log('Entry space: A')

  let baseGraph = {
    objects: {
      objectA1: objectWithTestComponent('objectA1', {parentId: 'spaceA', persistent: true}),
      objectA2: objectWithTestComponent('objectA2', {parentId: 'spaceA'}),
      objectB1: objectWithTestComponent('objectB1', {parentId: 'spaceB'}),
    },
    spaces: {
      spaceA: space('spaceA'),
      spaceB: space('spaceB'),
    },
    entrySpaceId: 'spaceA',
  }

  let debugGraph = JSON.parse(JSON.stringify(baseGraph))

  resetCounts()
  const sceneHandle = world.loadScene(debugGraph)
  world.tick()

  log('\nCan load entry space: A')
  expectSceneObjects(world, 2, 'initial scene objects')
  expectAdds(2, 'initial add callbacks')
  expectRemoves(0, 'initial remove callbacks')
  resetCounts()

  // switch to space b -> a1 should not despawn or respawn, parent should still be 0n
  // make debug edit on b1 (pass the new graph with a1 in space b)
  // make debug edits on a1
  // switch back to space a -> a1 should not despawn or respawn, but parent should still be 0n, and debug edits should be applied
  log('\nPersistent object should not despawn or respawn after switching spaces')
  log('Switch to space B, expect A1 to not despawn or respawn')
  let a1EidBefore = sceneHandle.graphIdToEid.get('objectA1')
  sceneHandle.loadSpace('spaceB')
  world.tick()
  let a1EidAfter = sceneHandle.graphIdToEid.get('objectA1')
  expectRemoves(1, 'despawn A2 only')
  expectAdds(1, 'spawn B1 only')
  expectSceneObjects(world, 2, 'space B objects')
  expectEqual(world.getParent(a1EidAfter), 0n, 'A1 parent is 0n')
  expectSameEid(a1EidBefore, a1EidAfter, 'A1 eid stays after switching to space B')
  resetCounts()

  log('\nCan make debug edits on persistent object')
  ;({debugGraph, baseGraph} = updateGraphs(sceneHandle, debugGraph, baseGraph, (graph) => {
    // NOTE: adding {parentId: 'spaceB'} is assuming that worldToSceneGraph will resolve the correct space id for persistent objects
    // not intended as a user debug edit
    graph = updateObject(graph, 'objectA1', {position: [5, 5, 5], parentId: 'spaceB'})
    return graph
  }))
  world.tick()
  expectAdds(0, 'add callback for updating A1')
  expectRemoves(0, 'remove callback for updating A1')
  expectSceneObjects(world, 2, 'scene objects after edit')
  let a1Position = ecs.Position.get(world, sceneHandle.graphIdToEid.get('objectA1'))
  expectEqual([5, 5, 5], [a1Position.x, a1Position.y, a1Position.z], 'A1 position updated')
  resetCounts()

  log('\nSwitch back to space A, expect A1 to not despawn or respawn')
  a1EidBefore = sceneHandle.graphIdToEid.get('objectA1')
  sceneHandle.loadSpace('spaceA')
  world.tick()
  a1EidAfter = sceneHandle.graphIdToEid.get('objectA1')
  expectRemoves(1, 'despawn space B')
  expectAdds(1, 'spawn space A')
  expectSceneObjects(world, 2, 'space A objects')
  expectSameEid(a1EidBefore, a1EidAfter, 'A1 eid stays after switching to space A')
  expectEqual(world.getParent(sceneHandle.graphIdToEid.get('objectA1')), 0n, 'A1 parent is 0n')
  a1Position = ecs.Position.get(world, sceneHandle.graphIdToEid.get('objectA1'))
  expectEqual([5, 5, 5], [a1Position.x, a1Position.y, a1Position.z], 'A1 position stays after switching to space A')
  resetCounts()

  log('\n====================\n')
  world.destroy()
}

const runPartialUpdateTest = () => {
  const world = ecs.createWorld(...initThree())

  log('PARTIAL UPDATE TEST')

  let baseGraph = {
    objects: {
      objectA: objectWithTestComponent('objectA', {
        parentId: 'spaceA',
        position: [1, 1, 1],
      }),
    },
    spaces: {
      spaceA: space('spaceA'),
    },
    entrySpaceId: 'spaceA',
  }

  log('loading')

  const sceneHandle = world.loadScene(baseGraph)

  log('loaded')

  const expectPosition = (eid, expectedPosition, label) => {
    const position = ecs.Position.get(world, eid)
    expectEqual(expectedPosition, [position.x, position.y, position.z], label)
  }

  const objectA = sceneHandle.graphIdToEid.get('objectA')
  if (!objectA) {
    log(xEmoji, 'objectA not found in sceneHandle')
  } else {
    log(checkEmoji, 'objectA found in sceneHandle')
  }

  expectPosition(objectA, [1, 1, 1], 'starts at 1, 1, 1')

  ecs.Position.set(world, objectA, {x: 2, y: 2, z: 2})

  expectPosition(objectA, [2, 2, 2], 'updated to 2, 2, 2')

  baseGraph = {
    ...baseGraph,
    objects: {
      objectA: objectWithTestComponent('objectA', {
        parentId: 'spaceA',
        position: [1, 1, 1],
      }),
    },
  }

  sceneHandle.updateBaseObjects(baseGraph.objects)
  sceneHandle.updateDebug(baseGraph)

  expectPosition(objectA, [2, 2, 2], 'still at 2, 2, 2 after updateBaseObjects')

  if (ecs.Hidden.has(world, objectA)) {
    log(xEmoji, 'objectA should not be hidden before update')
  } else {
    log(checkEmoji, 'objectA is visible before update')
  }

  baseGraph = {
    ...baseGraph,
    objects: {
      objectA: objectWithTestComponent('objectA', {
        parentId: 'spaceA',
        position: [1, 1, 1],
        hidden: true,
      }),
    },
  }

  sceneHandle.updateBaseObjects(baseGraph.objects)
  sceneHandle.updateDebug(baseGraph)

  expectPosition(objectA, [2, 2, 2], 'still at 2, 2, 2 after updateBaseObjects')
  if (ecs.Hidden.has(world, objectA)) {
    log(checkEmoji, 'objectA is hidden after update')
  } else {
    log(xEmoji, 'objectA should be hidden after update')
  }
  world.destroy()
}

const runTests = () => {
  runLoadSpaceTest()
  runUpdateDebugTestWithSaveEdits()
  runPersistenceTestWithSaveEdits()
  runPartialUpdateTest()
  log('Done!')
}

ecs.ready().then(runTests)
