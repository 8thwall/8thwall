/* eslint-disable max-classes-per-file */
/* eslint-disable no-console */

import * as capnp from 'capnp-ts'

import type {Window} from '@nia/c8/dom/dom'

import * as Proto from '@nia/c8/xrapi/xrapi-proto.capnp'

import type {
  XrApi, XrWebGlLayer, XrWebGlLayerInit, XrFrameCallback, XrSession, XrSessionMode, XrFrame,
  XrView, XrViewport, XrHandedness, XrInputSource, XrPose,
  XrReferenceSpace, XrReferenceSpaceType, GamepadHapticActuator,
  GamepadHapticsResult, GamepadButton, XrSessionInit,
  XrHand, XrHandJoint, XrJointSpace, XrWebGlRenderingContext,
  XrRenderState,
  XrProjectionLayer,
  XrProjectionLayerInit,
  XrRigidTransform,
  DOMPointReadOnly,
} from './xrapi-types'

import type {InternalXrProjectionLayer} from './xrapi-layers'
import {
  type InternalXrSession,
  getEnabledFeatures,
} from './session'
import type {InternalXrView} from './view'

import {
  createXrSpace,
  createXrReferenceSpace,
  updateSpaceOriginPose,
  updateSpaceOriginOffset,
  getRelativeTransform,
  getPose,
  createXrJointSpace,
  updateJointSpace,
  getJointPose,
} from './space'
import {XrRigidTransformImpl, floatsToMatrix, floatsToTransform} from './transform'
import {installXrApi as baseInstall} from './xrapi-install'
import {XrInputSourcesChangeEvent} from './xr-input-sources-change-event'
import {XrInputSourceEvent} from './xr-input-source-event'
import {XrWebGlBindingImpl} from './webgl-binding'
import {createCallbacks} from './callbacks'

type XrApiAddon = {
  isSessionSupported(mode: XrSessionMode): boolean
  requestSession(
    mode: XrSessionMode,
    resolve: () => void,
    dispatchFrame: (data: Uint8Array) => void
  ): void
  getSupportedFrameRates(): Float32Array
  tryGetReferenceSpacePose(type: XrReferenceSpaceType): Uint8Array
  initializeSwapchain(textureId: number, spaceWarpRequested: boolean): Uint8Array
  sendHapticFeedback(handedness: string, value: number, duration: number): void
  updateFixedFoveation(value: number): void
  updateDeltaPose(position: DOMPointReadOnly, orientation: DOMPointReadOnly): void
  updateRenderState(depthNear: number, depthFar: number): void
  updateRenderTexture(textureId: number): void
  updateSpaceWarpTextures(motionVectorTextureId: number, motionVectorDepthTextureId: number): void
  updateTargetFrameRate(frameRate: number): void
  setIsSpaceWarpActiveCallback(
    isSpaceWarpActive: () => boolean
  ): void
}

const addon: XrApiAddon = (globalThis as any).xrApiAddon
const QUEST_PRIMARY_TRIGGER_IDX = 0
const QUEST_PRIMARY_SQUEEZE_IDX = 1
const QUEST_PRIMARY_THUMBSTICK_CLICK = 3
const QUEST_PRIMARY_BUTTON_IDX = 4
const QUEST_SECONDARY_BUTTON_IDX = 5
const QUEST_PRIMARY_MENU_IDX = 6
const QUEST_PRIMARY_THUMBSTICK_X = 2
const QUEST_PRIMARY_THUMBSTICK_Y = 3

// Loosely-calibrated offsets for the Quest 3 controllers to align the controller.
// TODO(lynn): Refactor to support other devices, and maybe move this to the native layer.
const CONTROLLER_OFFSET_TRANSLATION = {x: 0, y: -0.01, z: -0.024, w: 1}
const CONTROLLER_OFFSET_QUATERNION = {x: -0.1305262, y: 0, z: 0, w: 0.9914449}
const RAY_OFFSET_L_TRANSLATION = {x: 0.009, y: 0, z: 0.024, w: 1}
const RAY_OFFSET_L_QUATERNION = {x: 0, y: -0.0436194, z: 0, w: 0.9990482}
const RAY_OFFSET_R_TRANSLATION = {x: -0.009, y: 0, z: 0.024, w: 1}
const RAY_OFFSET_R_QUATERNION = {x: 0, y: 0.0436194, z: 0, w: 0.9990482}

