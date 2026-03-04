import type {DeepReadonly} from 'ts-essentials'

import type {SceneGraph, Sky} from './scene-graph'
import {DEFAULT_BOTTOM_COLOR, DEFAULT_TOP_COLOR} from './sky-constants'

const getSkyFromSceneGraph = (
  sceneGraph: DeepReadonly<SceneGraph>, activeSpaceId: string | undefined
): DeepReadonly<Sky> => {
  const graphSky = sceneGraph.spaces && activeSpaceId
    ? sceneGraph.spaces[activeSpaceId]?.sky
    : sceneGraph.sky
  return graphSky ?? {type: 'gradient', colors: [DEFAULT_TOP_COLOR, DEFAULT_BOTTOM_COLOR]}
}

export {
  getSkyFromSceneGraph,
}
