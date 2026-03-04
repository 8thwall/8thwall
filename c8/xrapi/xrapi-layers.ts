import type {
  XrCompositionLayer,
  XrLayerLayout,
  XrProjectionLayer,
  XrRigidTransform,
  XrTextureType,
  XrWebGlRenderingContext,
} from './xrapi-types'

import type {InternalXrSession} from './session'

// https://immersive-web.github.io/layers/#list-of-color-formats-for-projection-layers
const REQUIRED_WEBGL_COLOR_FORMATS = [
  0x1907,  // RGB
  0x1908,  // RGBA
  0x8C40,  // SRGB_EXT
  0x8C41,  // SRGB_ALPHA_EXT
]

const REQUIRED_WEBGL2_COLOR_FORMATS = [
  ...REQUIRED_WEBGL_COLOR_FORMATS,
  0x8051,  // RGB8
  0x8058,  // RGBA8
  0x8C41,  // SRGB8
  0x8C43,  // SRGB8_ALPHA8
]

// https://immersive-web.github.io/layers/#list-of-depth-formats-for-projection-layers
const REQUIRED_WEBGL_DEPTH_FORMATS = [
  0x1902,  // DEPTH_COMPONENT
  0x84F9,  // DEPTH_STENCIL
]

const REQUIRED_WEBGL2_DEPTH_FORMATS = [
  ...REQUIRED_WEBGL_DEPTH_FORMATS,
  0x81A6,  // DEPTH_COMPONENT24
  0x88F0,  // DEPTH24_STENCIL8
]

const REQUIRED_MOTION_VECTOR_FORMATS = [
  0x881A,  // RGBA16F
]

type WebGLTexture = any & {_: number}  // Set by headless-gl

// https://immersive-web.github.io/layers/#xrcompositionlayertype
type InternalXrCompositionLayer = XrCompositionLayer & {
  _session: InternalXrSession

  _context: XrWebGlRenderingContext | null

  // XRCompositionLayer has an internal boolean isStatic that indicates
  // that the author can only draw to this layer when needsRedraw is true.
  _isStatic: boolean

  _colorTextures: WebGLTexture[]

  _depthStencilTextures: WebGLTexture[]

  _motionVectorTextures: WebGLTexture[]

  // Remove readonly modifier from XrCompositionLayer
  needsRedraw: boolean

  mipLevels: number

  layout: XrLayerLayout
}

// https://immersive-web.github.io/layers/#intialize-a-composition-layer
// [Implementation Note]: Pass layer, to use internal properties.
const initCompositionLayer = (
  layer: XrCompositionLayer, session: InternalXrSession, context?: XrWebGlRenderingContext
): void => {
  const layerInternal = layer as InternalXrCompositionLayer

  layerInternal._session = session
  layerInternal._context = context ?? layerInternal._context
  layerInternal.blendTextureSourceAlpha = true
  layerInternal.opacity = 1
}

type InternalXrProjectionLayer = XrProjectionLayer & InternalXrCompositionLayer & {
  _secondaryColorTextures: WebGLTexture[] | null

  _secondaryDepthStencilTextures: WebGLTexture[] | null

  _secondaryMotionVectorTextures: WebGLTexture[] | null

  // Remove readonly modifier from XrProjectionLayer
  textureWidth: number

  textureHeight: number

  textureArrayLength: number

  ignoreDepthValues: boolean

  // [IMPLEMENTATION NOTE]: Internal property used for top level getter/setter
  _fixedFoveation: number
  _deltaPose?: XrRigidTransform
}

const newXrProjectionLayer = (): InternalXrProjectionLayer => ({} as InternalXrProjectionLayer)

enum TextureAllocationType {
  Color,
  DepthStencil,
  MotionVector,
}