if (!addon) {
  throw new Error('[xrapi] XrApiAddon not found')
}

type WebGLFramebuffer = any & {_: number}  // Set by headless-gl

class BindingsXrLayer extends EventTarget implements XrWebGlLayer {
  antialias: boolean

  ignoreDepthValues: boolean

  framebuffer: WebGLFramebuffer

  framebufferWidth: number

  framebufferHeight: number

  _niaCtx: XrWebGlRenderingContext

  _leftViewport: XrViewport

  _rightViewport: XrViewport

  _fixedFoveation: number = 0

  constructor(session: XrSession, gl: XrWebGlRenderingContext, layerInit?: XrWebGlLayerInit) {
    super()

    this.antialias = layerInit?.antialias ?? true
    this.ignoreDepthValues = layerInit?.ignoreDepthValues ?? false

    this._niaCtx = gl
    this.framebuffer = gl.createFramebuffer()
    const texture = gl.createTexture()
    gl.bindTexture(gl.TEXTURE_2D, texture)
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE, null)
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR)
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE)
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE)
    gl.bindFramebuffer(gl.FRAMEBUFFER, this.framebuffer)
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, texture, 0)

    gl.enable(gl.DEPTH_TEST)

    const depthTexture = gl.createTexture()
    gl.bindTexture(gl.TEXTURE_2D, depthTexture)
    gl.texImage2D(
      gl.TEXTURE_2D, 0, gl.DEPTH_COMPONENT24, 1, 1, 0, gl.DEPTH_COMPONENT, gl.UNSIGNED_INT, null
    )
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR)
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE)
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE)
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.TEXTURE_2D, depthTexture, 0)

    const spaceWarpRequested = false  // Note: SpaceWarp not specced for XrWebGlLayer
    const data = addon.initializeSwapchain(texture._, spaceWarpRequested)
    const message = new capnp.Message(data, false /* packed */)
    const res: Proto.XrSwapchainSetup = message.getRoot(Proto.XrSwapchainSetup)
    const width = res.getWidth()
    const height = res.getHeight()

    this._leftViewport = {x: 0, y: 0, width, height}
    this._rightViewport = {x: width, y: 0, width, height}
    this.framebufferWidth = width * 2
    this.framebufferHeight = height

    gl.bindTexture(gl.TEXTURE_2D, texture)
    gl.texImage2D(
      gl.TEXTURE_2D, 0, gl.RGBA, width * 2, height, 0, gl.RGBA,
      gl.UNSIGNED_BYTE, null
    )

    gl.bindTexture(gl.TEXTURE_2D, depthTexture)
    gl.texImage2D(
      gl.TEXTURE_2D, 0, gl.DEPTH_COMPONENT24, width * 2, height, 0,
      gl.DEPTH_COMPONENT, gl.UNSIGNED_INT, null
    )

    gl.bindFramebuffer(gl.FRAMEBUFFER, null)
  }

  getViewport(view: XrView) {
    if (view.eye === 'left') {
      return this._leftViewport
    } else {
      return this._rightViewport
    }
  }

  get fixedFoveation() {
    return this._fixedFoveation
  }

  set fixedFoveation(value: number) {
    // NOTE(dat): Currently model manager sets foveation to 1.0 which seems to drop perf for NAE
    // in splat view a lot. @see web-model-view.ts
    // TODO(lreyna): Renable foveation when issue is resolved.
    return
    if (this._fixedFoveation !== value) {
      this._fixedFoveation = Math.min(Math.max(value, 0.0), 1.0)  // clamp to [0, 1]
      addon.updateFixedFoveation(value)
    }
  }
}

