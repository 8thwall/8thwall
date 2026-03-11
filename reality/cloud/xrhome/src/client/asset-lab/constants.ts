import type {TFunction} from 'i18next'

import type {
  FalAiFluxKontext, ImageToImage, ImageToImageGpt,
} from '../../shared/genai/types/image-to-image'
import type {
  FalAiFluxSchnell,
  TextToImage, TextToImageFlux, TextToImageGpt, TextToImageStabilityImageCore,
} from '../../shared/genai/types/text-to-image'
import type {
  ImageToMesh, ImageToMeshHunyuanBase, ImageToMeshTrellisBase,
} from '../../shared/genai/types/image-to-mesh'
import type {MeshToAnimation} from '../../shared/genai/types/mesh-to-animation'
import type {AssetLabWorkflow} from './asset-lab-context'

const MULTIVEW_COUNT_WITHOUT_FRONT = 3

type ToImage = ImageToImage | TextToImage
type ToImageModelIds = ToImage['modelId']

/* eslint-disable local-rules/hardcoded-copy */
// TODO(dat): Get the full description for these models to appear in the drop down. Will need
// translation then.
const ImageModelToDescription: Partial<Record<ToImageModelIds, string>> = {
  'stability.stable-image-core-v1:1': 'Stable Image Core',
  'flux-pro-1.1': 'Flux Pro 1.1',
  'gpt-image-1': 'GPT Image 1',
  'fal-ai/flux-pro/kontext': 'Flux Pro Kontext',
  'fal-ai/flux-1/schnell': 'Flux.1 Schnell',
}
/* eslint-enable local-rules/hardcoded-copy */

interface ModelDropdownOption {
  modelId: ToImageModelIds | ToMeshModelIds | ToAnimModelIds
  // A string so we can display a range, e.g. 4.5+
  costRange: string
  // Prefix for translation of name and description
  translationKey: string
}

const ImageModelOptions: ModelDropdownOption[] = [
  {
    modelId: 'stability.stable-image-core-v1:1',
    costRange: '2',
    translationKey: 'asset_lab.model.stable_image_core',
  },
  {modelId: 'flux-pro-1.1', costRange: '2', translationKey: 'asset_lab.model.flux_pro_1_1'},
  {
    modelId: 'fal-ai/flux-pro/kontext',
    costRange: '2',
    translationKey: 'asset_lab.model.flux_pro_kontext',
  },
  {
    modelId: 'fal-ai/flux-1/schnell',
    costRange: '0.25',
    translationKey: 'asset_lab.model.flux_1_schnell',
  },
  {
    modelId: 'gpt-image-1',
    costRange: '2.5',
    translationKey: 'asset_lab.model.gpt_image_1',
  },
]

const ImageModelOptionsForAnimation: ModelDropdownOption[] = [{
  modelId: 'gpt-image-1',
  costRange: '2.5',
  translationKey: 'asset_lab.model.gpt_image_1',
}]

type AllToImageKeys = keyof TextToImageStabilityImageCore
  | keyof TextToImageFlux
  | keyof TextToImageGpt
  | keyof TextToImageGpt | keyof ImageToImageGpt
  | keyof FalAiFluxKontext
  | keyof FalAiFluxSchnell

type ModelTypeKeys = {
  'stability.stable-image-core-v1:1': keyof TextToImageStabilityImageCore
  'flux-pro-1.1': keyof TextToImageFlux
  'gpt-image-1': keyof TextToImageGpt | keyof ImageToImageGpt
  'fal-ai/flux-pro/kontext': keyof FalAiFluxKontext
  'fal-ai/flux-1/schnell': keyof FalAiFluxSchnell
}

type MeshModelTypeKeys = {
  'fal-ai/hunyuan3d/v2': keyof ImageToMeshHunyuanBase
  'fal-ai/hunyuan3d-v21': keyof ImageToMeshHunyuanBase
  'fal-ai/trellis': keyof ImageToMeshTrellisBase
}

// Use mapped type to infer the Set type for each modelId automatically
const ModelToParameters: {[K in keyof ModelTypeKeys]: Set<ModelTypeKeys[K]>} = {
  'stability.stable-image-core-v1:1': new Set([
    'prompt', 'style', 'aspectRatio', 'outputFormat', 'negativePrompt',
  ]),
  'flux-pro-1.1': new Set(['prompt', 'style', 'aspectRatio', 'outputFormat']),
  'gpt-image-1': new Set([
    'prompt', 'style', 'aspectRatio', 'outputFormat', 'images', 'background',
  ]),
  'fal-ai/flux-pro/kontext': new Set([
    'prompt', 'style', 'aspectRatio', 'outputFormat', 'images',
  ]),
  'fal-ai/flux-1/schnell': new Set([
    'prompt', 'style', 'aspectRatio', 'outputFormat',
  ]),
}

