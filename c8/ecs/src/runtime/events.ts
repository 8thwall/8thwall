import type {World} from './world'
import type {Eid} from '../shared/schema'
import type {EventListener, Events, ListenerMap, QueuedEvent} from './events-types'

type InternalState = {
  globalId: Eid
  _world: World
  _listeners: ListenerMap
  _queue: QueuedEvent[]
}

type InternalEvents = Events & InternalState

const addListener = (
  state: InternalState, target: Eid, name: string,
  listener: EventListener<any>
) => {
  const listeners = state._listeners.get(target) ?? new Map<string, Set<EventListener>>()
  const eventListeners = listeners.get(name) ?? new Set<EventListener>()
  eventListeners.add(listener)
  listeners.set(name, eventListeners)
  state._listeners.set(target, listeners)
}

const removeListener = (
  state: InternalState,
  target: Eid,
  name: string,
  listener: EventListener
) => {
  state._listeners.get(target)?.get(name)?.delete(listener)
}

const MAX_BUBBLE_DEPTH = 100

const dispatchEvent = (state: InternalState, target: Eid, name: string, data: unknown) => {
  let currentTarget = target

  let depth = 0
  while (depth++ < MAX_BUBBLE_DEPTH) {
    state._queue.push({target, name, data, currentTarget})
    if (currentTarget === state.globalId) {
      break
    }
    currentTarget = state._world.getParent(currentTarget) || state.globalId
  }
}

const flushEvent = (state: InternalState, event: QueuedEvent) => {
  const liveListeners = state._listeners.get(event.currentTarget)?.get(event.name)
  if (liveListeners) {
    // ignore listeners added after we started iterating
    const listenersCopy = [...liveListeners]
    for (const listener of listenersCopy) {
      // the listener might have been removed from liveListeners since the copy
      if (liveListeners.has(listener)) {
        try {
          listener(event)
        } catch (err) {
          // eslint-disable-next-line no-console
          console.error(err)
        }
      }
    }
  }
}

const dispatchEventImmediate = (state: InternalState, target: Eid, name: string, data: unknown) => {
  let currentTarget = target

  let depth = 0
  while (depth++ < MAX_BUBBLE_DEPTH) {
    flushEvent(state, {target, name, data, currentTarget})
    if (currentTarget === state.globalId) {
      break
    }
    currentTarget = state._world.getParent(currentTarget) || state.globalId
  }
}

const flushEvents = (state: InternalState) => {
  const events = [...state._queue]
  state._queue.length = 0
  for (const event of events) {
    flushEvent(state, event)
  }
}

const createEvents = (world: World): InternalEvents => {
  const state: InternalState = {
    globalId: 0n,
    _listeners: new Map(),
    _queue: [],
    _world: world,
  }

  return Object.assign(state, {
    addListener: addListener.bind(null, state),
    removeListener: removeListener.bind(null, state),
    dispatch: dispatchEvent.bind(null, state),
  })
}

export {
  createEvents,
  flushEvents,
  dispatchEventImmediate,
}

export type {
  EventListener,
  QueuedEvent,
  Events,
  InternalEvents,
}
