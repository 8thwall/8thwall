import {extractBuildIf} from './extract-buildif'

type DefinitionValue = (
  'isTest' |
  'isQa' |
  'isLocal' |
  'isExperimental' |
  'isMature' |
  boolean
)

const definitions = {
  LOCAL_DEV: 'isLocal',
  ALL_QA: 'isQa',
  EXPERIMENTAL: 'isExperimental',
  MATURE: 'isMature',
  LOCAL: 'isLocal',
  DISABLED: false,
  FULL_ROLLOUT: true,
  UI_TEST: 'isTest',
  UNIT_TEST: false,
  ALWAYS_BRAND8_20251009: false,
  CLOUD_STUDIO_AI_20240413: false,
  CONFIGURATOR_TOOLTIPS_20250808: 'isExperimental',
  FBX_CONVERSION_20260211: false,
  FONT8_CONVERSION_20260211: false,
  IMAGE_TARGET_TEST_20260415: false,
  MANIFEST_EDIT_20250618: false,
  MIGRATE_PADDING_20250610: 'isExperimental',
  NAE_IOS_APP_CONNECT_API_20250822: false,
  NAE_LAUNCH_SCREEN_UPLOAD_20250910: 'isLocal',
  NAE_MODALS_VISIBLE_20260202: false,
  NESTED_PREFABS_20250625: false,
  SCENE_DIFF_20250730: 'isExperimental',
  SCENE_JSON_VISIBLE_20240616: 'isMature',
  STATIC_IMAGE_TARGETS_20250721: 'isExperimental',
  STUDIO_ASSET_LAB_20260209: false,
  STUDIO_DEV8_INTEGRATION_20260205: false,
  STUDIO_IMAGE_TARGETS_20260210: true,
  STUDIO_MESH_MATERIAL_CONFIGURATOR_20240828: 'isLocal',
  STUDIO_MULTI_ADD_COMPONENTS_20241202: 'isExperimental',
  STUDIO_OFFLINE_LOG_CONTAINER_20260205: false,
  STUDIO_RUNTIME_CONFIG_20260209: 'isLocal',
  // NOTE(christoph): These keys are alphabetically sorted
} as const

type DefinitionKey = keyof typeof definitions

type DefinitionRecord = Record<DefinitionKey, DefinitionValue>

// If definitions is not assignable to definitionsRecord, it means there is a typo in definitions
const definitionsRecord: DefinitionRecord = definitions

type BuildIfReplacements = {
  [x in DefinitionKey]: boolean
}

type BuildIfEnv = {
  isLocalDev: boolean
  isRemoteDev: boolean
  isTest: boolean
  flagLevel: 'experimental' | 'mature' | 'launch'
}

const getBuildIfReplacements = (env: BuildIfEnv, overrides: DefinitionRecord = null) => (
  extractBuildIf({...definitionsRecord, ...overrides}, env)
)

export {
  BuildIfEnv,
  BuildIfReplacements,
  getBuildIfReplacements,
  DefinitionKey,
  DefinitionRecord,
}
