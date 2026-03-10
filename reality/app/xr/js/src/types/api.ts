// @visibility(//visibility:public)
import type {EmscriptenTexture} from './emscripten'
import type {Module} from './module'
import type {RunConfig} from './pipeline'
import type {Compatibility} from './common'
import type {DeviceInfo, FullDeviceInfo, Compatibilities} from './device'

type TODO = unknown

type LocalizationTarget = {
  id: string
  name: string
  lat: number
  lng: number
  title: string
  node: string
  spaceId: string
  imageUrl: string
  anchorPayload: string
}

type Xr8GlTextureRenderer = {
  getGLctxParameters: TODO
  setGLctxParameters: TODO
  create(args: {
    GLctx: WebGL2RenderingContext
    mirroredDisplay?: boolean
    toTexture?: {width: number, height: number}
    flipY?: boolean
  }): TODO
  fillTextureViewport(
    videoWidth: number,
    videoHeight: number,
    canvasWidth: number,
    canvasHeight: number
  ): void
  setForegroundTextureProvider(provider: TODO): void
  setTextureProvider(provider: TODO): void
  pipelineModule(args?: TODO): Module
  configure(config: TODO): void
}

type Xr8ThreeJs = {
  ThreejsRenderer: TODO
}

type Xr8CanvasScreenshot = {
  canvasScreenshot: TODO
  takeScreenshot(): Promise<string>
  setForegroundCanvas(canvas: HTMLCanvasElement): void
  pipelineModule(): Module
}

type XrControllerConfig = TODO

type CameraProjectionMatrix = {
  cam?: {
    pixelRectWidth: number
    pixelRectHeight: number
    nearClipPlane: number
    farClipPlane: number
  }
  origin: {x: number, y: number, z: number}
  facing: {x: number, y: number, z: number, w: number}
  updateRecenterPoint?: boolean
}

type Xr8XrController = {
  configure(config: XrControllerConfig): void
  recenter(): void
  pipelineModule(): Module
  updateCameraProjectionMatrix(matrix: CameraProjectionMatrix): void
}

type Xr8FaceController = {
  configure(config: TODO): void
  pipelineModule(): Module
}

type Xr8LayersController = {
  configure(config: TODO): void
  pipelineModule(): Module
  recenter(): void
}

type DeviceConfig = {
  MOBILE: 'mobile'
  MOBILE_AND_HEADSETS: 'mobile-and-headsets'
  ANY: 'any'
}

type CameraConfig = {
  FRONT: 'front'
  BACK: 'back'
  ANY: 'any'
}

type Xr8XrConfig = {
  device(): DeviceConfig
  camera(): CameraConfig
}

type PermissionsConfig = {
  CAMERA: 'camera'
  DEVICE_MOTION: 'devicemotion'
  DEVICE_ORIENTATION: 'deviceorientation'
  DEVICE_GPS: 'geolocation'
  MICROPHONE: 'microphone'
}

type Xr8XrPermissions = {
  permissions(): PermissionsConfig
}

type Xr8CameraPixelArray = {
  pipelineModule(): Module
}

type Xr8Device = {
  IncompatibilityReasons: {
    UNSPECIFIED: number
    UNSUPPORTED_OS: number
    UNSUPPORTED_BROWSER: number
    MISSING_DEVICE_ORIENTATION: number
    MISSING_USER_MEDIA: number
    MISSING_WEB_ASSEMBLY: number
  }

  deviceEstimate: () => DeviceInfo
  deviceInfo: () => Promise<FullDeviceInfo>
  compatibilities: () => Compatibilities
  isDeviceBrowserCompatible: (runConfig: RunConfig) => boolean
  incompatibleReasons: (runConfig: RunConfig) => number[]
  incompatibleReasonDetails: (runConfig: RunConfig) => Compatibility
}

type ChunkName = 'face' | 'slam'

type Jsxr = {
  run(config: RunConfig): void
  loadChunk(chunkName: ChunkName): Promise<void>
  version(): string
  featureFlags(): Array<string>

  CanvasScreenshot: Xr8CanvasScreenshot
  MediaRecorder: TODO
  CameraPixelArray: Xr8CameraPixelArray
  GlTextureRenderer: Xr8GlTextureRenderer
  FaceController: Xr8FaceController
  LayersController: Xr8LayersController
  XrController: Xr8XrController

  AFrame: TODO
  Babylonjs: TODO
  PlayCanvas: TODO
  Sumerian: TODO
  Threejs: Xr8ThreeJs
  CloudStudioThreejs: TODO

  XrConfig: Xr8XrConfig
  XrDevice: Xr8Device
  XrPermissions: Xr8XrPermissions

  initialize(): Promise<void>
  isInitialized(): boolean

  pause(): void
  resume(): void
  isPaused(): boolean
  stop(): void

  reconfigureSession(config: RunConfig): void
  addCameraPipelineModule(module: Module): void
  addCameraPipelineModules(modules: Module[]): void
  removeCameraPipelineModule(module: string): void
  removeCameraPipelineModules(modules: Array<string | {name: string}>): void
  clearCameraPipelineModules(): void
  runPreRender(timestamp: number): void
  runRender(): void
  runPostRender(): void
  requiredPermissions(): Set<string>
  drawTexForComputeTex(texture: EmscriptenTexture): WebGLTexture
}

export type {
  Jsxr,
  Jsxr as IXR8,
  Xr8GlTextureRenderer,
  Xr8ThreeJs,
  Xr8CanvasScreenshot,
  XrControllerConfig,
  CameraProjectionMatrix,
  Xr8XrController,
  Xr8FaceController,
  Xr8LayersController,
  Xr8XrConfig,
  Xr8XrPermissions,
  Xr8CameraPixelArray,
  ChunkName,
  LocalizationTarget,
}
