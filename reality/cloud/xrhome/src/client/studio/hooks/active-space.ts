import type {DeepReadonly} from 'ts-essentials'
import type {SceneGraph, Space} from '@ecs/shared/scene-graph'

import {SceneContext, useSceneContext} from '../scene-context'
import {StudioStateContext, useStudioStateContext} from '../studio-state-context'

const deriveActiveSpace = (
  scene: DeepReadonly<SceneGraph>,
  activeSpace: string | undefined
): DeepReadonly<Space> | undefined => {
  const spaces = scene.spaces ? Object.values(scene.spaces) : []
  return spaces.find(space => space.id === activeSpace) ??
    spaces.find(space => space.id === scene.entrySpaceId) ??
    spaces[0]
}

const getActiveSpaceFromCtx = (
  ctx: SceneContext, stateCtx: StudioStateContext
): DeepReadonly<Space> | undefined => (
  deriveActiveSpace(ctx.scene, stateCtx.state.activeSpace)
)

const useActiveSpace = (): DeepReadonly<Space> | undefined => {
  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()
  return getActiveSpaceFromCtx(ctx, stateCtx)
}

export {
  deriveActiveSpace,
  useActiveSpace,
  getActiveSpaceFromCtx,
}
