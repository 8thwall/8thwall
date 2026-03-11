import {
  ADD_GALLERY_TARGETS, ADD_IMAGE_TARGETS_FOR_APP, AddGalleryTargetsAction,
  AddImageTargetsForAppAction, AppImageTargetInfo, CLEAR_GALLERY_TARGETS,
  CLEAR_IMAGE_TARGETS_FOR_APP, ClearGalleryTargetsAction, ClearImageTargetsForAppAction,
  SET_GALLERY_FILTER, SetTargetsGalleryFilterAction, ImageTargetAction, ImageTargetMessage,
  ImageTargetReduxState, LOADING_GALLERY_TARGETS, LoadingGalleryTargetsAction, UPDATE_IMAGE_TARGET,
  UpdateImageTargetAction, ImageTargetFilterOptions, SetUploadUrlAction, SET_UPLOAD_URL,
  ImageTargetGallery, DeleteImageTargetAction, DELETE_IMAGE_TARGET_FOR_APP,
} from './types'

type ActionFunction =
  (state: ImageTargetReduxState, action: ImageTargetAction) => ImageTargetReduxState

const initialState: ImageTargetReduxState = {
  targetsByUuid: {},
  targetInfoByApp: {},
  scanTargetUploadUrlByApp: {},
}

const DEFAULT_FILTER_OPTIONS: ImageTargetFilterOptions = {
  nameLike: null,
  type: [],
  autoload: [],
  metadata: [],
  by: ['created'],
  dir: ['desc'],
}

const DEFAULT_GALLERY: ImageTargetGallery = {
  filters: DEFAULT_FILTER_OPTIONS,
  uuids: [],
  status: 'loaded',
  continuation: null,
}

const getTargetInfoForApp = (state: ImageTargetReduxState, appUuid: string): AppImageTargetInfo => {
  const targetInfoForApp: AppImageTargetInfo = state.targetInfoByApp[appUuid] || {
    targetUuids: null,
    galleries: {},
  }
  return targetInfoForApp
}

const getUpdateForAppState = (
  state: ImageTargetReduxState,
  appUuid: string,
  update: Partial<AppImageTargetInfo>
): ImageTargetReduxState => {
  const appImageTargetState = getTargetInfoForApp(state, appUuid)
  const appImageTargetStateWithUpdate = {...appImageTargetState, ...update}
  const byAppUuidWithUpdate = {...state.targetInfoByApp, [appUuid]: appImageTargetStateWithUpdate}
  return {...state, targetInfoByApp: byAppUuidWithUpdate}
}

const addImageTargets = (
  state: ImageTargetReduxState,
  action: AddImageTargetsForAppAction
): ImageTargetReduxState => {
  const targetsByUuid = action.imageTargets.reduce((acc, it) => {
    acc[it.uuid] = it
    return acc
  }, {...state.targetsByUuid})
  const targetInfoForApp = getTargetInfoForApp(state, action.appUuid)
  return getUpdateForAppState({...state, targetsByUuid}, action.appUuid, {
    targetUuids: [
      ...new Set([
        ...(targetInfoForApp.targetUuids || []),
        ...action.imageTargets.map(it => it.uuid),
      ]),
    ],
  })
}

const clearImageTargets = (
  state: ImageTargetReduxState,
  action: ClearImageTargetsForAppAction
): ImageTargetReduxState => {
  const {targetsByUuid, targetInfoByApp} = state
  const {appUuid} = action
  const targetsToRemove = new Set(targetInfoByApp[appUuid]?.targetUuids)
  return {
    ...state,
    targetsByUuid: Object.keys(targetsByUuid).reduce((acc, uuid) => {
      if (!targetsToRemove.has(uuid)) {
        acc[uuid] = targetsByUuid[uuid]
      }
      return acc
    }, {}),
    targetInfoByApp: {
      ...targetInfoByApp,
      [appUuid]: null,
    },
  }
}

const updateImageTarget = (
  state: ImageTargetReduxState,
  action: UpdateImageTargetAction
): ImageTargetReduxState => ({
  ...state,
  targetsByUuid: {
    ...state.targetsByUuid,
    [action.imageTarget.uuid]: action.imageTarget,
  },
})

