const ROOT_PATH = '/'

const HOME_PATH = '/home'

const LOCAL_STUDIO_PATH_FORMAT = '/local-studio/:appKey'

const getLocalStudioPath = (appKey: string) => (
  LOCAL_STUDIO_PATH_FORMAT.replace(':appKey', encodeURIComponent(appKey))
)

type LocalStudioPathParams = {
  appKey: string
}

export {
  HOME_PATH,
  ROOT_PATH,
  getLocalStudioPath,
  LOCAL_STUDIO_PATH_FORMAT,
}

export type {
  LocalStudioPathParams,
}
