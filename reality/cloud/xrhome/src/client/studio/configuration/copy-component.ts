import type {DeepReadonly} from 'ts-essentials'

import type {GraphComponent, GraphObject, SceneGraph} from '@ecs/shared/scene-graph'
import {MapPointControlledProperties} from '@ecs/shared/map-controller'

import {getInstanceAndScopedId, isScopedObjectId} from '@ecs/shared/object-hierarchy'

import type {StudioStateContext} from '../studio-state-context'
import type {SceneContext} from '../scene-context'
import type {DerivedScene} from '../derive-scene'

const copyDirectProperties = (
  stateCtx: DeepReadonly<StudioStateContext>,
  properties: DeepReadonly<Partial<GraphObject>>
) => {
  stateCtx.update(state => ({
    ...state,
    clipboard: {
      type: 'directProperties',
      properties,
    },
  }))
}

const copyComponent = (
  stateCtx: DeepReadonly<StudioStateContext>,
  component: DeepReadonly<GraphComponent>
) => {
  stateCtx.update(state => ({
    ...state,
    clipboard: {
      type: 'customComponent',
      component,
    },
  }))
}
const pasteDirectPropertiesIntoObject = (
  scene: DeepReadonly<SceneGraph>,
  targetIds: DeepReadonly<string[]>,
  properties: DeepReadonly<Partial<GraphObject>>
) => {
  const newObjects = {...scene.objects}
  // handle non-scoped object ids (normal objects)
  const targetObjects = targetIds.filter(id => !isScopedObjectId(id))
    .map(id => scene.objects[id]).filter(o => o)
  targetObjects.forEach((targetObject) => {
    newObjects[targetObject.id] = {
      ...targetObject,
      ...properties,
    }
  })

  // handle scoped object ids (instance children)
  const scopedIds = targetIds.filter(id => isScopedObjectId(id))
  scopedIds.forEach((scopedId) => {
    const [instanceId, instanceScopedId] = getInstanceAndScopedId(scopedId)
    const instance = scene.objects[instanceId]
    if (!instance) {
      return
    }

    const newInstanceData = {
      ...instance.instanceData,
      children: {
        ...instance.instanceData?.children,
        [instanceScopedId]: {
          ...instance.instanceData?.children?.[instanceScopedId],
          ...properties,
        },
      },
    }

    newObjects[instanceId] = {...instance, instanceData: newInstanceData}
  })

  return {...scene, objects: newObjects}
}

const pasteDirectProperties = (
  ctx: DeepReadonly<SceneContext>,
  targetIds: DeepReadonly<string[]>,
  properties: DeepReadonly<Partial<GraphObject>>
) => {
  ctx.updateScene(scene => pasteDirectPropertiesIntoObject(scene, targetIds, properties))
}

const pasteCustomComponentPropertiesIntoObject = (
  scene: DeepReadonly<SceneGraph>,
  targetIds: DeepReadonly<string[]>,
  component: DeepReadonly<GraphComponent>
) => {
  const newObjects = {...scene.objects}

  const targetObjects = targetIds.filter(id => !isScopedObjectId(id))
    .map(id => scene.objects[id]).filter(o => o)
  targetObjects.forEach((targetObject) => {
    const newComponents = {...targetObject.components}
    const oldId = Object.keys(newComponents).find(
      id => newComponents[id].name === component.name
    )

    const newId = oldId || component.id
    newComponents[newId] = {
      ...component,
      id: newId,
    }

    newObjects[targetObject.id] = {
      ...targetObject,
      components: newComponents,
    }
  })
  const scopedIds = targetIds.filter(id => isScopedObjectId(id))
  scopedIds.forEach((scopedId) => {
    const [instanceId, instanceScopedId] = getInstanceAndScopedId(scopedId)
    const instance = scene.objects[instanceId]
    if (!instance) {
      return
    }

    const newComponents = {...instance.instanceData?.children?.[instanceScopedId]?.components}
    const oldId = Object.keys(newComponents).find(
      componentId => newComponents[componentId].name === component.name
    )

    const newId = oldId || component.id
    newComponents[newId] = {
      ...component,
      id: newId,
    }

    const newInstanceData = {
      ...instance.instanceData,
      children: {
        ...instance.instanceData?.children,
        [instanceScopedId]: {
          ...instance.instanceData?.children?.[instanceScopedId],
          components: newComponents,
        },
      },
    }

    newObjects[instanceId] = {...instance, instanceData: newInstanceData}
  })

  return {...scene, objects: newObjects}
}

const pasteCustomComponent = (
  ctx: DeepReadonly<SceneContext>,
  targetIds: DeepReadonly<string[]>,
  component: DeepReadonly<GraphComponent>
) => {
  ctx.updateScene(scene => pasteCustomComponentPropertiesIntoObject(scene, targetIds, component))
}

const canPasteComponent = (
  stateCtx: DeepReadonly<StudioStateContext>,
  derivedScene: DerivedScene,
  targetIds: DeepReadonly<string[]>
) => {
  // prevent assignment of controlled properties to map points
  const blockPaste = targetIds.some(id => !!derivedScene.getObject(id)?.mapPoint) &&
    MapPointControlledProperties.some(
      key => stateCtx.state.clipboard.type === 'directProperties' &&
      Object.keys(stateCtx.state.clipboard.properties).includes(key)
    )

  return !blockPaste &&
    (stateCtx.state.clipboard.type === 'customComponent' ||
    stateCtx.state.clipboard.type === 'directProperties')
}

const pasteComponent = (
  stateCtx: DeepReadonly<StudioStateContext>,
  ctx: DeepReadonly<SceneContext>,
  targetIds: DeepReadonly<string[]>
) => {
  if (stateCtx.state.clipboard.type === 'directProperties') {
    const {properties} = stateCtx.state.clipboard
    pasteDirectProperties(ctx, targetIds, properties)
  } else if (stateCtx.state.clipboard.type === 'customComponent') {
    const {component} = stateCtx.state.clipboard
    pasteCustomComponent(ctx, targetIds, component)
  }
}

export {
  pasteDirectPropertiesIntoObject,
  pasteCustomComponentPropertiesIntoObject,
  copyDirectProperties,
  copyComponent,
  canPasteComponent,
  pasteComponent,
}
