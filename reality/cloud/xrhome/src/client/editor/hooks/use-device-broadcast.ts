import type {DebugMessage} from '@ecs/shared/debug-messaging'

import {useAppPreviewWindow} from '../../common/app-preview-window-context'
import {INLINE_SIMULATOR_SESSION_ID} from '../app-preview/app-preview-constants'
import {useEnclosedAppKey} from '../../apps/enclosed-app-context'

const useDeviceBroadcast = () => {
  const {getInlinePreviewWindow} = useAppPreviewWindow()
  const appKey = useEnclosedAppKey()

  const sendData = (
    deviceId: string,
    data: DebugMessage
  ) => {
    if (deviceId === INLINE_SIMULATOR_SESSION_ID) {
      const targetWindow = getInlinePreviewWindow()
      targetWindow?.postMessage(data, '*')
      return
    }

    // NOTE(christoph): deviceId should be equivalent to sessionId here because sessionId used as
    // both if sessionId is present, which it generally always is.
    window.electron.dev8Socket.toDevice.dispatch({appKey, sessionId: deviceId, data})
  }

  return sendData
}

export {
  useDeviceBroadcast,
}
