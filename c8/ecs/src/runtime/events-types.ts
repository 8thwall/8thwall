import type {Eid} from '../shared/schema'

type QueuedEvent<D = unknown> = {
  // The entity the event was dispatched on
  target: Eid
  // The entity the event listener was attached to
  currentTarget: Eid
  name: string
  data: D
}

type EventListener<D = unknown> = (event: QueuedEvent<D>) => void

type ListenerMap = Map<Eid, Map<string, Set<EventListener>>>

declare global {
  interface EcsEventTypes {
    // NOTE(christoph): Since this is in the global scope, it can be extended by any runtime file
    // or by user code.
  }
}

type EcsEventTypes = globalThis.EcsEventTypes

type DataForEvent<EVENT> = EVENT extends keyof EcsEventTypes
  ? EcsEventTypes[EVENT]
  : unknown

type EventListenerForEvent<EVENT> = EventListener<DataForEvent<EVENT>>

interface Events {
  globalId: Eid
  addListener: <T extends string>(
    target: Eid,
    name: T,
    listener: EventListener<DataForEvent<T>>
  ) => void
  removeListener: (target: Eid, name: string, listener: EventListener) => void
  dispatch: (target: Eid, name: string, data?: unknown) => void
}

export type {
  ListenerMap,
  EventListener,
  QueuedEvent,
  Events,
  DataForEvent,
  EventListenerForEvent,
}
