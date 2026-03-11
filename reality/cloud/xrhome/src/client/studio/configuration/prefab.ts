import type {
  BaseGraphObject, GraphObject, SceneGraph, PrefabInstanceChildrenData,
} from '@ecs/shared/scene-graph'
import type {DeepReadonly} from 'ts-essentials'

import {v4 as uuid} from 'uuid'

import {
  descopeObjectId, isDescendantOf, isPrefab, isPrefabInstance, isScopedObjectId,
  resolveSpaceForObject, getSourceObjectId,
} from '@ecs/shared/object-hierarchy'

import {makeObjectName} from '../common/studio-files'
import type {IdMapper} from '../id-generation'
import {
  createObjectUpdateRefs, cleanComponentEntityRefs,
} from './update-scene-entity-refs'
import {deriveScene, type DerivedScene} from '../derive-scene'
import {POSITION_COMPONENT} from './direct-property-components'
import {MESH_COMPONENTS} from './direct-property-components'

const createInstanceObject = (
  scene: DeepReadonly<SceneGraph>, prefab: DeepReadonly<GraphObject>, newParentId: string,
  newId?: string, position?: Readonly<GraphObject['position']>
): DeepReadonly<GraphObject> => ({
  id: newId || uuid(),
  position: position || [0, 0, 0],
  name: makeObjectName(prefab.name, scene, resolveSpaceForObject(scene, newParentId)?.id),
  instanceData: {instanceOf: prefab.id, deletions: {}, children: {}},
  parentId: newParentId,
  components: {},
})

const makePrefabAndReplaceWithInstance = (
  scene: DeepReadonly<SceneGraph>, id: string, sourceIds: IdMapper
): DeepReadonly<SceneGraph> => {
  const sourceObject = scene.objects[id]

  if (!sourceObject || sourceObject.prefab) {
    return scene
  }

  const newSource = {
    ...sourceObject,
    id: sourceIds.fromId(id),
    prefab: true as const,
  }
  delete newSource.parentId

  const newInstance: DeepReadonly<GraphObject> & Pick<GraphObject, 'parentId'> = {
    id,
    position: sourceObject.position,
    scale: sourceObject.scale,
    rotation: sourceObject.rotation,
    name: sourceObject.name,
    instanceData: {instanceOf: newSource.id, deletions: {}, children: {}},
    ...(sourceObject.parentId ? {parentId: sourceObject.parentId} : {}),
    components: {},
  }

  const sourceChildObjects: DeepReadonly<GraphObject>[] = []
  const externalObjects: DeepReadonly<GraphObject>[] = []

  const originalToSourceIds = new Map<string, string>()
  const originalToInstanceIds = new Map<string, string>()

  originalToSourceIds.set(id, newSource.id)

  const isDescendantCache = new Map<string, boolean>()
  isDescendantCache.set(id, true)

  Object.values(scene.objects).forEach((object) => {
    if (object.id === id) {
      return
    }
    if (isDescendantOf(scene, object.id, id)) {
      originalToInstanceIds.set(object.id, `${newInstance.id}/${sourceIds.fromId(object.id)}`)
      originalToSourceIds.set(object.id, sourceIds.fromId(object.id))
      sourceChildObjects.push(object)
    } else {
      externalObjects.push(object)
    }
  })

  const newObjects: Record<string, DeepReadonly<GraphObject>> = {
    [newInstance.id]: newInstance,
    [newSource.id]: newSource,
  }

  sourceChildObjects.forEach((object) => {
    const newSourceId = sourceIds.fromId(object.id)
    const remappedObject = createObjectUpdateRefs(object, originalToSourceIds)
    remappedObject.parentId = sourceIds.fromId(object.parentId)
    remappedObject.id = newSourceId
    newObjects[newSourceId] = remappedObject
  })

  externalObjects.forEach((object) => {
    newObjects[object.id] = createObjectUpdateRefs(object, originalToInstanceIds)
  })

  return {
    ...scene,
    objects: newObjects,
  }
}

