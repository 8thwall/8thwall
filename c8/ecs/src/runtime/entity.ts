import type {DeepReadonly} from 'ts-essentials'

import THREE from './three'

import {Position, Quaternion, Scale, ThreeObject} from './components'
import type {SceneGraph, GraphObject, InputMap} from '../shared/scene-graph'
import type {World} from './world'
import {asm} from './asm'
import type {Eid} from '../shared/schema'
import {setGlobalDeleteCallback} from './global-callbacks'
import type {Object3D} from './three-types'
import {resolveSpaceForObject} from '../shared/object-hierarchy'
import {addChild} from './matrix-refresh'
import {disposeObject} from './dispose'

const createEntity = (world: World): Eid => {
  const eid = asm.createEntity(world._id)
  world.allEntities.add(eid)

  const object = new THREE.Mesh(
    undefined,
    // NOTE(christoph): It seems like different versions of THREE behave differently,
    // where passing undefined gives you a default material.
    new THREE.MeshBasicMaterial({visible: false})
  ) as unknown as Object3D
  object.userData.entity = true
  object.userData.eid = eid
  object.matrixAutoUpdate = false
  addChild(world.three.scene, object)
  world.three.entityToObject.set(eid, object)

  ThreeObject.reset(world, eid)
  Position.reset(world, eid)
  Scale.reset(world, eid)
  Quaternion.reset(world, eid)

  return eid
}

// NOTE(christoph): _deleteEntity is responsible for removing the entity from the scene graph after
// it's been deleted from flecs. If a single world.deleteEntity call is made, it will call
// _deleteEntity multiple times for each child object.
const _deleteEntity = (world: World, eid: Eid) => {
  const object = world.three.entityToObject.get(eid)

  if (object) {
    disposeObject(object)
    if (object.parent) {
      object.parent.remove(object)
    }
  }

  world.allEntities.delete(eid)
  world.eidToEntity.delete(eid)
  world.three.entityToObject.delete(eid)
}

const deleteEntity = (world: World, eid: Eid) => {
  const callbackToRestore = setGlobalDeleteCallback(_deleteEntity.bind(null, world))
  asm.deleteEntity(world._id, eid)
  // NOTE(christoph): Restoring the previous callback allows us to recursively call
  // deleteEntity in response to a separate world's deleteEntity theoretically and still
  // work after we're done with the inner delete.
  setGlobalDeleteCallback(callbackToRestore)
}

const setActiveCamera = (
  world: World, graphIdToEid: Map<string, Eid>, activeCamera: string | undefined
) => {
  if (activeCamera && graphIdToEid.has(activeCamera)) {
    world.camera.setActiveEid(graphIdToEid.get(activeCamera)!)
  }
}

const readInputMap = (
  world: World, inputs: DeepReadonly<InputMap> | undefined, activeMap: string | undefined
) => {
  if (inputs) {
    world.input.readInputMap(inputs)
    if (activeMap) {
      world.input.setActiveMap(activeMap)
    }
  }
}

const getObjectsFromSpaces = (
  world: World, graph: DeepReadonly<SceneGraph>, spaces: Array<string | undefined>
): DeepReadonly<GraphObject>[] => Object.values(graph.objects)
  .filter(e => !e.ephemeral || (e.ephemeral && !world.allEntities.has(BigInt(e.id))))
  .filter((e) => {
    const resolvedSpace = resolveSpaceForObject(graph, e.id)
    return resolvedSpace ? spaces.includes(resolvedSpace.id) : !spaces.length
  })

export {
  createEntity,
  deleteEntity,
  getObjectsFromSpaces,
  setActiveCamera,
  readInputMap,
}
