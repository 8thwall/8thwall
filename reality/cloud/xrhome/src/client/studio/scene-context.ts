import React from 'react'

import type {DeepReadonly} from 'ts-essentials'

import type {GraphObject, SceneGraph} from '@ecs/shared/scene-graph'

type MutateCallback<T> = (old: DeepReadonly<T>) => DeepReadonly<T>
interface RawSceneContext {
  scene: SceneGraph
  updateScene: (cb: MutateCallback<SceneGraph>) => void
  updateObject: (id: string, cb: MutateCallback<GraphObject>) => void
  isRenamingById: Record<string, boolean>
  setIsRenaming: (id: string, isRenaming: boolean) => void
  isDraggingGizmoRef: DeepReadonly<{current: boolean}>
  isDraggingGizmo: boolean
  setIsDraggingGizmo: (isDragging: boolean) => void
  canUndo: boolean
  undo: () => void
  canRedo: boolean
  redo: () => void
}

type SceneContext = DeepReadonly<RawSceneContext>

const sceneContext = React.createContext<SceneContext | null>(null)

const useSceneContext = (): SceneContext => {
  const ctx = React.useContext(sceneContext)
  if (!ctx) {
    throw new Error('useSceneContext must be used within a SceneContextProvider')
  }
  return ctx
}

const SceneContextProvider = sceneContext.Provider
export {
  SceneContextProvider,
  useSceneContext,
}

export type {
  MutateCallback,
  SceneContext,
}
