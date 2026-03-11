import {batch} from 'react-redux'

import authenticatedFetch from '../common/authenticated-fetch'
import {
  ADD_GALLERY_TARGETS,
  ADD_IMAGE_TARGETS_FOR_APP, CLEAR_GALLERY_TARGETS,
  DELETE_IMAGE_TARGET_FOR_APP,
  SET_GALLERY_FILTER,
  ImageTargetFilterOptions,
  LOADING_GALLERY_TARGETS,
  SET_UPLOAD_URL,
} from './types'
import {derivedImageTarget} from '../apps/image-targets'
import type {DispatchifiedActions} from '../common/types/actions'
import {dispatchify} from '../common'
import {DEFAULT_FILTER_OPTIONS} from './reducer'
import {selectTargetsGalleryFilterOptions} from './state-selectors'

const onError = msg => dispatch => dispatch({type: 'ERROR', msg})

const resetGalleryFilterOptionsForApp = (appUuid: string, galleryUuid: string) => ({
  type: SET_GALLERY_FILTER,
  appUuid,
  galleryUuid,
  options: {...DEFAULT_FILTER_OPTIONS},
})

const fetchImageTargetsForApp = (
  appUuid: string, galleryUuid: string, limit: number = 20
) => async (dispatch, getState) => {
  const options = selectTargetsGalleryFilterOptions(appUuid, galleryUuid, getState().imageTargets)
  let searchParams = `?limit=${limit}&`
  Object.keys(options).forEach((key) => {
    if (Array.isArray(options[key])) {
      options[key].forEach((value) => {
        searchParams += `${key}=${encodeURIComponent(value)}&`
      })
    } else if (typeof options[key] === 'string') {
      searchParams += `${key}=${encodeURIComponent(options[key])}&`
    }
  })
  dispatch({
    type: LOADING_GALLERY_TARGETS,
    appUuid,
    galleryUuid,
    beginPagination: true,
  })

  try {
    const res = await dispatch(authenticatedFetch(`/v1/image-targets/${appUuid}${searchParams}`))
    batch(() => {
      dispatch({
        type: CLEAR_GALLERY_TARGETS,
        appUuid,
        galleryUuid,
      })
      dispatch({
        type: ADD_IMAGE_TARGETS_FOR_APP,
        appUuid,
        imageTargets: res.imageTargets.map(derivedImageTarget),
      })
      dispatch({
        type: ADD_GALLERY_TARGETS,
        appUuid,
        galleryUuid,
        targetUuids: res.imageTargets.map(({uuid}) => uuid),
        continuation: res.continuationToken,
      })
    })

    return res.imageTargets
  } catch (error) {
    dispatch(onError(error.message))
    throw error
  }
}

const fetchAdditionalGalleryTargets = (
  appUuid: string, galleryUuid: string
) => async (dispatch, getState) => {
  const galleries = getState().imageTargets.targetInfoByApp[appUuid]?.galleries

  if (!galleries || !galleries[galleryUuid]) {
    return
  }

  const {continuation, status} = galleries[galleryUuid]

  if (status?.startsWith('loading') || !continuation) {
    return
  }

  dispatch({
    type: LOADING_GALLERY_TARGETS,
    appUuid,
    galleryUuid,
    beginPagination: false,
  })

  try {
    const {imageTargets, continuationToken} = await dispatch(
      authenticatedFetch(`/v1/image-targets/${appUuid}?continuation=${continuation}`, {
        method: 'GET',
      })
    )
    batch(() => {
      dispatch({
        type: ADD_IMAGE_TARGETS_FOR_APP,
        appUuid,
        imageTargets: imageTargets.map(derivedImageTarget),
      })
      dispatch({
        type: ADD_GALLERY_TARGETS,
        appUuid,
        galleryUuid,
        targetUuids: imageTargets.map(({uuid}) => uuid),
        continuation: continuationToken,
      })
    })
  } catch (err) {
    dispatch(onError(err.message))
  }
}

const fetchSingleTargetForApp = (
  appUuid: string,
  uuid: string
) => (dispatch) => {
  dispatch(authenticatedFetch(
    `/v1/image-targets/${appUuid}?by=uuid&start=${encodeURIComponent(uuid)}&limit=1`
  )).then((res) => {
    dispatch({
      type: ADD_IMAGE_TARGETS_FOR_APP,
      appUuid,
      imageTargets: res.imageTargets.map(derivedImageTarget),
    })
  }).catch((error) => {
    dispatch(onError(error.message))
  })
}

const fetchSingleTargetByNameForApp = (
  appUuid: string,
  name: string
) => async (dispatch) => {
  try {
    const res = await dispatch(authenticatedFetch(
      `/v1/image-targets/${appUuid}?by=name&start=${encodeURIComponent(name)}&limit=1`
    ))
    dispatch({
      type: ADD_IMAGE_TARGETS_FOR_APP,
      appUuid,
      imageTargets: res.imageTargets.map(derivedImageTarget),
    })
  } catch (error) {
    dispatch(onError(error.message))
  }
}

const deleteImageTargetForApp = (
  appUuid: string,
  targetUuid: string
) => async (dispatch) => {
  dispatch({
    type: DELETE_IMAGE_TARGET_FOR_APP,
    appUuid,
    targetUuid,
  })
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
  dispatch(fetchImageTargetsForApp(appUuid, galleryUuid))
}

const getScanUploadInfoForApp = (appUuid: string) => async (dispatch) => {
  try {
    const uploadInfo = await dispatch(authenticatedFetch(
      `/v1/image-targets/scan-upload-info/${appUuid}`
    ))

    dispatch({
      type: SET_UPLOAD_URL,
      appUuid,
      url: uploadInfo.uploadUrl,
    })
  } catch (error) {
    dispatch(onError(error.message))
  }
}

export const rawActions = {
  fetchSingleTargetForApp,
  fetchSingleTargetByNameForApp,
  fetchImageTargetsForApp,
  deleteImageTargetForApp,
  fetchAdditionalGalleryTargets,
  setGalleryFilterOptionsForApp,
  resetGalleryFilterOptionsForApp,
  getScanUploadInfoForApp,
}

export type ImageTargetActions = DispatchifiedActions<typeof rawActions>

export default dispatchify(rawActions)
