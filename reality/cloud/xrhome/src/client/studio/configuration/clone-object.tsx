import {v4 as uuid} from 'uuid'
import type {DeepReadonly} from 'ts-essentials'
import type {GraphObject, SceneGraph} from '@ecs/shared/scene-graph'

import {makeObjectName} from '../common/studio-files'

const cloneObject = (
  scene: DeepReadonly<SceneGraph>,
  targetObject: DeepReadonly<GraphObject>,
  spaceId: string | undefined,
  newId?: string,
  parentId?: string
) => {
  const newObjectId = newId || uuid()

  return {
    ...targetObject,
    parentId,
    id: newObjectId,
    name: makeObjectName(targetObject.name, scene, spaceId, undefined, targetObject.prefab),
  }
}

export {
  cloneObject,
}