const makePrefab = (
  scene: DeepReadonly<SceneGraph>, id: string
): DeepReadonly<SceneGraph> => {
  const sourceObject = scene.objects[id]

  if (!sourceObject || sourceObject.prefab) {
    return scene
  }

  const newSource = {
    ...sourceObject,
    id,
    prefab: true as const,
  }
  delete newSource.parentId

  return {
    ...scene,
    objects: {
      ...scene.objects,
      [newSource.id]: newSource,
    },
  }
}

const makeInstance = (
  scene: DeepReadonly<SceneGraph>, prefabId: string, parentId: string, newId?: string,
  position?: Readonly<GraphObject['position']>
): DeepReadonly<SceneGraph> => {
  const prefab = scene.objects[prefabId]

  if (!prefab?.id) {
    return scene
  }

  const newInstance = createInstanceObject(scene, prefab, parentId, newId, position)

  const newObjects = {
    ...scene.objects,
    [newInstance.id as string]: newInstance,
  }

  return {
    ...scene,
    objects: newObjects,
  }
}

const prefabContainsLight = (derivedScene: DerivedScene, prefabId: string): boolean => {
  const objects = derivedScene.getDescendantObjectIds(prefabId)
  objects.push(prefabId)
  return objects.some((objectId) => {
    const object = derivedScene.getObject(objectId)
    return !!object.light
  })
}

const isOverriddenInstance = (
  scene: DeepReadonly<SceneGraph>, objectId: string
): boolean => {
  if (!objectId) {
    return false
  }

  if (isScopedObjectId(objectId)) {
    const [instanceId, sourceId] = descopeObjectId(objectId)
    const childObject = scene.objects[instanceId]?.instanceData?.children?.[sourceId]
    if (!childObject) {
      return false
    }

    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    const {id, components, deletions, ...overrides} = childObject
    if (deletions && Object.keys(deletions).length > 0) {
      return true
    }
    if (components && Object.keys(components).length > 0) {
      return true
    }
    if (
      overrides && Object.values(overrides).some(value => (value !== null && value !== undefined))
    ) {
      return true
    }
    return false
  }
  const object = scene.objects[objectId]
  if (object && isPrefabInstance(object)) {
    const {
      // eslint-disable-next-line @typescript-eslint/no-unused-vars
      instanceData, id, position, rotation, scale, name, parentId, components, order, ...overrides
    } = object
    return !!Object.keys(overrides).length || !!Object.keys(components).length ||
      !!Object.keys(object.instanceData?.deletions ?? {}).length
  }
  return false
}

type NonDirectComponent = {type: 'nonDirect', name: string}

type ComponentData = (keyof BaseGraphObject | NonDirectComponent)[]

const componentExists = (
  object: DeepReadonly<PrefabInstanceChildrenData>, name: string, isNotDirect?: boolean
) => (
  isNotDirect
    ? !!Object.values(object.components ?? []).filter(c => c.name === name).length
    : !!object[name]
)

// Return true if each object in objectIds has at least one of the componentNames overridden
const areComponentsOverridden = (
  scene: DeepReadonly<SceneGraph>,
  objectIds: DeepReadonly<string[]>,
  componentData: ComponentData
): boolean => {
  if (!objectIds || objectIds.length === 0 || !componentData || componentData.length === 0) {
    return false
  }

  return objectIds.every((objectId) => {
    if (!objectId) {
      return false
    }

    return componentData.some((component) => {
      if (isScopedObjectId(objectId)) {
        const [instanceId, sourceId] = descopeObjectId(objectId)
        const childObject = scene.objects[instanceId]?.instanceData?.children?.[sourceId]
        if (!childObject) {
          return false
        }
        return componentExists(
          childObject,
          typeof component === 'object' ? component.name : component,
          typeof component === 'object'
        )
      }

      const object = scene.objects[objectId]
      if (object && isPrefabInstance(object)) {
        return componentExists(
          object,
          typeof component === 'object' ? component.name : component,
          typeof component === 'object'
        )
      }
      return false
    })
  })
}

