import type {DeepReadonly} from 'ts-essentials'

import type {ImageToImage} from './types/image-to-image'
import type {ImageToMesh} from './types/image-to-mesh'
import type {MeshToAnimation} from './types/mesh-to-animation'
import type {TextToImage} from './types/text-to-image'
import type {GenerateRequest} from './types/generate'

type PriceOptions = {basePrice: number}
type ToImage = TextToImage | ImageToImage
type ToImagePrices = Record<ToImage['modelId'], PriceOptions>
type ImageToMeshPrices = Record<ImageToMesh['modelId'], PriceOptions>
type MeshToAnimationPrices = Record<MeshToAnimation['modelId'], PriceOptions>

type WorkflowPrices = DeepReadonly<{
  TO_IMAGE: ToImagePrices
  IMAGE_TO_MESH: ImageToMeshPrices
  MESH_TO_ANIMATION: MeshToAnimationPrices
}>

const AI_MODEL_COST: WorkflowPrices = {
  TO_IMAGE: {
    'stability.stable-image-core-v1:1': {basePrice: 2},
    'flux-pro-1.1': {basePrice: 2},
    'flux-kontext-pro': {basePrice: 2},
    'gpt-image-1': {basePrice: 2.5},
    'fal-ai/flux-1/schnell': {basePrice: 0.25},
    'fal-ai/flux-pro/kontext': {basePrice: 2},
    'fal-ai/imagen4/preview': {basePrice: 2},
  },
  IMAGE_TO_MESH: {
    'fal-ai/trellis': {basePrice: 2},
    'fal-ai/trellis/multi': {basePrice: 2},
    'fal-ai/hunyuan3d/v2': {basePrice: 48},
    'fal-ai/hunyuan3d/v2/turbo': {basePrice: 42},
    'fal-ai/hunyuan3d/v2/multi-view': {basePrice: 5},
    'fal-ai/hunyuan3d/v2/multi-view/turbo': {basePrice: 4.5},
    'fal-ai/hunyuan3d/v2/mini': {basePrice: 30},
    'fal-ai/hunyuan3d/v2/mini/turbo': {basePrice: 24},
    'fal-ai/hunyuan3d-v21': {basePrice: 90},
  },
  MESH_TO_ANIMATION: {
    'meshy-ai': {basePrice: 24},
  },
}

const getWorkflowStepPrice = (
  workflowStep: {type: keyof WorkflowPrices, modelId: string}
): number | null => {
  // It's possible to be in the wrong workflow that does not have your model
  let price = null
  switch (workflowStep.type) {
    case 'TO_IMAGE':
      price = AI_MODEL_COST.TO_IMAGE[workflowStep.modelId]?.basePrice
      break
    case 'IMAGE_TO_MESH':
      price = AI_MODEL_COST.IMAGE_TO_MESH[workflowStep.modelId]?.basePrice
      break
    case 'MESH_TO_ANIMATION':
      price = AI_MODEL_COST.MESH_TO_ANIMATION[workflowStep.modelId]?.basePrice
      break
    default:
      throw new Error('Unexpected generate request type')
  }
  return price
}

const calcGeneratePrice = (
  type: keyof WorkflowPrices, modelId: GenerateRequest['modelId'], multiplier: number
): number | null => getWorkflowStepPrice({type, modelId}) * multiplier

export {
  AI_MODEL_COST,
  getWorkflowStepPrice,
  calcGeneratePrice,
}

export type {
  WorkflowPrices,
}
