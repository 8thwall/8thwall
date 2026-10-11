import {defaultStrings} from '../../src/startflow/strings'
import type {
  Permission,
  PermissionsErrorEnvironment,
  StartFlowScreenType,
  StartFlowStrings,
} from '../../src/startflow/types'
import type {ScreenSelection} from './debug-types'

const DEFAULT_PREVIEW_PATH = 'https://8thwall.org/social-cover.png'

const SCREENS: {value: ScreenSelection, label: string}[] = [
  {value: 'all', label: 'All screens'},
  {value: 'start', label: 'Start'},
  {value: 'loading', label: 'Loading'},
  {value: 'permissions-error', label: 'Permissions error'},
  {value: 'unsupported-browser', label: 'Browser update required'},
  {value: 'in-app-browser', label: 'In-app browser'},
  {value: 'unsupported-device', label: 'Unsupported device'},
  {value: 'unexpected-error', label: 'Unexpected error'},
]

const INDIVIDUAL_SCREENS = SCREENS.filter((screen): screen is {
  value: StartFlowScreenType
  label: string
} => screen.value !== 'all')

const PERMISSIONS: Permission[] = ['camera', 'microphone', 'motion']
const PERMISSION_LABELS: Record<Permission, string> = {
  camera: 'Camera',
  microphone: 'Microphone',
  motion: 'Motion',
}
const PERMISSIONS_ERROR_ENVIRONMENTS: {
  label: string
  value: PermissionsErrorEnvironment
}[] = [
  {label: 'iOS — Safari', value: 'ios-safari'},
  {label: 'iOS — Chrome', value: 'ios-chrome'},
  {label: 'Android — Chrome/Chromium', value: 'android-chrome'},
  {label: 'Desktop — Safari', value: 'desktop-safari'},
  {label: 'Desktop — Chrome', value: 'desktop-chrome'},
  {label: 'Other or unknown platform', value: 'other'},
]

const readPermissions = (value: string, fallback: Permission[]) => {
  const values = (value || '').split(',').filter(item => PERMISSIONS.includes(item as Permission))
  return values.length ? values as Permission[] : fallback
}

const STRING_KEYS = Object.keys(defaultStrings) as (keyof StartFlowStrings)[]

export {
  DEFAULT_PREVIEW_PATH,
  INDIVIDUAL_SCREENS,
  PERMISSION_LABELS,
  PERMISSIONS,
  PERMISSIONS_ERROR_ENVIRONMENTS,
  SCREENS,
  STRING_KEYS,
  readPermissions,
}
