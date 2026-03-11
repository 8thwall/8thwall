import React from 'react'
import {useTranslation} from 'react-i18next'

import {FloatingTray} from '../ui/components/floating-tray'
import {useStudioStateContext} from './studio-state-context'
import {FloatingIconButton} from '../ui/components/floating-icon-button'
import {useStudioDebug} from '../editor/app-preview/use-studio-debug-state'
import {useSimulator, useSimulatorVisible} from '../editor/app-preview/use-simulator-state'
import {useSceneContext} from './scene-context'
import {ProductTourId} from './product-tour-constants'
import useCurrentApp from '../common/use-current-app'

interface IDebugControlsTray {
  onPlay: () => void
  simulatorId: string
}

const StudioDebugControlsTray: React.FC<IDebugControlsTray> = ({onPlay, simulatorId}) => {
  const stateCtx = useStudioStateContext()
  const {state: {isPreviewPaused}} = stateCtx
  const ctx = useSceneContext()
  const {t} = useTranslation('cloud-studio-pages')

  const {closeDebugSimulatorSession} = useStudioDebug()
  const {updateSimulatorState} = useSimulator()
  const app = useCurrentApp()
  const simulatorVisible = useSimulatorVisible(app)

  const handlePauseToggle = () => {
    const newPaused = !isPreviewPaused
    stateCtx.update(p => ({...p, isPreviewPaused: newPaused}))
    updateSimulatorState({studioPause: newPaused})
  }

  const handleStop = () => {
    updateSimulatorState({inlinePreviewVisible: false})
    closeDebugSimulatorSession(simulatorId)
  }

  return (
    <FloatingTray shrink nonInteractive={ctx.isDraggingGizmo}>
      {BuildIf.STUDIO_DEV8_INTEGRATION_20260205 && (
        <>
          <FloatingIconButton
            text={t('studio_play_button_tray.button.restart')}
            stroke='reset'
            onClick={() => {
              stateCtx.update(p => ({...p, restartKey: p.restartKey + 1}))
            }}
            isDisabled={!simulatorVisible}
          />
          <FloatingIconButton
            isActive={isPreviewPaused}
            text={t('studio_play_button_tray.button.pause')}
            onClick={handlePauseToggle}
            stroke='pause'
          />
        </>
      )}
      {simulatorVisible
        ? (
          <FloatingIconButton
            id={ProductTourId.DEBUG_PLAY_PAUSE}
            isActive
            text={t('studio_play_button_tray.button.stop')}
            onClick={handleStop}
            stroke='stop'
          />
        )
        : (
          <FloatingIconButton
            id={ProductTourId.DEBUG_PLAY_PAUSE}
            text={t('studio_play_button_tray.button.play')}
            onClick={onPlay}
            stroke='play'
          />
        )}
    </FloatingTray>
  )
}

export {
  StudioDebugControlsTray,
}
