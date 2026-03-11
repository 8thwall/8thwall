import type {DeepReadonly} from 'ts-essentials'

import type {Vec3Tuple} from '@ecs/shared/scene-graph'

import type {AssetGeneration} from '../common/types/db'
import type {
  AL_GET_ASSET_GENERATIONS, AL_GET_ASSET_REQUESTS, AL_UPDATE_ASSET_GENERATION_METADATA,
} from './action-types'
import type {IAssetRequest} from '../common/types/models'
import type {OptimizerTextureSizes} from './constants'

type RequestsByType = {
  image: string[]
  model: string[]
  animation: string[]
}

type AssetGenerationWithCost = AssetGeneration & {
  totalActionQuantity: number
}

type MutableAssetLabState = {
  assetGenerations: Record<string, AssetGenerationWithCost>
  assetGenerationsByAccount: Record<string, string[]>
  assetGenerationsByAssetRequest: Record<string, string[]>
  assetRequests: Record<string, IAssetRequest>
  assetRequestsByAccount: Record<string, string[]>
  assetRequestsByTypeByAccount: Record<string, RequestsByType>
}

type AssetLabState = DeepReadonly<MutableAssetLabState>

type GetAssetGenerationsAction = {
  type: typeof AL_GET_ASSET_GENERATIONS
  assetGenerations: AssetGenerationWithCost[]
  accountUuid: string
}
type GetAssetRequestsAction = {
  type: typeof AL_GET_ASSET_REQUESTS
  assetRequests: IAssetRequest[]
  accountUuid: string
}

type UpdateAssetGenerationMetadataAction = {
  type: typeof AL_UPDATE_ASSET_GENERATION_METADATA
  assetGenerationUuid: string
  updatedMetadata: Partial<AssetGeneration['metadata']>
}

type PivotPointOptions = 'center' | 'right' | 'left' | 'top' | 'bottom' | 'front' | 'back'

type PostProcessingSettings = {
  enableDraco: boolean
  enableOptimizations: boolean
  enableZip: boolean
  enableSimplify: boolean
  simplifyRatio: number
  enableTextureResize: boolean
  textureSize: OptimizerTextureSizes
  enableTextureBrightness: boolean
  textureBrightness: number
  enablePBR: boolean
  metalness: number
  roughness: number
  enableSmoothShading: boolean
}

// When AssetGeneration.assetType === 'MESH'
type AssetGenerationMeshMetadata = {
  optimizer?: {
    completed?: boolean
    settings?: PostProcessingSettings
    transform?: {
      position: Vec3Tuple
      rotation: Vec3Tuple
      scale: Vec3Tuple
    }
    timestampMs?: number
  }
}
interface AssetGenerationMesh extends AssetGeneration {
  type: 'MESH'
  metadata: AssetGenerationMeshMetadata
}

const isAssetGenerationMesh = (
  ag: AssetGeneration
): ag is AssetGenerationMesh => ag.assetType === 'MESH'

type FblrAsset = string | File

export {isAssetGenerationMesh}

export type {
  AssetGenerationMesh,
  AssetGenerationMeshMetadata,
  AssetGenerationWithCost,
  AssetLabState,
  FblrAsset,
  GetAssetGenerationsAction,
  GetAssetRequestsAction,
  MutableAssetLabState,
  PivotPointOptions,
  PostProcessingSettings,
  RequestsByType,
  UpdateAssetGenerationMetadataAction,
  OptimizerTextureSizes,
}
