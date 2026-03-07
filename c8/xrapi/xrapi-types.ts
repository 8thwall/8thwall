/* eslint-disable max-classes-per-file */

type XrViewport = {
  x: number
  y: number
  width: number
  height: number
}

type XrVisibilityState = 'visible' | 'visible-blurred' | 'hidden'

type XrLayer = EventTarget & {}

type XrWebGlLayerInit = {
  antialias?: boolean
  depth?: boolean
  stencil?: boolean
  alpha?: boolean
  ignoreDepthValues?: boolean
  framebufferScaleFactor?: number
}

// https://immersive-web.github.io/webxr/#xrwebgllayer-interface
declare class XrWebGlLayer extends EventTarget implements XrLayer {
  constructor(session: XrSession, context: any, layerInit?: XrWebGlLayerInit)

  antialias: boolean

  ignoreDepthValues: boolean

  framebuffer: any

  framebufferWidth: number

  framebufferHeight: number

  fixedFoveation?: number

  getViewport(view: XrView): XrViewport
}

type XrLayerLayout = 'default' | 'mono' | 'stereo' | 'stereo-top-bottom' | 'stereo-left-right'

type XrLayerQuality = 'default' | 'text-optimized' | 'graphics-optimized'

type XrCompositionLayer = XrLayer & {
  readonly layout: XrLayerLayout

  blendTextureSourceAlpha: boolean
  forceMonoPresentation: boolean
  opacity: number
  readonly mipLevels: number
  quality: XrLayerQuality

  readonly needsRedraw: boolean

  destroy(): void
}

type XrProjectionLayer = XrCompositionLayer & {
  readonly textureWidth: number

  readonly textureHeight: number

  readonly textureArrayLength: number

  readonly ignoreDepthValues: boolean

  fixedFoveation?: number

  deltaPose?: XrRigidTransform
}

type XrSubImage = {
  viewport: XrViewport
}

type WebGLTexture = any
type XrWebGlSubImage = XrSubImage & {
  readonly colorTexture: WebGLTexture
  readonly depthStencilTexture?: WebGLTexture
  readonly motionVectorTexture?: WebGLTexture

  readonly imageIndex?: number
  readonly colorTextureWidth: number
  readonly colorTextureHeight: number
  readonly depthStencilTextureWidth?: number
  readonly depthStencilTextureHeight?: number
  readonly motionVectorTextureWidth?: number
  readonly motionVectorTextureHeight?: number
}

type XrTextureType = 'texture' | 'texture-array'

type XrProjectionLayerInit = {
  textureType?: XrTextureType
  colorFormat?: number
  depthFormat?: number
  scaleFactor?: number
  clearOnAccess?: boolean
};

type XrEye = 'none' | 'left' | 'right'

type WebGLRenderingContext = any
type WebGL2RenderingContext = any
type XrWebGlRenderingContext = WebGLRenderingContext | WebGL2RenderingContext

// https://immersive-web.github.io/layers/#XRWebGLBindingtype
declare class XrWebGlBinding {
  constructor(session: XrSession, context: XrWebGlRenderingContext)

  readonly nativeProjectionScaleFactor: number

  readonly usesDepthValues: boolean

  createProjectionLayer(layerInit: XrProjectionLayerInit): XrProjectionLayer
  // TODO: createQuadLayer, createCylinderLayer, createEquirectLayer, etc.

  getSubImage(layer: XrCompositionLayer, frame: XrFrame, eye?: XrEye): XrWebGlSubImage

  getViewSubImage(layer: XrCompositionLayer, view: XrView): XrWebGlSubImage
}

declare class XrRigidTransform {
  constructor()

  constructor(position: DOMPointReadOnly)

  constructor(position: DOMPointReadOnly, orientation: DOMPointReadOnly)

  readonly matrix: Float32Array

  readonly position: DOMPointReadOnly

  readonly orientation: DOMPointReadOnly

  readonly inverse: XrRigidTransform
}

type XrInputSourcesChangeEventInit = {
  session: XrSession
  added: XrInputSource[]
  removed: XrInputSource[]
}

type XrInputSourceEventInit = {
  frame: XrFrame
  inputSource: XrInputSource
}

type XrFrameCallback = (time: number, frame: XrFrame) => void

// TODO(akashmahesh): add inlineVerticalFieldOfView property
type XrRenderState = {
  baseLayer: XrWebGlLayer | null
  depthNear?: number
  depthFar?: number

  // https://immersive-web.github.io/layers/#xrrenderstatechanges
  layers?: XrLayer[]
}

type XrSessionMode = 'inline' | 'immersive-vr' | 'immersive-ar'

type XrSession = {
  visibilityState: XrVisibilityState
  readonly frameRate?: number
  readonly supportedFrameRates?: Float32Array
  requestReferenceSpace(mode: string): Promise<XrReferenceSpace>
  requestAnimationFrame(callback: XrFrameCallback): number
  cancelAnimationFrame(id: number): void
  renderState: XrRenderState
  enabledFeatures: string[]
  updateRenderState(state: Partial<XrRenderState>): void
  updateTargetFrameRate(frameRate: number): Promise<void>
  addEventListener(event: string, listener: () => void): void
  dispatchEvent(event: Event): void
  end(): void
  inputSources: XrInputSource[]
}

type XrSpace = {

}

type XrFrame = {
  session: XrSession
  getViewerPose(referenceSpace: XrReferenceSpace): XrViewer
  getPose(space: XrSpace, baseSpace: XrSpace): XrPose
  getJointPose(joint: XrJointSpace, baseSpace: XrSpace): XrJointPose
}

