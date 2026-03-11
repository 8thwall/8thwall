import React from 'react'
import {useTranslation} from 'react-i18next'

import {useSceneContext} from './scene-context'
import {FloatingTray} from '../ui/components/floating-tray'
import {FloatingIconButton} from '../ui/components/floating-icon-button'
import {TransformGizmoTray} from './transform-gizmo-tray'

const StudioSceneEditControlTray: React.FC<{}> = () => {
  const {t} = useTranslation('app-pages')
  const ctx = useSceneContext()

  return (
    <>
      <FloatingTray nonInteractive={ctx.isDraggingGizmo}>
        <FloatingIconButton
          text={t('project_settings_page.shortcut_binding.action.undo')}
          onClick={ctx.undo}
          isDisabled={!ctx.canUndo}
          stroke='undo'
        />
        <FloatingIconButton
          text={t('project_settings_page.shortcut_binding.action.redo')}
          onClick={ctx.redo}
          isDisabled={!ctx.canRedo}
          stroke='redo'
        />
      </FloatingTray>
      <TransformGizmoTray />
    </>
  )
}

export {
  StudioSceneEditControlTray,
}
