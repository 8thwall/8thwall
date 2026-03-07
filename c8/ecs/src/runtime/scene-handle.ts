import type {DeepReadonly} from 'ts-essentials'

import {isEqual} from 'lodash-es'

import type {SceneGraph, GraphObject, BaseGraphObject, Sky} from '../shared/scene-graph'
import type {World} from './world'
import type {Eid} from '../shared/schema'
import type {SceneHandle, SpacesHandle} from './scene-types'
import {
  getAllIncludedSpaces, getIncludedSpaces, getParentPrefabId,
  isBaseGraphObject, isPrefab, isPrefabInstance,
  resolveSpaceForObject, resolveSpaceId,
  scopeObjectIds,
} from '../shared/object-hierarchy'

import {
  spawnInstanceIntoObject, spawnIntoObject, spawnIntoObjectWithDiff,
} from './spawn-into-object'
import {getActiveCameraIdFromSceneGraph} from '../shared/get-camera-from-scene-graph'
import {isEntityPersistent} from './persistent'
import {getObjectsFromSpaces, readInputMap, setActiveCamera} from './entity'
import {loadEnvironment} from './scene-environment'
import {asm} from './asm'
import {createPrefab, createPrefabChild, getInstanceChild, spawnPrefab} from './prefab-entity'
import {setFog} from './scene-fog'
import {setSky} from './scene-sky'

const setParentEntities = (
  world: World, graphIdToEid: Map<string, Eid>, graph: DeepReadonly<SceneGraph>
) => {
  graphIdToEid.forEach((eid, graphId) => {
    const parentId = graph.objects[graphId]?.parentId
    const parentEid = (parentId && graphIdToEid.get(parentId)) || 0n
    world.setParent(eid, parentEid)
  })
}

