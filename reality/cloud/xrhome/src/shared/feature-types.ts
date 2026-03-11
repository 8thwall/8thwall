import type {FeatureCategory, FeatureBillingType} from './feature-constants'

// Feature types
interface IFeatureOption {
  name: string
  planName: string
  price?: number
  billingType?: FeatureBillingType
  billingPeriods?: number[]
  requiredOptions?: readonly string[]
}
interface ICreditGrantOption extends IFeatureOption {
  creditAmount?: number
  presetCreditAmounts?: number[]
  minimumCreditAmount?: number
  maximumCreditAmount?: number
  isTopUp?: boolean
}
interface IFeature {
  name: string
}
interface ICreditGrantFeature extends IFeature {
  Free: ICreditGrantOption
  CoreSub: ICreditGrantOption
  PowerSub: ICreditGrantOption
  FreeTopUp: ICreditGrantOption
  CoreTopUp: ICreditGrantOption
  PowerTopUp: ICreditGrantOption
  Enterprise: ICreditGrantOption
}
interface IFeatures {
  name: FeatureCategory
}
interface IAccountFeatures extends IFeatures {
  CreditGrant: ICreditGrantFeature
}
interface IAppKeyFeature extends IFeature {
  Default: IFeatureOption
}
interface IAppFeatures extends IFeatures {
  AppKey: IAppKeyFeature
}
interface IAllFeatures {
  Account: IAccountFeatures
  App: IAppFeatures
}

// Feature Stripe types
interface IFeatureOptionStripe {
  productId: string
  priceId?: string
  priceIds?: {[key: number]: string}  // {<#_of_months>: <priceId>} For recurring billing only.
}
interface ILookupFeatureOptionStripe extends IFeatureOptionStripe {
  category: FeatureCategory
  featureName: string
  optionName: string
}
interface IFeatureStripe extends Record<string, IFeatureOptionStripe> {}
interface IFeaturesStripe extends Record<string, IFeatureStripe> {}
interface IAllFeaturesStripe extends Record<FeatureCategory, IFeaturesStripe> {}

export {
  IFeatureOption,
  IFeature,
  IFeatures,
  IAllFeatures,
  IAccountFeatures,
  ICreditGrantOption,
  ICreditGrantFeature,
  IFeatureOptionStripe,
  ILookupFeatureOptionStripe,
  IFeatureStripe,
  IFeaturesStripe,
  IAllFeaturesStripe,
}
