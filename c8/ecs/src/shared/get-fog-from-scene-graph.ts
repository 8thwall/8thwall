import type {DeepReadonly} from 'ts-essentials'

import type {Fog, SceneGraph} from './scene-graph'

const getFogFromSceneGraph = (
  sceneGraph: DeepReadonly<SceneGraph>, activeSpaceId: string | undefined
): DeepReadonly<Fog> | undefined => (
  activeSpaceId && sceneGraph.spaces ? sceneGraph.spaces?.[activeSpaceId]?.fog : undefined
)

export {
  getFogFromSceneGraph,
}