const listAllPrefabDependencies = (
  scene: DeepReadonly<SceneGraph>, derivedScene: DerivedScene, targetId: string,
  dependencies: Set<string> = new Set(), dependenciesChecked: Set<string> = new Set()
): Set<string> => {
  if (!scene.objects[targetId] || !isPrefab(scene.objects[targetId])) {
    throw new Error(`Object ${targetId} is not a prefab`)
  }

  dependenciesChecked.add(targetId)

  const collectDependencies = (id: string) => {
    const children = derivedScene.getChildren(id)
    children.forEach((childId) => {
      const child = scene.objects[childId]
      if (child && isPrefabInstance(child)) {
        dependencies.add(child.instanceData.instanceOf)
      }
      collectDependencies(childId)
    })
  }

  collectDependencies(targetId)

  // recursively check for more dependencies
  dependencies.forEach((p) => {
    if (p !== targetId && !dependenciesChecked.has(p)) {
      dependenciesChecked.add(p)
      listAllPrefabDependencies(scene, derivedScene, p, dependencies, dependenciesChecked)
    }
  })

  return dependencies
}

const findPrefabDependencyCycle = (
  scene: DeepReadonly<SceneGraph>, derivedScene: DerivedScene, targetId: string, newParentId: string
): boolean => {
  const newParentObject = scene.objects[newParentId]
  const targetObject = scene.objects[targetId]

  const newParentPrefabId = derivedScene.getParentPrefabId(newParentId)
  const targetPrefabId = derivedScene.getParentPrefabId(targetId)

  const targetInstanceId = targetObject?.instanceData?.instanceOf
  const newParentInstanceId = newParentObject?.instanceData?.instanceOf

  const isTargetPrefab = isPrefab(targetObject)

  // if the root of the prefab is being moved into its own tree
  const isSamePrefabTree = targetId === newParentPrefabId && isTargetPrefab

  const isSameTargetInstance = targetInstanceId && targetInstanceId === newParentPrefabId
  const isSameParentInstance = newParentInstanceId && newParentInstanceId === targetPrefabId

  if (isSamePrefabTree || isSameTargetInstance || isSameParentInstance) {
    return true
  }

  const rootTargetPrefabId = targetPrefabId || targetInstanceId
  if (!newParentObject || !rootTargetPrefabId) {
    return false
  }

  const dependencies = listAllPrefabDependencies(scene, derivedScene, rootTargetPrefabId)

  if (isPrefabInstance(newParentObject) && newParentObject.instanceData?.instanceOf) {
    return dependencies.has(newParentObject.instanceData.instanceOf)
  }

  if (newParentPrefabId) {
    return dependencies.has(newParentPrefabId)
  }

  return dependencies.has(newParentId)
}

const unlinkPrefabInstanceChildren = (
  derivedScene: DerivedScene, objectId: string, parentId: string,
  objects: Record<string, DeepReadonly<GraphObject>>
) => {
  const children = derivedScene.getChildren(objectId)
  children.forEach((childId) => {
    if (!isScopedObjectId(childId)) {
      return
    }

    const mergedObject = derivedScene.getObject(childId)

    if (!mergedObject) {
      return
    }

    const newId = uuid()
    const newObject = {
      ...mergedObject,
      id: newId,
      parentId,
    }

    objects[newId] = newObject
    unlinkPrefabInstanceChildren(derivedScene, childId, newId, objects)
  })
}

const unlinkPrefabInstance = (
  scene: DeepReadonly<SceneGraph>, objectId: string
): DeepReadonly<SceneGraph> => {
  const object = scene.objects[objectId]
  if (!object || !isPrefabInstance(object)) {
    return scene
  }
  const derivedScene = deriveScene(scene)

  const mergedObject = derivedScene.getObject(objectId)

  const newObjects = {
    ...scene.objects,
    [objectId]: mergedObject,
  }

  unlinkPrefabInstanceChildren(derivedScene, objectId, mergedObject.id, newObjects)

  return {
    ...scene,
    objects: newObjects,
  }
}

