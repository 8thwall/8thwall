import type {DeepReadonly} from 'ts-essentials'

import type {ImageTargetType} from '../common/types/db'

const SET_GALLERY_FILTER = 'IMAGE_TARGET/SET_GALLERY_FILTER'
const RESET_GALLERY_FILTER = 'IMAGE_TARGET/RESET_GALLERY_FILTER'

 type ImageTargetMessage = typeof SET_GALLERY_FILTER
  | typeof RESET_GALLERY_FILTER

 type ImageTargetStatus = 'loading-initial' | 'loading-additional' | 'loaded' | 'cleared'

interface ImageTargetGallery extends DeepReadonly<{
  filters: ImageTargetFilterOptions
}> {}

interface AppImageTargetInfo extends DeepReadonly<{
  galleries: Record<string, ImageTargetGallery>
}> {}

 type ImageTargetFilterFlag = 'hasMetadata'

interface ImageTargetFilterOptions extends DeepReadonly<{
  nameLike: string | null
  type: ImageTargetType[]
  hasMetadata: boolean
}> {}

interface ImageTargetReduxState extends DeepReadonly<{
  targetInfoByApp: Record<string, AppImageTargetInfo>
}> {}

interface SetTargetsGalleryFilterAction {
  type: typeof SET_GALLERY_FILTER
  appUuid: string
  galleryUuid: string
  options: Partial<ImageTargetFilterOptions>
}

interface ResetTargetsGalleryFilterAction {
  type: typeof RESET_GALLERY_FILTER
  appUuid: string
  galleryUuid: string
}

 type ImageTargetAction = SetTargetsGalleryFilterAction
  | ResetTargetsGalleryFilterAction

export {
  SET_GALLERY_FILTER,
  RESET_GALLERY_FILTER,
}

export type {
  ImageTargetGallery,
  ImageTargetMessage,
  ImageTargetStatus,
  AppImageTargetInfo,
  ImageTargetFilterFlag,
  ImageTargetFilterOptions,
  ImageTargetReduxState,
  SetTargetsGalleryFilterAction,
  ImageTargetAction,
  ResetTargetsGalleryFilterAction,
}
