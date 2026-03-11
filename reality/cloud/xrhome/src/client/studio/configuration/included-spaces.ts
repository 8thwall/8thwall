import type {DeepReadonly} from 'ts-essentials'

import type {Space} from '@ecs/shared/scene-graph'

import type {SceneContext} from '../scene-context'

const updateIncludedSpaces = (spaceId: string,
  ctx: SceneContext,
  includedSpaces: readonly string[],
  activeSpace: DeepReadonly<Space>) => {
  ctx.updateScene((scene) => {
    let newIncludedSpaces: string[]
    if (!includedSpaces) {
      newIncludedSpaces = [spaceId]
    } else {
      newIncludedSpaces = includedSpaces?.includes(spaceId)
        ? includedSpaces.filter(id => id !== spaceId)
        : includedSpaces.concat(spaceId)
    }
    return {
      ...scene,
      spaces: {
        ...scene.spaces,
        [activeSpace.id]: {
          ...scene.spaces[activeSpace.id],
          includedSpaces: newIncludedSpaces,
        },
      },
    }
  })
}

export {
  updateIncludedSpaces,
}
