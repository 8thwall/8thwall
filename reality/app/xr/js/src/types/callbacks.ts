import type {ImageTargetData} from '@repo/reality/shared/engine/image-targets'

import type * as Pipeline from './pipeline'
import type {CameraStatusChangeDetails} from './pipeline'

/* eslint-disable import/group-exports */

// NOTE(christoph): Most callbacks are called with runCallbacks, which adds a "framework" into the
// input.
export type WithFramework<T extends {}> = T & {
  framework: Pipeline.FrameworkHandle
}

export type WithoutFramework<T> = Omit<T, 'framework'>

export type BeforeRunInput = {
  config: Pipeline.RunConfig
}

export type BeforeRunHandler = (input: BeforeRunInput) => void | Promise<void>

export type RunConfigureInput = {
  config: Pipeline.RunConfig
}

export type RunConfigureHandler = (input: RunConfigureInput) => void

export type BeforeSessionInitializeInput = {
  config: Pipeline.RunConfig
  sessionAttributes: Pipeline.SessionAttributes
}

export type BeforeSessionInitializeHandler = (input: BeforeSessionInitializeInput) => void

export type StartInput = {
  canvas: HTMLCanvasElement
  GLctx: Pipeline.RenderContext
  computeCtx: Pipeline.RenderContext
  isWebgl2: boolean
  orientation: number
  videoWidth: number
  videoHeight: number
  canvasWidth: number
  canvasHeight: number
  rotation: number
  config: Pipeline.RunConfig
}

export type StartHandler = (input: StartInput) => void

export type ProcessGpuHandler = (input: WithFramework<Pipeline.ProcessGpuInput>) => unknown

export type ProcessCpuHandler = (input: WithFramework<Pipeline.ProcessCpuInput>) => unknown

export type UpdateHandler = (input: WithFramework<Pipeline.ExtendedUpdateInput>) => unknown

export type RenderHandler = () => void

export type PausedHandler = () => void

export type ResumeHandler = () => void

export type ExceptionInput = Error | unknown

export type ExceptionHandler = (input: ExceptionInput) => void

export type DeviceOrientationChangeInput = {
  GLctx: Pipeline.RenderContext
  computeCtx: Pipeline.RenderContext
  videoWidth: number
  videoHeight: number
  orientation: number
}

export type DeviceOrientationChangeHandler = (input: DeviceOrientationChangeInput) => void

export type CanvasSizeChangeInput = {
  GLctx: Pipeline.RenderContext
  computeCtx: Pipeline.RenderContext
  videoWidth: number
  videoHeight: number
  canvasWidth: number
  canvasHeight: number
}

export type CanvasSizeChangeHandler = (input: CanvasSizeChangeInput) => void

export type VideoSizeChangeInput = {
  GLctx: Pipeline.RenderContext
  computeCtx: Pipeline.RenderContext
  videoWidth: number
  videoHeight: number
  canvasWidth: number
  canvasHeight: number
  orientation: number
}

export type VideoSizeChangeHandler = (input: VideoSizeChangeInput) => void

export type SessionType = 'immersive-ar' | 'immersive-vr'

// TODO(dat): rendersOpaque, session are not always available. It depends on the value of status.
// We might want to change the type of AttachInput to use has types for the successful cases.
// As onAttach will not be called if the session fails to start.
export type CameraStatusChangeInput = WithFramework<CameraStatusChangeDetails> & {
  status: string
  rendersOpaque?: boolean
  sessionType?: SessionType
  session?: unknown
} & Pipeline.RunConfig

export type CameraStatusChangeHandler = (input: CameraStatusChangeInput) => void

export type AppResourcesLoadedInput = WithFramework<{
  xrSessionToken?: string
  version?: string
  imageTargets?: ImageTargetData[]
  coverImageUrl?: string
  smallCoverImageUrl?: string
  mediumCoverImageUrl?: string
  largeCoverImageUrl?: string
  shortLink?: string
  channel?: string
  sampleRatio?: number
}>

export type AppResourcesLoadedHandler = (input: AppResourcesLoadedInput) => void

// NOTE(christoph): onAttach gets called with basically all the data that's been passed to any
// other event up to this point
export type AttachInput = WithFramework<(
  BeforeSessionInitializeInput &
  CameraStatusChangeInput &
  StartInput &
  DeviceOrientationChangeInput &
  CanvasSizeChangeInput &
  VideoSizeChangeInput &
  Partial<AppResourcesLoadedInput>  // onAppResourcesLoaded is not always called before onAttach
)>

export type AttachHandler = (input: AttachInput) => void

export type DetachHandler = (input: WithFramework<{}>) => void

export type RemoveHandler = (input: WithFramework<{}>) => void

export type SessionManagerHandler = () => Pipeline.SessionManagerInstance

export type CallbackNameToHandler = {
  onBeforeRun: BeforeRunHandler
  onRunConfigure: RunConfigureHandler
  onBeforeSessionInitialize: BeforeSessionInitializeHandler
  onStart: StartHandler
  onProcessGpu: ProcessGpuHandler
  onProcessCpu: ProcessCpuHandler
  onUpdate: UpdateHandler
  onRender: RenderHandler
  onPaused: PausedHandler
  onResume: ResumeHandler
  onException: ExceptionHandler
  onDeviceOrientationChange: DeviceOrientationChangeHandler
  onCanvasSizeChange: CanvasSizeChangeHandler
  onVideoSizeChange: VideoSizeChangeHandler
  onCameraStatusChange: CameraStatusChangeHandler
  onAppResourcesLoaded: AppResourcesLoadedHandler
  onAttach: AttachHandler
  onDetach: DetachHandler
  onSessionAttach: AttachHandler
  onSessionDetach: DetachHandler
  onRemove: RemoveHandler
  sessionManager: SessionManagerHandler
}

export type CallbackName = keyof CallbackNameToHandler

export type InputForCallbackName<T extends CallbackName> = Parameters<CallbackNameToHandler[T]>[0]
