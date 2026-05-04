import type {SceneGraph} from '@ecs/shared/scene-graph'
import React, {useMemo} from 'react'
import {useTranslation} from 'react-i18next'
import type {DeepReadonly} from 'ts-essentials'

import {SceneContextProvider} from './scene-context'
import {DerivedSceneProvider} from './derived-scene-context'

interface IReadonlySceneEditContext {
  scene: DeepReadonly<SceneGraph> | null
  children: React.ReactNode
}

// NOTE(carson): This context is used to provide a read-only version of the scene edit,
// it is a copy of the scene edit context at scene-edit-context.tsx but without
// the ability to modify the scene. It also will not affect the URL state, so
// changes won't be reflected in the main editor.

const ReadonlySceneEditContext: React.FC<IReadonlySceneEditContext> = ({
  scene, children,
}) => {
  const {t} = useTranslation('cloud-studio-pages')

  const sceneContextValue = useMemo(() => {
    if (!scene || typeof scene.objects !== 'object') {
      return null
    }

    const noop = () => {}

    return {
      scene,
      updateScene: noop,
      updateObject: noop,
      // NOTE(carson): This leads to dragging the gizmo just turning into select
      // drag, which looks a bit weird currently, but the Gizmo will be removed
      // in the UI later for readonly.
      // TODO(carson): So that everything is in one place, maybe add 'using gizmo'
      // field in future MR?
      isDraggingGizmoRef: {current: false},
      isDraggingGizmo: false,
      setIsDraggingGizmo: noop,
      isRenamingById: {},
      setIsRenaming: noop,
      canUndo: false,
      undo: noop,
      canRedo: false,
      redo: noop,
    } as const
  }, [
    scene,
  ])

  if (!sceneContextValue) {
    return (
      <h2> {t('scene_edit_context.invalid_scene')} </h2>
    )
  }

  return (
    <SceneContextProvider value={sceneContextValue}>
      <DerivedSceneProvider>
        {children}
      </DerivedSceneProvider>
    </SceneContextProvider>
  )
}

export {
  ReadonlySceneEditContext,
}

export type {
  IReadonlySceneEditContext,
}
