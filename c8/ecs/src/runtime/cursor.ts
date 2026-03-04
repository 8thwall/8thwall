import type {Eid, OrderedSchema, Schema, Type, WriteData} from '../shared/schema'
import type {World} from './world'
import {asm} from './asm'
import {globalStringMap} from './string-storage'

type CursorInternals = {
  _world: World
  _eid: Eid
  _ptr: number
  _stride: number
  _index: number
  _data: DataView
}

type Cursor<T extends Schema> = CursorInternals & WriteData<T>

// NOTE(christoph): Little Endian must always be true for WASM to read the data correctly.
type NumberGetter = (ptr: number, littleEndian: true) => number
type NumberSetter = (ptr: number, value: number, littleEndian: true) => number

type BoundAccessors = {
  setBigUint64: (ptr: number, value: bigint, littleEndian: true) => void
  getFloat32: NumberGetter
  getFloat64: NumberGetter
  getInt32: NumberGetter
  getUint32: NumberGetter
  setFloat32: NumberSetter
  setFloat64: NumberSetter
  setInt32: NumberSetter
  setUint32: NumberSetter

  // NOTE(christoph): It doesn't matter if littleEndian is true or false for uint8 since
  // it's only one byte.
  getUint8: (ptr: number) => number
  setUint8: (ptr: number, value: number) => void
}

const bind = <T extends keyof BoundAccessors>(fn: T): BoundAccessors[T] => (
  (asm.dataView[fn] as any).bind(asm.dataView)
)

const makeStringAccessor = (cursor: CursorInternals, propertyOffset: number) => {
  const get = bind('getUint32')
  const set = bind('setUint32')

  return {
    get() {
      const ptr = cursor._index * cursor._stride + cursor._ptr + propertyOffset
      const stringId = get(ptr, true)
      return globalStringMap(cursor._world).get(stringId)
    },
    set(value: string) {
      const ptr = cursor._index * cursor._stride + cursor._ptr + propertyOffset
      const stringId = globalStringMap(cursor._world).store(value)
      set(ptr, stringId, true)
    },
    enumerable: true,
  }
}

const makeEidAccessor = (cursor: CursorInternals, propertyOffset: number) => {
  const set = bind('setBigUint64')
  return {
    get() {
      const ptr = cursor._index * cursor._stride + cursor._ptr + propertyOffset
      return asm.getEidFromComponentState(cursor._world._id, cursor._eid, ptr)
    },
    set(value: Eid) {
      const ptr = cursor._index * cursor._stride + cursor._ptr + propertyOffset
      set(ptr, value, true)
    },
    enumerable: true,
  }
}

const makeBooleanAccessor = (cursor: CursorInternals, propertyOffset: number) => {
  const get = bind('getUint8')
  const set = bind('setUint8')
  return {
    get() {
      const ptr = cursor._index * cursor._stride + cursor._ptr + propertyOffset
      return !!get(ptr)
    },
    set(value: boolean) {
      const ptr = cursor._index * cursor._stride + cursor._ptr + propertyOffset
      set(ptr, +!!value)
    },
    enumerable: true,
  }
}

const getAccessors = (type: Type) => {
  switch (type) {
    case 'ui8':
      return [bind('getUint8'), bind('setUint8')] as const
    case 'ui32':
      return [bind('getUint32'), bind('setUint32')] as const
    case 'i32':
      return [bind('getInt32'), bind('setInt32')] as const
    case 'f32':
      return [bind('getFloat32'), bind('setFloat32')] as const
    case 'f64':
      return [bind('getFloat64'), bind('setFloat64')] as const
    default:
      throw new Error(`Unknown type: ${type}`)
  }
}

const makePropertyAccessor = (cursor: CursorInternals, type: Type, propertyOffset: number) => {
  if (type === 'string') {
    return makeStringAccessor(cursor, propertyOffset)
  }
  if (type === 'eid') {
    return makeEidAccessor(cursor, propertyOffset)
  }
  if (type === 'boolean') {
    return makeBooleanAccessor(cursor, propertyOffset)
  }

  const [get, set] = getAccessors(type)

  return {
    get() {
      const byteIndex = cursor._index * cursor._stride + cursor._ptr + propertyOffset
      return get(byteIndex, true)
    },
    set(value: number) {
      const byteIndex = cursor._index * cursor._stride + cursor._ptr + propertyOffset
      return set(byteIndex, value, true)
    },
    enumerable: true,
  }
}

const createCursor = <T extends Schema>(world: World, orderedSchema: OrderedSchema) => {
  const cursor = {} as Cursor<T>

  Object.defineProperties(cursor, {
    _world: {
      value: world,
      writable: false,
    },
    _eid: {
      value: BigInt(0),
      writable: true,
    },
    _ptr: {
      value: 0,
      writable: true,
    },
    _stride: {
      value: 0,
      writable: true,
    },
    _index: {
      value: 0,
      writable: true,
    },
  })

  orderedSchema.forEach(([key, type, offset]) => {
    Object.defineProperty(cursor, key, makePropertyAccessor(cursor, type, offset))
  })

  return cursor
}

export {
  createCursor,
}

export type {
  Cursor,
}
