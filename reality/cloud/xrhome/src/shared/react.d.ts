/* eslint-disable import/group-exports */
import type {FC} from 'react'

// adding this to the global namespace
// so that we can for components/entities that use Semantic UI
// It will be removed once we've fully transitioned from using it.

declare global {
  namespace React {
    export type StatelessComponent<T> = FC<T>
    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    export interface HTMLAttributes<T> {
      a8?: string  // 8th Wall Analytics
    }

    // NOTE(christoph): This is a workaround because React.ReactNode no longer extends {}.
    // Existing <Trans> elements are using children rather than the data={...} prop which is the
    // new preferred mechanism. This override should eventually be removed.
    type DeprecatedTranslationData =
      {contractLink: string} |
      {discountedPercent: number} |
      {fileName: string} |
      {interval: string} |
      {licenseName: string} |
      {listAccountsToUpgrade: string} |
      {maxSizeMb: number} |
      {nextChargeDate: string} |
      {planAmount: string} |
      {planInterval: string} |
      {planTypeDescription: string} |
      {planTypeForAccountType: string} |
      {savingsInCurrency: string} |
      {trialEnd: string} |
      {updatingDate: string} |
      {url: string} |
      {viewsIncluded: string}

    export interface DO_NOT_USE_OR_YOU_WILL_BE_FIRED_EXPERIMENTAL_REACT_NODES {
      translationData: DeprecatedTranslationData
    }
  }
}
