const BASE_STUDIO_HUB_PROTOCOL = 'com.the8thwall.desktop'
const CD_PROD = `${BASE_STUDIO_HUB_PROTOCOL}.cd:`
const CD_QA = `${BASE_STUDIO_HUB_PROTOCOL}.qa.cd:`
const LOCAL_CONSOLE = `${BASE_STUDIO_HUB_PROTOCOL}.local:`
const RC_PROD = `${BASE_STUDIO_HUB_PROTOCOL}.rc:`
const PROD = `${BASE_STUDIO_HUB_PROTOCOL}:`

type StudiohubProtocol = typeof CD_PROD | typeof CD_QA | typeof LOCAL_CONSOLE
  | typeof RC_PROD | typeof PROD

export {
  CD_PROD,
  CD_QA,
  LOCAL_CONSOLE,
  RC_PROD,
  PROD,
  BASE_STUDIO_HUB_PROTOCOL,
}

export type {StudiohubProtocol}
