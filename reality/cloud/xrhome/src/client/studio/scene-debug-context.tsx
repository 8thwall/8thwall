import React from 'react'
import type {DeepReadonly} from 'ts-essentials'
import {useTranslation} from 'react-i18next'
import type {SceneGraph} from '@ecs/shared/scene-graph'

import {getDefaultDeviceSupport} from '@ecs/shared/xr-config'

import {SceneContextProvider} from './scene-context'
import {useUndoStack} from './hooks/undo-stack'
import {SceneJsonEditor} from './studio-scenegraph-view'
import {useExpanse} from './hooks/expanse'
import {useSimulator} from '../editor/app-preview/use-simulator-state'
import {updateObject} from './update-object'
import {DerivedSceneProvider} from './derived-scene-context'

type MutateCallback<T> = (old: DeepReadonly<T>) => DeepReadonly<T>

interface ISceneDebugContext {
  children: React.ReactNode
}

const SceneDebugContext: React.FC<ISceneDebugContext> = ({
  children,
}) => {
  const expanseState = useExpanse()
  const {t} = useTranslation('cloud-studio-pages')
  const isDraggingGizmoRef = React.useRef(false)
  const [isDraggingGizmo, setIsDraggingGizmoState] = React.useState(false)
  const undoStack = useUndoStack<DeepReadonly<SceneGraph>>()
  const [isRenamingById, setIsRenamingById] = React.useState<Record<string, boolean>>({})

  const {updateSimulatorState} = useSimulator()

  const setIsRenaming = (id: string, isRenaming: boolean) => {
    setIsRenamingById(old => ({...old, [id]: isRenaming}))
  }

  const activeCamera = expanseState.scene?.objects?.[expanseState.scene?.activeCamera]
  const activeCameraConfig = activeCamera?.camera?.xr

  React.useEffect(() => {
    const cameraType = activeCameraConfig?.xrCameraType ?? '3dOnly'
    const {phone, desktop, headset} = getDefaultDeviceSupport(cameraType)

    updateSimulatorState({
      cameraXrConfig: {
        xrCameraType: cameraType,
        phone: activeCameraConfig?.phone ?? phone,
        desktop: activeCameraConfig?.desktop ?? desktop,
        headset: activeCameraConfig?.headset ?? headset,
      },
    })
  }, [
    activeCameraConfig?.xrCameraType, activeCameraConfig?.phone,
    activeCameraConfig?.desktop, activeCameraConfig?.headset,
  ])

  const sceneContextValue = React.useMemo(() => {
    if (!expanseState.ready) {
      return null
    }
    const updateScene = (cb: MutateCallback<SceneGraph>) => {
      const previousVersion = expanseState.scene
      expanseState.update(cb)
      undoStack.addToUndo(previousVersion)
    }

    const setIsDraggingGizmo = (isDragging: boolean) => {
      isDraggingGizmoRef.current = isDragging
      setIsDraggingGizmoState(isDragging)
    }

    return {
      scene: expanseState.scene,
      updateScene,
      updateObject: updateObject.bind(null, updateScene),
      isDraggingGizmoRef,
      isDraggingGizmo,
      setIsDraggingGizmo,
      isRenamingById,
      setIsRenaming,
      canUndo: undoStack.canUndo,
      undo: () => {
        const oldScene = undoStack.undo(expanseState.scene)
        if (oldScene) {
          expanseState.update(() => oldScene)
        }
      },
      canRedo: undoStack.canRedo,
      redo: () => {
        const newScene = undoStack.redo(expanseState.scene)
        if (newScene) {
          expanseState.update(() => newScene)
        }
      },
      playsUsingRuntime: false,
      debugStatus: null,
    } as const
  }, [
    expanseState.scene, isDraggingGizmo,
    undoStack.canRedo, undoStack.canUndo, isRenamingById,
    // debugStatus: null,
  ])

  if (!expanseState.ready) {
    return null
  }

  if (!sceneContextValue) {
    return (
      <>
        <h2>{t('scene_edit_context.invalid_scene')}</h2>
        <SceneJsonEditor
          currentScene={expanseState.scene}
          onSceneChange={s => expanseState.update(() => s)}
        />
      </>
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
  SceneDebugContext,
}

export type {
  MutateCallback,
}
