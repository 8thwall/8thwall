/* eslint-disable class-methods-use-this */
/* eslint-disable @typescript-eslint/no-unused-vars */
import type {
  XrCompositionLayer,
  XrFrame,
  XrEye,
  XrSession,
  XrTextureType,
  XrProjectionLayer,
  XrProjectionLayerInit,
  XrView,
  XrWebGlBinding,
  XrWebGlRenderingContext,
  XrWebGlSubImage,
  XrLayerLayout,
  XrViewport,
} from './xrapi-types'

import {
  type InternalXrCompositionLayer,
  TextureAllocationType,
  allocateTextures,
  initCompositionLayer,
  getTextureType,
  newXrProjectionLayer,
} from './xrapi-layers'

import type {InternalXrSession} from './session'
import type {InternalXrView} from './view'

type WebGLTexture = any & {_: number}  // Set by headless-gl

type InternalXrWebGlSubImage = XrWebGlSubImage & {
  // Removes readonly modifier from XrWebGlSubImage
  colorTexture: WebGLTexture
  depthStencilTexture?: WebGLTexture
  motionVectorTexture?: WebGLTexture

  imageIndex?: number
  colorTextureWidth: number
  colorTextureHeight: number
  depthStencilTextureWidth?: number
  depthStencilTextureHeight?: number
  motionVectorTextureWidth?: number
  motionVectorTextureHeight?: number
}

const defaultXrProjectionLayerInit: XrProjectionLayerInit = {
  textureType: 'texture',
  colorFormat: 0x1908,  // RGBA
  depthFormat: 0x1902,  // DEPTH_COMPONENT
  scaleFactor: 1.0,
  clearOnAccess: true,
}

class XrWebGlBindingImpl implements XrWebGlBinding {
  private _session: InternalXrSession

  private _context: XrWebGlRenderingContext

  // TODO: Proper Caching
  private _lastView: InternalXrView | null = null

  private _lastLayer: InternalXrCompositionLayer | null = null

  private _lastSubImage: InternalXrWebGlSubImage | null = null

  constructor(session: XrSession, context: XrWebGlRenderingContext) {
    // TODO: Validate context / session with spec:
    // https://immersive-web.github.io/layers/#XRWebGLBindingtype
    this._context = context

    this._session = session as InternalXrSession
  }

  readonly nativeProjectionScaleFactor: number = 1

  readonly usesDepthValues: boolean = false

  // https://immersive-web.github.io/layers/#determine-the-layout-attribute
  private determineLayoutAttribute = (
    textureType: XrTextureType, context: XrWebGlRenderingContext, layout: XrLayerLayout
  ): XrLayerLayout => {
    // 1. If context is not an WebGL2RenderingContext and textureType is "texture-array",
    // throw TypeError and abort these steps.

    // Check if context has a WebGL2 only method
    const isWebGL2 = !!context.renderbufferStorageMultisample
    if (isWebGL2 && textureType === 'texture-array') {
      throw new TypeError('[xr-api] XrWebGlBinding: texture-array requires WebGL2RenderingContext')
    }

    // 2. If textureType is "texture-array" and not all of the session’s views in the list of views
    // have the same recommended WebGL color texture resolution,
    // throw a NotSupportedError and abort these steps.
    if (textureType === 'texture-array') {
      // [NOT IMPLEMENTED]
    }

    // 3. If layout is "mono", return layout and abort these steps.
    if (layout === 'mono') {
      return layout
    }

    // 4. If layout is "default", run the following steps:
    // 4.1 If the size of list of views is 1, return "mono" and abort these steps.
    if (this._session._views.length === 1) {
      return 'mono'
    }

    // 4.2 If textureType is "texture-array", return layout and abort these steps.
    if (textureType === 'texture-array') {
      return layout
    }

    // 5. If layout is "default" or "stereo" and textureType is "texture", run the following steps:
    if ((layout === 'default' || layout === 'stereo') && textureType === 'texture') {
      // 5.1 If the user agent prefers "stereo-left-right" layout,
      // return "stereo-left-right" and abort these steps.
      return 'stereo-left-right'
    }

    // 6. Return layout.
    return layout
  }

