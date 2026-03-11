import type {DeepReadonly} from 'ts-essentials'
import {z} from 'zod'

import {
  BaseGraphObject, ObjectIds, pathsSchema, PathValues, pathValuesSchema, readonlyObjectSchema,
} from '../common'
import {objectIdSchema} from '../ecs/scene-graph'

const getObjectsPropertyRequestSchema = z.object({
  objects: z.array(z.object({
    id: objectIdSchema,
    paths: pathsSchema,
    readonlyObject: readonlyObjectSchema,
  })).min(1).describe('Array of objects to get properties for.'),
})

const setObjectsPropertyRequestSchema = z.object({
  objects: z.array(z.object({
    id: objectIdSchema,
    pathValues: pathValuesSchema,
    readonlyObject: readonlyObjectSchema,
  })).min(1).describe('Array of objects to set properties for.'),
})

type GetObjectsPropertyRequest = z.infer<typeof getObjectsPropertyRequestSchema>
type FullObjectResponse = {
  id: string
  object: DeepReadonly<BaseGraphObject>
}
type PartialObjectResponse = {
  id: string
  pathValues: DeepReadonly<PathValues>
}
type ObjectResponse = FullObjectResponse | PartialObjectResponse
type GetObjectsPropertyResponse = {objects: ObjectResponse[]}
type SetObjectsPropertyRequest = z.infer<typeof setObjectsPropertyRequestSchema>
type SetObjectsPropertyResponse = {updatedObjectIds: ObjectIds}

export {
  getObjectsPropertyRequestSchema, setObjectsPropertyRequestSchema,
}

export type {
  GetObjectsPropertyRequest,
  GetObjectsPropertyResponse,
  SetObjectsPropertyRequest,
  SetObjectsPropertyResponse,
}
