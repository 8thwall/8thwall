import {BASIC, ENV_MAP_PRESETS as SHARED_ENV_MAP_PRESETS} from '@ecs/shared/environment-maps'
import type {TranslationKey} from 'react-i18next'

const ENV_MAP_PRESETS = SHARED_ENV_MAP_PRESETS satisfies Record<
  keyof typeof SHARED_ENV_MAP_PRESETS,
  TranslationKey<'cloud-studio-pages'>
>

export {
  BASIC,
  ENV_MAP_PRESETS,
}
