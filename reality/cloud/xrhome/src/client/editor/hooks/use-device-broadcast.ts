import type {DebugMessage} from '@ecs/shared/debug-messaging'

import {useAppPreviewWindow} from '../../common/app-preview-window-context'
import {INLINE_SIMULATOR_SESSION_ID} from '../app-preview/app-preview-constants'

const useDeviceBroadcast = () => {
  const {getInlinePreviewWindow} = useAppPreviewWindow()

  const sendData = (
    sessionId: string,
    data: DebugMessage
  ) => {
    if (sessionId === INLINE_SIMULATOR_SESSION_ID) {
    // TODO(christoph): Handle other device IDs, not just simulator
      const targetWindow = getInlinePreviewWindow()
      targetWindow?.postMessage(data, '*')
    }
    // eslint-disable-next-line no-console
    console.warn('Not implemented: Remote broadcast')
  }

  return sendData
}

export {
  useDeviceBroadcast,
}