type XrPose = {
  transform: XrRigidTransform
  linearVelocity: DOMPointReadOnly
  angularVelocity: DOMPointReadOnly
}

type XrJointPose = XrPose & {
  radius: number
}

type XrViewer = {
  views: XrView[]
}

// TODO(lreyna): Should be part of c8/dom
type DOMPointReadOnly = {
  x: number
  y: number
  z: number
  w: number
}

type XrView = {
  eye: XrEye
  transform: XrRigidTransform
  projectionMatrix: Float32Array
}

type XrReferenceSpaceType = 'viewer' | 'local' | 'local-floor' | 'bounded-floor' | 'unbounded'

type XrReferenceSpaceEvent = Event  // NOT IMPLEMENTED

type XrReferenceSpace = XrSpace & {
  onreset: (ev: XrReferenceSpaceEvent) => unknown

  getOffsetReferenceSpace(originOffset: XrRigidTransform): XrReferenceSpace
}

type XrSessionInit = Partial<{
  requiredFeatures: string[]
  optionalFeatures: string[]
}>

type XrSystem = EventTarget & {
  requestSession(mode: XrSessionMode, options?: XrSessionInit): Promise<XrSession>
  isSessionSupported(mode: XrSessionMode): Promise<boolean>
  offerSession?(mode: XrSessionMode, options?: XrSessionInit): Promise<XrSession>
}

type XrApi = {
  xr: XrSystem
  XrWebGlLayerClass: typeof XrWebGlLayer
  XrWebGlBindingClass: typeof XrWebGlBinding
  XrRigidTransformClass: typeof XrRigidTransform
}

type XrHand = Map<XrHandJoint, XrJointSpace>

type XrJointSpace = XrSpace & {
  jointName: XrHandJoint
}

type XrHandJoint =
  | 'wrist'
  | 'thumb-metacarpal'
  | 'thumb-phalanx-proximal'
  | 'thumb-phalanx-distal'
  | 'thumb-tip'
  | 'index-finger-metacarpal'
  | 'index-finger-phalanx-proximal'
  | 'index-finger-phalanx-intermediate'
  | 'index-finger-phalanx-distal'
  | 'index-finger-tip'
  | 'middle-finger-metacarpal'
  | 'middle-finger-phalanx-proximal'
  | 'middle-finger-phalanx-intermediate'
  | 'middle-finger-phalanx-distal'
  | 'middle-finger-tip'
  | 'ring-finger-metacarpal'
  | 'ring-finger-phalanx-proximal'
  | 'ring-finger-phalanx-intermediate'
  | 'ring-finger-phalanx-distal'
  | 'ring-finger-tip'
  | 'pinky-finger-metacarpal'
  | 'pinky-finger-phalanx-proximal'
  | 'pinky-finger-phalanx-intermediate'
  | 'pinky-finger-phalanx-distal'
  | 'pinky-finger-tip'

type XrHandedness = 'none' | 'left' | 'right'

type TargetRayModes = 'gaze' | 'tracked-pointer' | 'screen' | 'transient-pointer'

type Gamepad = {
  id: string
  index: number
  connected: boolean
  timestamp: number
  mapping: GamepadMappingType
  axes: number[]
  buttons: GamepadButton[]
  hapticActuators: GamepadHapticActuator[]
}

type GamepadMappingType = '' | 'standard' | 'xr-standard'

type GamepadButton = {
  pressed: boolean
  touched: boolean
  value: number
}

type GamepadHapticActuator = {
  // TODO(akashmahesh): uncomment and implement other actuator APIs
  // effects: GamepadHapticEffectType[]
  // playEffect(type: GamepadHapticEffectType,
  //   params: GamepadEffectParameters | undefined): Promise<GamepadHapticsResult>
  pulse(value: number, duration: number): Promise<boolean>
  reset(): Promise<GamepadHapticsResult>
}

type GamepadHapticEffectType = 'dual-rumble' | 'trigger-rumble'
type GamepadHapticsResult = 'complete' | 'preempted'

type GamepadEffectParameters = {
  duration: number
  startDelay: number
  strongMagnitude: number
  weakMagnitude: number
  leftTrigger: number
  rightTrigger: number
}

type XrInputSource = {
  targetRayMode: TargetRayModes
  targetRaySpace: XrSpace
  profiles: string[]
  gripSpace: XrSpace
  handedness: XrHandedness
  hand: XrHand | null
  gamepad: Gamepad | null
}

export type {
  XrSystem,
  XrSession,
  XrSessionMode,
  XrSessionInit,
  XrFrame,
  XrEye,
  XrViewer,
  XrView,
  XrReferenceSpace,
  XrReferenceSpaceType,
  XrReferenceSpaceEvent,
  XrLayer,
  XrCompositionLayer,
  XrProjectionLayer,
  XrProjectionLayerInit,
  XrWebGlBinding,
  XrWebGlLayerInit,
  XrWebGlLayer,
  XrWebGlRenderingContext,
  XrWebGlSubImage,
  XrLayerLayout,
  XrLayerQuality,
  XrApi,
  XrTextureType,
  XrSpace,
  XrFrameCallback,
  XrRenderState,
  XrRigidTransform,
  XrInputSource,
  XrHand,
  XrHandedness,
  XrViewport,
  TargetRayModes,
  DOMPointReadOnly,
  XrInputSourcesChangeEventInit,
  XrInputSourceEventInit,
  XrJointSpace,
  XrHandJoint,
  XrJointPose,
  XrPose,
  GamepadButton,
  GamepadHapticActuator,
  GamepadHapticEffectType,
  GamepadEffectParameters,
  GamepadHapticsResult,
}
