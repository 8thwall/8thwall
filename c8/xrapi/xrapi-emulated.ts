/* eslint-disable no-console */
import {mat4, vec3, quat, Mat4, Vec3Source} from '@nia/c8/ecs/src/runtime/math/math'

import type {
  XrApi, XrSystem, XrSession,
  XrSessionInit, XrSessionMode, XrFrameCallback,
  XrWebGlLayer, XrWebGlLayerInit, XrView, XrReferenceSpaceType, XrHandedness,
  XrInputSource, XrHand, XrJointSpace,
  XrHandJoint, XrJointPose, XrFrame,
  XrReferenceSpace,
} from './xrapi-types'
import {XrRigidTransformImpl, matrixToTransform} from './transform'
import {XrInputSourcesChangeEvent} from './xr-input-sources-change-event'
import {XrInputSourceEvent} from './xr-input-source-event'
import {XrWebGlBindingImpl} from './webgl-binding'
import {matrixToPose} from './pose'
import {getEnabledFeatures} from './session'
import {
  createXrSpace,
  createXrReferenceSpace,
  updateSpaceOriginPose,
  getPose,
  getEffectiveOrigin,
} from './space'
import {createCallbacks} from './callbacks'

type WebGLTexture = any
type WebGLFramebuffer = any
type WebGLRenderingContext = any
type Window = any
type GLenum = any

const ERROR_CHECK = false

const OFFSCREEN_RENDERING = false

const EMULATED_WIDTH = 600
const EMULATED_HEIGHT = 300

function createShader(gl: WebGLRenderingContext, type: GLenum, source: string) {
  const shader = gl.createShader(type)
  gl.shaderSource(shader, source)
  gl.compileShader(shader)

  if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
    gl.deleteShader(shader)
    throw new Error(`An error occurred compiling the shaders: ${gl.getShaderInfoLog(shader)}`)
  }

  return shader
}

function createProgram(gl: WebGLRenderingContext, vsSource: string, fsSource: string) {
  const vertexShader = createShader(gl, gl.VERTEX_SHADER, vsSource)
  const fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fsSource)

  const program = gl.createProgram()
  gl.attachShader(program, vertexShader)
  gl.attachShader(program, fragmentShader)
  gl.linkProgram(program)

  if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
    throw new Error(`Unable to initialize the shader program: ${gl.getProgramInfoLog(program)}`)
  }

  return program
}

class EmulatedXrLayer extends EventTarget implements XrWebGlLayer {
  antialias: boolean

  ignoreDepthValues: boolean

  framebuffer: WebGLFramebuffer

  framebufferWidth: number

  framebufferHeight: number

  texture: WebGLTexture

  public _niaCtx: WebGLRenderingContext

