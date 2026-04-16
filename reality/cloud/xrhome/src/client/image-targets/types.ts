import type {DeepReadonly} from 'ts-essentials'

const SET_GALLERY_FILTER = 'IMAGE_TARGET/SET_GALLERY_FILTER'

 type ImageTargetMessage = typeof SET_GALLERY_FILTER

 type ImageTargetStatus = 'loading-initial' | 'loading-additional' | 'loaded' | 'cleared'

interface ImageTargetGallery extends DeepReadonly<{
  filters: ImageTargetFilterOptions
}> {}

interface AppImageTargetInfo extends DeepReadonly<{
  galleries: Record<string, ImageTargetGallery>
}> {}

 type ImageTargetOrdering = 'created' | 'updated' | 'name'
 type ImageTargetOrderDirection = 'asc' | 'desc'
 type ImageTargetGeometryFilter = 'flat' | 'cylindrical' | 'conical'
 type ImageTargetFilterFlag = 'metadata'
 type ImageTargetFilterFlagValue = 'set' | 'unset' | 'true' | 'false'

interface ImageTargetFilterOptions extends DeepReadonly<{
  nameLike: string | null
  type: ImageTargetGeometryFilter[]
  metadata: ImageTargetFilterFlagValue[]
  by: ImageTargetOrdering[]
  dir: ImageTargetOrderDirection[]
}> {}

interface ImageTargetReduxState extends DeepReadonly<{
  targetInfoByApp: Record<string, AppImageTargetInfo>
}> {}

interface SetTargetsGalleryFilterAction {
  type: typeof SET_GALLERY_FILTER
  appUuid: string
  galleryUuid: string
  options: ImageTargetFilterOptions
}

 type ImageTargetAction = SetTargetsGalleryFilterAction

export {
  SET_GALLERY_FILTER,
}

export type {
  ImageTargetGallery,
  ImageTargetMessage,
  ImageTargetStatus,
  AppImageTargetInfo,
  ImageTargetOrdering,
  ImageTargetOrderDirection,
  ImageTargetGeometryFilter,
  ImageTargetFilterFlag,
  ImageTargetFilterFlagValue,
  ImageTargetFilterOptions,
  ImageTargetReduxState,
  SetTargetsGalleryFilterAction,
  ImageTargetAction,
}
