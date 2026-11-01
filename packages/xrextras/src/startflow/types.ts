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
  | 'landing'
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

type LandingMediaControlMode = 'none' | 'browser' | 'minimal'
type LandingOrbitIdle = 'none' | 'spin' | 'bounce'
type LandingOrbitInteraction = 'none' | 'drag'

interface StartFlowOptions {
  previewSrc: string | null
  showStartScreen: boolean
  waitForCameraTexture: boolean
  url: string | null
  mediaSrc: string | null
  logoSrc: string | null
  mediaAutoplay: boolean
  mediaAnimation: string
  mediaControls: LandingMediaControlMode
  sceneEnvMap: string
  sceneOrbitIdle: LandingOrbitIdle
  sceneOrbitInteraction: LandingOrbitInteraction
  sceneLightingIntensity: number
}

interface StartFlowScreenOptions extends StartFlowOptions {
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
  reset: () => void
  setStringOverrides: (overrides: StartFlowStringOverrides) => void
  show: () => void
  hide: () => void
  isVisible: () => boolean
}

interface StartFlowContentProps {
  options: StartFlowScreenOptions
  strings: StartFlowStrings
}

interface StartFlowViewProps extends StartFlowContentProps {
  screen: StartFlowScreen
}

export type {
  LandingMediaControlMode,
  LandingOrbitIdle,
  LandingOrbitInteraction,
  Permission,
  PermissionsErrorEnvironment,
  StartFlowController,
  StartFlowContentProps,
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
