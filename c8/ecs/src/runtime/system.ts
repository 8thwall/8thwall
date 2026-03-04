import type {World} from './world'
import {asm} from './asm'
import type {Eid, WriteData} from '../shared/schema'
import type {RootAttribute, SchemaOf} from './world-attribute'
import {createInstanced} from '../shared/instanced'
import {getSchemaSize} from './memory'
import {Cursor, createCursor} from './cursor'
import {extractListData, extractEids} from './list-data'
import {WASM_POINTER_SIZE} from './constants'

type Attributes = RootAttribute<{}>[]

type TableMatch<T extends Attributes> = {
  eids: Generator<Eid>
  ptrs: {[K in keyof T]: number}
  count: number
}

type SystemQuery<T extends Attributes> = (world: World) => Generator<TableMatch<T>>

type WriteDataForTerms<T extends Attributes> = {
  [K in keyof T]: WriteData<SchemaOf<T[K]>>
}

type CursorsForTerms<T extends Attributes> = {
  [K in keyof T]: Cursor<SchemaOf<T[K]>>
}

type SystemCallback<T extends Attributes> = (
  (world: World, eid: Eid, cursors: WriteDataForTerms<T>) => void
)

const defineSystemQuery = <T extends Attributes>(terms: T): SystemQuery<T> => {
  const instanced = createInstanced<World, number>((world) => {
    if (terms.length === 0) {
      throw new Error('Query must have at least one term')
    }
    const ids = terms.map(e => e.forWorld(world).id)
    const ptr = asm._malloc(ids.length * WASM_POINTER_SIZE)
    const view = new Uint32Array(asm.HEAPU32.buffer, ptr, ids.length)
    view.set(ids)

    // note: the last argument forces caching for all system queries.
    const newInstance = asm.createQuery(world._id, ptr, terms.length, true)
    asm._free(ptr)
    return newInstance
  })

  return function* query(world: World) {
    const instance = instanced(world)

    const iterPtr = asm._malloc(WASM_POINTER_SIZE)
    asm.dataView.setUint32(iterPtr, 0)
    const listPtr = asm._malloc(WASM_POINTER_SIZE)
    asm.dataView.setUint32(listPtr, 0)
    const dataPtr = asm._malloc(WASM_POINTER_SIZE * terms.length)
    asm.dataView.setUint32(dataPtr, 0)

    // eslint-disable-next-line no-constant-condition
    while (true) {
      const count = asm.executeQuery(world._id, instance, iterPtr, listPtr, dataPtr, terms.length)
      if (count === -1) {
        break
      }
      yield <TableMatch<T>>{
        eids: extractEids(listPtr, count),
        ptrs: [...extractListData(dataPtr, terms.length)],
        count,
      }
    }

    asm._free(iterPtr)
    asm._free(listPtr)
    asm._free(dataPtr)
  }
}

const defineSystem = <T extends Attributes>(terms: T, callback: SystemCallback<T>) => {
  const query = defineSystemQuery(terms)

  const instancedCursors = createInstanced<World, CursorsForTerms<T>>(world => terms.map((t) => {
    const cursor = createCursor(world, t.orderedSchema)
    cursor._stride = getSchemaSize(t.orderedSchema)
    return cursor
  }) as any as CursorsForTerms<T>)

  return (world: World) => {
    const cursors = instancedCursors(world)
    for (const match of query(world)) {
      for (let i = 0; i < cursors.length; i++) {
        cursors[i]._ptr = match.ptrs[i]
        cursors[i]._index = 0
      }
      for (const eid of match.eids) {
        for (const cursor of cursors) {
          cursor._eid = eid
        }
        callback(world, eid, cursors)
        for (const cursor of cursors) {
          // NOTE(christoph): The TableMatch gives us contiguous memory for the component
          // data so we can increment the cursors to progress to the next entity's data.
          cursor._index++
        }
      }
    }
  }
}

export {
  defineSystemQuery,
  defineSystem,
}
