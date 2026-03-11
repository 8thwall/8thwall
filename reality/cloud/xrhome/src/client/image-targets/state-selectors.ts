import type {ImageTargetFilterOptions, ImageTargetReduxState} from './types'
import type {IImageTarget} from '../common/types/models'
import {DEFAULT_FILTER_OPTIONS} from './reducer'

const selectImageTargetsForApp = (
  appUuid: string,
  state: ImageTargetReduxState
): IImageTarget[] => {
  const targetInfo = state.targetInfoByApp[appUuid]
  return targetInfo?.targetUuids?.map(uuid => state.targetsByUuid[uuid])
}

const selectImageTargetsInGallery = (
  appUuid: string,
  galleryUuid: string,
  state: ImageTargetReduxState
): IImageTarget[] => {
  const targetInfo = state.targetInfoByApp[appUuid]
  return targetInfo?.galleries?.[galleryUuid]?.uuids.map(uuid => state.targetsByUuid[uuid])
}

const selectTargetsGalleryFilterOptions = (
  appUuid: string,
  galleryUuid: string,
  state: ImageTargetReduxState
): ImageTargetFilterOptions => {
  const targetInfo = state.targetInfoByApp[appUuid]
  return targetInfo?.galleries?.[galleryUuid]?.filters ?? DEFAULT_FILTER_OPTIONS
}

export {
  selectImageTargetsForApp,
  selectImageTargetsInGallery,
  selectTargetsGalleryFilterOptions,
}
