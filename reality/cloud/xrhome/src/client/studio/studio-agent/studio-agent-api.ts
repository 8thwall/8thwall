/* eslint-disable local-rules/hardcoded-copy */
import type {DeepReadonly} from 'ts-essentials'
import {v4 as uuid} from 'uuid'
import type {TFunction} from 'react-i18next'
import type {
  GetObjectsPropertyRequest, GetObjectsPropertyResponse,
  SetObjectsPropertyRequest, SetObjectsPropertyResponse,
  CreateObjectsRequest, CreateObjectsResponse,
  GetAvailableComponentsResponse, ApplyComponentsRequest, ApplyComponentsResponse,
  UpdateComponentsRequest, RemoveComponentsRequest, UpdateComponentsResponse,
  RemoveComponentsResponse,
} from '@studiomcp/schema/scene-tools'
import type {StudioComponentData} from '@ecs/shared/studio-component'
import type {BaseGraphObject, SceneGraph, Space, InputMap, Action} from '@ecs/shared/scene-graph'
import {isPrefab, isPrefabInstance} from '@ecs/shared/object-hierarchy'
import {parseComponentAst} from '@ecs/shared/parse-component-ast'
import {DEFAULT_ACTION_MAP} from '@ecs/shared/default-action-maps'

import {makeObjectName} from '../common/studio-files'
import {deleteObjects} from '../configuration/delete-object'
import {canDuplicateObjects, duplicateObjects} from '../configuration/duplicate-object'
import {makeEmptyObject, makePrimitive} from '../make-object'
import {handleObjectReorder} from '../reparent-object'
import type {SceneContext} from '../scene-context'
import type {StudioStateContext} from '../studio-state-context'
import type {StudioAgentStateContext} from './studio-agent-context'
import {createIdSeed} from '../id-generation'
import {computeCentroid} from './centroid'
import {getPathValues, setPathValues} from './path-value'
import {validateIds} from './validate-ids'
import type {IStudioComponentsContext} from '../studio-components-context'
import {BUILTIN_COMPONENT_SCHEMA} from '../generated-schema'
import {DIRECT_PROPERTY_COMPONENTS} from '../hooks/available-components'
import {deriveScene, type DerivedScene} from '../derive-scene'
import {getActiveRoot} from '../../hooks/use-active-root'
import type {useAuthFetch} from '../../hooks/use-auth-fetch'
import {makePrefabAndReplaceWithInstance, unlinkPrefabInstance} from '../configuration/prefab'
import {extractProjectFileLists} from '../../editor/files/extract-file-lists'
import type {RepoState} from '../../git/git-redux-types'
import type {IImageTarget} from '../../common/types/models'
import {duplicateSpace} from '../configuration/duplicate-space'
import {addSpace, switchSpace} from '../spaces'
import {makeDuplicatedSpaceName} from '../common/studio-files'
import {deleteSpace} from '../configuration/delete-object'
import {getInputMap} from '../configuration/input-action-map-dropdown'
import {validateInput} from '../configuration/input-strings'

type ComponentSchemaField = {
  name: string
  type: string
  default?: any
}
type ComponentSchema = {
  name: string
  schema: ComponentSchemaField[]
}

const toComponentSchema = (compSchema: DeepReadonly<StudioComponentData>): ComponentSchema => ({
  name: compSchema.name,
  schema: Object.entries(compSchema.schema).map(([schemaName, schemaType]) => ({
    name: schemaName,
    type: schemaType,
    default: compSchema.schemaDefaults?.[schemaName],
  })),
})

