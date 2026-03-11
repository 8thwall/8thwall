import type {DeepReadonly} from 'ts-essentials'

import type {SceneGraph, Space} from '@ecs/shared/scene-graph'

import type {IdMapper} from '../id-generation'
import {makeDuplicatedSpaceName} from '../common/studio-files'
import {duplicateObjects} from './duplicate-object'
import {deriveScene} from '../derive-scene'

const duplicateSpace = (oldScene: DeepReadonly<SceneGraph>, spaceId: string, ids: IdMapper) => {
  const space = oldScene.spaces?.[spaceId]
  if (!space) {
    return oldScene
  }

  const derivedScene = deriveScene(oldScene)
  const newSpace: DeepReadonly<Space> = {
    ...space,
    id: ids.fromId(space.id),
    name: makeDuplicatedSpaceName(space.name, Object.values(oldScene.spaces)),
    activeCamera: space.activeCamera && ids.fromId(space.activeCamera),
  }

  const newSpaceScene = {
    ...oldScene,
    spaces: {
      ...oldScene.spaces,
      [newSpace.id]: newSpace,
    },
  }

  const topLevelSpaceObjectIds = derivedScene.getSpaceObjects(space.id)
    .filter(o => (o.parentId === space.id))
    .map(o => o.id)

  return duplicateObjects({
    scene: newSpaceScene,
    derivedScene,
    objectIds: topLevelSpaceObjectIds,
    ids,
    newParentId: newSpace.id,
    spaceId: newSpace.id,
  })
}

export {
  duplicateSpace,
}
