import {z} from 'zod'

import type {ObjectIds} from '../common'
import {geometryTypeSchema, lightTypeSchema, objectIdSchema} from '../ecs/scene-graph'

const createObjectsRequestSchema = z.object({
  objects: z.array(z.object({
    type: geometryTypeSchema
      .or(lightTypeSchema)
      .or(z.literal('empty'))
      .describe('Type of the object.'),
    name: z.string().describe('Name of the object.'),
    parentId: objectIdSchema.optional().describe('The ID of the parent object to attach the new object to. This field is optional. If not provided, the object will be added to the root of the scene.'),
  })).describe('Array of objects to create.'),
})

type CreateObjectsRequest = z.infer<typeof createObjectsRequestSchema>
type CreateObjectsResponse = {createdObjectIds: ObjectIds, errors: Record<string, string>}

export {
  createObjectsRequestSchema,
}

export type {
  CreateObjectsRequest,
  CreateObjectsResponse,
}
