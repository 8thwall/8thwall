import type {
  Permission,
  PermissionsErrorEnvironment,
  StartFlowScreenType,
  StartFlowStringOverrides,
} from '../../src/startflow/types'
import type {ViewMode} from './preview-workspace'
import type {StyleOverrides} from './style-controls'

type ScreenSelection = StartFlowScreenType | 'all'
interface DebugSettings {
  screen: ScreenSelection
  viewMode: ViewMode
  showAllPermissionsErrorCombinations: boolean
  options: {
    start: {previewSrc: string}
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

export type {DebugSettings, ScreenSelection, UpdateSettings}
