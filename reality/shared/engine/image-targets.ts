// @visibility(//visibility:public)

// xrhomeName is the value stored in the database
// apiName is the value we return to customers
const ImageTargetType = {
  PLANAR: {xrhomeName: 'PLANAR', apiName: 'FLAT'},
  CYLINDER: {xrhomeName: 'CYLINDER', apiName: 'CYLINDRICAL'},
  CONICAL: {xrhomeName: 'CONICAL', apiName: 'CONICAL'},
  UNSPECIFIED: {xrhomeName: 'UNSPECIFIED', apiName: 'UNSPECIFIED'},
} as const

type TargetMetadata = {
  left?: number
  top?: number
  width?: number
  height?: number
  isRotated?: boolean
  originalWidth?: number
  originalHeight?: number
  topRadius?: number
  bottomRadius?: number
  cylinderSideLength?: number
  cylinderCircumferenceTop?: number
  targetCircumferenceTop?: number
  cylinderCircumferenceBottom?: number
  arcAngle?: number
  coniness?: number
  inputMode?: 'BASIC' | 'ADVANCED'
  unit?: 'mm' | 'in'
  physicalWidthInMeters?: number
  moveable?: boolean
  staticOrientation?: {
    rollAngle: number
    pitchAngle: number
  }
}

// Image target data passed into the engine to set active.
type ImageTargetData = {
  imagePath: string
  metadata?: any
  moveable: boolean
  name: string
  physicalWidthInMeters: number | null
  type: 'PLANAR' | 'CYLINDER' | 'CONICAL'
  properties: TargetMetadata
  userMetadataIsJson?: boolean
}

type ImageTargetDataCodeExport = ImageTargetData & {
  loadAutomatically: boolean
  created: number
  updated: number
}

type ProcessedImageTargetData = {
  name: string
  type: 'FLAT' | 'CYLINDRICAL' | 'CONICAL' | 'UNSPECIFIED'
  metadata: unknown
  properties: TargetMetadata
  geometry?: Partial<{
    // If curved
    height: number
    radiusTop: number
    radiusBottom: number
    arcLengthRadians: number
    arcStartRadians: number
    // If flat
    scaledWidth: number
    scaledHeight: number
  }>
}

type DetectedImageTarget = ProcessedImageTargetData['geometry'] &{
  name: string
  metadata: unknown
  position: {
    x: number
    y: number
    z: number
  }
  rotation: {
    w: number
    x: number
    y: number
    z: number
  }
  scale: number
  type: 'FLAT' | 'CYLINDRICAL' | 'CONICAL' | 'UNSPECIFIED'
  properties: {
    width: number
    height: number
    top: number
    left: number
    originalWidth: number
    originalHeight: number
    isRotated: boolean
    staticOrientation?: {
      rollAngle: number
      pitchAngle: number
    }
  }
}

export {
  ImageTargetType,
  TargetMetadata,
  ImageTargetData,
  ProcessedImageTargetData,
  DetectedImageTarget,
  ImageTargetDataCodeExport,
}
