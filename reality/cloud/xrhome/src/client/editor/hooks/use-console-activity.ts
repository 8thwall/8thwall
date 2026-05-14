import type {DebugMessage} from '@ecs/shared/debug-messaging'

import {useEnclosedAppKey} from '../../apps/enclosed-app-context'
import useActions from '../../common/use-actions'
import editorActions from '../editor-actions'
import {useWindowMessageHandler} from '../../hooks/use-window-message-handler'
import ConsoleLogStreams from '../console-log-streams'

const useConsoleActivity = BuildIf.STUDIO_OFFLINE_LOG_CONTAINER_20260205
  ? () => {
    const appKey = useEnclosedAppKey()
    const {
      addEditorLogs,
      setLogStreamDebugHudStatus,
      setLogStreamDebugInitialHudStatus,
      clearEditorLogStreamOnRun,
    } = useActions(editorActions)

    useWindowMessageHandler((event: MessageEvent<DebugMessage>) => {
      const msg = event.data
      switch (msg.action) {
        case 'CONSOLE_ACTIVITY':
          addEditorLogs(appKey, [ConsoleLogStreams.messageLog(msg)].flat())
          break
        case 'SESSION_START':
          clearEditorLogStreamOnRun(
            appKey, msg.sessionId ?? msg.deviceId, msg.timestamp
          )
          break
        case 'INITIAL_DEBUG_HUD_STATUS':
          setLogStreamDebugInitialHudStatus(
            appKey,
            msg.deviceId,
            msg.sessionId || msg.deviceId,
            msg.status,
            msg.screenWidth,
            msg.screenHeight,
            msg.ua
          )
          break
        case 'SET_DEBUG_HUD_STATUS':
          setLogStreamDebugHudStatus(appKey, msg.sessionId ?? msg.deviceId, msg.status)
          break
        default:
      }
    })
  }
  : () => {}

export {
  useConsoleActivity,
}