const allocateTextures = (
  layer: XrProjectionLayer,
  textureType: XrTextureType,
  textureFormat: number,
  scaleFactor: number,
  type: TextureAllocationType
): WebGLTexture[] => {
  const castedLayer = layer as InternalXrProjectionLayer

  // 1. Let array be a new array in the relevant realm of context.
  const array: WebGLTexture[] = []

  // 2. Let context be layer’s context.
  const context = castedLayer._context

  // 3. Let session be layer’s session.
  const session = castedLayer._session

  // 4. Let numViews be the number of the session’s list of views excluding the secondary views.
  const numViews = session._views.length

  // 5. Let view be the first entry in the session’s list of views that is not a secondary views.
  // [NOT IMPLEMENTED]
  const view = session._views[0]

  // 6. Let width be the width of view’s recommended WebGL color texture resolution
  // multiplied by scaleFactor.
  let width

  // 7. Let height be the height of view’s recommended WebGL color texture resolution
  // multiplied by scaleFactor.
  let height

  // 8. If textureFormat is not in the list of color formats for projection layers,
  // throw a NotSupportedError and abort these steps.
  let validFormatWebgl1: number[] = []
  let validFormatWebgl2: number[] = []
  switch (type) {
    case TextureAllocationType.Color:
      width = view._recommendedColorWidth * scaleFactor
      height = view._recommendedColorHeight * scaleFactor
      validFormatWebgl1 = REQUIRED_WEBGL_COLOR_FORMATS
      validFormatWebgl2 = REQUIRED_WEBGL2_COLOR_FORMATS
      break
    case TextureAllocationType.DepthStencil:
      width = view._recommendedDepthWidth * scaleFactor
      height = view._recommendedDepthHeight * scaleFactor
      validFormatWebgl1 = REQUIRED_WEBGL_DEPTH_FORMATS
      validFormatWebgl2 = REQUIRED_WEBGL2_DEPTH_FORMATS
      break
    case TextureAllocationType.MotionVector:
      width = view._recommendedMotionVectorWidth * scaleFactor
      height = view._recommendedMotionVectorHeight * scaleFactor
      validFormatWebgl2 = REQUIRED_MOTION_VECTOR_FORMATS
      break
    default:
      throw new Error('NotSupportedError')
  }

  // Check if webgl2 only method exists
  const isWebGL2 = !!context.renderbufferStorageMultisample
  if ((!isWebGL2 && !validFormatWebgl1.includes(textureFormat)) ||
      (isWebGL2 && !validFormatWebgl2.includes(textureFormat))) {
    throw new Error('NotSupportedError: Invalid texture format')
  }

  // 9. If layer’s layout is "mono" or "default":
  if (castedLayer.layout === 'mono' || castedLayer.layout === 'default') {
    // 9.1 If textureType is "texture-array":
    if (textureType === 'texture-array') {
      // 9.1.1 If the session’s views in the list of views don’t all have the same
      // recommended WebGL color texture resolution excluding the secondary views,
      // throw a NotSupportedError and abort these steps.
      // [NOT IMPLEMENTED]

      // 9.1.2 Initialize array with 1 new instance of an opaque texture in the
      // relevant realm of context created as a TEXTURE_2D_ARRAY texture with numViews layers
      // using context, textureFormat, width and height.
      const tex = context.createTexture()
      context.bindTexture(context.TEXTURE_2D_ARRAY, tex)
      context.texStorage3D(context.TEXTURE_2D_ARRAY, 1, textureFormat, width, height, numViews)
      context.bindTexture(context.TEXTURE_2D_ARRAY, null)
      array.push(tex)

      // 9.1.3 Return array and abort these steps.
      return array
    } else {
      // 9.2 Otherwise: For each view in the session’s list of views:
      for (let i = 0; i < numViews; i++) {
        // 9.2.1 If view is a secondary view, continue.
        // [NOT IMPLEMENTED]

        // 9.2.2 - 9.2.3 Let view dimension be dimenstion multiplied by scaleFactor.
        // [IMPLEMENTATION NOTE]: These are set above in the switch statement
        const viewWidth = width
        const viewHeight = height

        // 9.2.4 let texture be a new instance of an opaque texture in the relevant realm of context
        // created as a TEXTURE_2D texture with context, textureFormat, width and height.
        const tex = context.createTexture()
        context.bindTexture(context.TEXTURE_2D, tex)
        context.texStorage2D(context.TEXTURE_2D, 1, textureFormat, viewWidth, viewHeight)
        context.bindTexture(context.TEXTURE_2D, null)
        array.push(tex)
      }
      return array
    }
  }

  // 10. If the session’s views in the list of views don’t all have the same recommended WebGL
  // color texture resolution excluding the secondary views,
  // throw a NotSupportedError and abort these steps.
  // [NOT IMPLEMENTED]

  // 11. If layer’s layout is stereo-left-right, initialize array with 1 new instance of
  // opaque texture in the relevant realm of context created as a textureType texture using context,
  // textureFormat, numViews multiplied by width and height.

  // 12. If layer’s layout is stereo-top-bottom, initialize array with 1 new instance of opaque
  // texture in the relevant realm of context created as a textureType texture using context,
  // textureFormat, width and numViews multiplied by height.

  if (castedLayer.layout === 'stereo-left-right' || castedLayer.layout === 'stereo-top-bottom') {
    const tex = context.createTexture()
    context.bindTexture(context.TEXTURE_2D, tex)

    const viewWidth = width * (castedLayer.layout === 'stereo-left-right' ? numViews : 1)
    const viewHeight = height * (castedLayer.layout === 'stereo-top-bottom' ? numViews : 1)

    context.texStorage2D(context.TEXTURE_2D, 1, textureFormat, viewWidth, viewHeight)
    context.bindTexture(context.TEXTURE_2D, null)
    array.push(tex)
  }

  // 13. Return array.
  return array
}

const getTextureType = (layer: XrCompositionLayer): XrTextureType => {
  const {textureArrayLength} = layer as XrProjectionLayer
  if (textureArrayLength) {
    return textureArrayLength > 1 ? 'texture-array' : 'texture'
  }

  // Add more cases for other layer types
  return 'texture'
}

export {
  type InternalXrCompositionLayer,
  type InternalXrProjectionLayer,
  TextureAllocationType,
  allocateTextures,
  getTextureType,
  initCompositionLayer,
  newXrProjectionLayer,
}
