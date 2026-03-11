import {useQuery} from '@tanstack/react-query'

import useCurrentAccount from '../common/use-current-account'
import {convertBipsToCredits} from '../../shared/feature-utils'
import type {CreditGrant, CreditGrantCategory} from '../common/types/db'
import {useAuthFetch} from '../hooks/use-auth-fetch'
import type {GetCreditGrantsResponse} from './credit-api'

const POLL_MS = 5000  // Polling interval in milliseconds, e.g., 5000 for 5 seconds

type CategorizedCreditGrants = Record<CreditGrantCategory, CreditGrant[]>

const useCreditQuery = () => {
  const account = useCurrentAccount()
  const authFetch = useAuthFetch()

  return useQuery({
    queryKey: ['creditGrants', account.uuid],
    queryFn: async () => {
      const {activeCreditGrants} = await authFetch<GetCreditGrantsResponse>(
        `/v1/credits/${account.uuid}/grants`,
        {method: 'GET'}
      )
      const categorizedCreditGrants = activeCreditGrants.reduce((acc, grant) => {
        if (!acc[grant.category]) {
          acc[grant.category] = []
        }
        acc[grant.category].push(grant)
        return acc
      }, {} as Record<CreditGrantCategory, CreditGrant[]>)
      const creditAmount = activeCreditGrants.reduce((
        acc,
        e
      ) => (acc + convertBipsToCredits(e.remainingQuantity)), 0)
      return {
        creditAmount,
        categorizedCreditGrants,
      }
    },
    refetchInterval: POLL_MS,
    // NOTE(kyle): Setting stale time to infinity with polling allows nested components to use this
    // hook without kicking off a new fetch on mount.
    staleTime: Infinity,
  })
}

export {
  useCreditQuery,
}

export type {
  CategorizedCreditGrants,
}
