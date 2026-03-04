interface WorldEffectCameraSchema {
  disableWorldTracking: boolean
  enableLighting: boolean
  enableWorldPoints: boolean
  leftHandedAxes: boolean
  mirroredDisplay: boolean
  scale: string
  direction: string
  allowedDevices: string
  enableVps: boolean
}

interface FaceEffectCameraSchema {
  nearClip: number
  farClip: number
  direction: string
  meshGeometryFace: boolean
  meshGeometryEyes: boolean
  meshGeometryIris: boolean
  meshGeometryMouth: boolean
  uvType: string
  maxDetections: number
  enableEars: boolean
  mirroredDisplay: boolean
  allowedDevices: string
}

export type {
  WorldEffectCameraSchema,
  FaceEffectCameraSchema,
}
