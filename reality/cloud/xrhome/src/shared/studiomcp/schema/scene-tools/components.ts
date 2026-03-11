/* eslint-disable max-len */
import {z} from 'zod'

import type {EntityReference} from '@ecs/shared/scene-graph'

import {objectIdSchema, entityReferenceSchema} from '../ecs/scene-graph'

const getAvailableComponentsRequestSchema = z.object({})
const getAvailableComponentsResponseSchema = z.object({
  availableComponents: z.array(z.object({
    name: z.string().describe('Name of the component. Can be used to add the component to an entity in the scene.'),
    schema: z.array(z.object({
      name: z.string().describe('Name of the attribute.'),
      type: z.string().describe('Type of the attribute.'),
      default: z.any().optional().describe('Default value of the attribute.'),
    })),
  })).describe('List of available components.'),
})

type GetAvailableComponentsRequest = z.infer<typeof getAvailableComponentsRequestSchema>
type GetAvailableComponentsResponse = z.infer<typeof getAvailableComponentsResponseSchema>

const componentParametersSchema = z.record(
  z.string(),
  z.union([z.string(), z.number(), z.boolean(), entityReferenceSchema as z.ZodType<EntityReference>])
).describe('Component parameters with their values')

const applyComponentsRequestSchema = z.object({
  componentsByObjectId: z.record(
    objectIdSchema,
    z.array(z.object({
      componentName: z.string().describe('Name of the component to apply'),
      parameters: componentParametersSchema,
    })).describe('Array of components to apply to this object')
  ).describe('Map of object IDs to array of components to apply'),
})

const applyComponentsResponseSchema = z.object({
  appliedComponents: z.record(
    objectIdSchema,
    z.array(z.object({
      componentId: z.string().describe('Generated component ID'),
      componentName: z.string().describe('Name of the applied component'),
      appliedParameters: componentParametersSchema,
    }))
  ).describe('Map of object IDs to successfully applied components'),
})

type ApplyComponentsRequest = z.infer<typeof applyComponentsRequestSchema>
type ApplyComponentsResponse = z.infer<typeof applyComponentsResponseSchema>

const updateComponentsRequestSchema = z.object({
  componentsByObjectId: z.record(
    objectIdSchema,
    z.array(z.object({
      componentId: z.string().describe('ID of the component to apply'),
      parameters: componentParametersSchema,
    })).describe('Array of components to apply to this object')
  ).describe('Map of object IDs to array of components to update'),
})

const updateComponentsResponseSchema = z.object({
  updatedComponents: z.record(
    objectIdSchema,
    z.array(z.object({
      componentId: z.string().describe('ID of the updated component'),
      parameters: componentParametersSchema,
    }))
  ).describe('Map of object IDs to successfully updated components'),
})

type UpdateComponentsRequest = z.infer<typeof updateComponentsRequestSchema>
type UpdateComponentsResponse = z.infer<typeof updateComponentsResponseSchema>

const removeComponentsRequestSchema = z.object({
  componentsByObjectId: z.record(
    objectIdSchema,
    z.array(z.string().describe('Component ID to remove'))
  ).describe('Map of object IDs to array of component IDs'),
})

const removeComponentResponseSchema = z.object({
  removedComponents: z.record(
    objectIdSchema,
    z.array(z.string().describe('Component ID that was removed'))
  ).describe('Map of object Ids to array of IDs of component that were removed'),
})

type RemoveComponentsRequest = z.infer<typeof removeComponentsRequestSchema>
type RemoveComponentsResponse = z.infer<typeof removeComponentResponseSchema>

export {
  getAvailableComponentsRequestSchema,
  getAvailableComponentsResponseSchema,
}

export type {
  GetAvailableComponentsRequest,
  GetAvailableComponentsResponse,
  ApplyComponentsRequest,
  ApplyComponentsResponse,
  UpdateComponentsRequest,
  UpdateComponentsResponse,
  RemoveComponentsRequest,
  RemoveComponentsResponse,
}
