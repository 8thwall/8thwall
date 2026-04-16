import {
  SET_GALLERY_FILTER,
  ImageTargetFilterOptions,
  RESET_GALLERY_FILTER,
  ResetTargetsGalleryFilterAction,
  SetTargetsGalleryFilterAction,
} from './types'
import type {DispatchifiedActions} from '../common/types/actions'
import {dispatchify} from '../common'

const resetGalleryFilterOptionsForApp = (
  appUuid: string, galleryUuid: string
): ResetTargetsGalleryFilterAction => ({
  type: RESET_GALLERY_FILTER || 'IMAGE_TARGET/RESET_GALLERY_FILTER',
  appUuid,
  galleryUuid,
})

// TODO(christoph): Clean up
const noopAction = (name: string): any => () => () => {
  // eslint-disable-next-line no-console
  console.warn(`${name} is a noop, will be deleted`)
}

// Performs a fresh fetch after setting options.
const setGalleryFilterOptionsForApp = (
  appUuid: string, galleryUuid: string, options: Partial<ImageTargetFilterOptions>
): SetTargetsGalleryFilterAction => ({
  type: SET_GALLERY_FILTER,
  appUuid,
  galleryUuid,
  options,
})

export const rawActions = {
  fetchSingleTargetForApp: noopAction('fetchSingleTargetForApp'),
  fetchImageTargetsForApp: noopAction('fetchImageTargetsForApp'),
  fetchAdditionalGalleryTargets: noopAction('fetchAdditionalGalleryTargets'),
  setGalleryFilterOptionsForApp,
  resetGalleryFilterOptionsForApp,
}

export type ImageTargetActions = DispatchifiedActions<typeof rawActions>

export default dispatchify(rawActions)
