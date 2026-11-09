import {BASIC, ENV_MAP_PRESETS as SHARED_ENV_MAP_PRESETS} from '@ecs/shared/environment-maps'
import type {TFuncKey} from 'react-i18next'

const ENV_MAP_PRESETS = SHARED_ENV_MAP_PRESETS satisfies Record<
  keyof typeof SHARED_ENV_MAP_PRESETS,
  TFuncKey<'cloud-studio-pages'>
>

export {
  BASIC,
  ENV_MAP_PRESETS,
}
