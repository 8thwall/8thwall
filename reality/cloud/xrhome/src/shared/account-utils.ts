import {AGENT_BETA} from './special-features'
import type {SpecialFeatureFlag} from './special-features'
import type {IFullAccount} from '../client/common/types/models'

type PickAccount<T extends keyof IFullAccount> = Pick<IFullAccount, T>

type TypeCheck = (account: PickAccount<'accountType'> | null) => boolean

/**
 * Checks if the account is on a Pro plan. Note: in Q4 2020 Agency was renamed to Pro.
 */
const isPro: TypeCheck = account => account && account.accountType === 'WebAgency'

/**
 * Checks if the account is on an Enterprise plan.
 */
const isEnterprise: TypeCheck = account => account?.accountType === 'WebEnterprise'

/**
 * Returns true if the given special feature is enabled on this account.
 * Primary use is for allowlisting certain features to select accounts.
 */
const isSpecialFeatureEnabled = (
  account: PickAccount<'specialFeatures'>,
  specialFeature: SpecialFeatureFlag
) => Boolean(account?.specialFeatures?.includes(specialFeature))

const isAgentBetaEnabled = (account: PickAccount<'specialFeatures'>) => (
  isSpecialFeatureEnabled(account, AGENT_BETA)
)

export {
  isEnterprise,
  isPro,
  isAgentBetaEnabled,
}

export type {
  PickAccount,
}
