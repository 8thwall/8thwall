import {useEnclosedApp} from '../apps/enclosed-app-context'

/**
 * This hook returns the current app, expecting to be wrapped in an EnclosedAppProvider.
 * If not, it will throw an error.
 */
const useCurrentApp = () => {
  const enclosedApp = useEnclosedApp()
  if (enclosedApp) {
    return enclosedApp
  }
  throw new Error('Missing EnclosedAppContext')
}

export default useCurrentApp
