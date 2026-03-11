import type {SubMenuCategory} from '../ui/submenu'

const PRESET_ROOT_OPTIONS = [
  {
    content: 'input_user_action_maps.option.empty_action_map',
    value: null,
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_user_action_maps.option.fly_controller',
    value: 'fly-controller',
    ns: 'cloud-studio-pages',
  },
  {
    content: 'input_user_action_maps.option.orbit_controller',
    value: 'orbit-controls',
    ns: 'cloud-studio-pages',
  },
] as const

const ALL_PRESET_CATEGORIES = [
  {
    value: 'input_user_action_maps.button.create_new_action_map',
    parent: null,
    options: PRESET_ROOT_OPTIONS,
  },
] as SubMenuCategory[]

export {
  ALL_PRESET_CATEGORIES,
  PRESET_ROOT_OPTIONS,
}
