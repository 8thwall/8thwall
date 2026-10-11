import {h} from 'preact'
import {useEffect, useState} from 'preact/hooks'

import {combine} from '../../src/classnames'
import {defaultStrings} from '../../src/startflow/strings'
import type {
  PermissionsErrorEnvironment,
  StartFlowScreen,
  StartFlowScreenType,
  StartFlowStrings,
} from '../../src/startflow/types'
import {StartFlowView} from '../../src/startflow/view'
import {
  INDIVIDUAL_SCREENS,
  PERMISSION_LABELS,
  PERMISSIONS,
  PERMISSIONS_ERROR_ENVIRONMENTS,
  SCREENS,
  STRING_KEYS,
} from './debug-config'
import styles from './debug.module.scss'
import type {DebugSettings, ScreenSelection} from './debug-types'
import {PreviewWorkspace} from './preview-workspace'
import type {PreviewItem, ViewMode} from './preview-workspace'
import {StringOverridesControl} from './string-controls'
import {StyleOverridesControl} from './style-controls'
import type {StyleToken} from './style-controls'
import {readSettings, writeSettings} from './url-settings'

const screenFromSettings = (
  type: StartFlowScreenType, settings: DebugSettings
): StartFlowScreen => {
  switch (type) {
    case 'loading':
      return {type, ...settings.state.loading}
    case 'permissions-error':
      return {type, ...settings.state.permissionsError}
    case 'unexpected-error':
      return {type, ...settings.state.unexpectedError}
    default:
      return type
  }
}

