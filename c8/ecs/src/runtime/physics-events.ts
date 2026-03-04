import type {World} from './world'
import {asm} from './asm'
import {WASM_POINTER_SIZE} from './constants'
import {extractEids} from './list-data'
import type {Eid} from '../shared/schema'
import {dispatchEventImmediate, InternalEvents} from './events'

const COLLISION_START_EVENT = 'physics-collision-start' as const
const COLLISION_END_EVENT = 'physics-collision-end' as const
const UPDATE_EVENT = 'physics-update' as const

const dispatchCollisionPairs = (
  events: InternalEvents, event: string, listPtr: number, count: number
) => {
  const eids = extractEids(listPtr, count)

  // NOTE(christoph): The eid list contains a pair of eids for each collision, in index 2n and 2n+1.
  // [1, 2, 3, 4] -> (1, 2), (3, 4)
  let prev: Eid = 0n
  for (const eid of eids) {
    if (prev !== 0n) {
      dispatchEventImmediate(events, eid, event, {other: prev})
      dispatchEventImmediate(events, prev, event, {other: eid})
      prev = 0n
    } else {
      prev = eid
    }
  }
}

const dispatchCollisionStart = (world: World, events: InternalEvents) => {
  const listPtr = asm._malloc(WASM_POINTER_SIZE)
  const count = asm.getCollisionStartEvents(world._id, listPtr)
  dispatchCollisionPairs(events, COLLISION_START_EVENT, listPtr, count)
  asm._free(listPtr)
}

const dispatchCollisionEnd = (world: World, events: InternalEvents) => {
  const listPtr = asm._malloc(WASM_POINTER_SIZE)
  const count = asm.getCollisionEndEvents(world._id, listPtr)
  dispatchCollisionPairs(events, COLLISION_END_EVENT, listPtr, count)
  asm._free(listPtr)
}

const dispatchPhysicsEvents = (world: World, events: InternalEvents) => {
  dispatchCollisionEnd(world, events)
  dispatchCollisionStart(world, events)
  dispatchEventImmediate(events, events.globalId, UPDATE_EVENT, {})
}

export {
  dispatchPhysicsEvents,
  COLLISION_END_EVENT,
  COLLISION_START_EVENT,
  UPDATE_EVENT,
}
