type ReferencedResources = {
  originalImage: string
  croppedImage: string
  thumbnailImage: string
  luminanceImage: string
  geometryImage?: string
}

type ImageTargetData = {
  imagePath: string
  metadata: unknown  // User metadata
  name: string
  // NOTE(christoph): The "resources" key was added partway during the export window, so not
  // all projects will contain them
  resources?: ReferencedResources
  created: number
  updated: number
} & CropResult

interface CropGeometry {
  top: number
  left: number
  width: number
  height: number
  isRotated?: boolean
  originalWidth: number
  originalHeight: number
}

type CylinderCropGeometry = CropGeometry & {
  targetCircumferenceTop: number
  cylinderSideLength: number
  cylinderCircumferenceTop: number
  cylinderCircumferenceBottom: number
  arcAngle: number
  coniness: number
  inputMode: 'ADVANCED' | 'BASIC'
  unit: 'mm' | 'in'
}

type ConicalCropGeometry = CylinderCropGeometry & {
  topRadius: number
  bottomRadius: number
}

type PlanarCropResult = {
  type: 'PLANAR'
  properties: CropGeometry
}

type CylinderCropResult = {
  type: 'CYLINDER'
  properties: CylinderCropGeometry
}

type ConicalCropResult = {
  type: 'CONICAL'
  properties: ConicalCropGeometry
}

type CropResult =
  | PlanarCropResult
  | CylinderCropResult
  | ConicalCropResult

const LIST_PATH = '/list'
const TEXTURE_PATH = '/texture'
const TARGET_PATH = '/target'
const UPLOAD_PATH = '/upload'

type ListTargetsParams = {
  appKey: string
}

type ListTargetsResponse = {
  targets: ImageTargetData[]
  invalidPaths: string[]
}

type TargetTextureType = 'original' | 'cropped' | 'geometry' | 'thumbnail' | 'luminance'

type GetTargetTextureParams = {
  appKey: string
  name: string
  type: TargetTextureType
  v?: string  // Optional, to bust the cache
}

type UploadTargetParams = {
  appKey: string
  name: string
  crop: string  // JSON encoded CropResult
}

type UploadTargetResponse = {
  target: ImageTargetData
}

type UpdateTargetParams = {
  appKey: string
  name: string
}

export {
  LIST_PATH,
  TEXTURE_PATH,
  TARGET_PATH,
  UPLOAD_PATH,
}

export type {
  ImageTargetData,
  ListTargetsParams,
  ListTargetsResponse,
  GetTargetTextureParams,
  TargetTextureType,
  UploadTargetParams,
  UpdateTargetParams,
  UploadTargetResponse,
  CropResult,
}
