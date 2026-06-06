import type {IApp} from '../client/common/types/models'

type PickApp<T extends keyof (IApp)> = Pick<IApp, T>

const getDisplayNameForApp = (app: PickApp<'appTitle' | 'appName'>) => app.appTitle || app.appName

type AppCheck = (app: {}) => boolean

const isActiveCommercialApp: AppCheck = () => false

export {
  getDisplayNameForApp,
  isActiveCommercialApp,
}
