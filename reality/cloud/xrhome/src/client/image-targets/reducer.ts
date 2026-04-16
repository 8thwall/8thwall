import {
  AppImageTargetInfo,
  SET_GALLERY_FILTER, SetTargetsGalleryFilterAction, ImageTargetAction, ImageTargetMessage,
  ImageTargetReduxState, ResetTargetsGalleryFilterAction, RESET_GALLERY_FILTER,
  ImageTargetFilterOptions,
  ImageTargetGallery,
} from './types'

type ActionFunction =
  (state: ImageTargetReduxState, action: ImageTargetAction) => ImageTargetReduxState

const initialState: ImageTargetReduxState = {
  targetInfoByApp: {},
}

const DEFAULT_FILTER_OPTIONS: ImageTargetFilterOptions = {
  nameLike: null,
  type: [],
  hasMetadata: false,
}

const DEFAULT_GALLERY: ImageTargetGallery = {
  filters: DEFAULT_FILTER_OPTIONS,
}

const getTargetInfoForApp = (state: ImageTargetReduxState, appUuid: string): AppImageTargetInfo => (
  state.targetInfoByApp[appUuid] || {
    galleries: {},
  }
)

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

const filterGalleryTargets = (
  state: ImageTargetReduxState,
  action: SetTargetsGalleryFilterAction
): ImageTargetReduxState => {
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

const resetGalleryFilter = (
  state: ImageTargetReduxState,
  action: ResetTargetsGalleryFilterAction
): ImageTargetReduxState => {
  const {galleryUuid, appUuid} = action
  const galleries = state.targetInfoByApp[appUuid]?.galleries
  return getUpdateForAppState(state, appUuid, {
    galleries: {
      ...galleries,
      [galleryUuid]: {
        filters: DEFAULT_FILTER_OPTIONS,
      },
    },
  })
}

const actions: Record<ImageTargetMessage, ActionFunction> = {
  [SET_GALLERY_FILTER]: filterGalleryTargets,
  [RESET_GALLERY_FILTER]: resetGalleryFilter,
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
