// @visibility(//visibility:public)

import type {ReadData} from '../shared/schema'
import type {SchemaOf} from './world-attribute'
import type {VideoControls} from './components'
import type {EcsTextureKey} from './texture-types'

type VideoControlsRuntimeSettings = ReadData<SchemaOf<typeof VideoControls>>

// Query parameters use AND logic - the query matches only if ALL provided parameters match.
// If no parameters are provided, it matches all videos.
type VideoQuery = {
  src?: string
  textureKey?: EcsTextureKey
}

type VideoTimeResult = {
  src: string
  textureKey: EcsTextureKey
  time: number
}

type VideoUserData = {
  url: string
  localUrl: string
  accessor?: number
  mimeType?: string
}

export type {
  VideoQuery,
  VideoTimeResult,
  VideoControlsRuntimeSettings,
  VideoUserData,
}
