import {useSelector} from '../../hooks'
import type {StudioDebugState, StudioDebugSession} from '../editor-reducer'
import useCurrentApp from '../../common/use-current-app'
import useActions from '../../common/use-actions'
import editorActions from '../editor-actions'

const useStudioDebugState = (appKey: string): StudioDebugState => (
  useSelector(state => state.editor.byKey[appKey]?.studioDebugState) || {}
)

type StudioDebugHook = {
  studioDebugState: StudioDebugState
  selectDebugSession: (session: StudioDebugSession, isInlineSimulatorSession: boolean) => void
  disconnectSession: () => void
  selectSimulatorDebugSession: (simulatorId: string) => void
  closeDebugSimulatorSession: (simulatorId: string) => void
}

const useStudioDebug = (): StudioDebugHook => {
  const app = useCurrentApp()
  const {
    selectDebugSession, disconnectDebugSession,
    selectSimulatorSession, removeSimulatorSession,
  } = useActions(editorActions)
  const studioDebugState = useStudioDebugState(app.appKey)
  return {
    studioDebugState,
    selectDebugSession: (session: StudioDebugSession, isInlineSimulatorSession: boolean) => {
      selectDebugSession(app.appKey, session, isInlineSimulatorSession)
    },
    disconnectSession: () => {
      disconnectDebugSession(app.appKey)
    },
    selectSimulatorDebugSession: (sessionSimulatorId: string) => {
      selectSimulatorSession(app.appKey, sessionSimulatorId)
    },
    closeDebugSimulatorSession: (sessionSimulatorId: string) => {
      removeSimulatorSession(app.appKey, sessionSimulatorId)
    },
  }
}

export {
  useStudioDebugState,
  useStudioDebug,
}