// https://www.w3.org/TR/webxr/#xr-compositor
const xrCompositor = {
  _niaCtx: null as XrWebGlRenderingContext | null,
}
const setUpXrCompositor = (session: InternalXrSession, gl: XrWebGlRenderingContext) => {
  xrCompositor._niaCtx = gl

  const spaceWarpRequested = session.enabledFeatures.includes('space-warp')
  const data = addon.initializeSwapchain(0, spaceWarpRequested)
  const message = new capnp.Message(data, false /* packed */)
  const res: Proto.XrSwapchainSetup = message.getRoot(Proto.XrSwapchainSetup)
  const width = res.getWidth()
  const height = res.getHeight()

  session._recommendedWidth = width
  session._recommendedHeight = height

  for (const view of session._views) {
    view._recommendedColorWidth = width
    view._recommendedColorHeight = height
    view._recommendedColorWidth = width
    view._recommendedColorHeight = height
    view._recommendedDepthWidth = res.getMotionVectorWidth()
    view._recommendedDepthHeight = res.getMotionVectorHeight()
    view._recommendedMotionVectorWidth = res.getDepthStencilWidth()
    view._recommendedMotionVectorHeight = res.getDepthStencilHeight()
  }
}

class BindingsXrWebGlBindingImpl extends XrWebGlBindingImpl {
  constructor(session: XrSession, context: XrWebGlRenderingContext) {
    super(session, context)

    setUpXrCompositor(session as InternalXrSession, context)
  }

  createProjectionLayer(layerInit: XrProjectionLayerInit): XrProjectionLayer {
    const layer = super.createProjectionLayer(layerInit) as InternalXrProjectionLayer

    Object.defineProperty(layer, 'fixedFoveation', {
      get: () => layer._fixedFoveation,
      set: (value: number) => {
        // TODO(lreyna): Renable foveation when issue is resolved.
        return
        if (layer._fixedFoveation !== value) {
          layer._fixedFoveation = Math.min(Math.max(value, 0.0), 1.0)  // clamp to [0, 1]
          addon.updateFixedFoveation(value)
        }
      },
      enumerable: true,
    })

    const identityTransform = new XrRigidTransformImpl()
    Object.defineProperty(layer, 'deltaPose', {
      get: () => layer._deltaPose,
      set: (value: XrRigidTransform) => {
        layer._deltaPose = value || identityTransform
        addon.updateDeltaPose(layer._deltaPose.position, layer._deltaPose.orientation)
      },
      enumerable: true,
    })

    // This will associate the texture with the native swapchain images.
    addon.updateRenderTexture(layer._colorTextures[0]._)

    if (layer._session.enabledFeatures.includes('space-warp')) {
      const targetMotionVectorTexture = layer._motionVectorTextures[0]
      addon.updateSpaceWarpTextures(
        targetMotionVectorTexture._,
        layer._depthStencilTextures[0]._
      )

      addon.setIsSpaceWarpActiveCallback(() => {
        if (!targetMotionVectorTexture) {
          return false
        }

        const isSpaceWarpActive = targetMotionVectorTexture._framebufferBinding &&
          !targetMotionVectorTexture._framebufferBindingDirty

        targetMotionVectorTexture._framebufferBindingDirty = true
        return isSpaceWarpActive
      })
    }

    return layer
  }
}

const delay = (ms: number) => (new Promise(resolve => setTimeout(resolve, ms)))

const createHapticActuators = (handedness: XrHandedness): GamepadHapticActuator[] => {
  async function pulse(value: number, duration: number): Promise<boolean> {
    try {
      addon.sendHapticFeedback(handedness, value, duration * 1e6)
    } catch (e) {
      return false
    }
    await delay(duration)
    return true
  }

  async function reset(): Promise<GamepadHapticsResult> {
    addon.sendHapticFeedback(handedness, 0, 0)
    return 'complete'
  }

  return [{
    pulse,
    reset,
  }]
}

const interactionProfileToProfiles = (interactionProfile: string): string[] => {
  const vendorName = interactionProfile.split('/')[2]
  switch (vendorName) {
    case 'oculus': return ['meta-quest-touch-plus']
    case 'hands': return ['generic-hand-tracking', 'generic-hand']
    default: return []
  }
}

const extractCapnpPose = (pose: Proto.XrPose): XrPose => {
  const pos = pose.getPosition().toArray()
  const rot = pose.getOrientation().toArray()
  return {
    transform: new XrRigidTransformImpl(
      {x: pos[0], y: pos[1], z: pos[2], w: 1},
      {x: rot[0], y: rot[1], z: rot[2], w: rot[3]},
      true
    ),  // assume data is already normalized
    linearVelocity: {x: 0, y: 0, z: 0, w: 0},
    angularVelocity: {x: 0, y: 0, z: 0, w: 0},
  }
}

