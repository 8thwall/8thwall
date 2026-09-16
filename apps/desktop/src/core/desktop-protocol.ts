import {
  CD_PROD, CD_QA, StudiohubProtocol, LOCAL_CONSOLE, PROD, RC_PROD,
} from '@repo/reality/shared/desktop/desktop-protocol-types'

const STUDIO_HUB_PROTOCOL: StudiohubProtocol = (() => {
  switch (process.env.DEPLOY_STAGE) {
    case 'prod':
      return PROD
    case 'rc-prod':
      return RC_PROD
    case 'cd-prod':
      return CD_PROD
    case 'cd-qa':
      return CD_QA
    case 'dev':
      return LOCAL_CONSOLE
    default:
      throw new Error(`Unknown DEPLOY_STAGE for default protocol: ${process.env.DEPLOY_STAGE}`)
  }
})()

export {
  STUDIO_HUB_PROTOCOL,
}
