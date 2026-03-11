import React from 'react'
import {useTranslation} from 'react-i18next'

import {FloatingTray} from '../ui/components/floating-tray'
import {TransformToolTypes, useStudioStateContext} from './studio-state-context'
import {FloatingIconButton} from '../ui/components/floating-icon-button'
import {useSceneContext} from './scene-context'

const TransformGizmoTray: React.FC<{}> = () => {
  const {t} = useTranslation(['cloud-studio-pages'])
  const stateCtx = useStudioStateContext()
  const {state: {transformMode}} = stateCtx
  const mode = transformMode
  const ctx = useSceneContext()

  const handleTransformMode = (nextMode: TransformToolTypes) => {
    if (mode !== nextMode) {
      stateCtx.update(p => ({...p, transformMode: nextMode}))
    }
  }

  return (
    <FloatingTray nonInteractive={ctx.isDraggingGizmo}>
      <FloatingIconButton
        text={t('transform_gizmo_tray.button.translate')}
        onClick={() => handleTransformMode(TransformToolTypes.TRANSLATE_TOOL)}
        stroke='translate'
        isActive={mode === TransformToolTypes.TRANSLATE_TOOL}
      />
      <FloatingIconButton
        text={t('transform_gizmo_tray.button.rotate')}
        onClick={() => handleTransformMode(TransformToolTypes.ROTATE_TOOL)}
        stroke='rotate'
        isActive={mode === TransformToolTypes.ROTATE_TOOL}
      />
      <FloatingIconButton
        text={t('transform_gizmo_tray.button.scale')}
        onClick={() => handleTransformMode(TransformToolTypes.SCALE_TOOL)}
        stroke='scale'
        isActive={mode === TransformToolTypes.SCALE_TOOL}
      />
    </FloatingTray>
  )
}

export {
  TransformGizmoTray,
}
