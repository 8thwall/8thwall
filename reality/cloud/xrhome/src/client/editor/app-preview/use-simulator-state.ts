import {useSelector} from '../../hooks'
import type {SimulatorState} from '../editor-reducer'
import useActions from '../../common/use-actions'
import editorActions from '../editor-actions'
import {useEnclosedAppKey} from '../../apps/enclosed-app-context'

const useSimulatorState = (appKey: string): SimulatorState => (
  useSelector(state => state.editor.byKey[appKey]?.simulatorState) || {}
)

type SimulatorHook = {
  // NOTE(christoph): inlinePreviewVisible is not the source of truth for whether the
  // simulator is visible. Use useSimulatorVisible.
  simulatorState: Omit<SimulatorState, 'inlinePreviewVisible'>
  updateSimulatorState: (status: Partial<SimulatorState>) => void
}

const useSimulator = (): SimulatorHook => {
  const appKey = useEnclosedAppKey()
  const {updateSimulatorState} = useActions(editorActions)
  return {
    simulatorState: useSimulatorState(appKey),
    updateSimulatorState: (status: Partial<SimulatorState>) => {
      updateSimulatorState(appKey, status)
    },
  }
}

const useSimulatorVisible = (app: {appKey: string}) => useSelector((s) => {
  const appState = s.editor.byKey[app.appKey]
  if (!appState?.simulatorState) {
    return false
  }

  return appState.simulatorState.inlinePreviewVisible
})

export {
  useSimulatorState,
  useSimulatorVisible,
  useSimulator,
}
