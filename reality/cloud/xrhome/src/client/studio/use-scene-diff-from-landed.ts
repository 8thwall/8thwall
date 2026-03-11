import type {Expanse} from '@ecs/shared/scene-graph'

import {useSceneContext} from './scene-context'
import {getChangeLog} from './get-change-log'
import type {SceneDiffContext} from './scene-diff-context'

const useSceneDiffFromLanded = (): SceneDiffContext | null => {
  const ctx = useSceneContext()
  const curExpanse = ctx.scene as Readonly<Expanse>
  // TODO(christoph): Get the previous expanse somehow
  const prevExpanse = curExpanse

  return {
    beforeScene: prevExpanse,
    afterScene: curExpanse,
    changeLog: getChangeLog(
      prevExpanse,
      curExpanse
    ),
  }
}

export {useSceneDiffFromLanded}
