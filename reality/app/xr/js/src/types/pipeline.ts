import type {CameraDirection, XrPermission} from './common'
import type {EmscriptenTexture} from './emscripten'
import type {EnvironmentUpdate} from './environment-update'

/* eslint-disable import/group-exports */

export type ModuleName = string

export type StepResult = Record<ModuleName, unknown>

export type SessionFrameStartResult = Record<string, unknown>

export type RenderContext = WebGL2RenderingContext | WebGLRenderingContext

// See https://emscripten.org/docs/api_reference/html5.h.html#id91
export type EmscriptenWebGLContextAttributes = WebGLContextAttributes & {
  majorVersion?: number
  enableExtensionsByDefault?: boolean
}

export type FrameStartResult = {
  cameraTexture: EmscriptenTexture
  computeTexture: EmscriptenTexture
  GLctx: RenderContext
  computeCtx: RenderContext
  orientation: number
  repeatFrame: boolean
  textureWidth?: number
  textureHeight?: number
  videoTime?: number
  frameTime: number
} & SessionFrameStartResult

export type ProcessGpuInput = {
  framework?: FrameworkHandle
  frameStartResult: FrameStartResult
}

export type ProcessCpuInput = ProcessGpuInput & {
  processGpuResult: StepResult
  cameraTextureReadyResult?: StepResult  // Deprecated
}

export type UpdateInput = ProcessCpuInput & {
  processCpuResult: StepResult
}

export type ExtendedUpdateInput = UpdateInput & {
  fps: number
  GLctx: RenderContext
  cameraTexture: WebGLTexture | null
  video: null
  cameraTextureReadyResult: StepResult  // Deprecated
  heapDataReadyResult: StepResult  // Deprecated
}

export type onFrameFinished = UpdateInput & {
  updateResults: StepResult
}

export type onAttachInput = EnvironmentUpdate & {
  GLctx?: WebGL2RenderingContext
  computeCtx: WebGLRenderingContext
  framework: FrameworkHandle
}

export type PipelineState = {
  processGpuInput?: ProcessGpuInput
  processCpuInput?: ProcessCpuInput
  updateInput?: UpdateInput
  heldForRelease?: Array<EmscriptenTexture['name']>
  onFrameFinished?: onFrameFinished
}

export type CameraConfig = {
  direction: CameraDirection
}

export type DefaultEnvironmentConfiguration = {
  disabled?: boolean
  floorScale?: number
  floorTexture?: string
  floorColor?: string
  fogIntensity?: number
  skyTopColor?: string
  skyBottomColor?: string
  skyGradientStrength?: number
}

export type SessionConfiguration = {
  disableDesktopCameraControls?: boolean
  disableDesktopTouchEmulation?: boolean
  disableCameraReparenting?: boolean
  disableXrTablet?: boolean
  disableXrTouchEmulation?: boolean
  xrTabletStartsMinimized?: boolean
  defaultEnvironment?: DefaultEnvironmentConfiguration
}

export type RunConfig = {
  verbose?: boolean
  webgl2?: boolean
  ownRunLoop?: boolean
  cameraConfig?: CameraConfig
  glContextConfig?: {}
  allowedDevices?: string
  sessionConfiguration?: SessionConfiguration
  // Default session init behavior is 'error'
  sessionInitBehavior?: 'error' | 'fallback'
  paused?: boolean
  canvas?: HTMLCanvasElement
  /**
  * @deprecated
  */
  allowFront?: boolean
  /**
  * @deprecated
  */
  render?: boolean

}

export type SessionAttributes = Record<string, unknown> & Partial<{
  providesRunLoop: boolean
  fillsCameraTexture: boolean
  controlsCamera: boolean
}>

export type CameraStatusChangeDetailsFailReason = 'UNSPECIFIED' | 'DENY_MICROPHONE' |
  'NO_MICROPHONE' | 'DENY_CAMERA' | 'NO_CAMERA' | 'NO_CAMERA_URL_SRC' | 'NO_LOCATION_SRC'
export type CameraStatusChangeDetails =
  {
    status: 'requesting'
    rendersOpaque: boolean
    cameraConfig: CameraConfig
    config: RunConfig
  } | {
    status: 'hasStream'
    stream: MediaStream
    rendersOpaque: boolean
  } | {
    status: 'hasVideo'
    video: HTMLVideoElement
    rendersOpaque: boolean
  } | {
    status: 'failed'
    reason: CameraStatusChangeDetailsFailReason
    config: RunConfig
  } | {
    status: 'hasDesktop3D'
    rendersOpaque: boolean
  } | {
    status: 'requestingWebXr'
    // NOTE(dat): Is there another type called inline?
    sessionType: 'immersive-vr' | 'immersive-ar'
    // TODO(dat): Find the WebXR session init type
    sessionInit: unknown
  } | {
    status: 'hasSessionWebXr'
    // NOTE(dat): Is there another type called inline?
    sessionType: 'immersive-vr' | 'immersive-ar'
    // TODO(dat): Find the WebXR session init type
    sessionInit: unknown
    // TODO(dat): find WebXR session type
    session: unknown
    rendersOpaque: boolean
  } | {
    status: 'hasContentWebXr'
    // NOTE(dat): Is there another type called inline?
    sessionType: 'immersive-vr' | 'immersive-ar'
    // TODO(dat): Find the WebXR session init type
    sessionInit: unknown
    // TODO(dat): find WebXR session type
    session: unknown
    rendersOpaque: boolean
  }

export type onCameraStatusChangeInput = {
  status: string
  video: HTMLVideoElement
  cameraConfig: CameraConfig
}

export type RunOnCameraStatusChangeCb = (data: CameraStatusChangeDetails) => void

export type SessionManagerInitializeInput = {
  drawCanvas: HTMLCanvasElement
  config: RunConfig
  runOnCameraStatusChange: RunOnCameraStatusChangeCb
  onError: (error: Error) => void
}

export type PermissionsManager = {
  get: () => Set<XrPermission>
}

export type SessionManagerFrameStartInput = {
  repeatFrame: boolean
  drawCtx: RenderContext
  computeCtx: RenderContext
  drawTexture: EmscriptenTexture
  computeTexture: EmscriptenTexture
}

export type SessionManagerInstance = {
  hasNewFrame: () => {repeatFrame: boolean, videoSize: {width: number, height: number}}
  getSessionAttributes: () => SessionAttributes
  initialize: (options: SessionManagerInitializeInput) => Promise<void>
  obtainPermissions: (
    permissionsManager: PermissionsManager, hasUserGesture?: boolean
  ) => Promise<void>
  // If we return true, the session start has been cancelled. `stop` will NOT be called. The
  // implementer needs to make sure they are not leaking resources.
  // If there is an unrecoverable problem when starting, the implementer should throw an error.
  start: () => Promise<boolean | undefined> | void | boolean
  // is called, when XR8.stop() or XR8.reconfigureSession() is called
  stop: () => void
  pause: () => void
  resume: () => Promise<boolean | void>
  resumeIfAutoPaused?: () => void
  frameStartResult: (input: SessionManagerFrameStartInput) => SessionFrameStartResult
}

export type SessionManagerDefinition = {
  name: string
  cb: () => SessionManagerInstance
}

export type FrameworkHandle = {
  dispatchEvent: (eventName: string, detail: unknown) => void
}

export type ModuleEvent = {
  name: string
  detail: unknown
}

export type EventName = string

export type EventHandler = {
  moduleName: ModuleName
  process: (data: unknown) => void
}

export type EventHandlerRegistration = EventHandler & {
  eventName: EventName
}

export type EventHandlers = Record<string, EventHandler[]>
