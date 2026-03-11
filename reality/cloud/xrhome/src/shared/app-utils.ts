import {
  COVER_IMAGE_PREVIEW_SIZES,
} from './app-constants'
import type {IApp} from '../client/common/types/models'

type PickApp<T extends keyof (IApp)> = Pick<IApp, T>

// TODO(christoph): Remove cdn dependency
const deriveAppCoverImageUrl = (_app: {}, size = COVER_IMAGE_PREVIEW_SIZES[1200]) => {
  const width = size[0]
  const height = size[1]
  return `https://cdn.8thwall.com/apps/cover/default0-preview-${width}x${height}`
}

const getDisplayNameForApp = (app: PickApp<'appTitle' | 'appName'>) => app.appTitle || app.appName

type AppCheck = (app: {}) => boolean

// TODO(christoph): Clean up
const isCloudStudioApp: AppCheck = () => true
const isActiveCommercialApp: AppCheck = () => false

export {
  deriveAppCoverImageUrl,
  getDisplayNameForApp,
  isActiveCommercialApp,
  isCloudStudioApp,
}

export type {
  // CommercialProjectType,
}
