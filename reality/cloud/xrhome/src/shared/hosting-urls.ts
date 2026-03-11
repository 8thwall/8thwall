import type {RefHead} from './nae/nae-types'

// eslint-disable-next-line @typescript-eslint/no-unused-vars
const makeHostedProductionUrl = (shortName: string, appName: string) => {
  throw new Error('Hosted production URLs are no longer supported')
}

// eslint-disable-next-line @typescript-eslint/no-unused-vars
const createNaeProjectUrl = (account: string, appName: string, ref: RefHead) => {
  throw new Error('NAE project URLs are no longer supported')
}

export {
  makeHostedProductionUrl,
  createNaeProjectUrl,
}
