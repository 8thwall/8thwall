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

type ImageTargetData = {
  imagePath: string
  metadata: null
  name: string
  type: 'PLANAR' | 'CYLINDER' | 'CONICAL'
  properties: CropGeometry | CylinderCropGeometry
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

 type PlanarCropResult = {
   type: 'PLANAR'
   geometry: CropGeometry
 }
 type CylinderCropResult = {
   type: 'CYLINDER'
   geometry: CylinderCropGeometry
 }
 type ConicalCropResult = {
   type: 'CONICAL'
   geometry: CropGeometry
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

export type {
  CliInterface,
  CropGeometry,
  CropResult,
  CylinderCropGeometry,
  CylinderCropResult,
  ImageMetadata,
  ImageTargetData,
  ReferencedResources,
}
