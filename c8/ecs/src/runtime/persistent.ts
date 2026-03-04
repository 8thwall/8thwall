import type {Eid} from '../shared/schema'
import {Persistent} from './components'
import type {World} from './world'

const isEntityPersistent = (world: World, eid: Eid) => {
  let cursor = eid
  while (cursor) {
    if (Persistent.has(world, cursor)) {
      return true
    }
    cursor = world.getParent(cursor)
  }
  return false
}

export {
  isEntityPersistent,
}
