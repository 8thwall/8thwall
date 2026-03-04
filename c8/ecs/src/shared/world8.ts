import type {DeepReadonly} from 'ts-essentials'

import type {World} from '../runtime/world'
import type {SceneGraph} from './scene-graph'
import type {SceneHandle} from '../runtime/scene-types'

type World8 = {
  world: World
  scene: DeepReadonly<SceneGraph>
  sceneHandle: SceneHandle
}

export type {
  World8,
}
