import type {DeepReadonly} from 'ts-essentials'
import type {GraphObject, GraphComponent} from '@ecs/shared/scene-graph'

type Objects = {type: 'objects', objects: DeepReadonly<GraphObject[]>}
type DirectProperties = {type: 'directProperties', properties: DeepReadonly<Partial<GraphObject>>}
type CustomComponent = {type: 'customComponent', component: DeepReadonly<GraphComponent>}
type Empty = {type: 'empty'}

type Clipboard = Objects | DirectProperties | CustomComponent | Empty

export type {
  Clipboard,
}