// These are not parameters you set on the parameter. They control which model we switch to when
// we are about to generate.
const ModelToMeshParametersExtra = {
  'fal-ai/trellis': new Set(['multiview']),
  'fal-ai/hunyuan3d/v2': new Set(['speed', 'multiview']),
  'fal-ai/hunyuan3d/v2/mini': new Set(['speed']),
}

const ModelToMeshParameters: {
  [K in keyof MeshModelTypeKeys]: Set<MeshModelTypeKeys[K]>
} = {
  'fal-ai/hunyuan3d/v2': new Set([
    'numInferenceSteps', 'guidanceScale', 'octreeResolution',
  ]),
  'fal-ai/hunyuan3d-v21': new Set([
    'numInferenceSteps', 'guidanceScale', 'octreeResolution',
  ]),
  'fal-ai/trellis': new Set([
    'ssGuidanceStrength', 'ssSamplingSteps', 'slatGuidanceStrength',
    'slatSamplingSteps', 'meshSimplify', 'textureSize',
  ]),
}

type ModelAttribute = {
  defaultValue: number
  render: boolean
  min?: number
  max?: number
  step?: number
  label?: string
}
const ModelAttributeDefaults: Partial<
Record<keyof ImageToMeshHunyuanBase |
  keyof ImageToMeshTrellisBase, ModelAttribute
>> = {
  'numInferenceSteps': {
    defaultValue: 50, render: false,
  },
  'guidanceScale': {
    defaultValue: 7.5,
    render: true,
    min: 0,
    max: 20,
    step: 0.5,
    label: 'asset_lab.model_gen.parameter.guidance_scale_label',
  },
  'octreeResolution': {
    defaultValue: 256,
    render: true,
    min: 1,
    max: 1024,
    step: 1,
    label: 'asset_lab.model_gen.parameter.shape_detail_label',
  },
  'ssGuidanceStrength': {
    defaultValue: 7.5,
    render: true,
    min: 0,
    max: 10,
    step: 0.1,
    label: 'asset_lab.model_gen.parameter.guidance_strength_label',
  },
  'ssSamplingSteps': {
    defaultValue: 50, render: false,
  },
  'slatGuidanceStrength': {
    defaultValue: 3,
    render: true,
    min: 1,
    max: 10,
    step: 0.1,
    label: 'asset_lab.model_gen.parameter.detail_guidance_label',
  },
  'slatSamplingSteps': {
    defaultValue: 50, render: false,
  },
  'meshSimplify': {
    defaultValue: 0.95,
    render: true,
    min: 0.9,
    max: 0.98,
    step: 0.01,
    label: 'asset_lab.model_gen.parameter.simplify_mesh_label',
  },
}

const modelsThatRequireImages = new Set([
  'fal-ai/flux-pro/kontext',
])

// singleView: workflow allows generating a single view.
// aspectRatio: workflow allows setting the aspect ratio.
// has3d: workflow will produce a 3d model.
// hasAnimation: workflow will produce an animated character.

const WorkflowToParameters: {[K in AssetLabWorkflow]: Set<string>} = {
  'gen-image': new Set(['aspectRatio', 'singleView']),
  'gen-3d-model': new Set(['has3d', 'singleView']),
  'gen-animated-char': new Set(['has3d', 'singleView', 'hasAnimation']),
}

/* eslint-disable local-rules/hardcoded-copy */
// TODO(dat): Get the full description for these models to appear in the drop down. Will need
// translation then.
type ToMeshModelIds = ImageToMesh['modelId']
const MeshModelToDescription = {
  'fal-ai/hunyuan3d/v2': 'Hunyuan 3D v2',
  'fal-ai/hunyuan3d/v2/mini': 'Hunyuan 3D v2 Mini',
  'fal-ai/hunyuan3d-v21': 'Hunyuan 3D v2.1',
  'fal-ai/trellis': 'Trellis',
}
type SelectableToMeshModelIds = keyof typeof MeshModelToDescription
const MeshModelToMulti: Record<keyof typeof MeshModelToDescription, ToMeshModelIds> = {
  'fal-ai/hunyuan3d/v2': 'fal-ai/hunyuan3d/v2/multi-view',
  'fal-ai/hunyuan3d/v2/mini': 'fal-ai/hunyuan3d/v2/mini',  // No multi-view for mini
  'fal-ai/hunyuan3d-v21': 'fal-ai/hunyuan3d-v21',  // No multi-view for v2.1
  'fal-ai/trellis': 'fal-ai/trellis/multi',
}