  private determineMaximumScaleFactor = (
    context: XrWebGlRenderingContext, layout: XrLayerLayout
  ): number => {
    // 1. Let largest width be the largest width of the recommended WebGL color texture resolution
    // from the session’s list of views excluding the secondary views.
    // TODO: Check against session's list of views
    let largestWidth = this._session._views[0]._recommendedColorWidth

    // 2. Let largest height be the largest height of the recommended WebGL color texture resolution
    // from the session’s list of views excluding the secondary views.
    let largestHeight = this._session._views[0]._recommendedColorHeight

    // 3. If layout is "stereo-left-right" layout, multiply largest width by 2.
    if (layout === 'stereo-left-right') {
      largestWidth *= 2
    }

    // 4. If layout is "stereo-top-bottom" layout, multiply largest height by 2.
    if (layout === 'stereo-top-bottom') {
      largestHeight *= 2
    }

    // 5. Let largest view dimension be the largest of largest width or largest height.
    const largestViewDimension = Math.max(largestWidth, largestHeight)

    // 6. Let largest texture dimension be the largest dimension of a
    // WebGLTexture created by context.
    const largestTextureDimension = context.getParameter(context.MAX_TEXTURE_SIZE)

    // 7. Return largest texture dimension divided by largest view dimension.
    return largestTextureDimension / largestViewDimension
  }

  // https://immersive-web.github.io/layers/#initialize-the-viewport
  private initializeViewport = (
    viewport: XrViewport,
    texture: WebGLTexture,
    layout: XrLayerLayout,
    offset: number,
    numViews: number
  ): void => {
    let x = 0
    let y = 0
    let width = texture._levelWidth[0]  // Defined by headless-gl
    let height = texture._levelHeight[0]

    // If layout is "stereo-left-right":
    // Set viewport’s x to the pixel width of texture multiplied by offset and divided by num.
    // Set viewport’s width to the pixel width of subimage’s texture divided by num.
    if (layout === 'stereo-left-right') {
      x = (width * offset) / numViews
      width /= numViews
    } else if (layout === 'stereo-top-bottom') {
      // Set viewport’s y to the pixel height of texture multiplied by offset and divided by num.
      // Set viewport’s height to the pixel height of subimage’s texture divided by num.
      y = (height * offset) / numViews
      height /= numViews
    }

    viewport.x = x
    viewport.y = y
    viewport.width = width
    viewport.height = height
  }