// TODO: Return a scene diff on all setters.
const makeBaseApi = (
  sceneCtx: SceneContext,
  derivedScene: DerivedScene,
  studioStateCtx: StudioStateContext,
  studioAgentCtx: StudioAgentStateContext,
  studioComponentCtx: IStudioComponentsContext,
  authFetch: ReturnType<typeof useAuthFetch>,
  accountUuid: string,
  appUuid: string,
  git: RepoState,
  imageTargetsFetch: () => Promise<IImageTarget[]>,
  t: TFunction
) => ({
  // TODO(kyle): Only return objects, filtering out lights, cameras, attachments, etc.
  getCurrentScene: () => sceneCtx.scene,
  createObjects: (
    params: CreateObjectsRequest, requestId: string
  ): CreateObjectsResponse => {
    const {objects} = params
    const errors: Record<string, string> = {}
    const createdObjects = objects.map((o) => {
      const activeRoot = getActiveRoot(sceneCtx, studioStateCtx)
      const parentExists = o.parentId &&
        (sceneCtx.scene.objects[o.parentId] || sceneCtx.scene.spaces?.[o.parentId])
      const parentId = parentExists ? o.parentId : activeRoot.id
      const invalidParentId = o.parentId && !parentExists
      const newObject = o.type === 'empty'
        ? makeEmptyObject(parentId)
        : makePrimitive(parentId, o.type)
      newObject.name = makeObjectName(o.name, sceneCtx.scene, parentId)
      if (invalidParentId) {
        // eslint-disable-next-line max-len
        errors[newObject.id] = `Parent with ID ${o.parentId} not found. Added to active root ${activeRoot.id} instead.`
      }
      return newObject
    })
    sceneCtx.updateScene(scene => ({
      ...scene,
      objects: {
        ...scene.objects,
        ...Object.fromEntries(createdObjects.map(o => [o.id, o])),
      },
    }))
    studioAgentCtx.pushUndoStep({
      requestId,
      focalPoint: computeCentroid(createdObjects.map(o => o.position)),
      affectedSceneIds: createdObjects.map(o => o.id),
    })
    return {createdObjectIds: createdObjects.map(o => o.id), errors}
  },
  getAvailableComponents: (): GetAvailableComponentsResponse => {
    // See the most up-to-date usage in useAvailableComponents.

    // Get the custom components in the code.
    const componentNames = studioComponentCtx.listComponents()
    const customComponents = componentNames.map(name => ({
      name,
      ...studioComponentCtx.getComponentSchema(name),
    }))

    // The built in components might contain components that the object already has.
    // Convert to a component format that the AI has a better grouping for.
    const allComponents = [...customComponents, ...BUILTIN_COMPONENT_SCHEMA].filter(
      component => !component.name.startsWith('debug-') &&
        !DIRECT_PROPERTY_COMPONENTS.includes(component.name as keyof BaseGraphObject)
    )
    // TODO(dat): Should we worry about duplicate components by name?
    // We filter out directly accessible components since the LLM should be able to edit those
    // directly on the scene graph.

    // TODO(dat): Not all components are available on allComponents. Things like 'material',
    // 'model', 'shadow', 'position', 'quaternion', 'scale', 'box-geometry', 'plane-geometry',
    // 'sphere-geometry', 'capsule-geometry', 'cone-geometry', 'cylinder-geometry',
    // 'tetrahedron-geometry', 'circle-geometry', 'velocity'.
    // Some of these are currently exposing via other LLM tools. Technically all missing components
    // can have custom tools built for them. Is that the right approach?

    // TODO(kyle): We should also filter out components that are already on the object.
    const availableComponents = allComponents.map(toComponentSchema)
    return {availableComponents}
  },
  getObjectsProperty: (params: GetObjectsPropertyRequest): GetObjectsPropertyResponse => {
    const {objects} = params
    // TODO(kyle): Set focal point from ID using a separate studio agent context function that does
    // not push onto the undo stack. Getters should not update the undo stack.
    validateIds(sceneCtx, objects.map(o => o.id))
    return {
      objects: objects.map(o => ({
        id: o.id,
        pathValues: o.paths.length > 0
          ? getPathValues(derivedScene.getObject(o.id), o.paths)
          : derivedScene.getObject(o.id),
      })),
    }
  },
  setObjectsProperty: (
    params: SetObjectsPropertyRequest,
    requestId: string
  ): SetObjectsPropertyResponse => {
    const {objects} = params
    const ids = validateIds(sceneCtx, objects.map(o => o.id))
    objects.forEach(o => sceneCtx.updateObject(
      o.id,
      oldObject => setPathValues(oldObject, o.pathValues)
    ))
    const updatedObjects = ids.map(id => derivedScene.getObject(id))
    studioAgentCtx.pushUndoStep({
      requestId,
      focalPoint: computeCentroid(updatedObjects.map(o => o.position)),
      affectedSceneIds: [...ids],
    })
    return {updatedObjectIds: ids}
  },
  setObjectParent: (
    {objectId, newParentId}: {objectId: string, newParentId: string},
    requestId: string
  ) => {
    const object = derivedScene.getObject(objectId)
    if (!object) {
      throw new Error(`Child objectId ${objectId} not found`)
    }
    const parentObject = derivedScene.getObject(newParentId)
    if (!parentObject) {
      throw new Error(`Parent objectId ${newParentId} not found`)
    }
    studioStateCtx.setSelection(newParentId)
    handleObjectReorder(
      sceneCtx, studioStateCtx, derivedScene, objectId, {position: 'within', from: newParentId}
    )
    studioAgentCtx.pushUndoStep({
      requestId,
      focalPoint: object.position,
      affectedSceneIds: [objectId],
    })
    return {parentObject}
  },
  deleteObjects: ({objectIds}: { objectIds: string[] }) => {
    sceneCtx.updateScene(scene => deleteObjects(scene, objectIds))
    // Do we want to store the undo step to where these objects used to be?
    return {objectIds}
  },
  getRandomColor: () => {
    const letters = '0123456789ABCDEF'
    let color = '#'
    for (let i = 0; i < 6; i++) {
      color += letters[Math.floor(Math.random() * 16)]
    }
    return {color}
  },
  duplicateObjects: ({objectIds}: { objectIds: string[] }) => {
    if (!canDuplicateObjects(derivedScene, objectIds)) {
      throw new Error('Cannot duplicate objects with POI locations')
    }
    const seed = createIdSeed()
    const newObjectIds = objectIds.map(seed.fromId)
    let newScene = sceneCtx.scene
    sceneCtx.updateScene((scene) => {
      newScene = duplicateObjects({scene, derivedScene, objectIds, ids: seed})
      return newScene
    })
    // TODO(dat): We should store the undo step to where these objects were
    return {duplicatedObjects: newObjectIds.map(id => newScene.objects[id])}
  },
  applyComponents: (
    params: ApplyComponentsRequest,
    requestId: string
  ): ApplyComponentsResponse => {
    const objectIds = Object.keys(params.componentsByObjectId)
    const ids = validateIds(sceneCtx, objectIds)
    const updatedObjects = ids.map(id => derivedScene.getObject(id))
    const appliedComponents = {}

    ids.forEach((id) => {
      appliedComponents[id] = []
      const components = params.componentsByObjectId[id]
      const currentObject = derivedScene.getObject(id)
      const newComponents = {...currentObject.components}

      components.forEach((component) => {
        const {componentName, parameters} = component
        const componentId = uuid()
        newComponents[componentId] = {id: componentId, name: componentName, parameters}
        appliedComponents[id].push({componentId, componentName, parameters})
      })

      sceneCtx.updateObject(id, oldObject => ({...oldObject, components: newComponents}))
    })

    studioAgentCtx.pushUndoStep({
      requestId,
      focalPoint: computeCentroid(updatedObjects.map(o => o.position)),
      affectedSceneIds: ids,
    })

    return {appliedComponents}
  },
  updateComponents: (
    params: UpdateComponentsRequest,
    requestId: string
  ): UpdateComponentsResponse => {
    const objectIds = Object.keys(params.componentsByObjectId)
    const ids = validateIds(sceneCtx, objectIds)
    const updatedObjects = ids.map(id => derivedScene.getObject(id))
    const updatedComponents = {}

    ids.forEach((id) => {
      updatedComponents[id] = []
      const components = params.componentsByObjectId[id]
      const currentObject = derivedScene.getObject(id)
      const newComponents = {...currentObject.components}

      components.forEach((component) => {
        const {componentId, parameters} = component
        const existingComponent = newComponents[componentId]

        if (existingComponent) {
          newComponents[componentId] = {
            ...existingComponent,
            parameters: {...existingComponent.parameters, ...parameters},
          }
          updatedComponents[id].push({componentId, parameters})
        }
      })

      sceneCtx.updateObject(id, oldObject => ({
        ...oldObject,
        components: newComponents,
      }))
    })

    studioAgentCtx.pushUndoStep({
      requestId,
      focalPoint: computeCentroid(updatedObjects.map(o => o.position)),
      affectedSceneIds: ids,
    })

    return {updatedComponents}
  },
  removeComponents: (
    params: RemoveComponentsRequest,
    requestId: string
  ): RemoveComponentsResponse => {
    const objectIds = Object.keys(params.componentsByObjectId)
    const ids = validateIds(sceneCtx, objectIds)
    const updatedObjects = ids.map(id => derivedScene.getObject(id))
    const removedComponents = {}

    ids.forEach((id) => {
      removedComponents[id] = []
      const componentIds = params.componentsByObjectId[id]
      const currentObject = derivedScene.getObject(id)
      const newComponents = {...currentObject.components}
      componentIds.forEach((componentId) => {
        delete newComponents[componentId]
        removedComponents[id].push(componentId)
      })
      sceneCtx.updateObject(id, oldObject => ({...oldObject, components: newComponents}))
    })

    studioAgentCtx.pushUndoStep({
      requestId,
      focalPoint: computeCentroid(updatedObjects.map(o => o.position)),
      affectedSceneIds: ids,
    })

    return {removedComponents}
  },
  getRealtimeToken: () => authFetch<{token: string}>(`/v1/realtime/access/${accountUuid}`),
  getAiApiToken: () => authFetch<{token: string}>(`/v1/ai-api/access/${accountUuid}/${appUuid}`),
  makePrefabs: ({objectIds}: { objectIds: string[] }, requestId: string) => {
    const ids = validateIds(sceneCtx, objectIds)
    let currentScene = sceneCtx.scene
    const newPrefabs: Array<{instanceId: string, prefabSourceId: string}> = []
    const objectIdsCannotBeMadePrefabs: string[] = []

    ids.forEach((id) => {
      const object = currentScene.objects[id]
      const isPartOfPrefab = !!derivedScene.getParentPrefabId(id)
      const targetIsPrefab = object && isPrefab(object)
      const canBecomePrefab = object && !targetIsPrefab &&
        !isPartOfPrefab && !isPrefabInstance(object)
      if (!canBecomePrefab) {
        objectIdsCannotBeMadePrefabs.push(id)
        return
      }
      const seed = createIdSeed()
      currentScene = makePrefabAndReplaceWithInstance(currentScene, id, seed)
      newPrefabs.push({
        instanceId: id,
        prefabSourceId: seed.fromId(id),
      })
    })

    sceneCtx.updateScene(() => currentScene)

    const affectedIds = newPrefabs.map(prefab => prefab.instanceId)
    if (affectedIds.length) {
      studioAgentCtx.pushUndoStep({
        requestId,
        focalPoint: computeCentroid(affectedIds.map(id => derivedScene.getObject(id).position)),
        affectedSceneIds: affectedIds,
      })
    }

    return {newPrefabs, objectIdsCannotBeMadePrefabs}
  },
  unlinkPrefabInstances: ({instanceIds}: {instanceIds: string[]}, requestId: string) => {
    const ids = validateIds(sceneCtx, instanceIds)

    if (!ids.length) {
      return {unlinkedInstanceIds: []}
    }

    const focalPoint = computeCentroid(ids.map(id => derivedScene.getObject(id).position))
    let currentScene = sceneCtx.scene

    ids.forEach((id) => {
      currentScene = unlinkPrefabInstance(currentScene, id)
    })

    sceneCtx.updateScene(() => currentScene)

    studioAgentCtx.pushUndoStep({
      requestId,
      focalPoint,
      affectedSceneIds: ids,
    })

    return {unlinkedInstanceIds: ids}
  },
  deletePrefabs: (
    {prefabIds, deleteInstances}: {prefabIds: string[], deleteInstances: boolean},
    requestId: string
  ) => {
    const idsToDelete = prefabIds.filter(id => sceneCtx.scene.objects[id] &&
      isPrefab(sceneCtx.scene.objects[id]))
    if (!idsToDelete.length) {
      return {deletedPrefabIds: [], affectedInstanceIds: []}
    }

    const focalPoint = computeCentroid(idsToDelete.map(id => derivedScene.getObject(id).position))
    let currentScene = sceneCtx.scene
    const affectedSceneIds: string[] = []

    idsToDelete.forEach((prefabId) => {
      Object.values(currentScene.objects).forEach((object) => {
        if (object.instanceData?.instanceOf === prefabId) {
          affectedSceneIds.push(object.id)
        }
      })

      if (deleteInstances) {
        currentScene = deleteObjects(currentScene, affectedSceneIds)
      } else {
        affectedSceneIds.forEach((instanceId) => {
          currentScene = unlinkPrefabInstance(currentScene, instanceId)
        })
      }
    })

    currentScene = deleteObjects(currentScene, idsToDelete)
    sceneCtx.updateScene(() => currentScene)

    studioAgentCtx.pushUndoStep({
      requestId,
      focalPoint,
      affectedSceneIds,
    })

    return {
      deletedPrefabIds: idsToDelete,
      affectedInstanceIds: affectedSceneIds,
    }
  },
  validateComponents: (
    {fileContents}: {fileContents: {filePath: string, content: string}[]}
  ) => {
    const errors: {filePath: string, errors: string[]}[] = []
    fileContents.forEach(({filePath, content}) => {
      const result = parseComponentAst(content)
      const messages: string[] = []
      result.errors.forEach((error) => {
        const location = error.location
          ? `[${error.location.startLine}:${error.location.startColumn}]`
          : ''
        messages.push(`${location}${error.severity}: ${error.message}`)
      })
      errors.push({filePath, errors: messages})
    })
    return errors
  },
  getAssets: () => {
    // NOTE(chloe): In the future, we are considering drawing images for 3D models to provide more
    // context to the agent into the assets. This is especially helpful for asset lab models
    // which lack descriptive names (and--by extension--filePaths).
    const files = extractProjectFileLists(git, () => false)
    return {filePaths: files.assets.filePaths}
  },
  getImageTargets: async () => {
    const imageTargets = await imageTargetsFetch()
    // NOTE(chloe): Returning more image target data here to provide the agent with more context
    // for when it may update the image target later.
    const imageTargetData = imageTargets.map(it => ({
      name: it.name,
      type: it.type,
      loadAutomatically: it.loadAutomatically,
      isRotated: it.isRotated,
      metaData: it.metadata,
      userMetadata: it.userMetadata,
      userMetadataIsJson: it.userMetadataIsJson,
    }))
    return {imageTargets: imageTargetData}
  },
  duplicateSpaces: ({spaceIds}: {spaceIds: string[]}) => {
    let currentScene = sceneCtx.scene
    if (!currentScene.spaces) {
      return {error: 'Failed to duplicate spaces: No spaces found in current scene.'}
    }
    spaceIds.forEach((id) => {
      const seed = createIdSeed()
      currentScene = duplicateSpace(currentScene, id, seed)
    })
    sceneCtx.updateScene(() => currentScene)
    return {updatedSpaces: currentScene.spaces}
  },
  addSpaces: ({names}: {names: string[]}) => {
    let currentScene = sceneCtx.scene
    let lastAddedId: string | undefined
    names.forEach((name) => {
      const {spaces: spacesById} = currentScene
      const spaces = spacesById ? Object.values(spacesById) : []
      const nameExists = spaces.some(space => space.name === name)
      // NOTE(chloe): Handle duplicate names by creating a unique name instead of returning errors
      // to reduce back-and-forth between studio-use and agent.
      const spaceName = nameExists ? makeDuplicatedSpaceName(name, spaces) : name
      lastAddedId = uuid()
      currentScene = addSpace(currentScene, spaceName, t, lastAddedId)
    })
    sceneCtx.updateScene(() => currentScene)
    if (lastAddedId) {
      switchSpace(studioStateCtx, deriveScene(currentScene), lastAddedId)
    }
    return {updatedSpaces: currentScene.spaces}
  },
  getActiveRoot: () => getActiveRoot(sceneCtx, studioStateCtx),
  deleteSpaces: ({spaceIds}: {spaceIds: string[]}) => {
    let currentScene = sceneCtx.scene
    if (!currentScene.spaces) {
      return {error: 'Failed to delete spaces: No spaces found in current scene.'}
    }
    const invalidSpaceIds: string[] = []
    spaceIds.forEach((id) => {
      if (!currentScene.spaces[id]) {
        invalidSpaceIds.push(id)
        return
      }
      currentScene = deleteSpace(currentScene, id)
    })
    let {entrySpaceId} = currentScene
    if (!entrySpaceId) {
      entrySpaceId = uuid()
      currentScene = addSpace(
        currentScene,
        t('studio_agent.delete_spaces.default_space'),
        t,
        entrySpaceId
      )
    }
    sceneCtx.updateScene(() => ({
      ...currentScene,
      entrySpaceId,
    }))
    return {updatedSpaces: currentScene.spaces, invalidSpaceIds}
  },
  setSpaceProperties: ({spaces}: {spaces: Space[]}) => {
    const currentSpaces = sceneCtx.scene.spaces
    if (!currentSpaces) {
      return {error: 'Failed to set space properties: No spaces found in current scene.'}
    }
    const {validSpaces, invalidSpaceIds} = spaces.reduce((acc, space) => {
      if (currentSpaces[space.id]) {
        acc.validSpaces.push(space)
      } else {
        acc.invalidSpaceIds.push(space.id)
      }
      return acc
    }, {validSpaces: [] as Space[], invalidSpaceIds: [] as string[]})

    const updatedSpaces = {
      ...currentSpaces,
      ...Object.fromEntries(validSpaces.map(({id, ...rest}) => [
        id, {...currentSpaces[id], ...rest},
      ])),
    }

    sceneCtx.updateScene(scene => ({...scene, spaces: updatedSpaces}))
    return {updatedSpaces, invalidSpaceIds}
  },
  // NOTE(chloe): Only picking entrySpaceId and activeMap since other expanse properties, such as
  // activeCamera, sky, and reflections, are now per-space properties and only exist at the expanse
  // level for backwards compatibility or already have more specific tools which have more tailored
  // logic for their respective updates.
  setExpanseProperties: ({properties}: {
    properties: Pick<SceneGraph, 'entrySpaceId' | 'activeMap'>
  }) => {
    let updatedExpanse = sceneCtx.scene
    sceneCtx.updateScene((scene) => {
      updatedExpanse = {...scene, ...properties}
      return updatedExpanse
    })
    return {updatedExpanse}
  },
  // NOTE(chloe): Naming actionMaps instead of inputMaps because "action map" is used in our
  // docs: https://www.8thwall.com/docs/api/studio/world/input/
  upsertActionMaps: ({actionMaps}: {actionMaps: InputMap}) => {
    const updatedInputs = {...getInputMap(sceneCtx.scene.inputs)}
    const errorsByMap: Record<string, string[]> = {}
    const validateActions = (actions: Action[]) => actions.reduce((acc, action) => {
      const {name: actionName, bindings} = action
      if (!actionName || acc.validActions.find(addedAction => addedAction.name === actionName)) {
        acc.errors.push(`Failed to add action ${actionName}: Duplicate or missing action name`)
        return acc
      }
      action.bindings = bindings.filter((binding) => {
        const {input, modifiers} = binding
        if (!validateInput(input)) {
          // eslint-disable-next-line max-len
          acc.errors.push(`Failed to add a binding for action ${actionName}: Input '${input}' cannot be found in input list`)
          return false
        }
        if (modifiers) {
          const invalidModifiers = modifiers.filter(modifier => !validateInput(modifier))
          if (invalidModifiers.length > 0) {
            // eslint-disable-next-line max-len
            acc.errors.push(`Failed to add a binding for action ${actionName}: Modifiers '${invalidModifiers.join(', ')}' cannot be found in input list`)
            return false
          }
        }
        return true
      })
      acc.validActions.push(action)
      return acc
    }, {validActions: [] as Action[], errors: [] as string[]})

    Object.entries(actionMaps).forEach(([name, actions]) => {
      const {validActions, errors} = validateActions(actions)
      updatedInputs[name] = validActions
      if (errors.length) {
        errorsByMap[name] = errors
      }
    })

    sceneCtx.updateScene(scene => ({...scene, inputs: updatedInputs}))
    return {updatedInputs, errorsByMap}
  },
  deleteActionMaps: ({actionMapNames}: {actionMapNames: string[]}) => {
    if (!sceneCtx.scene.inputs) {
      return {error: 'Failed to delete action maps: No action maps found in current scene.'}
    }
    const updatedInputs = {...getInputMap(sceneCtx.scene.inputs)}
    const invalidActionMapNames: string[] = []
    sceneCtx.updateScene((scene) => {
      actionMapNames.forEach((name) => {
        if (name === DEFAULT_ACTION_MAP) {
          invalidActionMapNames.push(name)
          return
        }
        if (!updatedInputs[name]) {
          invalidActionMapNames.push(name)
          return
        }
        delete updatedInputs[name]
      })
      const activeMapDeleted = scene.activeMap && actionMapNames.includes(scene.activeMap)
      const activeMap = activeMapDeleted ? {activeMap: DEFAULT_ACTION_MAP} : {}
      return {...scene, inputs: updatedInputs, ...activeMap}
    })
    return {updatedInputs, invalidActionMapNames}
  },
})

const makeMetaApi = (baseApi: ReturnType<typeof makeBaseApi>) => ({
  ...baseApi,
  batch: async ({requests}: {
    requests: {action: string, parameters: any}[]
  }) => {
    const api = makeMetaApi(baseApi)
    const responses = await Promise.all(
      requests.map(r => api[r.action](r.parameters))
    )
    return {responses}
  },
})

const makeStudioAgentApi = (
  sceneCtx: SceneContext,
  derivedScene: DerivedScene,
  studioStateCtx: StudioStateContext,
  studioAgentCtx: StudioAgentStateContext,
  studioComponentCtx: IStudioComponentsContext,
  authFetch: ReturnType<typeof useAuthFetch>,
  accountUuid: string,
  appUuid: string,
  git: RepoState,
  imageTargetsFetch: () => Promise<IImageTarget[]>,
  t: TFunction
) => makeMetaApi(makeBaseApi(
  sceneCtx, derivedScene, studioStateCtx, studioAgentCtx, studioComponentCtx, authFetch,
  accountUuid, appUuid, git, imageTargetsFetch, t
))

export {
  makeStudioAgentApi,
}
