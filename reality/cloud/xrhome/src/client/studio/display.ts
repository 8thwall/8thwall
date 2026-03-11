import type {DeepReadonly} from 'ts-essentials'

import type {GraphObject} from '@ecs/shared/scene-graph'

const displayNameForObject = (o: DeepReadonly<GraphObject>) => o.name || o.id.split('-')[0]

export {
  displayNameForObject,
}
