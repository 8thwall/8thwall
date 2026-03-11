import type React from 'react'
import type {TFunction} from 'react-i18next'

import type {
  AppleSigningType,
  HtmlShell,
  NaeBuildMode,
  RefHead,
  ScreenOrientation,
} from '../../../../shared/nae/nae-types'
import type {SceneContext} from '../../../studio/scene-context'
import type {JointToggleOption} from '../../../ui/components/joint-toggle-button'

const getDateAndHourText = (
  timestampMs: number | string
): { dateText: string; hoursText: string } => {
  const buildEndDate = new Date(Number(timestampMs))
  const dateOptions = {year: 'numeric', month: 'long', day: 'numeric'} as const
  const dateText = buildEndDate.toLocaleDateString(undefined, dateOptions)
  const hoursText = buildEndDate.toLocaleTimeString(undefined, {
    hour: '2-digit',
    minute: '2-digit',
    hour12: true,
  })
  return {dateText, hoursText}
}

const BUILD_MODE_OPTIONS = [
  {
    translationValueKey: 'editor_page.export_modal.build_mode.hot_reload',
    value: 'hot-reload' as const,
  },
  {
    translationValueKey: 'editor_page.export_modal.build_mode.static',
    value: 'static' as const,
  },
] as const

const getBuildModeOptions = (
  t: TFunction
): [
  JointToggleOption<NaeBuildMode>,
  JointToggleOption<NaeBuildMode>,
  ...JointToggleOption<NaeBuildMode>[]
] => BUILD_MODE_OPTIONS.map(({value, translationValueKey}) => ({
  value,
  content: t(translationValueKey),
})) as [
  JointToggleOption<NaeBuildMode>,
  JointToggleOption<NaeBuildMode>,
  ...JointToggleOption<NaeBuildMode>[]
]

const getBuildModeName = (
  t: TFunction, mode: NaeBuildMode
): React.ReactNode => getBuildModeOptions(t).find(option => option.value === mode)?.content || ''

const getEnvName = (t: TFunction, refhead: RefHead) => {
  switch (refhead) {
    case 'master':
      return t('editor_page.export_modal.build_env.master')
    case 'staging':
      return t('editor_page.export_modal.build_env.staging')
    case 'production':
      return t('editor_page.export_modal.build_env.production')
    default:
      return t('editor_page.export_modal.build_env.dev')
  }
}

const SCREEN_ORIENTATION_OPTIONS = [
  {
    translationValueKey: 'editor_page.export_modal.screen_orientation.portrait',
    value: 'portrait' as const,
  },
  {
    translationValueKey: 'editor_page.export_modal.screen_orientation.landscape_left',
    value: 'landscape-left' as const,
  },
  {
    translationValueKey: 'editor_page.export_modal.screen_orientation.landscape_right',
    value: 'landscape-right' as const,
  },
  {
    translationValueKey: 'editor_page.export_modal.screen_orientation.auto',
    value: 'auto' as const,
  },
  {
    translationValueKey: 'editor_page.export_modal.screen_orientation.auto_landscape',
    value: 'auto-landscape' as const,
  },
] as const

const getScreenOrientationOptions = (
  t: TFunction
): [
  JointToggleOption<ScreenOrientation>,
  JointToggleOption<ScreenOrientation>,
  ...JointToggleOption<ScreenOrientation>[]
] => SCREEN_ORIENTATION_OPTIONS.map(({value, translationValueKey}) => ({
  value,
  content: t(translationValueKey),
})) as [
  JointToggleOption<ScreenOrientation>,
  JointToggleOption<ScreenOrientation>,
  ...JointToggleOption<ScreenOrientation>[]
]

const getScreenOrientationName = (
  t: TFunction, screenOrientation: ScreenOrientation
): React.ReactNode => getScreenOrientationOptions(t)
  .find(option => option.value === screenOrientation)?.content || ''

const getExportDisabled = (sceneCtx: SceneContext, platform: HtmlShell) => {
  const xrNotSupported = ['android', 'html']
  if (!xrNotSupported.includes(platform)) {
    return false
  }

  const allObjects = Object.keys(sceneCtx.scene.objects)
  const no3dCameras = allObjects.every((id) => {
    const obj = sceneCtx.scene.objects[id]
    const {camera} = obj
    if (!camera) {
      return true
    }
    const xrCameraType = camera?.xr?.xrCameraType
    if (!xrCameraType) {
      // Old versions of camera may not have a xrCameraType. Assume this is a 3D camera and let
      // users export anyways just in case.
      return false
    }
    return xrCameraType !== '3dOnly'
  })

  return no3dCameras
}

const validateScreenOrientation =
(orientation: string) => SCREEN_ORIENTATION_OPTIONS.some(option => option.value === orientation)

const IOS_SIGNING_TYPE_OPTIONS: readonly {
  translationValueKey: string
  value: AppleSigningType
}[] = [
  {
    translationValueKey: 'editor_page.export_modal.ios_signing_type_development',
    value: 'development' as const,
  },
  {
    translationValueKey: 'editor_page.export_modal.ios_signing_type_distribution',
    value: 'distribution' as const,
  },
] as const

const getAppleSigningTypeOptions = (
  t: TFunction
): [
  JointToggleOption<AppleSigningType>,
  JointToggleOption<AppleSigningType>,
  ...JointToggleOption<AppleSigningType>[]
] => IOS_SIGNING_TYPE_OPTIONS.map(({value, translationValueKey}) => ({
  value,
  content: t(translationValueKey),
})) as [
  JointToggleOption<AppleSigningType>,
  JointToggleOption<AppleSigningType>,
  ...JointToggleOption<AppleSigningType>[]
]

const getAppleSigningTypeName = (
  t: TFunction, appleSigningType: AppleSigningType
): React.ReactNode => getAppleSigningTypeOptions(t)
  .find(option => option.value === appleSigningType)?.content || ''

type EmbedType = 'iframe' | 'full-html'

const getEmbedTypeOptions = (
  t: TFunction
): [
  JointToggleOption<EmbedType>,
  JointToggleOption<EmbedType>,
  ...JointToggleOption<EmbedType>[]
] => ['iframe', 'full-html'].map(value => ({
  value,
  content: t(`editor_page.export_modal.iframe.${value}`),
})) as [
  JointToggleOption<EmbedType>,
  JointToggleOption<EmbedType>,
  ...JointToggleOption<EmbedType>[]
]

const getFormattedDateTimeText = (timestampMs: number | string): string => {
  const date = new Date(Number(timestampMs))
  const dateText = date.toLocaleDateString(undefined, {
    month: 'long',
    day: 'numeric',
    year: 'numeric',
  })
  const timeText = date.toLocaleTimeString(undefined, {
    hour: '2-digit',
    minute: '2-digit',
    hour12: true,
  })
  return `${dateText} - ${timeText}`
}

const downloadBuild = (signedUrl: string) => {
  const link = document.createElement('a')
  link.href = signedUrl
  document.body.appendChild(link)
  link.click()
  document.body.removeChild(link)
}

type Steps = 'start' | 'configure-ios-signing' | 'configure' | 'finished' | 'permissions'

export {
  downloadBuild,
  getDateAndHourText,
  getFormattedDateTimeText,
  getBuildModeOptions,
  getBuildModeName,
  getEnvName,
  getExportDisabled,
  getScreenOrientationOptions,
  getScreenOrientationName,
  getAppleSigningTypeOptions,
  getAppleSigningTypeName,
  validateScreenOrientation,
  getEmbedTypeOptions,
}

export type {Steps, EmbedType}
