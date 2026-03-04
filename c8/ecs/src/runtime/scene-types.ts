import type {DeepReadonly} from 'ts-essentials'

import type {SceneGraph, GraphObject} from '../shared/scene-graph'
import type {Eid} from '../shared/schema'

type SpaceData = {
  id: string
  name: string
  spawned: boolean
}

type SpacesHandle = {
  loadSpace: (idOrName: string) => void
  listSpaces: () => SpaceData[] | undefined
  getActiveSpace: () => SpaceData | undefined
}

type PrefabsHandle = {
  graphIdToPrefab: Map<string, Eid>
  getPrefab: (name: string) => Eid | undefined
}

type SceneHandle = SpacesHandle & PrefabsHandle &{
  remove: () => void
  updateBaseObjects: (newObjects: DeepReadonly<Record<string, GraphObject>>) => void
  updateDebug: (newGraph: DeepReadonly<SceneGraph>) => void
  graphIdToEid: Map<string, Eid>
  eidToObject: Map<Eid, DeepReadonly<GraphObject>>
  graphIdToPrefab: Map<string, Eid>
  _graphIdToEidOrPrefab: Map<string, Eid>
}

export type {
  SpaceData,
  SceneHandle,
  SpacesHandle,
  PrefabsHandle,
}
