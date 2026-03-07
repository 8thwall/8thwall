// @sublibrary(:world)
import type {World} from './world'
import type {Eid, OrderedSchema, ReadData, Schema, WriteData} from '../shared/schema'

type SchemaOf<T extends RootAttribute<Schema>> = T extends RootAttribute<infer P> ? P : never

type RootAttribute<T extends Schema> = {
  set(world: World, eid: Eid, data?: Partial<ReadData<T>>): void
  get(world: World, eid: Eid): ReadData<T>
  has(world: World, eid: Eid): boolean
  cursor(world: World, eid: Eid): WriteData<T>
  mutate: (world: World, eid: Eid, fn: (cursor: WriteData<T>) => void | boolean) => void
  acquire(world: World, eid: Eid): WriteData<T>
  commit(world: World, eid: Eid, modified?: boolean): void
  reset(world: World, eid: Eid): void
  remove(world: World, eid: Eid): void
  dirty(world: World, eid: Eid): void
  forWorld: (world: World) => WorldAttribute<T>
  schema: T | undefined
  orderedSchema: OrderedSchema
  defaults: Partial<ReadData<T>> | undefined
}

type WorldAttribute<T extends Schema> = {
  id: number
  set(eid: Eid, data?: Partial<ReadData<T>>): void
  get(eid: Eid): ReadData<T>
  has(eid: Eid): boolean
  cursor(eid: Eid): WriteData<T>
  mutate(eid: Eid, fn: (cursor: WriteData<T>) => void | boolean): void
  acquire(eid: Eid): WriteData<T>
  commit(eid: Eid, modified?: boolean): void
  reset(eid: Eid): void
  remove(eid: Eid): void
  dirty(eid: Eid): void
}

export type {
  SchemaOf,
  RootAttribute,
  WorldAttribute,
}
