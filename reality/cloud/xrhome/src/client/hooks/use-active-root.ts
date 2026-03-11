import type {DeepReadonly} from 'ts-essentials'

import type {GraphObject, Space} from '@ecs/shared/scene-graph'

import {SceneContext, useSceneContext} from '../studio/scene-context'
import {StudioStateContext, useStudioStateContext} from '../studio/studio-state-context'
import {getActiveSpaceFromCtx} from '../studio/hooks/active-space'

const getActivePrefab = (
  ctx: SceneContext, stateCtx: StudioStateContext
): DeepReadonly<GraphObject> | undefined => {
  const maybePrefab = ctx.scene.objects[stateCtx.state.selectedPrefab]
  if (maybePrefab?.prefab) {
    return maybePrefab
  }
  return null
}

type ActiveRoot = DeepReadonly<{id: string | undefined} & (
  {prefab?: GraphObject, space: never} | {prefab: never, space?: Space}
)>

const getActiveRoot = (
  ctx: SceneContext,
  stateCtx: StudioStateContext
): ActiveRoot => {
  const activeSpace = getActiveSpaceFromCtx(ctx, stateCtx)
  if (stateCtx.state.selectedPrefab) {
    const activePrefab = getActivePrefab(ctx, stateCtx)
    if (activePrefab) {
      return {id: activePrefab.id, prefab: activePrefab, space: null as never}
    }
  }

  return {id: activeSpace?.id, space: activeSpace, prefab: null as never}
}

const useActiveRoot = () => {
  const ctx = useSceneContext()
  const stateCtx = useStudioStateContext()
  return getActiveRoot(ctx, stateCtx)
}

export {
  getActiveRoot,
  useActiveRoot,
}
