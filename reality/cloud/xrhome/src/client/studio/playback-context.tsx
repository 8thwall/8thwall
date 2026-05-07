import React from 'react'

import {INLINE_SIMULATOR_FEATURE} from '@ecs/shared/features/inline-simulator'

import {useFeatureEnabled} from './runtime-version/use-feature-enabled'
import {useProjectConfigStatus} from '../hooks/use-project-config'

type PlaybackContext = {
  simulatorEnabled: boolean
}

// NOTE(christoph): For now we only depend on the feature flag, but there may be additional logic
// needed later.

const usePlaybackContext = (): PlaybackContext | null => {
  const {missingDev8} = useProjectConfigStatus()
  const simulatorEnabled = useFeatureEnabled(INLINE_SIMULATOR_FEATURE) && !missingDev8
  return {simulatorEnabled}
}

const PlaybackContextProvider = React.Fragment
export {
  PlaybackContextProvider,
  usePlaybackContext,
}

export type {
  PlaybackContext,
}
