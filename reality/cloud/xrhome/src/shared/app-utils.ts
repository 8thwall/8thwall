import type {IApp} from '../client/common/types/models'

type PickApp<T extends keyof (IApp)> = Pick<IApp, T>

const getDisplayNameForApp = (app: PickApp<'appTitle' | 'appName'>) => app.appTitle || app.appName

type AppCheck = (app: {}) => boolean

// TODO(christoph): Clean up
const isCloudStudioApp: AppCheck = () => true
const isActiveCommercialApp: AppCheck = () => false

export {
  getDisplayNameForApp,
  isActiveCommercialApp,
  isCloudStudioApp,
}

export type {
  // CommercialProjectType,
}
