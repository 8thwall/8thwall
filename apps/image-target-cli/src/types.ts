interface ImageMetadata {
  width: number
  height: number
}

type ReferencedResources = {
  originalImage: string
  croppedImage: string
  thumbnailImage: string
  luminanceImage: string
  geometryImage?: string
}

type ImageTargetData = CropResult & {
  imagePath: string
  metadata: null
  name: string
  resources: ReferencedResources
  created: number
  updated: number
}
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
  inputMode: 'ADVANCED'
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

interface CliInterface {
  prompt(question: string): Promise<string>
  choose<T extends string>(
    question: string,
    options: T[],
    firstIsDefault?: boolean,
  ): Promise<T>
  confirm(question: string, defaultValue?: boolean): Promise<boolean>
  close(): void
  promptInteger(question: string): Promise<number>
  promptFloat(question: string): Promise<number>
}

type Point = {
  x: number
  y: number
}

type Color = [number, number, number]

type ConePixelPoints = {
  tl: Point
  bl: Point
  tr: Point
  apex: Point
  theta?: number
  isFez?: boolean
}

type UnconifiedData = {
  topRadius: number
  bottomRadius: number
  theta: number
  outputHeight: number
}

export type {
  CliInterface,
  CropGeometry,
  CropResult,
  CylinderCropGeometry,
  CylinderCropResult,
  ImageMetadata,
  ImageTargetData,
  ReferencedResources,
  Point,
  Color,
  ConePixelPoints,
  ConicalCropResult,
  PlanarCropResult,
  UnconifiedData,
}
