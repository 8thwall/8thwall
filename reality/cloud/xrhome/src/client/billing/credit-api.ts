import type {CreditGrant} from '../common/types/db'

type GetCreditGrantsResponse = {
  activeCreditGrants: CreditGrant[]
}

export type {
  GetCreditGrantsResponse,
}
