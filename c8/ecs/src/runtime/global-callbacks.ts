import type {Eid} from '../shared/schema'

type EntityCallback = ((entity: Eid) => void) | null

declare global {
  // eslint-disable-next-line
  var _ecsDeleteEntityCallback: EntityCallback
}

const setGlobalDeleteCallback = (callback: EntityCallback): EntityCallback => {
  const previous = globalThis._ecsDeleteEntityCallback
  globalThis._ecsDeleteEntityCallback = callback
  return previous
}

export {
  setGlobalDeleteCallback,
}