const MeshModelToDescriptionExtended = {
  ...MeshModelToDescription,
  'fal-ai/hunyuan3d/v2/multi-view': 'Hunyuan 3D v2 Multi-View',
  'fal-ai/hunyuan3d/v2/multi-view/turbo': 'Hunyuan 3D v2 Multi-View Turbo',
  'fal-ai/hunyuan3d/v2/mini': 'Hunyuan 3D v2 Mini',
  'fal-ai/hunyuan3d/v2/mini/turbo': 'Hunyuan 3D v2 Mini Turbo',
  'fal-ai/trellis/multi': 'Trellis Multi-View',
  'fal-ai/trellis/multi/turbo': 'Trellis Multi-View Turbo',
}
/* eslint-enable local-rules/hardcoded-copy */

const MeshModelOptions: ModelDropdownOption[] = [
  {modelId: 'fal-ai/trellis', costRange: '2', translationKey: 'asset_lab.model.trellis'},
  {
    modelId: 'fal-ai/hunyuan3d/v2',
    costRange: '4.5+',
    translationKey: 'asset_lab.model.hunyuan_3d_v2',
  },
  {
    modelId: 'fal-ai/hunyuan3d/v2/mini',
    costRange: '24+',
    translationKey: 'asset_lab.model.hunyuan_3d_v2_mini',
  },
  {
    modelId: 'fal-ai/hunyuan3d-v21',
    costRange: '90',
    translationKey: 'asset_lab.model.hunyuan_3d_v2_1',
  },
]

/* eslint-disable local-rules/hardcoded-copy */
// TODO(dat): Get the full description for these models to appear in the drop down. Will need
// translation then.
type ToAnimModelIds = MeshToAnimation['modelId']
const AnimModelToDescription = {
  'meshy-ai': 'Meshy AI',
}
/* eslint-enable local-rules/hardcoded-copy */
const AnimModelOptions: ModelDropdownOption[] = [
  {
    modelId: 'meshy-ai',
    costRange: '24',
    translationKey: 'asset_lab.model.meshy_ai',
  },
]

const AllModelToDescription = {
  ...ImageModelToDescription,
  ...MeshModelToDescriptionExtended,
  ...AnimModelToDescription,
}
type Options = {value: string, content: string}[]
const WorkflowDropdownOptions: Options = [
  {value: 'gen-image', content: 'asset_lab.workflow.image'},
  {value: 'gen-3d-model', content: 'asset_lab.workflow.model'},
  {value: 'gen-animated-char', content: 'asset_lab.workflow.animation'},
]

const translateContent = (options: Options, t: TFunction): Options => options.map((option) => {
  option.content = t(option.content)
  return option
})

const GENERATE_MODES = ['image', 'model', 'animation']

// eslint-disable-next-line local-rules/hardcoded-copy
const DEFAULT_ANIMATION_CLIP = 'Walk'

const ASSET_OPTIMIZER_TEXTURE_SIZES = [128, 256, 512, 1024] as const
type OptimizerTextureSizes = typeof ASSET_OPTIMIZER_TEXTURE_SIZES[number]

export {
  ASSET_OPTIMIZER_TEXTURE_SIZES,
  ImageModelToDescription,
  ImageModelOptions,
  MeshModelToDescription,
  MeshModelToMulti,
  MeshModelOptions,
  AnimModelToDescription,
  AnimModelOptions,
  WorkflowDropdownOptions,
  translateContent,
  GENERATE_MODES,
  ModelToParameters,
  ModelToMeshParameters,
  WorkflowToParameters,
  MULTIVEW_COUNT_WITHOUT_FRONT,
  AllModelToDescription,
  ModelAttributeDefaults,
  ModelToMeshParametersExtra,
  modelsThatRequireImages,
  ImageModelOptionsForAnimation,
  DEFAULT_ANIMATION_CLIP,
}

export type {
  ToImageModelIds,
  ToMeshModelIds,
  ToAnimModelIds,
  OptimizerTextureSizes,
  AllToImageKeys,
  ModelDropdownOption,
  ModelAttribute,
  SelectableToMeshModelIds,
}
