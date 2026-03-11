import type {DeepReadonly} from 'ts-essentials'
import type {
  GraphComponent, GraphObject, SceneGraph, EntityReference,
} from '@ecs/shared/scene-graph'

const cleanComponentEntityRefs = (
  component: GraphComponent, validEntityIds: Set<string>
): GraphComponent => {
  const cleanedParameters: Record<string, string | number | boolean | EntityReference> = {}

  Object.keys(component.parameters).forEach((key) => {
    const parameter = component.parameters[key]
    const entityRef = parameter as EntityReference

    // TODO(dale): Show an error or warning if an entity ref is being cleared
    cleanedParameters[key] = parameter && typeof parameter === 'object' &&
      entityRef.type === 'entity' && entityRef.id && !validEntityIds.has(entityRef.id)
      ? {type: 'entity', id: ''}
      : parameter
  })

  return {
    ...component,
    parameters: cleanedParameters,
  }
}

const updateSchemaRefs = (
  component: GraphComponent, oldToNewEid: Map<string, string>
) => {
  Object.keys(component.parameters).forEach((key) => {
    const parameter = component.parameters[key]
    if (parameter && typeof parameter === 'object' && parameter.type === 'entity' &&
      oldToNewEid.has(parameter.id)) {
      parameter.id = oldToNewEid.get(parameter.id) || parameter.id
    }
  })
}

const replaceObjectRefs = (
  newObject: GraphObject,
  oldToNewEid: Map<string, string>
) => {
  Object.keys(newObject).forEach((key) => {
    const attribute = newObject[key]
    // replace all attributes that have entity references on the first level
    if (!attribute || key === 'components' || typeof attribute !== 'object') {
      return
    }
    Object.keys(attribute).forEach((subKey) => {
      if (attribute[subKey] && attribute[subKey]?.type === 'entity' &&
          oldToNewEid.has(attribute[subKey].id)) {
        const entityRef = attribute[subKey]
        entityRef.id = oldToNewEid.get(entityRef.id) || entityRef.id
      }
    })
  })

  return newObject
}

const createObjectUpdateRefs = (
  currentObject: DeepReadonly<GraphObject>, oldToNewEid: Map<string, string>
) => {
  const objectDeepCopy = JSON.parse(JSON.stringify(currentObject))

  // update component target entity references
  Object.keys(objectDeepCopy.components).forEach((key) => {
    updateSchemaRefs(objectDeepCopy.components[key], oldToNewEid)
  })

  return replaceObjectRefs(objectDeepCopy, oldToNewEid)
}

const updateSceneEntityRefs = (
  scene: DeepReadonly<SceneGraph>,
  objectId: string,
  oldToNewEid: Map<string, string>
) => {
  const targetObject = scene.objects[objectId]
  if (!targetObject) {
    return scene
  }
  const newObject = createObjectUpdateRefs(targetObject, oldToNewEid)
  return {...scene, objects: {...scene.objects, [objectId]: newObject}}
}

export {
  cleanComponentEntityRefs,
  createObjectUpdateRefs,
  updateSceneEntityRefs,
}
