import {
  SET_GALLERY_FILTER,
  ImageTargetFilterOptions,
} from './types'
import type {DispatchifiedActions} from '../common/types/actions'
import {dispatchify} from '../common'
import {DEFAULT_FILTER_OPTIONS} from './reducer'

const resetGalleryFilterOptionsForApp = (appUuid: string, galleryUuid: string) => ({
  type: SET_GALLERY_FILTER,
  appUuid,
  galleryUuid,
  options: {...DEFAULT_FILTER_OPTIONS},
})

// TODO(christoph): Clean up
const noopAction = (name: string): any => () => () => {
  // eslint-disable-next-line no-console
  console.warn(`${name} is a noop, will be deleted`)
}

// Performs a fresh fetch after setting options.
const setGalleryFilterOptionsForApp = (
  appUuid: string, galleryUuid: string, options: Partial<ImageTargetFilterOptions>
) => (dispatch, getState) => {
  dispatch({
    type: SET_GALLERY_FILTER,
    appUuid,
    galleryUuid,
    options: {
      ...getState().imageTargets.targetInfoByApp[appUuid]?.galleryFilters,
      ...options,
    },
  })
}

export const rawActions = {
  fetchSingleTargetForApp: noopAction('fetchSingleTargetForApp'),
  fetchImageTargetsForApp: noopAction('fetchImageTargetsForApp'),
  fetchAdditionalGalleryTargets: noopAction('fetchAdditionalGalleryTargets'),
  setGalleryFilterOptionsForApp,
  resetGalleryFilterOptionsForApp,
}

export type ImageTargetActions = DispatchifiedActions<typeof rawActions>

export default dispatchify(rawActions)
