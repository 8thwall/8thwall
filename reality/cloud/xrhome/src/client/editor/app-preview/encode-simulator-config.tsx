import type {DeepReadonly} from 'ts-essentials'

import type {SimulatorConfig} from './app-preview-utils'
import type {IApp} from '../../common/types/models'
import {useSelector} from '../../hooks'
import {useProjectPreviewUrl} from './use-project-preview-url'

type LiveSyncMode = 'inline' | undefined

type EncodedUrlInfo = {
  // The base URL that the user would end up on (excluding simulatorConfig)
  rawDevUrl: string
  // The full URL with the simulatorConfig and token redirects inserted
  currentConfigUrl: string
}

const useSimulatorConfigUrl = (
  app: IApp,
  simulatorConfig: DeepReadonly<SimulatorConfig>,
  _alreadyAppliedToken: boolean,
  sessionId?: string,
  liveSyncMode?: LiveSyncMode
): EncodedUrlInfo => {
  const previewLinkDebugMode = useSelector(state => !!state.editor.previewLinkDebugMode)

  const projectUrl = useProjectPreviewUrl(app)

  if (!projectUrl) {
    return {rawDevUrl: null, currentConfigUrl: null}
  }

  const currentConfigUrl = new URL(projectUrl)

  currentConfigUrl.searchParams.set('d', String(previewLinkDebugMode))

  // NOTE(christoph): By including the 'd' parameter in rawDevUrl, we cause a reload if it changes.
  const rawDevUrl = currentConfigUrl.toString()

  currentConfigUrl.searchParams.set('simulatorConfig', JSON.stringify(simulatorConfig))

  if (sessionId) {
    currentConfigUrl.searchParams.set('sessionId', sessionId)
  }

  if (BuildIf.STUDIO_DEV8_INTEGRATION_20260205 && liveSyncMode) {
    currentConfigUrl.searchParams.set('liveSyncMode', liveSyncMode)
  }

  return {
    rawDevUrl,
    currentConfigUrl: currentConfigUrl.toString(),
  }
}

export {
  useSimulatorConfigUrl,
}

export type {
  LiveSyncMode,
}
