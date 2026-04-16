/* eslint-disable camelcase */
import type {DeepReadonly} from 'ts-essentials'
import type {ImageTargetData} from '@repo/reality/shared/desktop/image-target-api'

import type {
  Accounts,
  ImageTargets,
  AppTags,
  FeaturedAppImages,
  User_Accounts,
  Modules,
  ModuleUsers,
  PolicyViolation,
  Feature,
  AssetRequest,
} from './db'
import type {ResponsiveAccountIcons} from '../../../shared/responsive-account-icons'

type AssetGeneration = {
  uuid: NonNullable<string>
  RequestUuid: NonNullable<string>
  AccountUuid: NonNullable<string>
  UserUuid: NonNullable<string>
  assetType: NonNullable<'IMAGE' | 'MESH'>
  modelName: NonNullable<string>
  prompt: string | null
  negativePrompt: string | null
  style: string | null
  seed: NonNullable<string>
  metadata: Record<string, unknown> | null
  createdAt: NonNullable<string>
  updatedAt: NonNullable<string>
}

type IAppTag = DeepReadonly<AppTags & {
  deleted?: boolean
}>

interface IFeaturedAppImage extends DeepReadonly<FeaturedAppImages> {}

interface IApp {
  appKey: string
  uuid: string
  appTitle?: string | undefined
  appName: string
  repoId: string
  assetLimitOverrides?: undefined
}

type IUserAccount = User_Accounts

interface IFullAccount extends DeepReadonly<Omit<Accounts, 'icon'> & {
  isWeb: boolean
  isCamera: boolean
  webPublic: boolean
  is8: boolean
  maxAppCount: number
  icon: ResponsiveAccountIcons

  Accounts: IFullAccount[]
  Users: IUserAccount[]
  PolicyViolations?: PolicyViolation[]
  Features?: Feature[]
  shortNameChangeRequired: boolean
}> {}

interface ImageTarget extends Omit<ImageTargets & ImageTargetData,
  'originalImagePath' | 'imagePath'|'luminanceImagePath' | 'thumbnailImagePath' |
  'geometryTextureImagePath' | 'physicalWidthInMeters' | 'moveable' |
  'geometryTexturePath' | 'scanDataPath' | 'type'
> {}

interface IImageTarget extends ImageTarget {
  type: ImageTargetData['type']
  originalImageSrc: string
  imageSrc: string
  thumbnailImageSrc: string
  geometryTextureImageSrc: string
  appKey: string
}

type IModuleUser = ModuleUsers

interface IModule extends DeepReadonly<Modules & {
  moduleUser?: IModuleUser
  Tags?: IAppTag[]
  FeaturedImages?: IFeaturedAppImage[]
  Account?: never
}> {}

interface IPublicApp extends DeepReadonly<IApp> {}

type ExternalAccountFields = 'name' | 'uuid' | 'icon' | 'shortName' | 'accountType'

interface IExternalAccount extends DeepReadonly<{
  [k in keyof IFullAccount]: k extends ExternalAccountFields ? Accounts[k] : never
}> {}

type IAccount = IExternalAccount | IFullAccount

interface IAssetRequest extends DeepReadonly<AssetRequest> {
  AssetGenerations: DeepReadonly<AssetGeneration>[]
}
export {
  IApp,
  IAccount,
  IFullAccount,
  IAssetRequest,
  IImageTarget,
  IModule,
  IExternalAccount,
  IPublicApp,
}
