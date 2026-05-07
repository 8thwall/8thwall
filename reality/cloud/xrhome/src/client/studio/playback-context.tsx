import {INLINE_SIMULATOR_FEATURE} from '@ecs/shared/features/inline-simulator'
import React from 'react'

import {useFeatureEnabled} from './runtime-version/use-feature-enabled'

type PlaybackContext = {
  simulatorEnabled: boolean
}

const playbackContext = React.createContext<PlaybackContext | null>(null)

const usePlaybackContext = (): PlaybackContext | null => {
  const ctx = React.useContext(playbackContext)
  if (!ctx) {
    throw new Error('usePlaybackContext must be used within a PlaybackContextProvider')
  }
  return ctx
}

const PlaybackContextProvider: React.FC<{children: React.ReactNode}> = ({children}) => {
  const simulatorEnabled = useFeatureEnabled(INLINE_SIMULATOR_FEATURE)
  return (
    <playbackContext.Provider value={{simulatorEnabled}}>
      {children}
    </playbackContext.Provider>
  )
}

export {
  PlaybackContextProvider,
  usePlaybackContext,
}

export type {
  PlaybackContext,
}
