import React from 'react'

import './set-three'
import {ISceneEdit, SceneEdit} from './scene-edit'
import {StudioDebugControlsTray} from './studio-debug-controls-tray'

import {useStudioStateContext} from './studio-state-context'
import {YogaParentContextProvider} from './yoga-parent-context'
import {useSimulator} from '../editor/app-preview/use-simulator-state'
import {DebugSimulatorPanel} from './debug-simulator-panel'

interface ISceneFileEdit extends ISceneEdit {
  simulatorId: string
}

const SceneFileEdit: React.FC<ISceneFileEdit> = ({simulatorId, ...rest}) => {
  // const repo = useCurrentGit(git => git.repo)
  // const {ensureSimulatorReady} = useActions(gitActions)
  const {updateSimulatorState} = useSimulator()
  // const app = useCurrentApp()

  // const simulatorVisible = useSimulatorVisible(app)
  // const {selectSimulatorDebugSession} = useStudioDebug()
  const stateCtx = useStudioStateContext()
  const {simulatorCollapsed, simulatorMode} = stateCtx.state

  const maybeOpenSimulator = () => {
    // if (simulatorVisible) {
    //   selectSimulatorDebugSession(simulatorId)
    //   return
    // }

    // ensureSimulatorReady(repo)
    updateSimulatorState({inlinePreviewVisible: true, inlinePreviewDebugActive: true})
    stateCtx.update({
      simulatorCollapsed: false,
    })
  }

  const simulatorVisible = false

  const inFullscreenSimulator = simulatorMode === 'full' && simulatorVisible && !simulatorCollapsed

  return (
    <YogaParentContextProvider>
      <SceneEdit
        hideViewport={inFullscreenSimulator}
        playbackControls={(
          <StudioDebugControlsTray
            onPlay={maybeOpenSimulator}
            simulatorId={simulatorId}
          />
        )}
        simulatorPanel={<DebugSimulatorPanel simulatorId={simulatorId} />}
        {...rest}
      />
    </YogaParentContextProvider>
  )
}

export default SceneFileEdit
