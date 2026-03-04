import type {DeepReadonly} from 'ts-essentials'

import type {World} from '../runtime/world'
import type {SceneGraph} from './scene-graph'
import type {SceneHandle} from '../runtime/scene-types'

interface IApplication {
  getWorld: () => World
  getScene: () => SceneHandle
  init: (sceneGraph: DeepReadonly<SceneGraph>) => Promise<void>
}

export type {
  IApplication,
}
