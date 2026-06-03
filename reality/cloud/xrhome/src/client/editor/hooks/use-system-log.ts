import React from 'react'

import type {SystemLogEvent} from '@repo/reality/shared/desktop/system-log-types'

import {useEnclosedAppKey} from '../../apps/enclosed-app-context'
import {useEvent} from '../../hooks/use-event'
import useActions from '../../common/use-actions'
import editorActions from '../editor-actions'
import {SYSTEM_STREAM_NAME} from '../logs/log-constants'

const useSystemLog = () => {
  const appKey = useEnclosedAppKey()
  const {addEditorLogs} = useActions(editorActions)

  const handler = useEvent((e: SystemLogEvent) => {
    addEditorLogs(appKey, [{
      streamName: SYSTEM_STREAM_NAME,
      log: e,
    }])
  })

  React.useEffect(() => {
    window.electron.systemLog.setHandler(appKey, handler)
    return () => {
      window.electron.systemLog.clearHandler(appKey)
    }
  }, [appKey])
}

export {
  useSystemLog,
}