  // https://immersive-web.github.io/layers/#dom-xrwebglbinding-createprojectionlayer
  createProjectionLayer(layerInit: XrProjectionLayerInit): XrProjectionLayer {
    // [IMPLEMENTATION NOTE]
    // Apply defaults to layerInit
    const newLayerInit = {...defaultXrProjectionLayerInit, ...layerInit}

    // 1. Let layer be a new XRProjectionLayer in the relevant realm of this.
    const layer = newXrProjectionLayer()

    // 2. If session’s ended value is true, throw InvalidStateError and abort these steps.
    // [NOT IMPLEMENTED]

    // 3. If context is lost, throw InvalidStateError and abort these steps.
    // [NOT IMPLEMENTED]

    // 4. Run intialize a composition layer on layer with session and context.
    initCompositionLayer(layer, this._session, this._context)

    // 5. Initialize layer’s isStatic to false.
    layer._isStatic = false

    // 6. Initialize layer’s ignoreDepthValues as follows:
    // If init’s depthFormat is false and the XR Compositor will make use of depth values
    //    Initialize layer’s ignoreDepthValues to false
    // Otherwise
    //    Initialize layer’s ignoreDepthValues to true
    layer.ignoreDepthValues = !(!newLayerInit.depthFormat && this.usesDepthValues)

    // 7. Initialize layer’s fixedFoveation to 0.
    layer.fixedFoveation = 0

    // 8. let layout be the result of determining the layout attribute with init’s textureType,
    // context and "default"
    layer.layout = this.determineLayoutAttribute(
      newLayerInit.textureType!, this._context, 'default'
    )

    // 9. Let maximum scalefactor be the result of determining the maximum scalefactor with session,
    // context and layout.
    const maximumScaleFactor = this.determineMaximumScaleFactor(this._context, layer.layout)

    // 10. If scaleFactor is larger than maximum scalefactor, set scaleFactor to maximum scalefactor
    newLayerInit.scaleFactor = Math.min(
      newLayerInit.scaleFactor!, maximumScaleFactor
    )

    // 11. Initialize layer’s needsRedraw to true.
    layer.needsRedraw = true

    // 12. let layer’s colorTextures be the result of allocating color textures for
    // projection layers with layer, init’s textureType, init’s colorFormat and init’s scaleFactor.
    layer._colorTextures = allocateTextures(
      layer,
      newLayerInit.textureType!,
      newLayerInit.colorFormat!,
      newLayerInit.scaleFactor!,
      TextureAllocationType.Color
    )

    // 13. let layer’s depthStencilTextures be the result of allocating depth textures for
    // projection layers with layer, init’s textureType, init’s depthFormat and init’s scaleFactor.
    layer._depthStencilTextures = allocateTextures(
      layer,
      newLayerInit.textureType!,
      newLayerInit.depthFormat!,
      newLayerInit.scaleFactor!,
      TextureAllocationType.DepthStencil
    )

    // 14. let layer’s motionVectorTextures be the result of allocating motion vector textures for
    // projection layers with layer, init’s depthFormat and init’s scaleFactor.
    layer._motionVectorTextures = allocateTextures(
      layer,
      newLayerInit.textureType!,
      0x881A,  // RGBA16F
      newLayerInit.scaleFactor!,
      TextureAllocationType.MotionVector
    )

    // 15. Initialize the colortextures for secondary views as follows:
    if (this._session.enabledFeatures.includes('secondary-views')) {
      // 15.1 If the session was created with "secondary-views" enabled
      // Let colortextures for secondary views be the result of allocate the color textures for
      // secondary views with layer, init’s textureType, init’s colorFormat and init’s scaleFactor.
      layer._secondaryColorTextures = allocateTextures(
        layer,
        newLayerInit.textureType!,
        newLayerInit.colorFormat!,
        newLayerInit.scaleFactor!,
        TextureAllocationType.Color
      )
    } else {
      // 15.2 Otherwise, set colortextures for secondary views to null.
      layer._secondaryColorTextures = null
    }

    // 16. Initialize the depthstenciltextures for secondary views as follows:
    if (this._session.enabledFeatures.includes('secondary-views')) {
      // 16.1 If the session was created with "secondary-views" enabled
      // Let depthstenciltextures for secondary views be the result of allocate the depth textures
      // secondary views with layer, init’s textureType, init’s depthFormat and init’s scaleFactor.
      layer._secondaryDepthStencilTextures = allocateTextures(
        layer,
        newLayerInit.textureType!,
        newLayerInit.depthFormat!,
        newLayerInit.scaleFactor!,
        TextureAllocationType.DepthStencil
      )
    } else {
      // 16.2 Otherwise, set depthstenciltextures for secondary views to null.
      layer._secondaryDepthStencilTextures = null
    }

    // 17. If the XR Compositor knows that it will be unable to create the resources for the layer
    // for any reason, throw an OperationError and abort these steps.
    // [NOT IMPLEMENTED]

    // eslint-disable-next-line prefer-destructuring
    layer.textureWidth = layer._colorTextures[0]._levelWidth[0]

    // eslint-disable-next-line prefer-destructuring
    layer.textureHeight = layer._colorTextures[0]._levelHeight[0]

    layer.textureArrayLength = (newLayerInit.textureType === 'texture-array')
      ? layer._colorTextures[0]._levelDepth[0]
      : 1

    // 18. Return layer.
    return layer as XrProjectionLayer
  }

  getSubImage(layer: XrCompositionLayer, frame: XrFrame, eye?: XrEye): XrWebGlSubImage {
    // https://immersive-web.github.io/layers/#dom-xrwebglbinding-getsubimage
    return {} as XrWebGlSubImage
  }

