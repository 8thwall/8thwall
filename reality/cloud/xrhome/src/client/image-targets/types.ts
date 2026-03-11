import type {DeepReadonly} from 'ts-essentials'

import type {IImageTarget} from '../common/types/models'

const ADD_IMAGE_TARGETS_FOR_APP = 'IMAGE_TARGET/ADD'
const CLEAR_IMAGE_TARGETS_FOR_APP = 'IMAGE_TARGET/CLEAR'
const UPDATE_IMAGE_TARGET = 'IMAGE_TARGET/UPDATE'
const DELETE_IMAGE_TARGET_FOR_APP = 'IMAGE_TARGET/DELETE'

const ADD_GALLERY_TARGETS = 'IMAGE_TARGET/ADD_GALLERY'
const CLEAR_GALLERY_TARGETS = 'IMAGE_TARGET/CLEAR_GALLERY'
const LOADING_GALLERY_TARGETS = 'IMAGE_TARGET/LOADING_GALLERY'
const SET_GALLERY_FILTER = 'IMAGE_TARGET/SET_GALLERY_FILTER'
const SET_UPLOAD_URL = 'IMAGE_TARGET/SET_UPLOAD_URL'

 type ImageTargetMessage =
  typeof ADD_IMAGE_TARGETS_FOR_APP |
  typeof CLEAR_IMAGE_TARGETS_FOR_APP |
  typeof UPDATE_IMAGE_TARGET |
  typeof DELETE_IMAGE_TARGET_FOR_APP |
  typeof ADD_GALLERY_TARGETS |
  typeof CLEAR_GALLERY_TARGETS |
  typeof LOADING_GALLERY_TARGETS |
  typeof SET_GALLERY_FILTER |
  typeof SET_UPLOAD_URL

 type ImageTargetStatus = 'loading-initial' | 'loading-additional' | 'loaded' | 'cleared'

interface ImageTargetGallery extends DeepReadonly<{
  uuids: string[]
  status: ImageTargetStatus
  continuation: string
  filters: ImageTargetFilterOptions
}> {}

interface AppImageTargetInfo extends DeepReadonly<{
  targetUuids: string[]
  galleries: Record<string, ImageTargetGallery>
}> {}

 type ImageTargetOrdering = 'created' | 'updated' | 'name'
 type ImageTargetOrderDirection = 'asc' | 'desc'
 type ImageTargetGeometryFilter = 'flat' | 'cylindrical' | 'conical'
 type ImageTargetFilterFlag = 'autoload' | 'metadata'
 type ImageTargetFilterFlagValue = 'set' | 'unset' | 'true' | 'false'

interface ImageTargetFilterOptions extends DeepReadonly<{
  nameLike: string
  type: ImageTargetGeometryFilter[]
  autoload: ImageTargetFilterFlagValue[]
  metadata: ImageTargetFilterFlagValue[]
  by: ImageTargetOrdering[]
  dir: ImageTargetOrderDirection[]
}> {}

interface ImageTargetReduxState extends DeepReadonly<{
  targetsByUuid: Record<string, IImageTarget>
  targetInfoByApp: Record<string, AppImageTargetInfo>
  scanTargetUploadUrlByApp: Record<string, string>
}> {}

interface AddImageTargetsForAppAction {
  type: typeof ADD_IMAGE_TARGETS_FOR_APP
  appUuid: string
  imageTargets: IImageTarget[]
}

interface ClearImageTargetsForAppAction {
  type: typeof CLEAR_IMAGE_TARGETS_FOR_APP
  appUuid: string
}

interface UpdateImageTargetAction {
  type: typeof UPDATE_IMAGE_TARGET
  imageTarget: IImageTarget
}

interface DeleteImageTargetAction {
  type: typeof DELETE_IMAGE_TARGET_FOR_APP
  appUuid: string
  targetUuid: string
}

// Since we reload it everytime options change it doesn't make sense to persist gallery per app.
// Since gallery view is paginated we always expect a continuation token, if it is missing
// that signals the end of the pagination.
// Sets gallery status to loaded.
interface AddGalleryTargetsAction {
  type: typeof ADD_GALLERY_TARGETS
  appUuid: string
  galleryUuid: string
  targetUuids: string[]
  continuation: string
}

interface ClearGalleryTargetsAction {
  type: typeof CLEAR_GALLERY_TARGETS
  appUuid: string
  galleryUuid: string
}

interface LoadingGalleryTargetsAction {
  type: typeof LOADING_GALLERY_TARGETS
  appUuid: string
  galleryUuid: string
  beginPagination: boolean
}

interface SetTargetsGalleryFilterAction {
  type: typeof SET_GALLERY_FILTER
  appUuid: string
  galleryUuid: string
  options: ImageTargetFilterOptions
}

interface SetUploadUrlAction {
  type: typeof SET_UPLOAD_URL
  appUuid: string
  url: string
}

 type ImageTargetAction =
  AddImageTargetsForAppAction |
  ClearImageTargetsForAppAction |
  UpdateImageTargetAction |
  DeleteImageTargetAction |
  AddGalleryTargetsAction |
  ClearGalleryTargetsAction |
  LoadingGalleryTargetsAction |
  SetTargetsGalleryFilterAction |
  SetUploadUrlAction

export {
  ADD_IMAGE_TARGETS_FOR_APP,
  CLEAR_IMAGE_TARGETS_FOR_APP,
  UPDATE_IMAGE_TARGET,
  DELETE_IMAGE_TARGET_FOR_APP,
  ADD_GALLERY_TARGETS,
  CLEAR_GALLERY_TARGETS,
  LOADING_GALLERY_TARGETS,
  SET_GALLERY_FILTER,
  SET_UPLOAD_URL,
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
  AddImageTargetsForAppAction,
  ClearImageTargetsForAppAction,
  UpdateImageTargetAction,
  DeleteImageTargetAction,
  AddGalleryTargetsAction,
  ClearGalleryTargetsAction,
  LoadingGalleryTargetsAction,
  SetTargetsGalleryFilterAction,
  SetUploadUrlAction,
  ImageTargetAction,
}
