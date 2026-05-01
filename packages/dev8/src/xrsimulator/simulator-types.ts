import type {
  SessionManagerInstance,
} from '@repo/reality/app/xr/js/src/types/pipeline'

interface ThreeJsonMetadata {
  type: string
  [key: string]: any
}
interface ThreeJsonObject {
  metadata: ThreeJsonMetadata
  [key: string]: any
}

type LocationVisualization = 'mesh' | 'splat' | 'none'

type SimulatorConfig = {
  // Version number is assigned in increasing order to indicate the most recent config.
  version: number
  simulatorId: string
  cameraUrl?: string
  sequenceUrl?: string
  isPaused?: boolean
  start?: number
  end?: number
  userAgent?: string
  studioPause?: boolean
  poiId?: string
  gpsLatitude?: number
  gpsLongitude?: number
  locationName?: string
  manualVpsEvents?: boolean

  imageTargetName?: string
  imageTargetType?: string
  imageTargetOriginalUrl?: string
  imageTargetMetadata?: string
  imageTargetQuaternion?: [number, number, number, number]

  loadedMesh?: object
  detectedMeshSessionId?: string
  detectedMesh?: ThreeJsonObject
  splatUrl?: string
  splatOffset?: number[]
  visualization?: LocationVisualization

  mockLat?: number
  mockLng?: number
  mockCoordinateValue?: string
}

// TODO(yuhsianghuang): Replace the corresponding fields in SimulatorConfig with this type.
type SimulatorRendererConfig = {
  // Version number is assigned in increasing order to indicate the most recent config.
  version: number
  loadedMesh?: object
  detectedMeshSessionId?: string
  detectedMesh?: ThreeJsonObject
  splatUrl?: string
  splatOffset?: number[]
  visualization?: LocationVisualization
}

type LoopBoundaries = {
  start?: number
  end?: number
}

type SimulatorSessionManager = SessionManagerInstance & {
  updateConfig: (config: SimulatorConfig) => void
  clear: () => void
}

type SequenceControls = {
  scrub: (progress: number) => void
  stopScrub: () => void
}

type LocationEventControls = {
  dispatchLocationScanning: () => void
  dispatchLocationFound: () => void
  dispatchLocationLost: () => void
  dispatchMeshFound: () => void
  dispatchMeshLost: () => void
}

type TargetGeometry = {
  left: number
  top: number
  width: number
  height: number
  isRotated: boolean
  originalWidth: number
  originalHeight: number
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
}

export type {
  LoopBoundaries,
  LocationEventControls,
  SequenceControls,
  SimulatorSessionManager,
  SimulatorConfig,
  SimulatorRendererConfig,
  ThreeJsonObject,
  TargetGeometry,
}