  // https://immersive-web.github.io/layers/#dom-xrwebglbinding-getviewsubimage
  getViewSubImage(layer: XrCompositionLayer, view: XrView): XrWebGlSubImage {
    const castedLayer = layer as InternalXrCompositionLayer
    const castedView = view as InternalXrView

    // 1. Initialize subimage as follows:
    let subImage: InternalXrWebGlSubImage
    if (castedLayer === this._lastLayer && castedView === this._lastView) {
      // 1.1 If getViewSubImage() was called previously with the same binding,
      // layer and view, the user agent MAY
      // Let subimage be the same XRWebGLSubImage object as returned by an
      // earlier call with the same arguments.
      subImage = this._lastSubImage!
    } else {
      // 1.2 Otherwise
      // Let subimage be a new XRWebGLSubImage in the relevant realm of this.
      subImage = {
        viewport: {
          x: 0,
          y: 0,
          width: 0,
          height: 0,
        },
      } as InternalXrWebGlSubImage
    }

    // 2. Let frame be view’s frame.
    const frame = castedView._frame

    // 3. Let session be this session.
    const session = this._session

    // 4. If validate the state of the XRWebGLSubImage creation function with layer
    // and frame is false, throw an InvalidStateError and abort these steps.
    // [NOT IMPLEMENTED]

    // 5. If layer is not in the session’s layers array, throw a TypeError and abort these steps.
    // [NOT IMPLEMENTED]

    // 6. If view’s active flag is false, throw an InvalidStateError and abort these steps.
    // [NOT IMPLEMENTED]

    // 7. Initialize index as follows:
    // 7.1 If view is a secondary view from session’s list of views
    // Let index be the offset of view’s view in session’s list of views
    // excluding the primary views.
    // [NOT IMPLEMENTED]

    // 7.2 Otherwise
    // Let index be the offset of view in session’s list of views.
    const index = session._views.indexOf(castedView)

    // 8. Initialize subimage’s imageIndex as follows:
    let imageIndex: number
    if (getTextureType(castedLayer) === 'texture-array') {
      // 8.1 If the layer was created with a textureType of "texture-array":
      // Initialize subimage’s imageIndex with index.
      imageIndex = index
    } else {
      // 8.2 Otherwise:
      // Initialize subimage’s imageIndex with 0.
      imageIndex = 0
    }
    subImage.imageIndex = imageIndex

    // 9. Initialize subimage’s colorTexture as follows:
    let colorTexture: WebGLTexture
    // 9.1 If view is a secondary view from session’s list of views
    // Initialize subimage’s colorTexture with the element at offset index of the layer’s
    // colorTextures for secondary views.
    // [NOT IMPLEMENTED]

    if (castedLayer.layout === 'default' && getTextureType(castedLayer) === 'texture') {
      // 9.2 Else if the layer’s layout is "default" and
      // the layer was created with a textureType of "texture"
      // Initialize subimage’s colorTexture with the element at offset
      // index of the layer’s colorTextures array.
      colorTexture = castedLayer._colorTextures[index]
    } else {
      // 9.3 Otherwise:
      // Initialize subimage’s colorTexture with the first element of the
      // layer’s colorTextures array.
      // eslint-disable-next-line prefer-destructuring
      colorTexture = castedLayer._colorTextures[0]
    }
    subImage.colorTexture = colorTexture

    // 10. Initialize subimage’s depthStencilTexture as follows:
    let depthStencilTexture: WebGLTexture
    if (!castedLayer._depthStencilTextures || castedLayer._depthStencilTextures.length === 0) {
      // 10.1 If the layer’s depthStencilTextures is an empty array
      // Initialize subimage’s depthStencilTexture with null.
      depthStencilTexture = null

      // 10.2 Else if view is a secondary view from session’s list of views
      // Initialize subimage’s colorTexture with the element at offset index
      // of the layer’s depthStencilTextures for secondary views.
      // [NOT IMPLEMENTED]
    } else if (castedLayer.layout === 'default' && getTextureType(castedLayer) === 'texture') {
      // 10.3 Else if the layer’s layout is "default" and the layer was created with a
      // textureType of "texture"
      // Initialize subimage’s depthStencilTexture with the element at offset index of the
      // layer’s depthStencilTextures array.
      depthStencilTexture = castedLayer._depthStencilTextures[index]
    } else {
      // 10.4 Otherwise
      // Initialize subimage’s depthStencilTexture with the first element of
      // layer’s depthStencilTextures array.
      // eslint-disable-next-line prefer-destructuring
      depthStencilTexture = castedLayer._depthStencilTextures[0]
    }
    subImage.depthStencilTexture = depthStencilTexture

    // 11. Initialize subimage’s motionVectorTexture as follows:
    let motionVectorTexture: WebGLTexture
    if (!castedLayer._motionVectorTextures || castedLayer._motionVectorTextures.length === 0) {
      // 11.1 If the layer’s motionVectorTextures is an empty array
      // Initialize subimage’s motionVectorTexture with null.
      motionVectorTexture = null

      // 11.2 Else if view is a secondary view from session’s list of views
      // Initialize subimage’s colorTexture with the element at offset index
      // of the layer’s motionVectorTextures for secondary views.
      // [NOT IMPLEMENTED]
    } else if (castedLayer.layout === 'default' && getTextureType(castedLayer) === 'texture') {
      // 11.3 Else if the layer’s layout is "default" and the layer was created with a
      // textureType of "texture"
      // Initialize subimage’s motionVectorTexture with the element at offset index of the
      // layer’s motionVectorTextures array.
      motionVectorTexture = castedLayer._motionVectorTextures[index]
    } else {
      // 11.4 Otherwise
      // Initialize subimage’s motionVectorTexture with the first element of
      // layer’s motionVectorTextures array.
      // eslint-disable-next-line prefer-destructuring
      motionVectorTexture = castedLayer._motionVectorTextures[0]
    }
    subImage.motionVectorTexture = motionVectorTexture

    // 12. Set subimage’s colorTextureWidth to the pixel width of subimage’s colorTexture.
    // eslint-disable-next-line prefer-destructuring
    subImage.colorTextureWidth = colorTexture._levelWidth[0]

    // 13. Set subimage’s colorTextureHeight to the pixel height of subimage’s colorTexture.
    // eslint-disable-next-line prefer-destructuring
    subImage.colorTextureHeight = colorTexture._levelHeight[0]

    // 14. If subimage’s depthStencilTexture is not null, set subimage’s depthStencilTextureWidth
    // to the pixel width of the first texture in the depthStencilTexture array.

    // 15. If subimage’s depthStencilTexture is not null, set subimage’s depthStencilTextureHeight
    // to the pixel height of the first texture in the depthStencilTexture array.
    if (depthStencilTexture) {
      // eslint-disable-next-line prefer-destructuring
      subImage.depthStencilTextureWidth = depthStencilTexture._levelWidth[0]

      // eslint-disable-next-line prefer-destructuring
      subImage.depthStencilTextureHeight = depthStencilTexture._levelHeight[0]
    }

    // 16. If subimage’s motionVectorTexture is not null, set subimage’s motionVectorTextureWidth
    // to the pixel width of the first texture in the motionVectorTexture array.

    // 17. If subimage’s motionVectorTexture is not null, set subimage’s motionVectorTextureHeight
    // to the pixel height of the first texture in the motionVectorTexture array.
    if (motionVectorTexture) {
      // eslint-disable-next-line prefer-destructuring
      subImage.motionVectorTextureWidth = motionVectorTexture._levelWidth[0]

      // eslint-disable-next-line prefer-destructuring
      subImage.motionVectorTextureHeight = motionVectorTexture._levelHeight[0]
    }

    // 18. Run initialize the viewport on subimage’s viewport with subimage’s colorTexture,
    // layer’s layout, index and the number of the session’s list of views.
    this.initializeViewport(
      subImage.viewport,
      colorTexture,
      castedLayer.layout,
      index,
      session._views.length
    )

    // 19. Set needsRedraw to false.
    castedLayer.needsRedraw = false

    this._lastLayer = castedLayer
    this._lastView = castedView
    this._lastSubImage = subImage

    // 20. return subimage
    return subImage
  }
}

export {XrWebGlBindingImpl}