const deleteImageTarget = (
  state: ImageTargetReduxState,
  action: DeleteImageTargetAction
): ImageTargetReduxState => {
  const {appUuid, targetUuid} = action
  const targetsByUuid = {...state.targetsByUuid}
  delete targetsByUuid[targetUuid]

  const targetInfoForApp = getTargetInfoForApp(state, appUuid)
  const {galleries} = targetInfoForApp
  const updatedGalleries = galleries
    ? Object.keys(galleries).reduce((acc, galleryUuid) => {
      const gallery = galleries[galleryUuid]
      return {
        ...acc,
        [galleryUuid]: {
          ...gallery,
          uuids: gallery.uuids?.filter(uuid => uuid !== targetUuid) || [],
        },
      }
    }, {})
    : {}

  return getUpdateForAppState({...state, targetsByUuid}, appUuid, {
    targetUuids: targetInfoForApp.targetUuids?.filter(uuid => uuid !== targetUuid) || [],
    galleries: updatedGalleries,
  })
}

const addGalleryImageTargets = (
  state: ImageTargetReduxState,
  action: AddGalleryTargetsAction
): ImageTargetReduxState => {
  const {targetUuids, galleryUuid, continuation, appUuid} = action
  const galleries = state.targetInfoByApp[appUuid]?.galleries
  return getUpdateForAppState(state, appUuid, {
    galleries: {
      ...galleries,
      [galleryUuid]: {
        uuids: [...new Set([...(galleries?.[galleryUuid]?.uuids || []), ...targetUuids])],
        status: 'loaded',
        continuation,
        filters: galleries?.[galleryUuid]?.filters || DEFAULT_FILTER_OPTIONS,
      },
    },
  })
}

const clearImageTargetGallery = (
  state: ImageTargetReduxState,
  action: ClearGalleryTargetsAction
): ImageTargetReduxState => {
  const {galleryUuid, appUuid} = action
  const galleries: Record<string, ImageTargetGallery> = state.targetInfoByApp[appUuid]?.galleries
  return getUpdateForAppState(state, appUuid, {
    galleries: {
      ...galleries,
      [galleryUuid]: {
        filters: galleries?.[galleryUuid]?.filters || DEFAULT_FILTER_OPTIONS,
        uuids: [],
        status: 'cleared',
        continuation: null,
      },
    },
  })
}

const loadingGalleryTargets = (
  state: ImageTargetReduxState,
  action: LoadingGalleryTargetsAction
): ImageTargetReduxState => {
  const {galleryUuid, appUuid} = action
  const galleries = state.targetInfoByApp[appUuid]?.galleries
  return getUpdateForAppState(state, appUuid, {
    galleries: {
      ...galleries,
      [galleryUuid]: {
        ...(galleries?.[galleryUuid] || DEFAULT_GALLERY),
        status: action.beginPagination ? 'loading-initial' : 'loading-additional',
      },
    },
  })
}

const filterGalleryTargets = (
  state: ImageTargetReduxState,
  action: SetTargetsGalleryFilterAction
) => {
  const {galleryUuid, appUuid, options} = action
  const galleries = state.targetInfoByApp[appUuid]?.galleries
  const update: ImageTargetGallery = galleries?.[galleryUuid] || DEFAULT_GALLERY
  return getUpdateForAppState(state, appUuid, {
    galleries: {
      ...galleries,
      [galleryUuid]: {
        ...update,
        filters: {...update.filters, ...options},
      },
    },
  })
}

const setUploadUrl = (
  state: ImageTargetReduxState,
  action: SetUploadUrlAction
): ImageTargetReduxState => ({
  ...state,
  scanTargetUploadUrlByApp: {
    ...state.scanTargetUploadUrlByApp,
    [action.appUuid]: action.url,
  },
})

const actions: Record<ImageTargetMessage, ActionFunction> = {
  [ADD_IMAGE_TARGETS_FOR_APP]: addImageTargets,
  [CLEAR_IMAGE_TARGETS_FOR_APP]: clearImageTargets,
  [UPDATE_IMAGE_TARGET]: updateImageTarget,
  [ADD_GALLERY_TARGETS]: addGalleryImageTargets,
  [CLEAR_GALLERY_TARGETS]: clearImageTargetGallery,
  [LOADING_GALLERY_TARGETS]: loadingGalleryTargets,
  [SET_GALLERY_FILTER]: filterGalleryTargets,
  [SET_UPLOAD_URL]: setUploadUrl,
  [DELETE_IMAGE_TARGET_FOR_APP]: deleteImageTarget,
}

const Reducer = (state = initialState, action: ImageTargetAction): ImageTargetReduxState => {
  const handler = actions[action.type]
  if (!handler) {
    return state
  }
  return handler(state, action)
}

export {
  DEFAULT_FILTER_OPTIONS,
}

export default Reducer
