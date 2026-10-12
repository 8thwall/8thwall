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

interface StartFlowOptions {
  previewSrc: string | null
  showStartScreen: boolean
  waitForCameraTexture: boolean
}

interface StartFlowScreenOptions {
  previewSrc: string | null
  onStart: () => void
  onRefresh: () => void
}

interface StartFlowPipelineModule {
  name: string
  onBeforeRun: (args: {config: unknown}) => void | Promise<void>
  onCameraStatusChange: (args: {reason?: string, status: string}) => void
  onAppResourcesLoaded: () => void
  onUpdate: (args: {processCpuResult?: unknown}) => void
  onException: (error: unknown) => void
  onRemove: () => void
}

interface StartFlowController {
  pipelineModule: () => StartFlowPipelineModule
  configure: (options: Partial<StartFlowOptions>) => void
  setStringOverrides: (overrides: StartFlowStringOverrides) => void
  show: () => void
  hide: () => void
  isVisible: () => boolean
}

interface StartFlowViewProps {
  screen: StartFlowScreen
  options: StartFlowScreenOptions
  strings: StartFlowStrings
}

export type {
  Permission,
  PermissionsErrorEnvironment,
  StartFlowController,
  StartFlowOptions,
  StartFlowPipelineModule,
  StartFlowScreen,
  StartFlowScreenOf,
  StartFlowScreenOptions,
  StartFlowScreenType,
  StartFlowStringOverrides,
  StartFlowStrings,
  StartFlowViewProps,
}
