import React, {useLayoutEffect} from 'react'

import type {UiThemeMode} from '../ui/theme'

const OVERRIDES_LOCAL_STORAGE_KEY = '_brand8_overrides'

type UiModeOverride = UiThemeMode | null

type Brand8QaSettings = Partial<{
  modeOverride: UiModeOverride
  semanticUiOutlineEnabled: boolean
  publicNavigationOverride: boolean
  privateNavigationOverride: boolean
}>

type Brand8QaContextValue = Brand8QaSettings & {
  update: (settings: Brand8QaSettings) => void
}

const Brand8Context = React.createContext<Brand8QaContextValue>({
  update: () => {
    throw new Error('No Brand8QaContextProvider found in component tree.')
  },
})

const getLocalStorageKeyString = (key: string): string | null => {
  try {
    return localStorage?.getItem(key)
  } catch {
    return null
  }
}

const setLocalStorageKey = (key: string, value: boolean | string) => {
  try {
    localStorage?.setItem(key, String(value))
  } catch {
    // No-op in a case where localStorage is not available.
  }
}

const Brand8QaContextProviderImpl = ({children}: {children: React.ReactNode}) => {
  const [settings, setSettings] = React.useState<Brand8QaSettings>(() => {
    try {
      return JSON.parse(getLocalStorageKeyString(OVERRIDES_LOCAL_STORAGE_KEY) || '{}')
    } catch {
      return {}
    }
  })

  const update = (newSettings: Brand8QaSettings) => {
    setSettings((oldSettings) => {
      const updatedSettings = {...oldSettings, ...newSettings}
      setLocalStorageKey(OVERRIDES_LOCAL_STORAGE_KEY, JSON.stringify(updatedSettings))
      return updatedSettings
    })
  }
  useLayoutEffect(() => {
    if (settings.semanticUiOutlineEnabled) {
      document.head.insertAdjacentHTML(
        'beforeend',
        '<style id="semantic-ui-outline-debug">.ui {outline: 1px solid red !important;}</style>'
      )

      return () => {
        document.getElementById('semantic-ui-outline-debug')?.remove()
      }
    } else {
      return undefined
    }
  }, [settings.semanticUiOutlineEnabled])

  return (
    <Brand8Context.Provider value={{...settings, update}}>
      {children}
    </Brand8Context.Provider>
  )
}

const useBrand8QaContext = (): Brand8QaContextValue => {
  const context = React.useContext(Brand8Context)
  return context
}

const Brand8QaContextProvider = ({children}: {children: React.ReactNode}) => {
  if (!BuildIf.ALL_QA) {
    // Don't wrap in a context provider if we aren't in a QA or development environment.
    // eslint-disable-next-line react/jsx-no-useless-fragment
    return <>{children}</>
  }

  return (
    <Brand8QaContextProviderImpl>
      {children}
    </Brand8QaContextProviderImpl>
  )
}

export {
  Brand8QaContextProvider,
  useBrand8QaContext,
}
