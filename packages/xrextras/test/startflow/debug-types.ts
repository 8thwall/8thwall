import type {
  Permission,
  PermissionsErrorEnvironment,
  StartFlowOptions,
  StartFlowScreenType,
  StartFlowStringOverrides,
} from '../../src/startflow/types'
import type {ViewMode} from './preview-workspace'
import type {StyleOverrides} from './style-controls'

type ScreenSelection = StartFlowScreenType | 'all'
type DebugLandingOptions = Omit<
  StartFlowOptions,
  | 'previewSrc'
  | 'showStartScreen'
  | 'waitForCameraTexture'
  | 'url'
  | 'mediaSrc'
  | 'logoSrc'
> & {
  url: string
  mediaSrc: string
  logoSrc: string
}

interface DebugSettings {
  screen: ScreenSelection
  viewMode: ViewMode
  showAllPermissionsErrorCombinations: boolean
  options: {
    start: {previewSrc: string}
    landing: DebugLandingOptions
  }
  stringOverrides: StartFlowStringOverrides
  styleOverrides: StyleOverrides
  state: {
    loading: {
      requestingPermissions: boolean
    }
    unexpectedError: {
      error: string
    }
    permissionsError: {
      environment: PermissionsErrorEnvironment
      failed: Permission[]
      requested: Permission[]
    }
  }
}

type UpdateSettings = (updater: (current: DebugSettings) => DebugSettings) => void

export type {DebugLandingOptions, DebugSettings, ScreenSelection, UpdateSettings}