const resetComponents = (
  scene: DeepReadonly<SceneGraph>, objectId: string, mutableInstance: GraphObject,
  componentIds: string[], nonDirect = false
) => {
  const isInstanceChild = isScopedObjectId(objectId)
  const sourceObjectId = getSourceObjectId(scene, objectId)
  const childOverrides = mutableInstance.instanceData?.children?.[sourceObjectId]

  if ((isInstanceChild && !childOverrides)) {
    return
  }

  const instance = isInstanceChild ? childOverrides : mutableInstance

  if (nonDirect) {
    componentIds.forEach((id) => {
      delete instance.components?.[id]
    })
    return
  }

  componentIds.forEach((id) => {
    // Note: if the instance is a root instance, we should not delete the position component
    if (!isInstanceChild && id === POSITION_COMPONENT) {
      return
    }
    delete instance[id]
  })

  const handleMeshReset = () => {
    if (!isInstanceChild) {
      if (mutableInstance.instanceData && mutableInstance.instanceData.deletions) {
        const filteredDeletions = Object.fromEntries(
          Object.entries(mutableInstance.instanceData.deletions)
            .filter(([key]) => !MESH_COMPONENTS.includes(key))
        )
        mutableInstance.instanceData.deletions = filteredDeletions
      }
    } else if (childOverrides.deletions) {
      const filteredDeletions = Object.fromEntries(
        Object.entries(childOverrides.deletions)
          .filter(([key]) => !MESH_COMPONENTS.includes(key))
      )
      childOverrides.deletions = filteredDeletions
    }
  }

  // NOTE: To properly reset mesh components, we need to ensure that we reset all related
  // deletions from that component
  if (componentIds.some(id => MESH_COMPONENTS.includes(id))) {
    handleMeshReset()
  }
}

const resetPrefabInstanceComponents = (
  scene: DeepReadonly<SceneGraph>, instanceId: string, components: string | string[],
  nonDirect: boolean = false
): DeepReadonly<SceneGraph> => {
  const isInstanceChild = isScopedObjectId(instanceId)
  const descopedId = descopeObjectId(instanceId)
  const instance = !isInstanceChild
    ? scene.objects[instanceId]
    : scene.objects[descopedId[0]]

  if (!instance) {
    throw new Error(`Prefab instance ${instanceId} not found`)
  }

  const mutableInstance = {...instance} as GraphObject
  const componentIds = Array.isArray(components) ? components : [components]
  resetComponents(scene, instanceId, mutableInstance, componentIds, nonDirect)

  const newObjects = {
    ...scene.objects,
    [mutableInstance.id]: {
      ...mutableInstance,
    },
  }

  return {
    ...scene,
    objects: {
      ...newObjects,
    },
  }
}

const unlinkAllPrefabInstances = (scene: DeepReadonly<SceneGraph>, prefabId: string) => {
  const instances: string[] = []
  Object.values(scene.objects).forEach((object) => {
    if (object.instanceData?.instanceOf === prefabId) {
      instances.push(object.id)
    }
  })

  let newScene = scene
  instances.forEach((instanceId) => {
    newScene = unlinkPrefabInstance(newScene, instanceId)
  })

  return newScene
}

const applyComponentOverrides = (
  componentIds: string[],
  mergedObject: DeepReadonly<BaseGraphObject>,
  validEntityIds: Set<string>,
  rootInstanceId: string,
  prefabId: string,
  targetPrefabObject: any
) => {
  componentIds.forEach((componentId) => {
    const overriddenComponent = mergedObject.components[componentId]
    if (!overriddenComponent) return

    const cleanedComponent = cleanComponentEntityRefs(overriddenComponent, validEntityIds)
    const remappedComponent = {...cleanedComponent}

    Object.entries(remappedComponent.parameters).forEach(([key, parameter]) => {
      if (
        parameter && typeof parameter === 'object' && parameter.type === 'entity' && parameter.id
      ) {
        let remappedId = parameter.id

        if (parameter.id === rootInstanceId) {
          remappedId = prefabId
        } else if (
          isScopedObjectId(parameter.id) && parameter.id.startsWith(`${rootInstanceId}/`)
        ) {
          const [, sourceId] = descopeObjectId(parameter.id)
          remappedId = sourceId
        }

        if (remappedId !== parameter.id) {
          remappedComponent.parameters[key] = {type: 'entity', id: remappedId}
        }
      }
    })

    targetPrefabObject.components = {
      ...targetPrefabObject.components,
      [componentId]: remappedComponent,
    }
  })
}

