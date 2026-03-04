import type {World} from './world'
import {asm} from './asm'
import type {Eid} from '../shared/schema'
import type {RootAttribute} from './world-attribute'
import {createInstanced} from '../shared/instanced'
import {extractEids} from './list-data'
import {WASM_POINTER_SIZE} from './constants'

type Query = (world: World) => Eid[]

type LifecycleQueries = {
  init: (world: World) => void
  enter: Query
  changed: Query
  exit: Query
}

interface RootQuery extends Query {
  terms: RootAttribute<any>[]
}

const extractIdsFromAttribute = (world: World, terms: RootAttribute<any>[]) => {
  const ids = terms.map(e => e.forWorld(world).id)
  const ptr = asm._malloc(ids.length * WASM_POINTER_SIZE)
  const view = new Uint32Array(asm.HEAPU32.buffer, ptr, ids.length)
  view.set(ids)
  return ptr
}

const defineQuery = (terms: RootAttribute<any>[]): RootQuery => {
  const instanced = createInstanced<World, number>((world) => {
    if (terms.length === 0) {
      throw new Error('Query must have at least one term')
    }
    const ptr = extractIdsFromAttribute(world, terms)
    const newInstance = asm.createQuery(world._id, ptr, terms.length)
    asm._free(ptr)
    return newInstance
  })

  return Object.assign((world: World) => {
    const instance = instanced(world)

    const iterPtr = asm._malloc(WASM_POINTER_SIZE)
    asm.dataView.setUint32(iterPtr, 0)
    const listPtr = asm._malloc(WASM_POINTER_SIZE)
    asm.dataView.setUint32(listPtr, 0)

    const res: Eid[] = []

    // eslint-disable-next-line no-constant-condition
    while (true) {
      const count = asm.executeQuery(world._id, instance, iterPtr, listPtr)
      if (count === -1) {
        break
      }
      res.push(...extractEids(listPtr, count))
    }

    asm._free(iterPtr)
    asm._free(listPtr)

    return res
  }, {terms})
}

const flushObserver = (observer: number) => {
  const listPtr = asm._malloc(WASM_POINTER_SIZE)
  asm.dataView.setUint32(listPtr, 0)
  const count = asm.flushObserver(observer, listPtr)
  const res: Eid[] = [...extractEids(listPtr, count)]
  asm._free(listPtr)
  return res
}

const makeObserver = (observerProvider: (world: World) => number) => {
  const instanced = createInstanced<World, number>(observerProvider)

  return (world: World) => {
    const observer = instanced(world)
    return flushObserver(observer)
  }
}

const enterQuery = (t: RootQuery): Query => makeObserver((world) => {
  const ptr = extractIdsFromAttribute(world, t.terms)
  const observer = asm.createAddObserver(world._id, ptr, t.terms.length)
  asm._free(ptr)
  return observer
})

const exitQuery = (t: RootQuery): Query => makeObserver((world) => {
  const ptr = extractIdsFromAttribute(world, t.terms)
  const observer = asm.createRemoveObserver(world._id, ptr, t.terms.length)
  asm._free(ptr)
  return observer
})

const changedQuery = (t: RootQuery): Query => makeObserver((world) => {
  const ptr = extractIdsFromAttribute(world, t.terms)
  const observer = asm.createChangeObserver(world._id, ptr, t.terms.length)
  asm._free(ptr)
  return observer
})

const lifecycleQueries = (t: RootQuery): LifecycleQueries => {
  const observerProvider = (world: World) => {
    const ptr = extractIdsFromAttribute(world, t.terms)
    const lifecycleObserver = asm.createLifecycleObserver(world._id, ptr, t.terms.length)
    asm._free(ptr)
    return lifecycleObserver
  }

  const instanced = createInstanced<World, number>(observerProvider)

  return {
    init: (world: World) => instanced(world),
    enter: (world: World) => {
      const lifecycle = instanced(world)
      const listPtr = asm._malloc(WASM_POINTER_SIZE)
      asm.dataView.setUint32(listPtr, 0)
      const count = asm.accessLifecycleAdded(lifecycle, listPtr)
      const res: Eid[] = [...extractEids(listPtr, count)]
      asm._free(listPtr)
      return res
    },
    changed: (world: World) => {
      const lifecycle = instanced(world)
      const listPtr = asm._malloc(WASM_POINTER_SIZE)
      asm.dataView.setUint32(listPtr, 0)
      const count = asm.accessLifecycleChanged(lifecycle, listPtr)
      const res: Eid[] = [...extractEids(listPtr, count)]
      asm._free(listPtr)
      return res
    },
    exit: (world: World) => {
      const lifecycle = instanced(world)
      const listPtr = asm._malloc(WASM_POINTER_SIZE)
      asm.dataView.setUint32(listPtr, 0)
      const count = asm.accessLifecycleRemoved(lifecycle, listPtr)
      const res: Eid[] = [...extractEids(listPtr, count)]
      asm._free(listPtr)
      return res
    },
  }
}

export {
  defineQuery,
  enterQuery,
  changedQuery,
  exitQuery,
  lifecycleQueries,
}
