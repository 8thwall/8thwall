import type {DeepReadonly} from 'ts-essentials'

import {ACCOUNT_FEATURES, CREDIT_GRANT_FEATURE} from './feature-config'
// TODO(kim): Please refactor to not depend on client side code
import type {CreditGrantCategory, Feature} from '../client/common/types/db'
import {FeatureBillingType, FeatureCategory} from './feature-constants'
import type {ICreditGrantOption} from './feature-types'

const CREDIT_TO_BIPS_CONVERSION = 10000
const BASE_UNIT_VALUE = 0.04
const CREDIT_GRANT_CLIENT_ATTRIBUTES = [
  'uuid',
  'AccountUuid',
  'category',
  'remainingQuantity',
  'expiresAt',
]

const isAccountFeature = (featureName: string, optionName: string): boolean => (
  Object.keys(ACCOUNT_FEATURES).includes(featureName) &&
    Object.keys(ACCOUNT_FEATURES[featureName]).includes(optionName)
)

const isAccountCreditGrantFeature = (featureName: string, optionName: string): boolean => (
  featureName === CREDIT_GRANT_FEATURE.name &&
  Object.keys(CREDIT_GRANT_FEATURE).includes(optionName)
)

const isAccountCreditGrantFeatureSubscription = (
  featureName: string, optionName: string
): boolean => (
  isAccountCreditGrantFeature(featureName, optionName) &&
  ACCOUNT_FEATURES[featureName][optionName].billingType === 'Recurring'
)

const isAccountCreditGrantFeatureTopUp = (
  featureName: string, optionName: string
): boolean => (
  isAccountCreditGrantFeature(featureName, optionName) &&
  ACCOUNT_FEATURES[featureName][optionName].billingType === 'OneTime' &&
  ACCOUNT_FEATURES[featureName][optionName].isTopUp
)

const convertCreditsToBips = (creditAmount: number): number => (
  creditAmount * CREDIT_TO_BIPS_CONVERSION
)

const convertBipsToCredits = (bipsAmount: number): number => (
  bipsAmount / CREDIT_TO_BIPS_CONVERSION
)

const getFeatureUnitPrice = (feature: ICreditGrantOption): number => (
  (feature.price / feature.creditAmount) * CREDIT_TO_BIPS_CONVERSION
)

const getFeatureTotalPrice = (feature: ICreditGrantOption, quantity?: number) => {
  const featureUnitPrice = getFeatureUnitPrice(feature)
  if (feature?.isTopUp && quantity) {
    return featureUnitPrice * quantity
  }

  return featureUnitPrice * feature.creditAmount
}

const getFeatureUnitValue = (): number => (
  BASE_UNIT_VALUE * CREDIT_TO_BIPS_CONVERSION
)

const getFeatureTotalValue = (feature: ICreditGrantOption, quantity?: number): number => {
  const unitValue = getFeatureUnitValue()

  if (feature?.isTopUp && quantity) {
    return unitValue * quantity
  }

  return unitValue * feature.creditAmount
}

const getActiveCreditGrant = (
  features?: DeepReadonly<Feature[]>
): Feature | undefined => features?.find((feature) => {
  const {optionName, status} = feature
  return (optionName === CREDIT_GRANT_FEATURE.PowerSub.name ||
    optionName === CREDIT_GRANT_FEATURE.CoreSub.name) &&
    status === 'ENABLED'
})

const getFeatureBillingType = (
  feature: ICreditGrantOption
): CreditGrantCategory => {
  if (feature.billingType === FeatureBillingType.OneTime) {
    return 'PAID_TOPUP'
  } else if (feature.billingType === FeatureBillingType.Recurring) {
    return 'PAID_PLAN'
  } else {
    throw new Error(`Unexpected billing type: ${feature.billingType}`)
  }
}

const hasActiveRecurringFeature = (
  features: Feature[], category: FeatureCategory, featureName: string
): boolean => features.some(
  feature => feature.category === category.toUpperCase() &&
  feature.featureName === featureName &&
  feature.billingType === 'RECURRING'
)

const hasRequiredEnabledOptions = (
  features: Feature[], category: FeatureCategory, featureName: string, requiredOptions: string[]
): boolean => features.some(
  feature => feature.category === category.toUpperCase() &&
  feature.featureName === featureName &&
  requiredOptions.includes(feature.optionName) &&
  feature.status === 'ENABLED'
)

export {
  CREDIT_TO_BIPS_CONVERSION,
  BASE_UNIT_VALUE,
  CREDIT_GRANT_CLIENT_ATTRIBUTES,
  isAccountFeature,
  isAccountCreditGrantFeature,
  isAccountCreditGrantFeatureSubscription,
  isAccountCreditGrantFeatureTopUp,
  convertCreditsToBips,
  convertBipsToCredits,
  getFeatureTotalPrice,
  getFeatureUnitPrice,
  getFeatureTotalValue,
  getFeatureUnitValue,
  getActiveCreditGrant,
  getFeatureBillingType,
  hasActiveRecurringFeature,
  hasRequiredEnabledOptions,
}
