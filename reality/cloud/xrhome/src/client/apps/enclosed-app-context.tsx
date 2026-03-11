import React from 'react'

import type {IApp} from '../common/types/models'

const EnclosedAppContext = React.createContext<IApp>(null)

/**
 * This hook returns the current app, if wrapped in an EnclosedAppProvider.
 * If outside an EnclosedAppProvider, it will return null.
 * This can be used to enable app-specific functionality if it is available.
 * This would be used most commonly in components that should work with both apps and modules, or
 * in studio so that the studio playground (/cloud-studio) doesn't crash.
 */

const EnclosedAppProvider = EnclosedAppContext.Provider

const useEnclosedAppKey = () => {
  const appContext = React.useContext(EnclosedAppContext)

  if (appContext) {
    return appContext.appKey
  }
  throw new Error('Missing EnclosedAppContext')
}

const useEnclosedApp = () => React.useContext(EnclosedAppContext)

export {
  useEnclosedApp,
  useEnclosedAppKey,
  EnclosedAppProvider,
}
