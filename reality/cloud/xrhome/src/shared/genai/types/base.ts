// NOTE(kyle): We are unable to generate these types using Zod's z.infer until xrhome is configured
// for strict mode. Without strict mode, zod makes every type optional. The workaround to use Zod's
// parsing but keep the non-optional types intact is to define the types manually and cast.

enum GenerateType {
  TEXT_TO_IMAGE = 'TEXT_TO_IMAGE',
  IMAGE_TO_IMAGE = 'IMAGE_TO_IMAGE',
  IMAGE_TO_MESH = 'IMAGE_TO_MESH',
  MESH_TO_ANIMATION = 'MESH_TO_ANIMATION',
}

enum ImageStyle {
  LOGO = 'logo',
  TILING = 'tiling',
  GAME_ASSET = 'game_asset',
  ANIMATED_GAME_ASSET = 'animated_game_asset',
  MULTIVIEW = 'multiview',
  SINGLEVIEW = 'singleview',
  ANIMATED_SINGLEVIEW = 'animated_singleview',
  ANIMATED_MULTIVIEW = 'animated_multiview',
}

enum ImageAspectRatio {
  // Square
  RATIO_1_1 = '1:1',
  // Portrait
  RATIO_2_3 = '2:3',
  RATIO_3_4 = '3:4',
  RATIO_4_5 = '4:5',
  RATIO_9_16 = '9:16',
  RATIO_9_21 = '9:21',
  // Landscape
  RATIO_3_2 = '3:2',
  RATIO_4_3 = '4:3',
  RATIO_5_4 = '5:4',
  RATIO_16_9 = '16:9',
  RATIO_21_9 = '21:9',
}

enum Background {
  OPAQUE = 'opaque',
  TRANSPARENT = 'transparent',
}

enum OutputFormat {
  PNG = 'png',
  JPEG = 'jpeg',
}

enum ImageBackground {
  TRANSPARENT = 'transparent',
  OPAQUE = 'opaque',
}

enum RerollOrientation {
  LEFT = 'left',
  BACK = 'back',
  RIGHT = 'right',
}

type UuidAssetInput = {
  type: 'uuid'
  value: string
}

type FileAssetInput = {
  type: 'file'
  value: File
}

type AssetInput = UuidAssetInput | FileAssetInput

type Base = {
  numRequests?: number
}

type ToImage = Base & {
  prompt: string
  style?: `${ImageStyle}`
  aspectRatio?: `${ImageAspectRatio}`
  outputFormat?: `${OutputFormat}`
  background?: `${Background}`
}

type TextToImageBase = ToImage & {
  type: `${GenerateType.TEXT_TO_IMAGE}`
}

type ImageToImageBase = ToImage & {
  type: `${GenerateType.IMAGE_TO_IMAGE}`
  images: AssetInput[]
  ParentRequestUuid?: string
  // Used for re-rolling BLR orientations.
  orientation?: `${RerollOrientation}`
}

type ImageToMeshBase = Base & {
  type: `${GenerateType.IMAGE_TO_MESH}`
  images: AssetInput[]
  ParentRequestUuid?: string
}

type MeshToAnimationBase = Base & {
  type: `${GenerateType.MESH_TO_ANIMATION}`
  mesh: AssetInput
  ParentRequestUuid?: string
}

export {
  GenerateType,
  ImageStyle,
  ImageAspectRatio,
  OutputFormat,
  ImageBackground,
  RerollOrientation,
}

export type {
  AssetInput,
  UuidAssetInput,
  FileAssetInput,
  Background,
  ToImage,
  TextToImageBase,
  ImageToImageBase,
  ImageToMeshBase,
  MeshToAnimationBase,
}