  constructor(session: XrSession, gl: WebGLRenderingContext, layerInit?: XrWebGlLayerInit) {
    super()

    this.antialias = layerInit?.antialias ?? true
    this.ignoreDepthValues = layerInit?.ignoreDepthValues ?? false

    if (!OFFSCREEN_RENDERING) {
      this.framebuffer = null
      this.framebufferWidth = EMULATED_WIDTH
      this.framebufferHeight = EMULATED_HEIGHT
      return
    }

    this._niaCtx = gl
    this.texture = gl.createTexture()!
    gl.bindTexture(gl.TEXTURE_2D, this.texture)
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, EMULATED_WIDTH, EMULATED_HEIGHT,
      0, gl.RGBA, gl.UNSIGNED_BYTE, null)
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR)
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE)
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE)
    this.glCheck('texImage2D')

    this.framebuffer = gl.createFramebuffer()!
    this.framebufferWidth = EMULATED_WIDTH
    this.framebufferHeight = EMULATED_HEIGHT

    gl.bindFramebuffer(gl.FRAMEBUFFER, this.framebuffer)
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, this.texture, 0)

    this.glCheck('framebuffer')

    this._niaInit()
  }

  glCheck(name: string) {
    if (!ERROR_CHECK) {
      return
    }
    const err = this._niaCtx.getError()
    let code: string
    switch (err) {
      case this._niaCtx.NO_ERROR:
        return
      case this._niaCtx.INVALID_ENUM:
        code = 'INVALID_ENUM'
        break
      case this._niaCtx.INVALID_VALUE:
        code = 'INVALID_VALUE'
        break
      case this._niaCtx.INVALID_OPERATION:
        code = 'INVALID_OPERATION'
        break
      case this._niaCtx.INVALID_FRAMEBUFFER_OPERATION:
        code = 'INVALID_FRAMEBUFFER_OPERATION'
        break
      case this._niaCtx.OUT_OF_MEMORY:
        code = 'OUT_OF_MEMORY'
        break
      default:
        code = 'UNKNOWN'
        break
    }

    const message = `${name}: gl error: ${code} (${err})`
    Object.assign((globalThis as any).window, {
      requestAnimationFrame: (): number => {
        throw new Error('blocking future frames after gl error')
      },
    })
    throw new Error(message)
  }

  // eslint-disable-next-line class-methods-use-this
  getViewport(view: XrView) {
    if (view.eye === 'left') {
      return {
        x: 0,
        y: 0,
        width: 300,
        height: 300,
      }
    } else {
      return {
        x: 300,
        y: 0,
        width: 300,
        height: 300,
      }
    }
  }

  program: any

  positionBuffer: any

  positionLocation: any

  textureLocation: any

  _niaInit() {
    const gl = this._niaCtx

    // Vertex shader source
    const vsSource = `
    attribute vec2 aPosition;
    varying vec2 vTexCoord;

    void main(void) {
        vTexCoord = (aPosition + 1.0) * 0.5;
        gl_Position = vec4(aPosition, 0.0, 1.0);
    }
`

    // Fragment shader source
    const fsSource = `
    precision mediump float;
    varying vec2 vTexCoord;
    uniform sampler2D uTexture;

    void main(void) {
        gl_FragColor = texture2D(uTexture, vTexCoord);
    }
`

    this.program = createProgram(gl, vsSource, fsSource)

    if (!this.program) {
      throw new Error('Failed to create shader')
    }

    this.glCheck('program')

    gl.useProgram(this.program)

    this.positionLocation = gl.getAttribLocation(this.program, 'aPosition')
    this.textureLocation = gl.getUniformLocation(this.program, 'uTexture')

    this.positionBuffer = gl.createBuffer()
    gl.bindBuffer(gl.ARRAY_BUFFER, this.positionBuffer)
    const positions = [
      -1, -1,
      1, -1,
      -1, 1,
      -1, 1,
      1, -1,
      1, 1,
    ]
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW)
    this.glCheck('buffer')
  }

  _niaDraw() {
    if (!OFFSCREEN_RENDERING) {
      return
    }
    const gl = this._niaCtx
    gl.bindFramebuffer(gl.FRAMEBUFFER, null)
    gl.canvas.width = 600
    gl.canvas.height = 300
    gl.clearColor(0.0, 0.0, 0.0, 1.0)
    gl.clear(gl.COLOR_BUFFER_BIT)
    gl.viewport(0, 0, 600, 300)
    this.glCheck('clear')

    gl.useProgram(this.program)

    for (let i = 0; i < 8; i++) { gl.disableVertexAttribArray(i) }

    gl.bindBuffer(gl.ARRAY_BUFFER, this.positionBuffer)
    gl.enableVertexAttribArray(this.positionLocation)
    this.glCheck('enableVertexAttribArray')
    gl.vertexAttribPointer(this.positionLocation, 2, gl.FLOAT, false, 0, 0)
    this.glCheck('vertexAttribPointer')

    gl.activeTexture(gl.TEXTURE0)
    gl.bindTexture(gl.TEXTURE_2D, this.texture)
    this.glCheck('texture')

    gl.uniform1i(this.textureLocation, 0)
    this.glCheck('textureLocation')

    gl.drawArrays(gl.TRIANGLES, 0, 6)
    this.glCheck('drawArrays')

    this.glCheck('_niaDraw')
  }
}

