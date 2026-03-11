import type {TextToImageBase, ImageBackground} from './base'

type TextToImageStabilityImageCore = TextToImageBase & {
  modelId: 'stability.stable-image-core-v1:1'
  negativePrompt?: string
}

type TextToImageFlux = TextToImageBase & {
  modelId: 'flux-pro-1.1'
}

type TextToImageFluxKontext = TextToImageBase & {
  modelId: 'flux-kontext-pro'
}

type TextToImageGpt = TextToImageBase & {
  modelId: 'gpt-image-1'
  background?: `${ImageBackground}`
}

type FalAiFluxSchnell = TextToImageBase & {
  modelId: 'fal-ai/flux-1/schnell'
}

type TextToImageImagen4Preview = TextToImageBase & {
  modelId: 'fal-ai/imagen4/preview'
}

type TextToImage =
  | TextToImageStabilityImageCore
  | TextToImageFlux
  | TextToImageFluxKontext
  | TextToImageGpt
  | FalAiFluxSchnell
  | TextToImageImagen4Preview

export type {
  TextToImage,
  TextToImageStabilityImageCore,
  TextToImageFlux,
  TextToImageFluxKontext,
  TextToImageGpt,
  FalAiFluxSchnell,
  TextToImageImagen4Preview,
}
