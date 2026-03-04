import type {Eid} from '../shared/schema'

// NOTE(christoph): Doesn't match the exact event name because generate-ecs-definition.ts
//  filters out any type starting with `Location`
type LocationSpawnedEvent = {
  id: string
  imageUrl: string
  title: string
  lat: number
  lng: number
  mapPoint: Eid  // defined if spawned, undefined if tracked
}

export type {
  LocationSpawnedEvent,
}
