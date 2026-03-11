import type {ImageToImageBase, ImageBackground} from './base'

type ImageToImageGpt = ImageToImageBase & {
  modelId: 'gpt-image-1'
  background?: `${ImageBackground}`
}

type FalAiFluxKontext = ImageToImageBase & {
  modelId: 'fal-ai/flux-pro/kontext'
}

type ImageToImage =
  | ImageToImageGpt
  | FalAiFluxKontext

export type {
  ImageToImage,
  ImageToImageGpt,
  FalAiFluxKontext,
}
