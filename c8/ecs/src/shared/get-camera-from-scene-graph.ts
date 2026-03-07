import type {DeepReadonly} from 'ts-essentials'

import type {GraphObject, SceneGraph} from './scene-graph'
import {getAllIncludedSpaces, resolveSpaceForObject} from './object-hierarchy'

const getActiveCameraIdFromSceneGraph = (
  sceneGraph: DeepReadonly<SceneGraph>, activeSpaceId: string | undefined
): string | undefined => (
  sceneGraph.spaces && activeSpaceId
    ? sceneGraph.spaces[activeSpaceId]?.activeCamera
    : sceneGraph.activeCamera
)

const getActiveCameraFromSceneGraph = (
  sceneGraph: DeepReadonly<SceneGraph>, spaceId: string | undefined
): DeepReadonly<GraphObject> | undefined => {
  const activeCameraId = getActiveCameraIdFromSceneGraph(sceneGraph, spaceId)
  if (!activeCameraId) {
    return undefined
  }
  const includedSpaces = getAllIncludedSpaces(sceneGraph, spaceId)
  if (includedSpaces.length === 0) {
    return sceneGraph.objects[activeCameraId]
  }
  const cameraSpace = resolveSpaceForObject(sceneGraph, activeCameraId)?.id
  return cameraSpace && includedSpaces.includes(cameraSpace)
    ? sceneGraph.objects[activeCameraId]
    : undefined
}

export {
  getActiveCameraIdFromSceneGraph,
  getActiveCameraFromSceneGraph,
}
