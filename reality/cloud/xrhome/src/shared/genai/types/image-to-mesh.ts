import type {ImageToMeshBase} from './base'

type TrellisTextureSize = 512 | 1024 | 2048
type TrellisMultiImageAlgo = 'stochastic' | 'multidiffusion'

type ImageToMeshTrellisBase = ImageToMeshBase & {
  ssGuidanceStrength?: number
  ssSamplingSteps?: number
  slatGuidanceStrength?: number
  slatSamplingSteps?: number
  meshSimplify?: number
  textureSize?: TrellisTextureSize
}

type ImageToMeshTrellis = ImageToMeshTrellisBase & {
  modelId: 'fal-ai/trellis'
}

type ImageToMeshTrellisMulti = ImageToMeshTrellisBase & {
  modelId: 'fal-ai/trellis/multi'
  // NOTE(kyle): multiimage is not a typo.
  multiimageAlgo?: TrellisMultiImageAlgo
}

type ImageToMeshHunyuanBase = ImageToMeshBase & {
  numInferenceSteps?: number
  guidanceScale?: number
  octreeResolution?: number
  texturedMesh?: boolean
}

type ImageToMeshHunyuan = ImageToMeshHunyuanBase & {
  modelId: 'fal-ai/hunyuan3d/v2'
}

type ImageToMeshHunyuanTurbo = ImageToMeshHunyuanBase & {
  modelId: 'fal-ai/hunyuan3d/v2/turbo'
}

type ImageToMeshHunyuanMulti = ImageToMeshHunyuanBase & {
  modelId: 'fal-ai/hunyuan3d/v2/multi-view'
}

type ImageToMeshHunyuanMultiTurbo = ImageToMeshHunyuanBase & {
  modelId: 'fal-ai/hunyuan3d/v2/multi-view/turbo'
}

type ImageToMeshHunyuanMini = ImageToMeshHunyuanBase & {
  modelId: 'fal-ai/hunyuan3d/v2/mini'
}

type ImageToMeshHunyuanMiniTurbo = ImageToMeshHunyuanBase & {
  modelId: 'fal-ai/hunyuan3d/v2/mini/turbo'
}

type ImageToMeshHunyuanV21 = ImageToMeshHunyuanBase & {
  modelId: 'fal-ai/hunyuan3d-v21'
}

type ImageToMesh =
  | ImageToMeshTrellis
  | ImageToMeshTrellisMulti
  | ImageToMeshHunyuan
  | ImageToMeshHunyuanTurbo
  | ImageToMeshHunyuanMulti
  | ImageToMeshHunyuanMultiTurbo
  | ImageToMeshHunyuanMini
  | ImageToMeshHunyuanMiniTurbo
  | ImageToMeshHunyuanV21

export type {
  ImageToMesh,
  ImageToMeshTrellis,
  ImageToMeshTrellisMulti,
  ImageToMeshHunyuan,
  ImageToMeshHunyuanTurbo,
  ImageToMeshHunyuanMulti,
  ImageToMeshHunyuanMultiTurbo,
  ImageToMeshHunyuanMini,
  ImageToMeshHunyuanMiniTurbo,
  ImageToMeshHunyuanV21,
  ImageToMeshHunyuanBase,
  ImageToMeshTrellisBase,
  TrellisTextureSize,
}