const updateButtonState = (button: GamepadButton, state: Proto.XrButtonState) => {
  button.pressed = state.getValue() > 0
  button.touched = state.getTouched()
  button.value = state.getValue()
}

const updateGamepadButtons = (inputSource: XrInputSource, controller: Proto.XrController) => {
  const controllerName = inputSource.profiles[0]
  const thumbstick = controller.getThumbstickState()
  const buttonStates = controller.getButtonStates()
  const {buttons, axes} = inputSource.gamepad!

  switch (controllerName) {
    case 'meta-quest-touch-plus':
      axes[QUEST_PRIMARY_THUMBSTICK_X] = thumbstick.getX()
      axes[QUEST_PRIMARY_THUMBSTICK_Y] = -thumbstick.getY()
      updateButtonState(buttons[QUEST_PRIMARY_THUMBSTICK_CLICK], thumbstick.getClick())
      updateButtonState(buttons[QUEST_PRIMARY_BUTTON_IDX], buttonStates.get(0))
      updateButtonState(buttons[QUEST_SECONDARY_BUTTON_IDX], buttonStates.get(1))
      if (inputSource.handedness === 'left') {
        updateButtonState(buttons[QUEST_PRIMARY_MENU_IDX], buttonStates.get(2))
      }
      break
    default:
      break
  }
}

const makeIdentityTransform = () => new XrRigidTransformImpl(
  {x: 0, y: 0, z: 0, w: 1},
  {x: 0, y: 0, z: 0, w: 1},
  true
)

const makeIdentityPose = (): XrPose => ({
  transform: makeIdentityTransform(),
  linearVelocity: {x: 0, y: 0, z: 0, w: 0},
  angularVelocity: {x: 0, y: 0, z: 0, w: 0},
})

const makeIdentityMatrix = () => (
  new Float32Array([
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1,
  ])
)