const applyPrefabOverridesToRoot = (
  scene: DeepReadonly<SceneGraph>, instanceId: string, components: string | string[],
  nonDirect: boolean = false
): DeepReadonly<SceneGraph> => {
  // TODO: Handle nested prefab instances. Should we apply overrides to the root prefab or the
  // direct parent instance?
  const isInstanceChild = isScopedObjectId(instanceId)
  const [, childSourceId] = descopeObjectId(instanceId)
  const instance = isInstanceChild ? scene.objects[childSourceId] : scene.objects[instanceId]

  if (!instance) {
    throw new Error(`Prefab instance ${instanceId} not found`)
  }

  if (!isPrefabInstance(instance) && !isInstanceChild) {
    throw new Error(`Object ${instanceId} is not a prefab instance`)
  }

  const componentIds = Array.isArray(components) ? components : [components]
  let prefabId: string
  let sourceObjectId: string

  if (isInstanceChild) {
    sourceObjectId = childSourceId
    const [rootInstanceId] = descopeObjectId(instanceId)
    const rootInstance = scene.objects[rootInstanceId]
    if (!rootInstance || !isPrefabInstance(rootInstance)) {
      throw new Error(`Root instance ${rootInstanceId} not found or not a prefab instance`)
    }
    prefabId = rootInstance.instanceData.instanceOf
  } else {
    prefabId = instance.instanceData.instanceOf
    sourceObjectId = prefabId
  }

  const prefab = scene.objects[prefabId]
  if (!prefab) {
    throw new Error(`Prefab ${prefabId} not found`)
  }

  const derivedScene = deriveScene(scene)
  const mergedObject = derivedScene.getObject(instanceId)
  if (!mergedObject) {
    throw new Error(`Could not get merged object for ${instanceId}`)
  }

  const rootInstanceId = isInstanceChild ? instanceId.split('/')[0] : instanceId
  const instanceDescendants = derivedScene.getDescendantObjectIds(rootInstanceId)
  const validEntityIds = new Set([rootInstanceId, ...instanceDescendants])

  const targetPrefabId = isInstanceChild ? sourceObjectId : prefabId
  const targetPrefabObject = isInstanceChild
    ? {...scene.objects[sourceObjectId]}
    : {...prefab}

  if (nonDirect) {
    applyComponentOverrides(
      componentIds, mergedObject, validEntityIds, rootInstanceId, prefabId, targetPrefabObject
    )
  } else {
    // Handle direct property overrides
    (componentIds as (keyof BaseGraphObject)[]).forEach((componentKey) => {
      if (componentKey in mergedObject) {
        (targetPrefabObject as any)[componentKey] = mergedObject[componentKey]
      }
    })
  }

  let newScene = {
    ...scene,
    objects: {
      ...scene.objects,
      [targetPrefabId]: targetPrefabObject,
    },
  }

  newScene = resetPrefabInstanceComponents(newScene, instanceId, components, nonDirect)

  return newScene
}

export {
  createInstanceObject,
  makePrefabAndReplaceWithInstance,
  makeInstance,
  makePrefab,
  prefabContainsLight,
  isOverriddenInstance,
  areComponentsOverridden,
  findPrefabDependencyCycle,
  unlinkPrefabInstance,
  unlinkAllPrefabInstances,
  resetPrefabInstanceComponents,
  applyPrefabOverridesToRoot,
}

export type {
  NonDirectComponent,
  ComponentData,
}
