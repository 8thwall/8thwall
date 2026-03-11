import type {BaseGraphObject} from '@ecs/shared/scene-graph'

import {useStudioComponentsContext} from '../studio-components-context'
import {BUILTIN_COMPONENT_SCHEMA} from '../generated-schema'
import type {SubMenuCategory} from '../ui/submenu'
import {
  ANIMATION_OPTIONS, ComponentCategoryType, LIGHTING_OPTIONS, MESH_OPTIONS, ROOT_OPTIONS,
  CONTROLLER_OPTIONS,
} from '../configuration/new-component-strings'
import {useDerivedScene} from '../derived-scene-context'

const AUDIO_COMPONENT = 'audio'
const VIDEO_COMPONENT = 'video-controls'
const COLLIDER_COMPONENT = 'collider'
const LIGHT_COMPONENT = 'light'
const MESH_COMPONENT = 'mesh'
const TRANSFORM_COMPONENT = 'transform'
const UI_COMPONENT = 'ui'
const PARTICLE_EMITTER_COMPONENT = 'particle-emitter'

// NOTE(jeffha): This list prevents built-in components from showing as a custom component in
//              the playground.
// Note(Dale): ORIGINAL_BUILT_IN_COMPONENTS is for the ecs attribute name, for scene graph
// components use direct-property-components.ts
const ORIGINAL_BUILT_IN_COMPONENTS = new Set([
  AUDIO_COMPONENT,
  COLLIDER_COMPONENT,
  'geometry',
  'camera',
  LIGHT_COMPONENT,
  'material',
  'splat',
  'model',
  'shadow',
  'position',
  'quaternion',
  'scale',
  'box-geometry',
  'plane-geometry',
  'sphere-geometry',
  'capsule-geometry',
  'cone-geometry',
  'cylinder-geometry',
  'tetrahedron-geometry',
  'circle-geometry',
  'polyhedron-geometry',
  'torus-geometry',
  'ring-geometry',
  'face-geometry',
  'velocity',
  UI_COMPONENT,
  'three-object',
  'hidden',
  'disabled',
  'persistent',
  'gltf-model',
  'orbit-controls',
  'fly-controller',
  'image-target',
  'face',
  'location',
  'map',
  'map-point',
  'map-theme',
  'unlit-material',
  'shadow-material',
  'hider-material',
  'video-material',
  VIDEO_COMPONENT,
  ...BUILTIN_COMPONENT_SCHEMA.map(component => component.name),
] as const)

const BUILT_IN_COMPONENTS: Set<string> = ORIGINAL_BUILT_IN_COMPONENTS

type BuiltInComponentName = typeof ORIGINAL_BUILT_IN_COMPONENTS extends Set<infer T> ? T : never

// NOTE(christoph): These are components that are stored directly on GraphObject.
const DIRECT_PROPERTY_COMPONENTS: Array<keyof BaseGraphObject> = [
  'audio',
  'collider',
  'face',
  'light',
  'ui',
  // NOTE(dale): When a direct property is added, it should be added to the exclude for
  // NewComponentValue if the runtime name is different from the scene graph name.
  'imageTarget',
  'videoControls',
]

type BuiltInDirectProperty = typeof DIRECT_PROPERTY_COMPONENTS extends Array<infer T> ? T : never

type Component = {
  value: BuiltInComponentName | BuiltInDirectProperty
  content: string
  isDirectProperty?: boolean
}

type NewComponentValue = Exclude<
  BuiltInComponentName | BuiltInDirectProperty | 'mesh', 'image-target' | 'video-controls'
>

// TODO(christoph): This isn't how it's really going to work, but it works in the prototype.
// We're still thinking about how to load user-defined schema, either parse by parsing the source
// code, manifest, or by communicating from a runtime session, if there's one running potentially.

const useAvailableComponents = (objectId: string): SubMenuCategory[] => {
  const derivedScene = useDerivedScene()
  const components = useStudioComponentsContext().listComponents()
  const object = derivedScene.getObject(objectId)
  const hasMesh = object?.geometry || object?.material || object?.gltfModel

  if (!object) { return [] }

  const currentComponents = Object.values(object.components)

  const customComponents = components
    .filter(name => !(
      BUILT_IN_COMPONENTS.has(name) ||
      currentComponents.some(e => e.name === name) ||
      name.startsWith('debug-')
    ))
    .map(name => ({
      value: name,
      content: name,
    }))

  const getFilteredComponents = (componentList) => {
    if (componentList.length === 0) {
      return []
    }
    return componentList.filter((component) => {
      if (DIRECT_PROPERTY_COMPONENTS.includes(component.value)) {
        return !object[component.value]
      }
      return !currentComponents.some(e => e.name === component.value)
    }).map(component => ({
      value: component.value,
      content: component.content,
      ...component.ns && {ns: component.ns},
      isDirectProperty: !!DIRECT_PROPERTY_COMPONENTS.includes(component.value),
    }))
  }

  const categories = [
    ...customComponents.length === 0
      ? []
      : [{
        value: ComponentCategoryType.CUSTOM,
        parent: null,
        options: getFilteredComponents(customComponents),
      }],
    {
      value: ComponentCategoryType.ANIMATION,
      parent: null,
      options: getFilteredComponents(ANIMATION_OPTIONS),
    },
    {
      value: ComponentCategoryType.LIGHTING,
      parent: null,
      options: getFilteredComponents(LIGHTING_OPTIONS),
    },
    {
      value: null,
      parent: null,
      options: getFilteredComponents([
        ...hasMesh ? [] : MESH_OPTIONS,
        ...ROOT_OPTIONS,
      ]),
    },
    {
      value: ComponentCategoryType.CONTROLS,
      parent: null,
      options: getFilteredComponents(CONTROLLER_OPTIONS),
    },
  ] as SubMenuCategory[]

  return categories.filter(section => section.options.length > 0)
}

export {
  type Component,
  useAvailableComponents,
  AUDIO_COMPONENT,
  COLLIDER_COMPONENT,
  LIGHT_COMPONENT,
  MESH_COMPONENT,
  TRANSFORM_COMPONENT,
  UI_COMPONENT,
  PARTICLE_EMITTER_COMPONENT,
  DIRECT_PROPERTY_COMPONENTS,
  VIDEO_COMPONENT,
  ORIGINAL_BUILT_IN_COMPONENTS as BUILT_IN_COMPONENTS,
}

export type {
  BuiltInComponentName,
  NewComponentValue,
  ComponentCategoryType,
  BuiltInDirectProperty,
}
