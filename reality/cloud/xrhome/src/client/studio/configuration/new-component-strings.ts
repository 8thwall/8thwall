import type {DeepReadonly} from 'ts-essentials'

import type {NewComponentValue} from '../hooks/available-components'

type NewComponentOption = {
  content: string
  value: NewComponentValue
  ns: 'cloud-studio-pages'
}

type NewComponentOptions = DeepReadonly<NewComponentOption[]>
const ANIMATION_OPTIONS: NewComponentOptions = [
  {
    content: 'new_component_strings.option.position_animation',
    value: 'position-animation',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'new_component_strings.option.scale_animation',
    value: 'scale-animation',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'new_component_strings.option.rotate_animation',
    value: 'rotate-animation',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'new_component_strings.option.custom_property_animation',
    value: 'custom-property-animation',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'new_component_strings.option.custom_vec3_animation',
    value: 'custom-vec3-animation',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'new_component_strings.option.follow_animation',
    value: 'follow-animation',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'new_component_strings.option.look_at_animation',
    value: 'look-at-animation',
    ns: 'cloud-studio-pages',
  },
] as const

const LIGHTING_OPTIONS: NewComponentOptions = [
  {content: 'light_configurator.title', value: 'light', ns: 'cloud-studio-pages'},
]

const CONTROLLER_OPTIONS: NewComponentOptions = [
  {
    content: 'new_component_strings.option.orbit_controls',
    value: 'orbit-controls',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'new_component_strings.option.fly_controls',
    value: 'fly-controller',
    ns: 'cloud-studio-pages',
  },
]

const MESH_OPTIONS: NewComponentOptions = [
  {content: 'mesh_configurator.title', value: 'mesh', ns: 'cloud-studio-pages'},
]

const ROOT_OPTIONS: NewComponentOptions = [
  {content: 'collider_configurator.title', value: 'collider', ns: 'cloud-studio-pages'},
  {
    content: 'new_component_strings.option.particle_emitter',
    value: 'particle-emitter',
    ns: 'cloud-studio-pages',
  },
  {content: 'ui_configurator.title', value: 'ui', ns: 'cloud-studio-pages'},
  {content: 'audio_configurator.title', value: 'audio', ns: 'cloud-studio-pages'},
  {
    content: 'image_target_configurator.title',
    value: 'imageTarget',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'video_controls_configurator.title',
    value: 'videoControls',
    ns: 'cloud-studio-pages',
  },
]

const ALL_NEW_COMPONENT_OPTIONS: NewComponentOptions = [
  ...ANIMATION_OPTIONS,
  ...LIGHTING_OPTIONS,
  ...CONTROLLER_OPTIONS,
  ...MESH_OPTIONS,
  ...ROOT_OPTIONS,
]

enum ComponentCategoryType {
  ANIMATION = 'new_component_strings.category.animation',
  CUSTOM = 'new_component_strings.category.custom',
  LIGHTING = 'new_component_strings.category.lighting',
  CONTROLS = 'new_component_strings.category.controls',
}

export {
  ComponentCategoryType,
  ALL_NEW_COMPONENT_OPTIONS,
  ANIMATION_OPTIONS,
  LIGHTING_OPTIONS,
  ROOT_OPTIONS,
  MESH_OPTIONS,
  CONTROLLER_OPTIONS,
}