const spawnScene = (
  world: World, originalGraph: DeepReadonly<SceneGraph>
): SceneHandle => {
  const graphIdToEid = new Map<string, Eid>()
  const eidToObject = new Map<Eid, DeepReadonly<GraphObject>>()
  const graphIdToPrefab = new Map<string, Eid>()
  const prefabToObject = new Map<Eid, DeepReadonly<GraphObject>>()
  const nameToPrefab = new Map<string, Eid>()

  // Note(Dale): graphIdToEidOrPrefab is used to store the prefab or normal eid for each graph id.
  // Used when we want all entities regardless of whether they are prefabs or not.
  const graphIdToEidOrPrefab = new Map<string, Eid>()

  let debugGraph = originalGraph
  let baseObjects = originalGraph.objects
  let activeSpace: string | undefined
  let previousSpaceSky: DeepReadonly<Sky> | undefined

  const createPrefabChildren = (id: string, eid: Eid, graph: DeepReadonly<SceneGraph>) => {
    // TODO(Dale): Update for nested prefabs
    const prefabChildren = Object.values(graph.objects)
      .filter((object): object is BaseGraphObject => (
        getParentPrefabId(graph, object.id) === id && isBaseGraphObject(object) && object.id !== id
      ))
    prefabChildren.forEach((object) => {
      const childEid = graphIdToPrefab.get(object.id) || createPrefabChild(world, eid)
      graphIdToPrefab.set(object.id, childEid)
      prefabToObject.set(childEid, object)
      graphIdToEidOrPrefab.set(object.id, childEid)
    })
  }

  const createPrefabs = (graph: DeepReadonly<SceneGraph>) => {
    Object.values(graph.objects).forEach((object) => {
      // TODO(Dale): Update for prefab variants/stacked prefabs
      const baseGraphObject = isBaseGraphObject(object)
      const prefab = isPrefab(object)
      if (prefab && !baseGraphObject) {
        throw new Error(`Stacked prefabs are not supported yet: ${object.id}`)
      }
      if (prefab && isBaseGraphObject(object)) {
        const eid = graphIdToPrefab.get(object.id) || createPrefab(world)
        graphIdToPrefab.set(object.id, eid)
        prefabToObject.set(eid, object)
        nameToPrefab.set(object.name || object.id, eid)
        graphIdToEidOrPrefab.set(object.id, eid)

        createPrefabChildren(object.id, eid, graph)
      }
    })

    graphIdToPrefab.forEach((eid, id) => {
      const parentId = graph.objects[id]?.parentId
      const parentEid = (parentId && graphIdToPrefab.get(parentId)) || 0n
      asm.entitySetParent(world._id, eid, parentEid)
    })

    graphIdToPrefab.forEach((eid, id) => {
      const object = graph.objects[id]
      if (!object) {
        throw new Error(`Prefab ${id} not found in graph`)
      }
      if (!isBaseGraphObject(object)) {
        throw new Error(`Stacked prefabs are not supported yet: ${object.id}`)
      }
      spawnIntoObject(world, eid, object, graphIdToEidOrPrefab)
    })
  }

  const removePrefabs = () => {
    nameToPrefab.forEach((eid) => {
      world.deleteEntity(eid)
    })
    nameToPrefab.clear()
    graphIdToPrefab.clear()
  }

  const createInstance = (object: DeepReadonly<GraphObject>) => {
    const {instanceOf} = object.instanceData!
    const prefabEid = graphIdToPrefab.get(instanceOf)
    if (!prefabEid) {
      throw new Error(`Prefab ${instanceOf} not found`)
    }
    const instanceEid = graphIdToEid.get(object.id) || spawnPrefab(world, prefabEid)

    graphIdToEid.set(object.id, instanceEid)
    graphIdToEidOrPrefab.set(object.id, instanceEid)
    eidToObject.set(instanceEid, object)

    const addChildren = (eid: Eid) => {
      for (const child of world.getChildren(eid)) {
        const graphId = prefabToObject.get(child)?.id
        if (graphId) {
          const instanceChild = getInstanceChild(world, instanceEid, child)
          // Note(Dale): There's no object associated with the instance child, so we don't set it
          // on eidToObject (or graphIdToEid)
          graphIdToEidOrPrefab.set(scopeObjectIds([object.id, graphId]), instanceChild)
        }
        addChildren(child)
      }
    }

    addChildren(prefabEid)
  }

  const spawnObjects = (objects: DeepReadonly<GraphObject>[]) => {
    const prefabInstances = objects.filter(object => (isPrefabInstance(object)))
    prefabInstances.forEach(object => (createInstance(object)))

    const baseGraphObjects = objects.filter((object): object is BaseGraphObject => (
      isBaseGraphObject(object) && !graphIdToPrefab.has(object.id)
    ))
    baseGraphObjects.forEach((object) => {
      const eid = graphIdToEid.get(object.id) || world.createEntity()
      graphIdToEid.set(object.id, eid)
      graphIdToEidOrPrefab.set(object.id, eid)
      eidToObject.set(eid, object)
    })

    prefabInstances.forEach((object) => {
      const eid = graphIdToEid.get(object.id)!
      spawnInstanceIntoObject(
        world, eid, object, graphIdToEidOrPrefab, graphIdToPrefab, null
      )
    })

    baseGraphObjects.forEach((object) => {
      const eid = graphIdToEid.get(object.id)!
      spawnIntoObject(world, eid, object, graphIdToEidOrPrefab)
    })
  }

  const updateObjects = (objects: DeepReadonly<GraphObject>[]) => {
    objects.forEach((object) => {
      if (isPrefabInstance(object)) {
        const prefabEid = graphIdToPrefab.get(object.instanceData.instanceOf)
        if (!prefabEid) {
          throw new Error(`Prefab ${object.instanceData.instanceOf} not found`)
        }
        const instanceEid = graphIdToEid.get(object.id)!
        const oldObject = eidToObject.get(instanceEid)
        graphIdToEid.set(object.id, instanceEid)
        graphIdToEidOrPrefab.set(object.id, instanceEid)
        eidToObject.set(instanceEid, object)
        spawnInstanceIntoObject(
          world, instanceEid, object, graphIdToEidOrPrefab, graphIdToPrefab, oldObject
        )
        return
      }
      if (!isBaseGraphObject(object) || graphIdToPrefab.has(object.id)) {
        return
      }
      const eid = graphIdToEid.get(object.id)!
      const oldObject = eidToObject.get(eid) as BaseGraphObject
      eidToObject.set(eid, object)
      spawnIntoObjectWithDiff(world, eid, object, graphIdToEidOrPrefab, oldObject)
    })
  }

  const despawnObjects = (objects: DeepReadonly<GraphObject>[]) => {
    objects.forEach((object) => {
      const eid = graphIdToEid.get(object.id)
      if (eid) {
        world.deleteEntity(eid)
        graphIdToEid.delete(object.id)
        graphIdToEidOrPrefab.delete(object.id)
        eidToObject.delete(eid)
      }
    })
  }

  const sceneHandle: SceneHandle = {
    graphIdToEid,
    eidToObject,
    graphIdToPrefab,
    _graphIdToEidOrPrefab: graphIdToEidOrPrefab,
    remove: () => {
      despawnObjects([...eidToObject.values()])
      removePrefabs()
      graphIdToEidOrPrefab.clear()
      activeSpace = undefined
    },
    updateBaseObjects: (newObjects: DeepReadonly<Record<string, GraphObject>>) => {
      baseObjects = newObjects
    },
    updateDebug: (newGraph: DeepReadonly<SceneGraph>) => {
      debugGraph = newGraph

      const spacesToSpawn: Array<string | undefined> = getAllIncludedSpaces(newGraph, activeSpace)

      const objectsToSpawn: DeepReadonly<GraphObject>[] = []
      const objectsToDespawn: DeepReadonly<GraphObject>[] = []
      const objectsToUpdate: DeepReadonly<GraphObject>[] = []

      createPrefabs(newGraph)

      Object.values(newGraph.objects).forEach((object) => {
        if (getParentPrefabId(newGraph, object.id)) {
          return
        }
        const oldEntity = graphIdToEid.get(object.id)
        const oldObject = eidToObject.get(oldEntity!)
        const objectSpaceId = resolveSpaceForObject(newGraph, object.id)?.id
        const shouldBeSpawned = (!spacesToSpawn.length && !objectSpaceId) ||
          spacesToSpawn.includes(objectSpaceId)

        if (!oldEntity && shouldBeSpawned) {
        // If the object is not spawned and is in one of the spacesToSpawn, spawn it
          objectsToSpawn.push(object)
        } else if (oldEntity && !shouldBeSpawned) {
          // If the object is spawned and is not in one of the spacesToSpawn, despawn it
          objectsToDespawn.push(object)
        } else if (oldEntity && !isEqual(object, oldObject)) {
          // If the object is spawned and has changed, update it
          objectsToUpdate.push(object)
        }
      })

      sceneHandle.graphIdToEid.forEach((eid, id) => {
        if (!newGraph.objects[id]) {
          // If the object is not in the new graph but is spawned, despawn it
          objectsToDespawn.push(eidToObject.get(eid)!)
        }
      })

      despawnObjects(objectsToDespawn)
      spawnObjects(objectsToSpawn)
      updateObjects(objectsToUpdate)

      const activeCamera = getActiveCameraIdFromSceneGraph(newGraph, activeSpace)

      setParentEntities(world, sceneHandle.graphIdToEid, newGraph)
      setActiveCamera(world, sceneHandle.graphIdToEid, activeCamera)
      readInputMap(world, newGraph.inputs, newGraph.activeMap)

      setFog(world, newGraph, activeSpace)
      previousSpaceSky = setSky(world, newGraph, activeSpace, previousSpaceSky)
    },
    loadSpace: (idOrName: string) => {
      const id = resolveSpaceId(debugGraph, idOrName)
      if (!id) {
        return
      }
      const oldLoadedSpaces = getAllIncludedSpaces(debugGraph, activeSpace)
      const newIncludedSpaces = getIncludedSpaces(debugGraph, id)
      const spacesToUnmount = oldLoadedSpaces.filter(s => !newIncludedSpaces.includes(s))

      const spacesToMount = getIncludedSpaces(debugGraph, id)
        .filter(s => !oldLoadedSpaces.includes(s))
      spacesToMount.push(id!)

      const objectsToDespawn = getObjectsFromSpaces(world, debugGraph, spacesToUnmount)
        .filter(o => !isEntityPersistent(world, graphIdToEid.get(o.id)!))

      const sceneToSpawn = {...debugGraph, objects: baseObjects}

      const objectsToSpawn = getObjectsFromSpaces(world, sceneToSpawn, spacesToMount)

      despawnObjects(objectsToDespawn)
      spawnObjects(objectsToSpawn)
      updateObjects(objectsToSpawn)

      // TODO(Dale): Should this stuff be from the debug graph or the base graph?
      const activeCamera = getActiveCameraIdFromSceneGraph(debugGraph, id)

      setParentEntities(world, sceneHandle.graphIdToEid, debugGraph)
      setActiveCamera(world, sceneHandle.graphIdToEid, activeCamera)

      loadEnvironment(world, debugGraph, sceneHandle, id)
      setFog(world, debugGraph, id)
      previousSpaceSky = setSky(world, debugGraph, id, previousSpaceSky)

      activeSpace = id
    },
    listSpaces: () => {
      if (!debugGraph.spaces) {
        return undefined
      }
      const allIncludedSpaces = getAllIncludedSpaces(debugGraph, activeSpace)
      return Object.values(debugGraph.spaces).map(s => ({
        id: s.id,
        name: s.name,
        spawned: allIncludedSpaces.includes(s.id),
      }))
    },
    getActiveSpace: () => {
      if (!activeSpace || !debugGraph.spaces) {
        return undefined
      }
      const {name} = debugGraph.spaces[activeSpace]
      return {id: activeSpace, name, spawned: true}
    },
    getPrefab: (name: string) => nameToPrefab.get(name),
  }

  createPrefabs(originalGraph)

  if (originalGraph.entrySpaceId && originalGraph.spaces) {
    sceneHandle.loadSpace(originalGraph.entrySpaceId)
  } else {
    // TODO(Dale): call load space here instead
    const spaceObjects = getObjectsFromSpaces(world, originalGraph, [])

    spawnObjects(spaceObjects)
    updateObjects(spaceObjects)

    const {activeCamera} = originalGraph
    setParentEntities(world, sceneHandle.graphIdToEid, originalGraph)
    setActiveCamera(world, sceneHandle.graphIdToEid, activeCamera)
  }

  loadEnvironment(world, originalGraph, sceneHandle, originalGraph.entrySpaceId)
  setFog(world, originalGraph, activeSpace)
  previousSpaceSky = setSky(world, originalGraph, activeSpace, previousSpaceSky)

  // Note(Dale): Input map is not scoped to spaces
  readInputMap(world, debugGraph.inputs, debugGraph.activeMap)

  return sceneHandle
}

export {
  spawnScene,
}

export type {
  SceneHandle,
  SpacesHandle,
}
