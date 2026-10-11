type Permission = 'camera' | 'microphone' | 'motion'
type PermissionsErrorEnvironment =
  | 'ios-safari'
  | 'ios-chrome'
  | 'android-chrome'
  | 'desktop-safari'
  | 'desktop-chrome'
  | 'other'
type StartFlowScreen =
  | 'start'
  | 'in-app-browser'
  | 'unsupported-browser'
  | 'unsupported-device'
  | {type: 'loading', requestingPermissions: boolean}
  | {type: 'unexpected-error', error: string}
  | {
    type: 'permissions-error'
    environment: PermissionsErrorEnvironment
    failed: Permission[]
    requested: Permission[]
  }

type TaggedStartFlowScreen = Exclude<StartFlowScreen, string>

type StartFlowScreenType = Extract<StartFlowScreen, string> | TaggedStartFlowScreen['type']

type StartFlowScreenOf<Type extends TaggedStartFlowScreen['type']> =
  Extract<TaggedStartFlowScreen, {type: Type}>

type StartFlowStrings = typeof import('./strings').defaultStrings

type StartFlowStringOverrides = Partial<StartFlowStrings>

interface StartFlowScreenOptions {
  previewSrc: string | null
  onStart: () => void
  onRefresh: () => void
}

export type {
  Permission,
  PermissionsErrorEnvironment,
  StartFlowScreen,
  StartFlowScreenOf,
  StartFlowScreenOptions,
  StartFlowScreenType,
  StartFlowStringOverrides,
  StartFlowStrings,
}
