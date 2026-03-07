import type {DeepReadonly} from 'ts-essentials'

import type {Resource, SceneGraph} from './scene-graph'
import {inferResourceObject} from './resource'

const getReflectionsFromSceneGraph = (
  sceneGraph: DeepReadonly<SceneGraph>, activeSpaceId: string | undefined
): DeepReadonly<Resource> | undefined => {
  if (sceneGraph.spaces && activeSpaceId && sceneGraph.spaces[activeSpaceId]?.reflections) {
    return inferResourceObject(sceneGraph.spaces[activeSpaceId].reflections)
  } else if (!activeSpaceId && !!sceneGraph.reflections) {
    return inferResourceObject(sceneGraph.reflections)
  }
  return undefined
}

export {
  getReflectionsFromSceneGraph,
}
