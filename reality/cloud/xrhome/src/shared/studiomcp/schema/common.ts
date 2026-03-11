import {z} from 'zod'

import {baseGraphObjectSchema, objectIdSchema} from './ecs/scene-graph'

const requestSchema = z.object({
  action: z.string(),
  parameters: z.record(z.string(), z.any()),
})

const successResponseSchema = z.object({
  requestId: z.string().uuid(),
  isError: z.literal(false).optional(),
  response: z.optional(z.any()),
})

const errorResponseSchema = z.object({
  requestId: z.string().uuid(),
  isError: z.literal(true),
  error: z.unknown(),
})

const responseSchema = z.discriminatedUnion('isError', [
  successResponseSchema,
  errorResponseSchema,
])

const objectIdsSchema = z.array(objectIdSchema).describe('An array of entity IDs.')

// NOTE(kyle): Setting this to baseGraphObjectSchema.keyof().or(z.string()) makes the LLM
// overemphasize on using top-level properties instead of dot notation.
const pathSchema = z.string()
  .optional()
  .describe('The path to the object property to get. Nested properties are written using dot notation. ALWAYS prefer dot notation when getting nested properties. Top level properties like "geometry" and "ui" may return objects while leaf properties like "geometry.type" and "ui.padding" will return primitives. If no property is provided, the entire object is returned.')
const pathsSchema = z.array(pathSchema)
  .describe('An array of paths to get. If an empty array is provided, all properties are returned.')

// NOTE(kyle): Setting this to baseGraphObjectSchema.optional().or(z.any()) makes the LLM
// overemphasize on constructing a baseGraphObjectSchema. I thought we could cheat by encoding the
// graph object schema but give LLM the flexibility to construct any object.
const pathValueSchema = z.any()
  .describe('The path to the object property to set and the value to set it to. Nested properties are written using dot notation. ALWAYS prefer dot notation when setting nested properties. Setting top level properties like "geometry" and "ui" will replace the property\'s entire object tree. Leaf properties like "geometry.type" and "ui.padding" will only replace the property\'s primitive value.')
const pathValuesSchema = z.record(pathSchema, pathValueSchema)
  .describe('A record of paths and their corresponding values.')

// This is a hack to pass BaseGraphObject into the context window but tell the LLM that it should
// never be used. This is useful for getters and will help the LLM construct dot notation paths.
const readonlyObjectSchema = baseGraphObjectSchema
  .describe('This schema is provided solely for informational purposes to help you understand the scene graph structure. It contains the complete BaseGraphObject type definition which can be used as reference when constructing property paths with dot notation (e.g., "geometry.type", "material.color"). The value passed to this parameter should *ALWAYS* be undefined - it exists only to provide context about available properties and their structure.')
  .optional()
  .refine(val => val === undefined, {message: 'readOnlyObjectSchema should never have a value'})

type ObjectIds = z.infer<typeof objectIdsSchema>
type BaseGraphObject = z.infer<typeof baseGraphObjectSchema>
type Path = z.infer<typeof pathSchema>
type Paths = z.infer<typeof pathsSchema>
type PathValue = z.infer<typeof pathValueSchema>
type PathValues = z.infer<typeof pathValuesSchema>

export {
  requestSchema,
  responseSchema,
  objectIdsSchema,
  pathsSchema,
  pathValuesSchema,
  readonlyObjectSchema,
}

export type {
  ObjectIds,
  BaseGraphObject,
  Path,
  Paths,
  PathValue,
  PathValues,
}
