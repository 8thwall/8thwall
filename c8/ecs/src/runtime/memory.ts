import type {OrderedSchema, Schema, Type, WriteData, ReadData} from '../shared/schema'
import {validateCustomDefaults} from './validate'

const hex = (ptr: number) => `0x${ptr.toString(16)}`

const TYPE_TO_SIZE = {
  'eid': 8,
  'f32': 4,
  'f64': 8,
  'i32': 4,
  'ui8': 1,
  'ui32': 4,
  'string': 4,  // Index into a string table that belongs to the world
  'boolean': 1,
} as const

const TYPE_TO_DEFAULT = {
  'eid': BigInt(0),
  'f32': 0,
  'f64': 0,
  'i32': 0,
  'ui8': 0,
  'ui32': 0,
  'string': '',
  'boolean': false,
} as const

const toOrderedSchema = <T extends Schema>(schema?: T) => {
  const orderedSchema: OrderedSchema = []

  if (schema) {
    // NOTE(christoph): Larger fields are first.
    const entries = Object.entries(schema).sort(([, a], [, b]) => TYPE_TO_SIZE[b] - TYPE_TO_SIZE[a])
    let totalSize = 0
    entries.forEach(([key, type]) => {
      orderedSchema.push([key, type, totalSize])
      totalSize += TYPE_TO_SIZE[type]
    })
  }

  return orderedSchema
}

const getSchemaSize = (orderedSchema: OrderedSchema) => {
  if (!orderedSchema.length) {
    return 1
  }
  const [, type, offset] = orderedSchema[orderedSchema.length - 1]
  return offset + TYPE_TO_SIZE[type]
}

const getSchemaAlignment = (orderedSchema: OrderedSchema) => (
  Math.max(1, ...orderedSchema.map(([, type]) => TYPE_TO_SIZE[type]))
)

const getDefaults = <T extends Schema>(schema?: T, customDefaults?: Partial<ReadData<T>>) => {
  const defaults = {} as WriteData<T>
  validateCustomDefaults(schema, customDefaults)
  if (schema) {
    Object.entries(schema).forEach(([key, type]: [keyof T, Type]) => {
      defaults[key] = (customDefaults?.[key] ?? TYPE_TO_DEFAULT[type]) as WriteData<T>[keyof T]
    })
  }
  return defaults
}

export {
  hex,
  toOrderedSchema,
  getSchemaSize,
  getSchemaAlignment,
  getDefaults,
}
