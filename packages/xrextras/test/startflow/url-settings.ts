import type {
  PermissionsErrorEnvironment,
  StartFlowStringOverrides,
} from '../../src/startflow/types'
import {
  DEFAULT_PREVIEW_PATH,
  PERMISSIONS_ERROR_ENVIRONMENTS,
  SCREENS,
  STRING_KEYS,
  readPermissions,
} from './debug-config'
import type {DebugSettings, ScreenSelection} from './debug-types'
import type {ViewMode} from './preview-workspace'
import {STYLE_FIELDS} from './style-controls'
import type {StyleOverrides} from './style-controls'

const readSettings = (): DebugSettings => {
  const params = new URLSearchParams(window.location.search)
  const requestedScreen = params.get('screen') as ScreenSelection
  const screen = SCREENS.some(({value}) => value === requestedScreen) ? requestedScreen : 'start'
  const requestedViewMode = params.get('view') as ViewMode
  let viewMode: ViewMode = 'mobile'
  if (requestedViewMode === 'full' || requestedViewMode === 'full-no-sidebar') {
    viewMode = requestedViewMode
  }
  const stringOverrides: StartFlowStringOverrides = {}
  const styleOverrides: StyleOverrides = {}
  STRING_KEYS.forEach((key) => {
    const value = params.get(key)
    if (value !== null) {
      stringOverrides[key] = value
    }
  })
  STYLE_FIELDS.forEach(({token}) => {
    const value = params.get(token)
    if (value !== null) {
      styleOverrides[token] = value
    }
  })

  return {
    screen,
    viewMode,
    showAllPermissionsErrorCombinations:
      params.get('showAllPermissionsErrorCombinations') === 'true',
    options: {
      start: {previewSrc: params.get('previewSrc') || DEFAULT_PREVIEW_PATH},
    },
    state: {
      loading: {requestingPermissions: params.get('requestingPermissions') === 'true'},
      unexpectedError: {error: params.get('error') || 'XR initialization failed'},
      permissionsError: {
        environment: PERMISSIONS_ERROR_ENVIRONMENTS.some(({value}) => (
          value === params.get('permissionsEnvironment')
        ))
          ? params.get('permissionsEnvironment') as PermissionsErrorEnvironment
          : 'ios-safari',
        failed: readPermissions(params.get('failed'), ['camera']),
        requested: readPermissions(
          params.get('errorRequested') || params.get('requested'),
          ['camera']
        ),
      },
    },
    stringOverrides,
    styleOverrides,
  }
}

const writeSettings = (settings: DebugSettings) => {
  const params = new URLSearchParams()
  params.set('screen', settings.screen)
  params.set('view', settings.viewMode)
  params.set(
    'showAllPermissionsErrorCombinations',
    String(settings.showAllPermissionsErrorCombinations)
  )
  if (settings.options.start.previewSrc !== DEFAULT_PREVIEW_PATH) {
    params.set('previewSrc', settings.options.start.previewSrc)
  }
  if (settings.state.unexpectedError.error) {
    params.set('error', settings.state.unexpectedError.error)
  }
  params.set('requestingPermissions', String(settings.state.loading.requestingPermissions))
  params.set('requested', settings.state.permissionsError.requested.join(','))
  params.set('errorRequested', settings.state.permissionsError.requested.join(','))
  params.set('failed', settings.state.permissionsError.failed.join(','))
  params.set('permissionsEnvironment', settings.state.permissionsError.environment)
  STRING_KEYS.forEach((key) => {
    if (settings.stringOverrides[key] !== undefined) {
      params.set(key, settings.stringOverrides[key])
    }
  })
  STYLE_FIELDS.forEach(({token}) => {
    if (settings.styleOverrides[token] !== undefined) {
      params.set(token, settings.styleOverrides[token])
    }
  })
  window.history.replaceState(null, '', `${window.location.pathname}?${params}`)
}

export {readSettings, writeSettings}
