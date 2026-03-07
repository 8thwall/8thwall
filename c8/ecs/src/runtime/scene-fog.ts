import type {DeepReadonly} from 'ts-essentials'

import type {SceneGraph} from '../shared/scene-graph'
import type {World} from './world'
import {getFogFromSceneGraph} from '../shared/get-fog-from-scene-graph'

const setFog = (
  world: World, sceneGraph: DeepReadonly<SceneGraph>, activeSpace: string | undefined
) => {
  if (!activeSpace) {
    return
  }

  const fog = getFogFromSceneGraph(sceneGraph, activeSpace)
  world.effects.setFog(fog)
}

export {
  setFog,
}
