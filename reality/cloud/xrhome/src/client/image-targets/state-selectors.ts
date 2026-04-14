import type {ImageTargetFilterOptions, ImageTargetReduxState} from './types'
import {DEFAULT_FILTER_OPTIONS} from './reducer'

const selectTargetsGalleryFilterOptions = (
  appUuid: string,
  galleryUuid: string,
  state: ImageTargetReduxState
): ImageTargetFilterOptions => {
  const targetInfo = state.targetInfoByApp[appUuid]
  return targetInfo?.galleries?.[galleryUuid]?.filters ?? DEFAULT_FILTER_OPTIONS
}

export {
  selectTargetsGalleryFilterOptions,
}
