import type {Eid} from '../../shared/schema'
import type {FaceEffectCameraSchema, WorldEffectCameraSchema} from './xr-camera-pipeline-types'
import type {EcsRenderOverride} from './xr-ecs-types'

type XrManager = {
  createWorldEffect: (config: Partial<WorldEffectCameraSchema>, eid: Eid) => number
  startCameraPipeline: (handle: number) => void
  stopCameraPipeline: (handle: number) => void
  createFaceEffect: (config: Partial<FaceEffectCameraSchema>, eid: Eid) => number
  startMediaRecorder: () => void
  stopMediaRecorder: () => void
  takeScreenshot: () => Promise<Blob>
  drawPausedBackground: () => void
  setEcsRenderOverride: (renderOverride: EcsRenderOverride) => void
  attach: () => void
  detach: () => void
  tick: () => void
  tock: () => void
}

interface FrameInfo {
  elapsedTimeMs: number
  maxRecordingMs: number
  ctx: CanvasRenderingContext2D
  canvas: HTMLCanvasElement
}

interface ProgressInfo {
  progress: number
  total: number
}

export type {
  XrManager, FrameInfo, ProgressInfo,
}
