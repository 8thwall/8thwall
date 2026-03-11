import {objectExists} from '@ecs/shared/object-hierarchy'
import type {SceneGraph} from '@ecs/shared/scene-graph'
import type {DeepReadonly} from 'ts-essentials'

import {getObjectOrMergedInstance} from './derive-scene-operations'

const calculateDescalingFactor = (
  id: string, scene: DeepReadonly<SceneGraph>
) => {
  const factor = [1, 1, 1]
  let nextObjectId = id
  while (objectExists(scene, nextObjectId)) {
    const currentObj = getObjectOrMergedInstance(scene, nextObjectId)
    factor[0] *= currentObj.scale[0]
    factor[1] *= currentObj.scale[1]
    factor[2] *= currentObj.scale[2]
    nextObjectId = currentObj.parentId
  }
  return factor
}

export {
  calculateDescalingFactor,
}