const requestSession = async (mode: XrSessionMode, options?: XrSessionInit): Promise<XrSession> => {
  if (!addon.isSessionSupported(mode)) {
    throw new Error(`[xrapi] Session mode not supported: ${mode}`)
  }

  const callbacks_ = createCallbacks<XrFrameCallback>()

  const renderState_: XrRenderState = {
    baseLayer: <BindingsXrLayer | null>null,
    depthNear: 0.1,
    depthFar: 1000,

    // For THREE JS, comment out the next line to use XrWebGlLayer instead of XrProjectionLayer.
    layers: [],
  }

  let session_: InternalXrSession
  let currentFrame_: XrFrame
  const eventTarget_ = new EventTarget()

  const createXrHand = (): XrHand => new Map<XrHandJoint, XrJointSpace>([
    ['wrist',
      createXrJointSpace(session_, 'wrist')],
    ['thumb-metacarpal',
      createXrJointSpace(session_, 'thumb-metacarpal')],
    ['thumb-phalanx-proximal',
      createXrJointSpace(session_, 'thumb-phalanx-proximal')],
    ['thumb-phalanx-distal',
      createXrJointSpace(session_, 'thumb-phalanx-distal')],
    ['thumb-tip',
      createXrJointSpace(session_, 'thumb-tip')],
    ['index-finger-metacarpal',
      createXrJointSpace(session_, 'index-finger-metacarpal')],
    ['index-finger-phalanx-proximal',
      createXrJointSpace(session_, 'index-finger-phalanx-proximal')],
    ['index-finger-phalanx-intermediate',
      createXrJointSpace(session_, 'index-finger-phalanx-intermediate')],
    ['index-finger-phalanx-distal',
      createXrJointSpace(session_, 'index-finger-phalanx-distal')],
    ['index-finger-tip',
      createXrJointSpace(session_, 'index-finger-tip')],
    ['middle-finger-metacarpal',
      createXrJointSpace(session_, 'middle-finger-metacarpal')],
    ['middle-finger-phalanx-proximal',
      createXrJointSpace(session_, 'middle-finger-phalanx-proximal')],
    ['middle-finger-phalanx-intermediate',
      createXrJointSpace(session_, 'middle-finger-phalanx-intermediate')],
    ['middle-finger-phalanx-distal',
      createXrJointSpace(session_, 'middle-finger-phalanx-distal')],
    ['middle-finger-tip',
      createXrJointSpace(session_, 'middle-finger-tip')],
    ['ring-finger-metacarpal',
      createXrJointSpace(session_, 'ring-finger-metacarpal')],
    ['ring-finger-phalanx-proximal',
      createXrJointSpace(session_, 'ring-finger-phalanx-proximal')],
    ['ring-finger-phalanx-intermediate',
      createXrJointSpace(session_, 'ring-finger-phalanx-intermediate')],
    ['ring-finger-phalanx-distal',
      createXrJointSpace(session_, 'ring-finger-phalanx-distal')],
    ['ring-finger-tip',
      createXrJointSpace(session_, 'ring-finger-tip')],
    ['pinky-finger-metacarpal',
      createXrJointSpace(session_, 'pinky-finger-metacarpal')],
    ['pinky-finger-phalanx-proximal',
      createXrJointSpace(session_, 'pinky-finger-phalanx-proximal')],
    ['pinky-finger-phalanx-intermediate',
      createXrJointSpace(session_, 'pinky-finger-phalanx-intermediate')],
    ['pinky-finger-phalanx-distal',
      createXrJointSpace(session_, 'pinky-finger-phalanx-distal')],
    ['pinky-finger-tip',
      createXrJointSpace(session_, 'pinky-finger-tip')],
  ])

  const makeInputSource = (handedness_: XrHandedness, profiles: string[],
    isHand: boolean): XrInputSource => ({
    targetRayMode: 'tracked-pointer',
    gripSpace: createXrSpace(session_),
    profiles,
    targetRaySpace: createXrSpace(session_),
    handedness: handedness_,
    hand: isHand ? createXrHand() : null,
    gamepad: isHand
      ? null
      : {
        id: '',
        index: 0,
        mapping: 'xr-standard',
        connected: true,
        timestamp: 0,
        buttons: [
          {pressed: false, touched: false, value: 0},
          {pressed: false, touched: false, value: 0},
          {pressed: false, touched: false, value: 0},
          {pressed: false, touched: false, value: 0},
          {pressed: false, touched: false, value: 0},
          {pressed: false, touched: false, value: 0},
          {pressed: false, touched: false, value: 0},
        ],
        axes: [0, 0, 0, 0],
        hapticActuators: createHapticActuators(handedness_),
      },
  })

  const dispatchTriggerEvents = (triggerType: string, triggerIndex: number,
    triggerState: number, inputSource: XrInputSource) => {
    if (!inputSource.gamepad) {
      return
    }
    const {buttons} = inputSource.gamepad
    const isPressed = triggerState > 0.1
    const wasPressed = buttons[triggerIndex].value > 0.1
    buttons[triggerIndex].pressed = triggerState > 0.1
    buttons[triggerIndex].touched = isPressed
    buttons[triggerIndex].value = triggerState
    if (isPressed && !wasPressed) {
      session_.dispatchEvent(new XrInputSourceEvent(`${triggerType}start`, {
        frame: currentFrame_,
        inputSource,
      }))
    } else if (!isPressed && wasPressed) {
      session_.dispatchEvent(new XrInputSourceEvent(`${triggerType}end`, {
        frame: currentFrame_,
        inputSource,
      }))
      session_.dispatchEvent(new XrInputSourceEvent(triggerType, {
        frame: currentFrame_,
        inputSource,
      }))
    }
  }

  const dispatchConnectEvents = (events: ReturnType<Proto.XrFrameData['getConnectEvents']>) => {
    const added: XrInputSource[] = []
    const removed: XrInputSource[] = []
    events.forEach((event: Proto.XrConnectEvent) => {
      switch (event.getConnectionState()) {
        case Proto.XrConnectionState.CONNECTED: {
          const newDevice = makeInputSource(event.getHandedness() as XrHandedness,
            interactionProfileToProfiles(event.getInteractionProfile()), event.getIsHand())
          const initialPose = makeIdentityPose()
          updateSpaceOriginPose(newDevice.gripSpace, initialPose)
          updateSpaceOriginPose(newDevice.targetRaySpace, initialPose)

          // Apply offsets for the controller and ray spaces.
          updateSpaceOriginOffset(
            newDevice.gripSpace,
            CONTROLLER_OFFSET_TRANSLATION,
            CONTROLLER_OFFSET_QUATERNION
          )
          if (event.getHandedness() === 'left') {
            updateSpaceOriginOffset(
              newDevice.targetRaySpace,
              RAY_OFFSET_L_TRANSLATION,
              RAY_OFFSET_L_QUATERNION
            )
          } else if (event.getHandedness() === 'right') {
            updateSpaceOriginOffset(
              newDevice.targetRaySpace,
              RAY_OFFSET_R_TRANSLATION,
              RAY_OFFSET_R_QUATERNION
            )
          }
          added.push(newDevice)
          session_.inputSources.push(newDevice)
          break
        }
        case Proto.XrConnectionState.DISCONNECTED:
          removed.push(...session_.inputSources.filter(
            inputSource => inputSource.handedness === event.getHandedness()
          ))
          session_.inputSources = session_.inputSources.filter(
            inputSource => inputSource.handedness !== event.getHandedness()
          )
          break
        default:
          break
      }
    })
    if (added.length > 0 || removed.length > 0) {
      eventTarget_.dispatchEvent(new XrInputSourcesChangeEvent('inputsourceschange', {
        session: session_,
        added,
        removed,
      }))
    }
  }

  const setControllerState = (inputSource: XrInputSource, controller: Proto.XrController) => {
    if (controller.getIsHand()) {
      const jointPoses = controller.getJointPoses()
      const palmPose = extractCapnpPose(jointPoses.get(0).getPose())
      updateSpaceOriginPose(inputSource.gripSpace, palmPose)
      updateSpaceOriginPose(inputSource.targetRaySpace, palmPose)
      Array.from(inputSource.hand!.values()).forEach((jointSpace, i) => {
        const xrJointPose = extractCapnpPose(jointPoses.get(i + 1).getPose())
        updateJointSpace(jointSpace, xrJointPose, jointPoses.get(i + 1).getRadius())
      })
    } else {
      const xrPose = extractCapnpPose(controller.getPose())
      const xrAimPose = extractCapnpPose(controller.getAimPose())
      updateSpaceOriginPose(inputSource.gripSpace, xrPose)
      updateSpaceOriginPose(inputSource.targetRaySpace, xrAimPose)

      dispatchTriggerEvents('select', QUEST_PRIMARY_TRIGGER_IDX,
        controller.getSelectState(), inputSource)
      dispatchTriggerEvents('squeeze', QUEST_PRIMARY_SQUEEZE_IDX,
        controller.getSqueezeState(), inputSource)
      updateGamepadButtons(inputSource, controller)
    }
  }

  const updateReferenceSpacePose = (referenceSpace: XrReferenceSpace) => {
    // @ts-ignore
    const type = referenceSpace._type
    let poseFromNative: XrPose
    const message = addon.tryGetReferenceSpacePose(type)
    if (!message) {
      console.error('[bindings] failed to get reference space pose')
      poseFromNative = makeIdentityPose()
    } else {
      const res: Proto.XrPose = new capnp.Message(message, false).getRoot(Proto.XrPose)
      poseFromNative = extractCapnpPose(res)
    }

    updateSpaceOriginPose(referenceSpace, poseFromNative)
  }

  const dispatchFrame = (data: Uint8Array) => {
    const message = new capnp.Message(data, false /* packed */)
    const frameData: Proto.XrFrameData = message.getRoot(Proto.XrFrameData)

    dispatchConnectEvents(frameData.getConnectEvents())

    const time: number = frameData.getTime()
    const frame: XrFrame = {
      session: session_,
      getPose,
      getViewerPose: (space: XrReferenceSpace) => {
        const views: InternalXrView[] = frameData.getViews().map(
          ((view: Proto.XrFrameView, i: number): InternalXrView => {
            const projection = view.getProjection().toArray()
            const transform = view.getTransform().toArray()
            updateReferenceSpacePose(space)
            return {
              _frame: frame,
              eye: (i === 0) ? 'left' : 'right',
              projectionMatrix: floatsToMatrix(projection),
              transform: getRelativeTransform(floatsToTransform(transform), space),
            } as InternalXrView
          })
        )

        session_._views = views

        return {views}
      },
      getJointPose,
    }
    currentFrame_ = frame

    frameData.getControllers().forEach((controller: Proto.XrController, i: number) => {
      // NOTE(akashmahesh): The order in getControllers is [left, right]
      // but inputSources depends on the order the controllers connected
      const handedness: XrHandedness = i === 0 ? 'left' : 'right'
      const inputSource = session_.inputSources.find(source => source.handedness === handedness)
      if (!inputSource) {
        return
      }
      setControllerState(inputSource, controller)
    })

    callbacks_.dispatch(time, frame)
  }

  await new Promise<void>((resolve) => {
    addon.requestSession(mode, resolve, dispatchFrame)
  })

  // Resolve the user requested features for this sesssion
  // TODO(lreyna): Implement native feature check
  const enabledFeatures = getEnabledFeatures(mode, options, () => true)
  if (!enabledFeatures) {
    throw new Error('[xrapi] Required session features not supported')
  }

  // This needs to happen after the native session is created
  const availableFrameRates = addon.getSupportedFrameRates()
  const updateTargetFrameRate = async (frameRate: number) => {
    if (!availableFrameRates.includes(frameRate)) {
      throw new Error(`[xrapi] Frame rate ${frameRate} not supported`)
    }

    // TODO(lreyna): Actually make this async
    addon.updateTargetFrameRate(frameRate)
  }

  session_ = {
    requestReferenceSpace: async (type: XrReferenceSpaceType) => {
      const referenceSpace = createXrReferenceSpace(session_, type)
      updateReferenceSpacePose(referenceSpace)
      return referenceSpace
    },
    requestAnimationFrame: callbacks_.add,
    cancelAnimationFrame: callbacks_.remove,
    renderState: renderState_,
    updateRenderState: (args) => {
      Object.assign(renderState_, args)
      addon.updateRenderState(renderState_.depthNear!, renderState_.depthFar!)
    },
    updateTargetFrameRate,
    addEventListener: eventTarget_.addEventListener.bind(eventTarget_),
    dispatchEvent: eventTarget_.dispatchEvent.bind(eventTarget_),
    end: () => {
      console.error('[bindings] session.end not implemented')
    },
    enabledFeatures,
    inputSources: [],
    visibilityState: 'visible',
    frameRate: availableFrameRates[0],  // Default to the first available frame rate for now
    supportedFrameRates: availableFrameRates,
    _recommendedHeight: 0,
    _recommendedWidth: 0,

    // TODO: Determine number of views from the session.
    _views: [{
      _recommendedColorWidth: 0,
      _recommendedColorHeight: 0,
      _recommendedDepthWidth: 0,
      _recommendedDepthHeight: 0,
      _recommendedMotionVectorWidth: 0,
      _recommendedMotionVectorHeight: 0,
      _frame: null as any,
      eye: 'left',
      projectionMatrix: makeIdentityMatrix(),
      transform: makeIdentityTransform(),
    }, {
      _recommendedColorWidth: 0,
      _recommendedColorHeight: 0,
      _recommendedDepthWidth: 0,
      _recommendedDepthHeight: 0,
      _recommendedMotionVectorWidth: 0,
      _recommendedMotionVectorHeight: 0,
      _frame: null as any,
      eye: 'right',
      projectionMatrix: makeIdentityMatrix(),
      transform: makeIdentityTransform(),
    }],
  }

  return session_
}

const createXrBindings = (): XrApi => {
  const eventTarget = new EventTarget()

  return {
    xr: {
      requestSession,
      isSessionSupported: async mode => addon.isSessionSupported(mode),
      addEventListener: eventTarget.addEventListener.bind(eventTarget),
      dispatchEvent: eventTarget.dispatchEvent.bind(eventTarget),
      removeEventListener: eventTarget.removeEventListener.bind(eventTarget),
      offerSession: requestSession,
    },
    XrWebGlLayerClass: BindingsXrLayer,
    XrWebGlBindingClass: BindingsXrWebGlBindingImpl,
    XrRigidTransformClass: XrRigidTransformImpl,
  }
}

const installXrBindings = (target: Window) => {
  baseInstall(target, createXrBindings())
}

export {
  createXrBindings,
  installXrBindings,
}
