import type {ReadData, Schema, Type, TypeToValue, WriteData} from './schema'

type ExtendedSchemaValue<T extends Type> = T | [T] | [T, TypeToValue[T]]

type ExtendedSchemaEntry<S extends Schema, K extends keyof S> = [K, ExtendedSchemaValue<S[K]>]

type ExtendedSchema<S extends Schema> = {
  [key in keyof S]: ExtendedSchemaValue<S[key]>
}

type BaseSchema<S extends ExtendedSchema<Schema>> = {
  [K in keyof S]: S[K] extends ExtendedSchemaValue<infer T> ? T : never
}

const extractFieldType = <T extends Type>(value: ExtendedSchemaValue<T>): Type => {
  if (Array.isArray(value)) {
    return value[0]
  }
  return value
}

const extractSchema = <T extends ExtendedSchema<Schema>>(
  extendedSchema: T
): BaseSchema<T> => {
  const entries = Object.entries(extendedSchema)
  const mappedEntries = entries.map(([key, value]) => [key, extractFieldType(value)])
  return Object.fromEntries(mappedEntries) as BaseSchema<T>
}

const extractDefaults = <T extends ExtendedSchema<Schema>>(
  extendedSchema: T
): Partial<ReadData<BaseSchema<T>>> => {
  const defaults: Partial<WriteData<BaseSchema<T>>> = {}
  Object.entries(extendedSchema).forEach(([key, value]) => {
    if (Array.isArray(value) && value.length >= 2) {
      const [, defaultValue] = value
      // @ts-ignore
      defaults[key] = defaultValue
    }
  })
  return defaults
}

export {
  extractSchema,
  extractDefaults,
}

export type {
  ExtendedSchema,
  ExtendedSchemaEntry,
  BaseSchema,
}
