import React from 'react'
import type {DebugMessage, ScopedDebugMessage} from '@ecs/shared/debug-messaging'

import {useEnclosedAppKey} from '../../apps/enclosed-app-context'
import useActions from '../../common/use-actions'
import editorActions from '../editor-actions'
import {useWindowMessageHandler} from '../../hooks/use-window-message-handler'
import ConsoleLogStreams from '../console-log-streams'
import {useEvent} from '../../hooks/use-event'

const useConsoleActivity = () => {
  const appKey = useEnclosedAppKey()
  const {
    addEditorLogs,
    setLogStreamDebugHudStatus,
    setLogStreamDebugInitialHudStatus,
    clearEditorLogStreamOnRun,
  } = useActions(editorActions)

  const handleMessage = useEvent((msg: DebugMessage) => {
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

  useWindowMessageHandler(e => handleMessage(e.data))

  React.useEffect(() => {
    const handleDevice = (e: ScopedDebugMessage) => {
      if (appKey === e.appKey) {
        handleMessage(e.data)
      }
    }
    window.electron.dev8Socket.setListener(handleDevice)
    return () => {
      window.electron.dev8Socket.setListener(null)
    }
  }, [appKey])
}

export {
  useConsoleActivity,
}