const EYE_TO_BRIDGE_DISTANCE = 0.1
const HEAD_HEIGHT = 1.675

const generateBasePose = (yaw: number, pitch: number, move: boolean) => {
  const rotation = move ? quat.xRadians(pitch).times(quat.yRadians(yaw)) : quat.zero()
  const position = vec3.xyz(0, HEAD_HEIGHT, 0)
  return mat4.tr(position, rotation)
}

// https://jsantell.com/3d-projection/
const createPerspectiveMatrix = (
  near: number, far: number, left: number, right: number, top: number, bottom: number
) => mat4.rows([
  [(2 * near) / (right - left), 0, (right + left) / (right - left), 0],
  [0, (2 * near) / (top - bottom), (top + bottom) / (top - bottom), 0],
  [0, 0, (far + near) / (near - far), (2 * far * near) / (near - far)],
  [0, 0, -1, 0],
])

const createEyeView = (direction: number, basePose: Mat4): XrView => {
  const eyeOffset = vec3.xyz(direction * EYE_TO_BRIDGE_DISTANCE, 0, 0)

  const eyePose = mat4.t(eyeOffset).setTimes(basePose)

  const eyeTransform = matrixToTransform(eyePose)

  const projectionMatrix = createPerspectiveMatrix(
    0.1, 100, -0.1, 0.1, 0.1, -0.1
  )

  return {
    eye: (direction < 0) ? 'left' : 'right',
    transform: eyeTransform,
    projectionMatrix: new Float32Array(projectionMatrix.data()),
  }
}

const normalize = (num: number, max: number, a: number, b: number) => a + (num / max) * (b - a)

const createXrJointPose = (joint: XrJointSpace): XrJointPose => {
  const emuHandJointPoses: { [key in XrHandJoint]: { position: Vec3Source, radius: number } } = {
    'wrist': {position: vec3.xyz(0.097, 1.670, -0.296), radius: 0.021},
    'thumb-metacarpal': {position: vec3.xyz(0.139, 1.645, -0.303), radius: 0.019},
    'thumb-phalanx-proximal': {position: vec3.xyz(0.160, 1.620, -0.307), radius: 0.012},
    'thumb-phalanx-distal': {position: vec3.xyz(0.178, 1.595, -0.320), radius: 0.010},
    'thumb-tip': {position: vec3.xyz(0.190, 1.575, -0.327), radius: 0.009},
    'index-finger-metacarpal': {position: vec3.xyz(0.134, 1.653, -0.311), radius: 0.021},
    'index-finger-phalanx-proximal': {position: vec3.xyz(0.175, 1.647, -0.352), radius: 0.010},
    'index-finger-phalanx-intermediate': {position: vec3.xyz(0.194, 1.620, -0.370), radius: 0.009},
    'index-finger-phalanx-distal': {position: vec3.xyz(0.202, 1.598, -0.375), radius: 0.008},
    'index-finger-tip': {position: vec3.xyz(0.207, 1.576, -0.379), radius: 0.007},
    'middle-finger-metacarpal': {position: vec3.xyz(0.120, 1.653, -0.320), radius: 0.021},
    'middle-finger-phalanx-proximal': {position: vec3.xyz(0.158, 1.648, -0.366), radius: 0.011},
    'middle-finger-phalanx-intermediate': {position: vec3.xyz(0.168, 1.606, -0.367), radius: 0.008},
    'middle-finger-phalanx-distal': {position: vec3.xyz(0.155, 1.601, -0.343), radius: 0.008},
    'middle-finger-tip': {position: vec3.xyz(0.147, 1.621, -0.330), radius: 0.007},
    'ring-finger-metacarpal': {position: vec3.xyz(0.107, 1.654, -0.329), radius: 0.019},
    'ring-finger-phalanx-proximal': {position: vec3.xyz(0.139, 1.641, -0.371), radius: 0.010},
    'ring-finger-phalanx-intermediate': {position: vec3.xyz(0.147, 1.604, -0.366), radius: 0.008},
    'ring-finger-phalanx-distal': {position: vec3.xyz(0.138, 1.603, -0.341), radius: 0.007},
    'ring-finger-tip': {position: vec3.xyz(0.131, 1.624, -0.331), radius: 0.006},
    'pinky-finger-metacarpal': {position: vec3.xyz(0.100, 1.649, -0.332), radius: 0.018},
    'pinky-finger-phalanx-proximal': {position: vec3.xyz(0.118, 1.633, -0.371), radius: 0.008},
    'pinky-finger-phalanx-intermediate': {position: vec3.xyz(0.129, 1.605, -0.367), radius: 0.007},
    'pinky-finger-phalanx-distal': {position: vec3.xyz(0.126, 1.605, -0.347), radius: 0.006},
    'pinky-finger-tip': {position: vec3.xyz(0.121, 1.625, -0.339), radius: 0.005},
  }

  const emulatedJointPose = mat4.t(emuHandJointPoses[joint.jointName].position)
  const emulatedJoinTransform = matrixToTransform(emulatedJointPose)

  return {
    transform: emulatedJoinTransform,
    linearVelocity: {x: 0, y: 0, z: 0, w: 0},
    angularVelocity: {x: 0, y: 0, z: 0, w: 0},
    radius: emuHandJointPoses[joint.jointName].radius,
  }
}

