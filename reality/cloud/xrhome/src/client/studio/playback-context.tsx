import React from 'react'

type PlaybackContext = {
  supportsSequences: boolean
  supportsStudioSync: boolean
}

const playbackContext = React.createContext<PlaybackContext | null>(null)

const CURRENT_VALUE: PlaybackContext = BuildIf.STUDIO_DEV8_INTEGRATION_20260205
  ? {
    supportsSequences: true,
    supportsStudioSync: true,
  }
  : {
    supportsSequences: false,
    supportsStudioSync: false,
  }

const usePlaybackContext = (): PlaybackContext | null => {
  const ctx = React.useContext(playbackContext)
  if (!ctx) {
    throw new Error('usePlaybackContext must be used within a PlaybackContextProvider')
  }
  return ctx
}

const PlaybackContextProvider: React.FC<{children: React.ReactNode}> = ({children}) => (
  <playbackContext.Provider value={CURRENT_VALUE}>
    {children}
  </playbackContext.Provider>
)

export {
  PlaybackContextProvider,
  usePlaybackContext,
}

export type {
  PlaybackContext,
}
