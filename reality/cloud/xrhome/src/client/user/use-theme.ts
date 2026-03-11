import React from 'react'

import {usePreferences} from '../desktop/preferences-modal'

type ThemeName = 'dark' | 'light'

const useTheme = (): ThemeName => {
  const setting = usePreferences().data.theme

  const [systemPrefersDark, setSystemPrefersDark] = React.useState(
    window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches
  )

  React.useEffect(() => {
    const mediaQuery = window.matchMedia('(prefers-color-scheme: dark)')
    const handler = (e: MediaQueryListEvent) => setSystemPrefersDark(e.matches)
    mediaQuery.addEventListener('change', handler)
    return () => mediaQuery.removeEventListener('change', handler)
  }, [])

  if (setting === 'system') {
    return systemPrefersDark ? 'dark' : 'light'
  }
  return setting
}

export {useTheme}

export type {ThemeName}