const DebugApp = () => {
  const [settings, setSettings] = useState<DebugSettings>(readSettings)
  const strings = {...defaultStrings, ...settings.stringOverrides}
  const previewStyle = [
    '--xrextras-z-index: 1',
    ...Object.entries(settings.styleOverrides).map(([token, value]) => `${token}: ${value}`),
  ].join('; ')

  useEffect(() => {
    writeSettings(settings)
  }, [settings])

  useEffect(() => {
    const handlePopState = () => setSettings(readSettings())
    window.addEventListener('popstate', handlePopState)
    return () => window.removeEventListener('popstate', handlePopState)
  }, [])

  const setScreen = (screen: ScreenSelection) => setSettings(current => ({...current, screen}))
  const setViewMode = (viewMode: ViewMode) => setSettings(current => ({...current, viewMode}))
  const setString = (key: keyof StartFlowStrings, value: string) => setSettings((current) => {
    const stringOverrides = {...current.stringOverrides}
    if (value === defaultStrings[key]) {
      delete stringOverrides[key]
    } else {
      stringOverrides[key] = value
    }
    return {...current, stringOverrides}
  })
  const setStyle = (token: StyleToken, value: string, defaultValue: string) => {
    setSettings((current) => {
      const styleOverrides = {...current.styleOverrides}
      if (!value || value === defaultValue) {
        delete styleOverrides[token]
      } else {
        styleOverrides[token] = value
      }
      return {...current, styleOverrides}
    })
  }
  const isSelected = (screen: StartFlowScreenType) => (
    settings.screen === 'all' || settings.screen === screen
  )
  const isCombinationMode = settings.screen === 'permissions-error' &&
    settings.showAllPermissionsErrorCombinations
  const isCollectionMode = settings.screen === 'all' || isCombinationMode
  const combinationPreviews = PERMISSIONS_ERROR_ENVIRONMENTS.map(environment => ({
    label: environment.label,
    screen: {
      type: 'permissions-error' as const,
      environment: environment.value,
      failed: settings.state.permissionsError.failed,
      requested: settings.state.permissionsError.requested,
    },
  }))
  const renderScreen = (screen: StartFlowScreen) => {
    const showLoading = (requestingPermissions: boolean) => setSettings(current => ({
      ...current,
      screen: 'loading',
      state: {...current.state, loading: {requestingPermissions}},
    }))
    return (
      <StartFlowView
        screen={screen}
        options={{
          previewSrc: settings.options.start.previewSrc,
          onStart: settings.screen === 'all' ? () => undefined : () => showLoading(false),
          onRefresh: isCollectionMode ? () => undefined : () => showLoading(true),
        }}
        stringOverrides={settings.stringOverrides}
      />
    )
  }
  let previewItems: PreviewItem[]
  if (settings.screen === 'all') {
    previewItems = INDIVIDUAL_SCREENS.map(({value, label}) => ({
      label,
      screen: screenFromSettings(value, settings),
    }))
  } else if (isCombinationMode) {
    previewItems = combinationPreviews
  } else {
    previewItems = [{
      label: settings.screen,
      screen: screenFromSettings(settings.screen, settings),
    }]
  }

  return (
    <div className={combine(
      styles.app,
      settings.viewMode === 'full-no-sidebar' && styles.noSidebar
    )}
    >
      <aside className={styles.panel}>
        <fieldset className={styles.fieldset}>
          <legend>Screen</legend>
          <label className={styles.field}>
            <span>Current screen</span>
            <select
              aria-label='Current screen'
              value={settings.screen}
              onChange={event => setScreen(event.currentTarget.value as ScreenSelection)}
            >
              {SCREENS.map(({value, label}) => (
                <option key={value} value={value}>{label}</option>
              ))}
            </select>
          </label>
        </fieldset>

        {isSelected('start') && (
          <fieldset className={styles.fieldset}>
            <legend>{settings.screen === 'all' ? 'Start options' : 'Options'}</legend>
            <label className={styles.field}>
              <span>Preview image URL</span>
              <input
                type='url'
                aria-label='Preview image URL'
                placeholder='Use the built-in preview'
                value={settings.options.start.previewSrc}
                onInput={event => setSettings(current => ({
                  ...current,
                  options: {
                    ...current.options,
                    start: {previewSrc: event.currentTarget.value},
                  },
                }))}
              />
            </label>
          </fieldset>
        )}

        {isSelected('loading') && (
          <fieldset className={combine(styles.fieldset, styles.screenState)}>
            <legend>{settings.screen === 'all' ? 'Loading state' : 'Screen state'}</legend>
            <label className={styles.checkboxField}>
              <input
                type='checkbox'
                checked={settings.state.loading.requestingPermissions}
                onChange={event => setSettings(current => ({
                  ...current,
                  state: {
                    ...current.state,
                    loading: {requestingPermissions: event.currentTarget.checked},
                  },
                }))}
              />
              <span>Requesting permissions</span>
            </label>
          </fieldset>
        )}

        {isSelected('permissions-error') && (
          <fieldset className={combine(styles.fieldset, styles.screenState)}>
            <legend>
              {settings.screen === 'all' ? 'Permissions error state' : 'Screen state'}
            </legend>
            {settings.screen === 'permissions-error' && (
              <label className={styles.checkboxField}>
                <input
                  type='checkbox'
                  checked={settings.showAllPermissionsErrorCombinations}
                  onChange={event => setSettings(current => ({
                    ...current,
                    showAllPermissionsErrorCombinations: event.currentTarget.checked,
                  }))}
                />
                <span>Show all valid combinations</span>
              </label>
            )}
            {!(settings.screen === 'permissions-error' &&
              settings.showAllPermissionsErrorCombinations) && (
              <label className={styles.field}>
                <span>Platform and browser</span>
                <select
                  value={settings.state.permissionsError.environment}
                  onChange={event => setSettings(current => ({
                    ...current,
                    state: {
                      ...current.state,
                      permissionsError: {
                        ...current.state.permissionsError,
                        environment: event.currentTarget.value as PermissionsErrorEnvironment,
                      },
                    },
                  }))}
                >
                  {PERMISSIONS_ERROR_ENVIRONMENTS.map(({label, value}) => (
                    <option key={value} value={value}>{label}</option>
                  ))}
                </select>
              </label>
            )}
            {PERMISSIONS.map(permission => (
              <div key={permission} className={styles.permissionControls}>
                <span>{PERMISSION_LABELS[permission]}</span>
                <label className={styles.checkboxField}>
                  <input
                    type='checkbox'
                    checked={settings.state.permissionsError.requested.includes(permission)}
                    onChange={event => setSettings((current) => {
                      const permissionState = current.state.permissionsError
                      const requested = event.currentTarget.checked
                        ? [...permissionState.requested, permission]
                        : permissionState.requested.filter(item => item !== permission)
                      return {
                        ...current,
                        state: {
                          ...current.state,
                          permissionsError: {...permissionState, requested},
                        },
                      }
                    })}
                  />
                  <span>Requested</span>
                </label>
                <label className={styles.checkboxField}>
                  <input
                    type='checkbox'
                    checked={settings.state.permissionsError.failed.includes(permission)}
                    onChange={event => setSettings((current) => {
                      const permissionState = current.state.permissionsError
                      const failed = event.currentTarget.checked
                        ? [...permissionState.failed, permission]
                        : permissionState.failed.filter(item => item !== permission)
                      return {
                        ...current,
                        state: {
                          ...current.state,
                          permissionsError: {...permissionState, failed},
                        },
                      }
                    })}
                  />
                  <span>Failed</span>
                </label>
              </div>
            ))}
          </fieldset>
        )}

        {isSelected('unexpected-error') && (
          <fieldset className={combine(styles.fieldset, styles.screenState)}>
            <legend>{settings.screen === 'all' ? 'Unexpected error state' : 'Screen state'}</legend>
            <label className={styles.field}>
              <span>Error detail</span>
              <input
                type='text'
                value={settings.state.unexpectedError.error}
                onInput={event => setSettings(current => ({
                  ...current,
                  state: {
                    ...current.state,
                    unexpectedError: {error: event.currentTarget.value},
                  },
                }))}
              />
            </label>
          </fieldset>
        )}

        <StyleOverridesControl
          overrides={settings.styleOverrides}
          onChange={setStyle}
          onPreset={values => setSettings(current => ({
            ...current,
            styleOverrides: {...values},
          }))}
          onReset={() => setSettings(current => ({...current, styleOverrides: {}}))}
        />
        <StringOverridesControl
          keys={STRING_KEYS}
          onChange={setString}
          strings={strings}
        />
      </aside>

      <PreviewWorkspace
        collection={isCollectionMode}
        items={previewItems}
        onViewModeChange={setViewMode}
        previewStyle={previewStyle}
        renderScreen={renderScreen}
        viewMode={settings.viewMode}
      />
    </div>
  )
}

export {DebugApp}