const createXrHand = (): XrHand => new Map<XrHandJoint, XrJointSpace>([
  ['wrist', {jointName: 'wrist'}],
  ['thumb-metacarpal', {jointName: 'thumb-metacarpal'}],
  ['thumb-phalanx-proximal', {jointName: 'thumb-phalanx-proximal'}],
  ['thumb-phalanx-distal', {jointName: 'thumb-phalanx-distal'}],
  ['thumb-tip', {jointName: 'thumb-tip'}],
  ['index-finger-metacarpal', {jointName: 'index-finger-metacarpal'}],
  ['index-finger-phalanx-proximal', {jointName: 'index-finger-phalanx-proximal'}],
  ['index-finger-phalanx-intermediate', {jointName: 'index-finger-phalanx-intermediate'}],
  ['index-finger-phalanx-distal', {jointName: 'index-finger-phalanx-distal'}],
  ['index-finger-tip', {jointName: 'index-finger-tip'}],
  ['middle-finger-metacarpal', {jointName: 'middle-finger-metacarpal'}],
  ['middle-finger-phalanx-proximal', {jointName: 'middle-finger-phalanx-proximal'}],
  ['middle-finger-phalanx-intermediate', {jointName: 'middle-finger-phalanx-intermediate'}],
  ['middle-finger-phalanx-distal', {jointName: 'middle-finger-phalanx-distal'}],
  ['middle-finger-tip', {jointName: 'middle-finger-tip'}],
  ['ring-finger-metacarpal', {jointName: 'ring-finger-metacarpal'}],
  ['ring-finger-phalanx-proximal', {jointName: 'ring-finger-phalanx-proximal'}],
  ['ring-finger-phalanx-intermediate', {jointName: 'ring-finger-phalanx-intermediate'}],
  ['ring-finger-phalanx-distal', {jointName: 'ring-finger-phalanx-distal'}],
  ['ring-finger-tip', {jointName: 'ring-finger-tip'}],
  ['pinky-finger-metacarpal', {jointName: 'pinky-finger-metacarpal'}],
  ['pinky-finger-phalanx-proximal', {jointName: 'pinky-finger-phalanx-proximal'}],
  ['pinky-finger-phalanx-intermediate', {jointName: 'pinky-finger-phalanx-intermediate'}],
  ['pinky-finger-phalanx-distal', {jointName: 'pinky-finger-phalanx-distal'}],
  ['pinky-finger-tip', {jointName: 'pinky-finger-tip'}],
])

