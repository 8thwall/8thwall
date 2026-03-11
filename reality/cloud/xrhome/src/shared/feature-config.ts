import {FeatureCategory, FeatureBillingType} from './feature-constants'
import type {IAllFeatures, ICreditGrantOption} from './feature-types'

// The structure of features: {category: {featureName: {optionName: {...metadata}}}
const FEATURES: IAllFeatures = {
  Account: {
    name: FeatureCategory.Account,

    CreditGrant: {
      name: 'CreditGrant',

      Free: {
        name: 'Free',
        planName: 'Free',
        creditAmount: 50,
        price: 0,
      },

      CoreSub: {
        name: 'CoreSub',
        planName: 'Core',
        creditAmount: 500,
        price: 20,
        billingType: FeatureBillingType.Recurring,
      },

      PowerSub: {
        name: 'PowerSub',
        planName: 'Power',
        creditAmount: 3125,
        price: 100,
        billingType: FeatureBillingType.Recurring,
      },

      FreeTopUp: {
        name: 'FreeTopUp',
        planName: 'TopUp',
        creditAmount: 1,
        price: 0.1,
        billingType: FeatureBillingType.OneTime,
        presetCreditAmounts: [100, 200],
        minimumCreditAmount: 100,  // $10
        maximumCreditAmount: 10000000 - 10,  // $1M - $1
        isTopUp: true,
      },

      CoreTopUp: {
        name: 'CoreTopUp',
        planName: 'TopUp',
        creditAmount: 1,
        price: 0.05,
        billingType: FeatureBillingType.OneTime,
        requiredOptions: ['CoreSub'],
        presetCreditAmounts: [200, 400],
        minimumCreditAmount: 200,  // $10
        maximumCreditAmount: 20000000 - 20,  // $1M - $1
        isTopUp: true,
      },

      PowerTopUp: {
        name: 'PowerTopUp',
        planName: 'TopUp',
        creditAmount: 1,
        price: 0.04,
        billingType: FeatureBillingType.OneTime,
        requiredOptions: ['PowerSub'],
        presetCreditAmounts: [500, 1250],
        minimumCreditAmount: 250,  // $10
        maximumCreditAmount: 25000000 - 25,  // $1M - $1
        isTopUp: true,
      },

      Enterprise: {
        name: 'Enterprise',
        planName: 'Enterprise',
      },
    },
  },
  App: {
    name: FeatureCategory.App,

    AppKey: {
      name: 'AppKey',
      Default: {
        name: 'AppKey',
        planName: 'AppKey',
        billingType: FeatureBillingType.Recurring,
        billingPeriods: [
          1,  // 1 month
          12,  // 1 year
        ],
      },
    },
  },
}

const ACCOUNT_FEATURES = FEATURES.Account
const CREDIT_GRANT_FEATURE = ACCOUNT_FEATURES.CreditGrant
const FREE_TOP_UP_OPTION = CREDIT_GRANT_FEATURE.FreeTopUp
const CORE_TOP_UP_OPTION = CREDIT_GRANT_FEATURE.CoreTopUp
const POWER_TOP_UP_OPTION = CREDIT_GRANT_FEATURE.PowerTopUp

const getTopUpOption = (optionName: string) => Object
  .values(CREDIT_GRANT_FEATURE).find(
    (option): option is ICreditGrantOption => typeof option !== 'string' &&
      option.billingType === FeatureBillingType.OneTime &&
      option.requiredOptions?.includes(optionName)
  )

export {
  FEATURES,
  ACCOUNT_FEATURES,
  CREDIT_GRANT_FEATURE,
  FREE_TOP_UP_OPTION,
  CORE_TOP_UP_OPTION,
  POWER_TOP_UP_OPTION,
  getTopUpOption,
}
