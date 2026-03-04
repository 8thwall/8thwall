import type {World} from './world'
import type {
  Eid, OrderedSchema, ReadData, Schema, WriteData,
} from '../shared/schema'
import type {RootAttribute, WorldAttribute} from './world-attribute'
import {asm} from './asm'
import {createCursor} from './cursor'
import {getDefaults, getSchemaAlignment, getSchemaSize, toOrderedSchema} from './memory'
import {
  BaseSchema, ExtendedSchema, extractDefaults, extractSchema,
} from '../shared/extended-schema'

const createWorldAttribute = <T extends Schema>(
  world: World, id: number, orderedSchema: OrderedSchema, defaults: ReadData<T>
): WorldAttribute<T> => {
  const worldId = world._id

  const cursor = createCursor<T>(world, orderedSchema)

  const prepareCursor = (eid: Eid, pointer: number) => {
    if (!pointer) {
      throw new Error('Failed to establish cursor')
    }
    cursor._eid = eid
    cursor._ptr = pointer
    cursor._index = 0
    return cursor
  }

  const getCursor = (eid: Eid) => {
    const mutIndex = asm.entityGetMutComponent(worldId, eid, id)
    return prepareCursor(eid, mutIndex)
  }

  const acquireExistingComponent = (eid: Eid) => {
    const mutIndex = asm.entityStartMutExistingComponent(worldId, eid, id)
    return prepareCursor(eid, mutIndex)
  }

  const acquire = (eid: Eid) => {
    const mutIndex = asm.entityStartMutComponent(worldId, eid, id)
    return prepareCursor(eid, mutIndex)
  }

  const commit = (eid: Eid, modified: boolean = true) => {
    asm.entityEndMutComponent(worldId, eid, id, modified)
  }

  const has = (eid: Eid) => !!asm.entityHasComponent(worldId, eid, id)

  let isMutating = false
  const mutate = (eid: Eid, fn: (cursor: WriteData<T>) => void | boolean) => {
    if (isMutating) {
      throw new Error('Cannot nest mutate calls.')
    }
    isMutating = true
    const deferredCursor = acquireExistingComponent(eid)
    const modified = !fn(deferredCursor)
    commit(eid, modified)
    isMutating = false
  }

  return {
    id,
    set: (eid, data) => {
      const didHave = has(eid)
      const entityCursor = acquire(eid)
      if (didHave) {
        Object.assign(entityCursor, data)
      } else {
        Object.assign(entityCursor, defaults, data)
      }
      commit(eid)
    },
    reset: (eid) => {
      const entityCursor = acquire(eid)
      Object.assign(entityCursor, defaults)
      commit(eid)
    },
    mutate,
    dirty: (eid) => {
      asm.entityGetMutComponent(worldId, eid, id)
    },
    cursor: getCursor,
    acquire: acquireExistingComponent,
    commit,
    get: (eid) => {
      const ptr = asm.entityGetComponent(worldId, eid, id)
      return prepareCursor(eid, ptr)
    },
    remove: eid => asm.entityRemoveComponent(worldId, eid, id),
    has,
  }
}

const createAttribute = <T extends Schema>(
  schema?: T, customDefaults?: Partial<ReadData<T>>
): RootAttribute<T> => {
  const handleMap = new WeakMap<World, WorldAttribute<T>>()

  const orderedSchema = toOrderedSchema(schema)
  const sizing = getSchemaSize(orderedSchema)
  const alignment = getSchemaAlignment(orderedSchema)
  const defaults = getDefaults(schema, customDefaults)

  const forWorld = (world: World) => {
    if (handleMap.has(world)) {
      return handleMap.get(world)!
    } else {
      const id = asm.createComponent(world._id, sizing, alignment)
      const handle = createWorldAttribute<T>(world, id, orderedSchema, defaults)
      handleMap.set(world, handle)
      return handle
    }
  }

  const root: RootAttribute<T> = {
    set: (world, eid, data) => forWorld(world).set(eid, data),
    reset: (world, eid) => forWorld(world).reset(eid),
    dirty: (world, eid) => forWorld(world).dirty(eid),
    get: (world, eid) => forWorld(world).get(eid),
    cursor: (world, eid) => forWorld(world).cursor(eid),
    acquire: (world, eid) => forWorld(world).acquire(eid),
    commit: (world, eid) => forWorld(world).commit(eid),
    mutate: (world, eid, fn) => forWorld(world).mutate(eid, fn),
    has: (world, eid) => forWorld(world).has(eid),
    remove: (world, eid) => forWorld(world).remove(eid),
    forWorld,
    schema,
    orderedSchema,
    defaults,
  }

  return root
}

const createExtendedAttribute = <T extends ExtendedSchema<Schema>>(
  extendedSchema: T | undefined, customDefaults?: Partial<ReadData<BaseSchema< T>>>
): RootAttribute<BaseSchema<T>> => {
  if (!extendedSchema) {
    return createAttribute()
  }
  const schema = extractSchema(extendedSchema)
  let defaults = extractDefaults(extendedSchema)
  if (customDefaults) {
    if (Object.keys(defaults).length > 0) {
      throw new Error('When providing defaults within schema, schemaDefaults is not supported.')
    }
    defaults = customDefaults
  }
  return createAttribute<BaseSchema<T>>(schema, defaults)
}

export {
  createWorldAttribute,
  createAttribute,
  createExtendedAttribute,
}