const createEmulatedRequestSession = (
  window: Window
) => async (mode: XrSessionMode, options?: XrSessionInit): Promise<XrSession> => {
  if (mode !== 'immersive-vr' && mode !== 'immersive-ar') {
    throw new Error(`[xrapi] Session mode not supported: ${mode}`)
  }

  const callbacks_ = createCallbacks<XrFrameCallback>()
  let session_: XrSession
  let currentFrame_: XrFrame
  let rotation_ = 0

  const simMovement_ = false
  const eventTarget_ = new EventTarget()

  const renderState_ = {
    baseLayer: <EmulatedXrLayer | null>null,

    // TODO: Enable XrProjectionLayer
    // layers: [],
  }

  const setControllerPosition = (
    source: XrInputSource,
    x: number,
    y: number,
    z: number,
    yaw: number
  ) => {
    const rotation = quat.xRadians(1.5).times(quat.yRadians(yaw))
    const pose = matrixToPose(mat4.tr({x, y, z}, rotation))
    updateSpaceOriginPose(source.targetRaySpace, pose)
    updateSpaceOriginPose(source.gripSpace, pose)
  }

  const dispatchFrame = (time: number) => {
    window.requestAnimationFrame(dispatchFrame)

    const [yaw, pitch] = [Math.sin(time * 0.001), Math.sin(time * 0.0016) * 0.12]

    currentFrame_ = {
      session: session_,
      getViewerPose: (space: XrReferenceSpace) => {
        const basePose = generateBasePose(yaw, pitch, simMovement_).times(
          mat4.of(Array.from(getEffectiveOrigin(space).matrix))
        )
        return {
          views: [
            createEyeView(-1, basePose),
            createEyeView(1, basePose),
          ],
        }
      },
      getPose,
      getJointPose: (joint: XrJointSpace) => (
        createXrJointPose(joint)
      ),
    }

    callbacks_.dispatch(time, currentFrame_)

    renderState_.baseLayer?._niaDraw()
  }

  const makeInputSource = (handedness_: XrHandedness, isHand: boolean = false): XrInputSource => {
    const startPos = handedness_ === 'left' ? -0.2 : 0.2

    const controller: XrInputSource = {
      targetRayMode: 'tracked-pointer',
      gripSpace: createXrSpace(session_),
      profiles: ['oculus-touch-v2'],
      targetRaySpace: createXrSpace(session_),
      handedness: handedness_,
      hand: isHand ? createXrHand() : null,
      gamepad: {
        id: 'oculus-touch-v2',
        index: 0,
        mapping: 'xr-standard',
        connected: true,
        timestamp: 0,
        buttons: [
          {pressed: false, touched: false, value: 0},
          {pressed: false, touched: false, value: 0},
          {pressed: false, touched: false, value: 0},
          {pressed: false, touched: false, value: 0},
        ],
        axes: [0, 0],
        hapticActuators: [],
      },
    }
    setControllerPosition(controller, startPos, HEAD_HEIGHT, -0.5, 0)
    return controller
  }

  const enabledFeatures = getEnabledFeatures(mode, options, () => true)
  if (!enabledFeatures) {
    throw new Error('[xrapi] Required session features not supported')
  }

  const defaultFrameRate = 60
  const availableFrameRates = new Float32Array([60, 120])
  const updateTargetFrameRate = async (frameRate: number) => {
    if (!availableFrameRates.includes(frameRate)) {
      throw new Error(`[xrapi] Frame rate ${frameRate} not supported`)
    }

    // TODO(lreyna): Implement frame rate change
  }

  session_ = {
    visibilityState: 'visible',
    requestReferenceSpace: async (type: XrReferenceSpaceType) => {
      const referenceSpace = createXrReferenceSpace(session_, type)
      updateSpaceOriginPose(referenceSpace, matrixToPose(mat4.i()))
      return referenceSpace
    },
    requestAnimationFrame: callbacks_.add,
    cancelAnimationFrame: callbacks_.remove,
    renderState: renderState_,
    updateRenderState: (args) => {
      Object.assign(renderState_, args)
    },
    addEventListener: (event: string, listener: () => void) => {
      eventTarget_.addEventListener(event, listener)
    },
    dispatchEvent: (event: Event) => {
      eventTarget_.dispatchEvent(event)
    },
    end: () => {
      console.error('[emulated] session.end not implemented')
    },
    // inputSources must initially be an empty list
    inputSources: [],
    enabledFeatures,
    frameRate: defaultFrameRate,
    supportedFrameRates: availableFrameRates,
    updateTargetFrameRate,
  }

  setTimeout(() => {
    const added = [makeInputSource('left'), makeInputSource('right')]
    eventTarget_.dispatchEvent(new XrInputSourcesChangeEvent('inputsourceschange', {
      session: session_,
      added,
      removed: [],
    }))
    session_.inputSources = added
  }, 300)

  window.addEventListener('pointerdown', () => {
    const source = session_.inputSources[0]
    if (!source || !currentFrame_) {
      return
    }

    session_.dispatchEvent(new XrInputSourceEvent('select', {
      frame: currentFrame_,
      inputSource: source,
    }))
    session_.dispatchEvent(new XrInputSourceEvent('selectstart', {
      frame: currentFrame_,
      inputSource: source,
    }))
  })

  window.addEventListener('pointerup', () => {
    const source = session_.inputSources[0]
    if (!source || !currentFrame_) {
      return
    }

    session_.dispatchEvent(new XrInputSourceEvent('selectend', {
      frame: currentFrame_,
      inputSource: source,
    }))
  })

  window.addEventListener('pointermove', (event: Event) => {
    const source = session_.inputSources[0]
    if (!source) {
      return
    }

    const newX = normalize((event as any).clientX, 800, -5, 5)
    const newY = normalize((event as any).clientY, 600, 5, -5)
    const currentZ = getEffectiveOrigin(source.targetRaySpace).position.z

    setControllerPosition(source, newX, newY, currentZ, rotation_)
  })

  window.addEventListener('wheel', (event: Event) => {
    const source = session_.inputSources[0]
    if (!source) {
      return
    }
    const {
      x: currentX, y: currentY, z: currentZ,
    } = getEffectiveOrigin(source.targetRaySpace).position

    const newZ = currentZ + (event as any).deltaY * 0.001
    rotation_ += (event as any).deltaX * 0.01

    setControllerPosition(source, currentX, currentY, newZ, rotation_)
  })

  window.requestAnimationFrame(dispatchFrame)

  return session_
}

const createEmulatedXr = (window: Window): XrSystem => {
  const eventTarget = new EventTarget()

  return {
    requestSession: createEmulatedRequestSession(window),
    isSessionSupported: async () => true,
    addEventListener: eventTarget.addEventListener.bind(eventTarget),
    dispatchEvent: eventTarget.dispatchEvent.bind(eventTarget),
    removeEventListener: eventTarget.removeEventListener.bind(eventTarget),
    offerSession: createEmulatedRequestSession(window),
  }
}

const createEmulatedXrApi = (window: Window): XrApi => ({
  xr: createEmulatedXr(window),
  XrWebGlLayerClass: EmulatedXrLayer,
  XrWebGlBindingClass: XrWebGlBindingImpl,
  XrRigidTransformClass: XrRigidTransformImpl,
})

export {
  createEmulatedXrApi,
}
