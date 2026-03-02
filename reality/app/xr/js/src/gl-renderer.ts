// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

import type {Module} from './types/module'
import type {FrameStartResult, RenderContext, SessionAttributes, StepResult} from './types/pipeline'

const noopVertexSource =
  'attribute vec3 position;\n' +
  'attribute vec2 uv;\n' +
  'varying vec2 texUv;\n' +
  'void main() {\n' +
  '  gl_Position = vec4(position, 1.0);\n' +
  '  texUv = uv;\n' +
  '}\n'

const noopFragmentSource =
  'precision lowp float;\n' +
  'varying vec2 texUv;\n' +
  'uniform sampler2D sampler;\n' +
  'void main() {\n' +
  '  gl_FragColor = texture2D(sampler, texUv);\n' +
  '}\n'

const foregroundFragmentSource = `
precision lowp float;
varying vec2 texUv;
uniform sampler2D foreground;
uniform sampler2D mask;
uniform int foregroundFlipY;
uniform int maskFlipY;

void main() {
  float foregroundY = foregroundFlipY == 1 ? 1.0 - texUv.y : texUv.y;
  float maskY = maskFlipY == 1 ? 1.0 - texUv.y : texUv.y;

  vec4 foregroundColor = texture2D(foreground, vec2(texUv.x, foregroundY));
  vec4 maskColor = texture2D(mask, vec2(texUv.x, maskY));

  gl_FragColor = vec4(foregroundColor.rgb, foregroundColor.a * maskColor.r);
}`

const checkGLError = ({GLctx, msg}) => {
  const err = GLctx.getError()
  if (err !== GLctx.NO_ERROR) {
    // eslint-disable-next-line
    console.error(`[XR:WebGL] ${msg}: glError 0x${err.toString(16)}`)
  }
}

// Gets the current set of WebGL bindings so that they can be restored later.
//
// GLctx: The WebGLRenderingContext or WebGL2RenderingContext to get bindings from.
// textureunits: The texture units to preserve state for, e.g. [GLctx.TEXTURE0]
const getGLctxParameters =
  (GLctx, textureUnits) => {
    const params = {
      ACTIVE_TEXTURE: GLctx.getParameter(GLctx.ACTIVE_TEXTURE),
      UNPACK_FLIP_Y_WEBGL: GLctx.getParameter(GLctx.UNPACK_FLIP_Y_WEBGL),
      FRONT_FACE: GLctx.getParameter(GLctx.FRONT_FACE),
      ARRAY_BUFFER_BINDING: GLctx.getParameter(GLctx.ARRAY_BUFFER_BINDING),
      FRAMEBUFFER_BINDING: GLctx.getParameter(GLctx.FRAMEBUFFER_BINDING),
      ELEMENT_ARRAY_BUFFER_BINDING: GLctx.getParameter(GLctx.ELEMENT_ARRAY_BUFFER_BINDING),
      VERTEX_ARRAY_BINDING: GLctx.getParameter(GLctx.VERTEX_ARRAY_BINDING),  // TODO fix this?
      CURRENT_PROGRAM: GLctx.getParameter(GLctx.CURRENT_PROGRAM),
      VIEWPORT: GLctx.getParameter(GLctx.VIEWPORT),
    }

    const activeTextures = textureUnits
      ? textureUnits.reduce((o, v) => {
        GLctx.activeTexture(v)
        o[v] = GLctx.getParameter(GLctx.TEXTURE_BINDING_2D)
        return o
      }, {})
      : {}

    if (textureUnits) {
      GLctx.activeTexture(textureUnits[0])
    } else {
      activeTextures[params.ACTIVE_TEXTURE] = GLctx.getParameter(GLctx.TEXTURE_BINDING_2D)
    }

    const enableMap = {
      'DEPTH_TEST': GLctx.isEnabled(GLctx.DEPTH_TEST),
      'BLEND': GLctx.isEnabled(GLctx.BLEND),
    }

    return {params, activeTextures, enableMap}
  }

// Restores the WebGL bindings that were saved with getGLctxParameters.
//
// GLctx: The WebGLRenderingContext or WebGL2RenderingContext to restore bindings on.
// restoreParams: the output of getGLctxParameters.
const setGLctxParameters = (GLctx, restoreParams) => {
  const {params, activeTextures, enableMap} = restoreParams
  for (const key in enableMap) {
    if (enableMap[key]) {
      GLctx.enable(GLctx[key])
    } else {
      GLctx.disable(GLctx[key])
    }
  }
  for (const key in activeTextures) {
    GLctx.activeTexture(key)
    GLctx.bindTexture(GLctx.TEXTURE_2D, activeTextures[key])
  }

  GLctx.activeTexture(params.ACTIVE_TEXTURE)
  GLctx.pixelStorei(GLctx.UNPACK_FLIP_Y_WEBGL, params.UNPACK_FLIP_Y_WEBGL)
  GLctx.frontFace(params.FRONT_FACE)
  GLctx.bindBuffer(GLctx.ARRAY_BUFFER, params.ARRAY_BUFFER_BINDING)
  GLctx.bindFramebuffer(GLctx.FRAMEBUFFER, params.FRAMEBUFFER_BINDING)
  if (GLctx.bindVertexArray) {
    GLctx.bindVertexArray(params.VERTEX_ARRAY_BINDING)
  }
  GLctx.bindBuffer(GLctx.ELEMENT_ARRAY_BUFFER, params.ELEMENT_ARRAY_BUFFER_BINDING)
  GLctx.useProgram(params.CURRENT_PROGRAM)
  GLctx.viewport(params.VIEWPORT[0], params.VIEWPORT[1], params.VIEWPORT[2], params.VIEWPORT[3])
}

const compileShader = (inputs: {GLctx: RenderContext, source: string, shaderType: number}) => {
  const {GLctx, source, shaderType} = inputs
  const shader = GLctx.createShader(shaderType)
  GLctx.shaderSource(shader, source)
  GLctx.compileShader(shader)
  if (!GLctx.getShaderParameter(shader, GLctx.COMPILE_STATUS)) {
    // eslint-disable-next-line
    console.error('[XR:WebGL]', GLctx.getShaderInfoLog(shader))
    return null
  }
  return shader
}

const compileShaders = (
  inputs: {GLctx: RenderContext, vertexSource: string, fragmentSource: string}
) => {
  const {GLctx, vertexSource = noopVertexSource, fragmentSource = noopFragmentSource} = inputs
  const vertexShader = compileShader({
    GLctx,
    source: vertexSource,
    shaderType: GLctx.VERTEX_SHADER,
  })

  const fragmentShader = compileShader({
    GLctx,
    source: fragmentSource,
    shaderType: GLctx.FRAGMENT_SHADER,
  })

  if (vertexShader == null || fragmentShader == null) {
    return null
  }

  const shader = GLctx.createProgram()
  GLctx.attachShader(shader, vertexShader)
  GLctx.attachShader(shader, fragmentShader)
  GLctx.bindAttribLocation(shader, 0, 'position')
  GLctx.bindAttribLocation(shader, 1, 'uv')
  GLctx.linkProgram(shader)

  if (!GLctx.getProgramParameter(shader, GLctx.LINK_STATUS)) {
    // eslint-disable-next-line
    console.error('[XR:WebGL] Error linking vertex/fragement shaders.')
    return null
  }
  GLctx.deleteShader(vertexShader)
  GLctx.deleteShader(fragmentShader)

  return shader
}

type ConfigParams = {
  vertexSource?: string
  fragmentSource?: string
  flipY?: boolean
  mirroredDisplay?: boolean
  toTexture?: {width: number, height: number}
}
type InitParams = {
  GLctx: RenderContext
  verbose?: boolean
} & ConfigParams
const init = (inputs: InitParams) => {
  const {
    GLctx,
    vertexSource: vertexSource_,
    fragmentSource: fragmentSource_,
    flipY: flipY_,
    mirroredDisplay: mirroredDisplay_,
    toTexture: toTexture_,
    verbose,
  } = inputs
  const vertexSource = vertexSource_ || noopVertexSource
  const fragmentSource = fragmentSource_ || noopFragmentSource
  const flipY = !!flipY_
  const mirroredDisplay = !!mirroredDisplay_
  const toTexture = toTexture_ || null

  const hasVA = 'bindVertexArray' in GLctx
  // NOTE(dat): Because of type narrowing, when hasVA is in the true branch, GLctx's type is
  //            WebGL2RenderingContext.
  const VAext = hasVA ? null : GLctx.getExtension('OES_vertex_array_object')
  const createVertexArray = () => (hasVA ? GLctx.createVertexArray() : VAext.createVertexArrayOES())
  const bindVertexArray = v => (hasVA ? GLctx.bindVertexArray(v) : VAext.bindVertexArrayOES(v))
  const vertexArrayBinding = () => GLctx.getParameter(
    hasVA ? GLctx.VERTEX_ARRAY_BINDING : VAext.VERTEX_ARRAY_BINDING_OES
  )

  const restoreVertexArray = vertexArrayBinding()
  const restoreArrayBuffer = GLctx.getParameter(GLctx.ARRAY_BUFFER_BINDING)
  const restoreTex = toTexture ? GLctx.getParameter(GLctx.TEXTURE_BINDING_2D) : null
  const restoreFramebuffer = toTexture ? GLctx.getParameter(GLctx.FRAMEBUFFER_BINDING) : null

  const shader = compileShaders({GLctx, vertexSource, fragmentSource})
  const foregroundShader = compileShaders(
    {GLctx, vertexSource: noopVertexSource, fragmentSource: foregroundFragmentSource}
  )

  if (!shader || !foregroundShader) {
    return null
  }

  // Create VAO and bind it.
  const vertexArray = createVertexArray()
  bindVertexArray(vertexArray)

  // Construct a buffer of triangle corner positions for a quad that covers the whole viewport.
  const positionBuffer = GLctx.createBuffer()
  GLctx.bindBuffer(GLctx.ARRAY_BUFFER, positionBuffer)
  GLctx.bufferData(
    GLctx.ARRAY_BUFFER,
    new Float32Array([
      -1.0, 1.0, 0.0,
      -1.0, -1.0, 0.0,
      1.0, 1.0, 0.0,
      1.0, -1.0, 0.0,
    ]),
    GLctx.STATIC_DRAW
  )

  GLctx.vertexAttribPointer(0, 3, GLctx.FLOAT, false, 0, 0)
  GLctx.enableVertexAttribArray(0)

  // Construct a buffer of mesh UV coordinates for each position on the quad.
  const uvBuffer = GLctx.createBuffer()
  GLctx.bindBuffer(GLctx.ARRAY_BUFFER, uvBuffer)
  const points = new Float32Array([
    0.0, 0.0,
    0.0, 1.0,
    1.0, 0.0,
    1.0, 1.0,
  ])
  if (flipY) {
    points[1] = 1.0 - points[1]
    points[3] = 1.0 - points[3]
    points[5] = 1.0 - points[5]
    points[7] = 1.0 - points[7]
  }
  if (mirroredDisplay) {
    points[0] = 1.0 - points[0]
    points[2] = 1.0 - points[2]
    points[4] = 1.0 - points[4]
    points[6] = 1.0 - points[6]
  }
  GLctx.bufferData(GLctx.ARRAY_BUFFER, points, GLctx.STATIC_DRAW)

  GLctx.vertexAttribPointer(1, 2, GLctx.FLOAT, false, 0, 0)
  GLctx.enableVertexAttribArray(1)

  // Construct a buffer of two triangles (represented as indices to the positionBuffer) to
  // represent a quad that covers the whole viewport.
  const triangleBuffer = GLctx.createBuffer()
  GLctx.bindBuffer(GLctx.ELEMENT_ARRAY_BUFFER, triangleBuffer)
  GLctx.bufferData(
    GLctx.ELEMENT_ARRAY_BUFFER,
    new Uint16Array([0, 1, 2, 2, 1, 3]),
    GLctx.STATIC_DRAW
  )

  let framebuffer: WebGLFramebuffer | null = null
  let texture: WebGLTexture | null = null
  if (toTexture) {
    // We want to output into a texture instead. Let's create the texture here and bind it + a fb
    texture = GLctx.createTexture()
    GLctx.bindTexture(GLctx.TEXTURE_2D, texture)

    framebuffer = GLctx.createFramebuffer()
    GLctx.bindFramebuffer(GLctx.FRAMEBUFFER, framebuffer)

    GLctx.hint(GLctx.GENERATE_MIPMAP_HINT, GLctx.FASTEST)
    GLctx.texParameteri(GLctx.TEXTURE_2D, GLctx.TEXTURE_WRAP_S, GLctx.CLAMP_TO_EDGE)
    GLctx.texParameteri(GLctx.TEXTURE_2D, GLctx.TEXTURE_WRAP_T, GLctx.CLAMP_TO_EDGE)
    GLctx.texParameteri(GLctx.TEXTURE_2D, GLctx.TEXTURE_MAG_FILTER, GLctx.LINEAR)
    GLctx.texParameteri(GLctx.TEXTURE_2D, GLctx.TEXTURE_MIN_FILTER, GLctx.LINEAR)

    GLctx.texImage2D(
      GLctx.TEXTURE_2D,
      0,
      GLctx.RGBA,
      toTexture.width,
      toTexture.height,
      0,
      GLctx.RGBA,
      GLctx.UNSIGNED_BYTE,
      null
    )

    GLctx.framebufferTexture2D(
      GLctx.FRAMEBUFFER, GLctx.COLOR_ATTACHMENT0, GLctx.TEXTURE_2D, texture, 0
    )
  }

  // restore state.
  bindVertexArray(restoreVertexArray)
  GLctx.bindBuffer(GLctx.ARRAY_BUFFER, restoreArrayBuffer)
  if (toTexture) {
    GLctx.bindTexture(GLctx.TEXTURE_2D, restoreTex)
    GLctx.bindFramebuffer(GLctx.FRAMEBUFFER, restoreFramebuffer)
  }

  // Make sure nothing went wrong.
  if (verbose) {
    checkGLError({GLctx, msg: 'GLRenderer.onAttach'})
  }

  const destroy = () => {
    if (!GLctx) {
      return
    }

    const deleteVertexArray =
      v => (
        'deleteVertexArray' in GLctx
          ? GLctx.deleteVertexArray(v)
          : VAext.deleteVertexArrayOES(v)
      )

    if (shader) {
      GLctx.deleteProgram(shader)
    }
    if (foregroundShader) {
      GLctx.deleteProgram(foregroundShader)
    }
    if (positionBuffer) {
      GLctx.deleteBuffer(positionBuffer)
    }
    if (uvBuffer) {
      GLctx.deleteBuffer(uvBuffer)
    }
    if (triangleBuffer) {
      GLctx.deleteBuffer(triangleBuffer)
    }
    if (texture) {
      GLctx.deleteTexture(texture)
    }
    if (framebuffer) {
      GLctx.deleteFramebuffer(framebuffer)
    }
    if (vertexArray) {
      deleteVertexArray(vertexArray)
    }
  }

  return {
    GLctx,
    VAext,
    shader,
    foregroundShader,
    vertexArray,
    framebuffer,
    texture,
    verbose,
    destroy,
  }
}

type ForegroundTextureAndMask = {
  foregroundTexture: WebGLTexture
  foregroundMaskTexture: WebGLTexture
  foregroundTextureFlipY?: boolean
  foregroundMaskTextureFlipY?: boolean
}
type DrawParams = ReturnType<typeof init> & {
  renderTexture: WebGLTexture
  foregroundTexturesAndMasks: ForegroundTextureAndMask[]
  viewport: Viewport
  foregroundViewport: Viewport
}
const draw = (input: DrawParams): WebGLTexture => {
  const {
    GLctx,
    VAext,
    shader,
    foregroundShader,
    vertexArray,
    framebuffer,
    texture,
    renderTexture,
    foregroundTexturesAndMasks,
    viewport,
    foregroundViewport,
    verbose,
  } = input
  if (!renderTexture) {
    return null
  }

  const hasVA = 'bindVertexArray' in GLctx
  const bindVertexArray = v => (hasVA ? GLctx.bindVertexArray(v) : VAext.bindVertexArrayOES(v))
  const vertexArrayBinding = () => GLctx.getParameter(
    hasVA ? GLctx.VERTEX_ARRAY_BINDING : VAext.VERTEX_ARRAY_BINDING_OES
  )

  // Cache the current state of the opengl state machine to restore it later.
  const restoreProgram = GLctx.getParameter(GLctx.CURRENT_PROGRAM)
  const restoreFramebuffer = framebuffer ? GLctx.getParameter(GLctx.FRAMEBUFFER_BINDING) : null
  const restoreViewport = GLctx.getParameter(GLctx.VIEWPORT)
  const restoreDepth = GLctx.isEnabled(GLctx.DEPTH_TEST)
  const restoreBlend = GLctx.isEnabled(GLctx.BLEND)
  const restoreBlendSrcRGB = GLctx.getParameter(GLctx.BLEND_SRC_RGB)
  const restoreBlendDstRGB = GLctx.getParameter(GLctx.BLEND_DST_RGB)
  const restoreBlendSrcAlpha = GLctx.getParameter(GLctx.BLEND_SRC_ALPHA)
  const restoreBlendDstAlpha = GLctx.getParameter(GLctx.BLEND_DST_ALPHA)
  // First we get the active texture unit.
  const restoreActive = GLctx.getParameter(GLctx.ACTIVE_TEXTURE)
  const restoreTex = GLctx.getParameter(GLctx.TEXTURE_BINDING_2D)
  // If the active texture unit was not 0 then save texture at unit 0 because we'll overwrite it.
  let restoreTex0 = null
  if (restoreActive !== GLctx.TEXTURE0) {
    GLctx.activeTexture(GLctx.TEXTURE0)
    restoreTex0 = GLctx.getParameter(GLctx.TEXTURE_BINDING_2D)
  }
  // If the active texture unit was not 1 then save texture at unit 1 because we'll overwrite it.
  let restoreTex1 = null
  if (restoreActive !== GLctx.TEXTURE1) {
    GLctx.activeTexture(GLctx.TEXTURE1)
    restoreTex1 = GLctx.getParameter(GLctx.TEXTURE_BINDING_2D)
  }
  const restoreFrontFace = GLctx.getParameter(GLctx.FRONT_FACE)
  const restoreVertexArray = vertexArrayBinding()

  // Set the active shader.
  GLctx.useProgram(shader)

  if (framebuffer) {
    GLctx.bindFramebuffer(GLctx.FRAMEBUFFER, framebuffer)
  }

  // Set the drawing viewport.
  GLctx.viewport(
    viewport.offsetX || 0,
    viewport.offsetY || 0,
    viewport.width,
    viewport.height
  )

  // Set the texture source for the shader.
  GLctx.activeTexture(GLctx.TEXTURE0)
  GLctx.bindTexture(GLctx.TEXTURE_2D, renderTexture)

  // Draw the triangles of the quad.
  if (restoreDepth) {
    GLctx.disable(GLctx.DEPTH_TEST)
  }
  bindVertexArray(vertexArray)
  GLctx.frontFace(GLctx.CCW)
  if (restoreBlend) {
    GLctx.disable(GLctx.BLEND)
  }
  GLctx.drawElements(GLctx.TRIANGLES, 6, GLctx.UNSIGNED_SHORT, 0)

  // Optionally draw the foreground texture.
  if (foregroundTexturesAndMasks?.length && foregroundViewport) {
    // Use the foreground shader.
    GLctx.useProgram(foregroundShader)

    // Enable blending so the camera feed, which was already drawn, is left in the background.
    GLctx.enable(GLctx.BLEND)
    GLctx.blendFuncSeparate(
      GLctx.SRC_ALPHA,
      GLctx.ONE_MINUS_SRC_ALPHA,
      GLctx.ONE,
      GLctx.ONE_MINUS_SRC_ALPHA
    )

    GLctx.viewport(
      foregroundViewport.offsetX,
      foregroundViewport.offsetY,
      foregroundViewport.width,
      foregroundViewport.height
    )

    foregroundTexturesAndMasks.forEach((f) => {
      // TODO(paris): Move the getUniformLocation() calls to initialization and only do once.

      // Add the foreground texture which we will draw in the foreground of the camera.
      const foregroundTextureLocation = GLctx.getUniformLocation(foregroundShader, 'foreground')
      GLctx.uniform1i(foregroundTextureLocation, 0)
      GLctx.activeTexture(GLctx.TEXTURE0)
      GLctx.bindTexture(GLctx.TEXTURE_2D, f.foregroundTexture)

      // Add the mask which we use to determine whether to draw the foreground.
      const foregroundMaskLocation = GLctx.getUniformLocation(foregroundShader, 'mask')
      GLctx.uniform1i(foregroundMaskLocation, 1)
      GLctx.activeTexture(GLctx.TEXTURE1)
      GLctx.bindTexture(GLctx.TEXTURE_2D, f.foregroundMaskTexture)

      // Specify whether to flip Y values.
      const foregroundFlipYLocation = GLctx.getUniformLocation(foregroundShader, 'foregroundFlipY')
      GLctx.uniform1i(foregroundFlipYLocation, f.foregroundTextureFlipY ? 1 : 0)

      const maskFlipYLocation = GLctx.getUniformLocation(foregroundShader, 'maskFlipY')
      GLctx.uniform1i(maskFlipYLocation, f.foregroundMaskTextureFlipY ? 1 : 0)

      GLctx.drawElements(GLctx.TRIANGLES, 6, GLctx.UNSIGNED_SHORT, 0)
    })
  }

  GLctx.finish()

  // Restore the previous state.
  bindVertexArray(restoreVertexArray)
  GLctx.frontFace(restoreFrontFace)
  if (restoreTex0) {
    GLctx.activeTexture(GLctx.TEXTURE0)
    GLctx.bindTexture(GLctx.TEXTURE_2D, restoreTex0)
  }
  if (restoreTex1) {
    GLctx.activeTexture(GLctx.TEXTURE1)
    GLctx.bindTexture(GLctx.TEXTURE_2D, restoreTex1)
  }
  GLctx.activeTexture(restoreActive)
  GLctx.bindTexture(GLctx.TEXTURE_2D, restoreTex)
  if (restoreDepth) {
    GLctx.enable(GLctx.DEPTH_TEST)
  }
  if (restoreBlend) {
    GLctx.enable(GLctx.BLEND)
  } else {
    GLctx.disable(GLctx.BLEND)
  }
  GLctx.blendFuncSeparate(
    restoreBlendSrcRGB, restoreBlendDstRGB, restoreBlendSrcAlpha, restoreBlendDstAlpha
  )
  GLctx.viewport(restoreViewport[0], restoreViewport[1], restoreViewport[2], restoreViewport[3])
  if (framebuffer) {
    GLctx.bindFramebuffer(GLctx.FRAMEBUFFER, restoreFramebuffer)
  }
  GLctx.useProgram(restoreProgram)

  // Make sure nothing went wrong.
  if (verbose) {
    checkGLError({GLctx, msg: 'GLRenderer.render'})
  }
  return texture
}

type Viewport = {
  offsetX: number
  offsetY: number
  width: number
  height: number
}

const constructVideoViewport = ({videoWidth, videoHeight, canvasWidth, canvasHeight}): Viewport => {
  const videoAspect = videoWidth / videoHeight
  const canvasAspect = canvasWidth / canvasHeight

  let videoCroppedWidth
  let videoCroppedHeight
  if (videoAspect < canvasAspect) {
    videoCroppedWidth = videoWidth
    videoCroppedHeight = videoCroppedWidth / canvasAspect
  } else {
    videoCroppedHeight = videoHeight
    videoCroppedWidth = videoCroppedHeight * canvasAspect
  }

  // height and width are equivalent to use here because
  // canvas and videoCropped have the same aspect ratio
  const videoDisplayScale = canvasHeight / videoCroppedHeight

  const videoViewport = {
    offsetX: 0.5 * (canvasWidth - videoWidth * videoDisplayScale),
    offsetY: 0.5 * (canvasHeight - videoHeight * videoDisplayScale),
    width: videoWidth * videoDisplayScale,
    height: videoHeight * videoDisplayScale,
  }

  return videoViewport
}

const constructForegroundViewport = ({canvasWidth, canvasHeight}): Viewport => ({
  offsetX: 0,
  offsetY: 0,
  width: canvasWidth,
  height: canvasHeight,
})

// Creates an object that can render textures to a canvas or another texture.
//
// GLctx:  The WebGlRenderingContext (or WebGl2RenderingContext) to use for rendering. If no
//   toTexture is specified, content will be drawn to this context's canvas.
// vertexSource: [Optional] The vertex shader source to use for rendering.
// fragmentSource: [Optional] The fragment shader source to use for rendering.
// toTexture: [Optional] Output to a texture of width, height. If not set, drawing will be to the
//            canvas.
// flipY: [Optional] If true, flip the rendering upside-down.
// mirroredDisplay: [Optional] If true, flip the rendering left-right.
function create({
  GLctx,
  vertexSource = null,
  fragmentSource = null,
  toTexture,
  flipY = false,
  mirroredDisplay = false,
  verbose = false,
}: InitParams) {
  const p_ = init({
    GLctx,
    vertexSource,
    fragmentSource,
    toTexture,
    flipY,
    mirroredDisplay,
    verbose,
  })
  return {
    // Renders the renderTexture to the specified viewport. Depending on if 'toTexture' is supplied,
    // the viewport is either on the canvas that created GLctx, or it's relative to the render
    // texture provided.
    //
    // renderTexture: A WebGlTexture (source) to draw.
    // foregroundTexture: A WebGlTexture (source) to draw in the foreground after drawing the
    //   renderTexture.
    // foregroundMaskTexture: A WebGlTexture to use when drawing the foregroundTexture as an alpha
    //   mask. Uses the red channel: 255 means we draw the foregroundTexture, 0 means we don't.
    // viewport: The region of the canvas or output texture to draw to; this can be constructed
    //   manually, or using GlTextureRenderer.fillTextureViewport(). The viewport is specified by
    //   {
    //     width: The width (in pixels) to draw.
    //     height: The height (in pixels) to draw.
    //     offsetX: [Optional] The minimum x-coordinate (in pixels) to draw to.
    //     offsetY: [Optional] The minimum y-coordinate (in pixels) to draw to.
    //   }
    render: ({renderTexture, foregroundTexturesAndMasks, viewport, foregroundViewport}) => draw(
      // eslint-disable-next-line prefer-object-spread
      Object.assign({renderTexture, foregroundTexturesAndMasks, viewport, foregroundViewport}, p_)
    ),
    // Clean up resources associated with this GlTextureRenderer
    destroy: p_.destroy,
    // Gets a handle to the shader being used to draw the texture.
    shader: () => p_.shader,
    // Gets a handle to the framebuffer being used to draw if toTexture is specified.
    framebuffer: () => p_.framebuffer,
  }
}

type GlRenderer = ReturnType<typeof create>
type TexOnUpdateProvider = (
  input: {
    frameStartResult: FrameStartResult
    processGpuResult: StepResult
    processCpuResult: StepResult
  }
) => WebGLTexture
type ForegroundTexOnUpdateProvider = (
  input: {
    frameStartResult: FrameStartResult
    processGpuResult: StepResult
    processCpuResult: StepResult
  }
) => ForegroundTextureAndMask[]

// Creates a camera pipeline module that, when installed, draws the camera feed to the canvas.
type GlRendererPipelineModule = Module & {
  configure: (args: ConfigParams) => void
  setTexOnUpdateProvider: (newProvider: TexOnUpdateProvider) => void
  setForegroundTexOnUpdateProvider: (newProvider: ForegroundTexOnUpdateProvider) => void
}
function basePipelineModule(args: InitParams = null): GlRendererPipelineModule {
  // Set externally -- don't reset on detach
  let {vertexSource, fragmentSource, toTexture, flipY, mirroredDisplay} = args || {}

  let renderTexture_: WebGLTexture = null
  let srcTexOnUpdateProvider_: TexOnUpdateProvider = null

  let foregroundTexturesAndMasks_: ForegroundTextureAndMask[] = []
  let srcForegroundTexOnUpdateProvider_: ForegroundTexOnUpdateProvider = null

  // Owned by pipeline -- reset on detach
  let r_: GlRenderer = null
  let viewport_: Partial<Viewport> = {}
  let foregroundViewport_: Partial<Viewport> = {}
  let GLctx_: RenderContext = null
  let sessionAttributes_: SessionAttributes = {}
  return {
    name: 'gltexturerenderer',
    onAttach: ({GLctx, config}) => {
      // the only way to toggle on verbose is by setting XR8.run({verbose: true})
      const verbose = !!config?.verbose
      GLctx_ = GLctx
      r_ = create({
        GLctx: GLctx_,
        vertexSource,
        fragmentSource,
        toTexture,
        flipY,
        mirroredDisplay,
        verbose,
      })
    },
    onSessionAttach: ({canvasWidth, canvasHeight, videoWidth, videoHeight, sessionAttributes}) => {
      sessionAttributes_ = sessionAttributes
      viewport_ = constructVideoViewport({videoWidth, videoHeight, canvasWidth, canvasHeight})
      foregroundViewport_ = constructForegroundViewport({canvasWidth, canvasHeight})
    },
    onSessionDetach: () => {
      viewport_ = {}
      foregroundViewport_ = {}
      GLctx_ = null
      renderTexture_ = null
      foregroundTexturesAndMasks_ = []
    },
    onDetach: () => {
      if (r_) {
        r_.destroy()
      }
      r_ = null
    },
    onUpdate: ({frameStartResult, processGpuResult, processCpuResult}) => {
      if (srcForegroundTexOnUpdateProvider_) {
        const foregroundTexturesAndMasks =
          srcForegroundTexOnUpdateProvider_({frameStartResult, processGpuResult, processCpuResult})
        if (foregroundTexturesAndMasks) {
          foregroundTexturesAndMasks_ = foregroundTexturesAndMasks
        }
      }

      if (srcTexOnUpdateProvider_) {
        const drawTex =
          srcTexOnUpdateProvider_({frameStartResult, processGpuResult, processCpuResult})
        if (drawTex) {
          renderTexture_ = drawTex
        }
        return
      }

      // NOTE(dat): Would be nice to be able to narrow the type here using
      // `'reality' in processCpuResult`
      const drawTex: WebGLTexture = processCpuResult.reality
        ? (processCpuResult.reality as any).realityTexture
        : frameStartResult.cameraTexture
      if (drawTex) {
        renderTexture_ = drawTex
      }
    },
    onProcessGpu: () => ({viewport: viewport_, shader: r_.shader()}),
    onRender: () => {
      if (sessionAttributes_.fillsCameraTexture) {
        // As an optimization, don't draw a fake camera feed if there isn't a real one.
        r_.render({
          renderTexture: renderTexture_,
          foregroundTexturesAndMasks: foregroundTexturesAndMasks_,
          viewport: viewport_,
          foregroundViewport: foregroundViewport_,
        })
      }
    },
    // TODO(paris): Should we also call this when onDeviceOrientationChange is fired?
    onCanvasSizeChange: ({videoWidth, videoHeight, canvasWidth, canvasHeight}) => {
      viewport_ = constructVideoViewport({videoWidth, videoHeight, canvasWidth, canvasHeight})
      foregroundViewport_ = constructForegroundViewport({canvasWidth, canvasHeight})
    },
    onVideoSizeChange: ({videoWidth, videoHeight, canvasWidth, canvasHeight}) => {
      viewport_ = constructVideoViewport({videoWidth, videoHeight, canvasWidth, canvasHeight})
      foregroundViewport_ = constructForegroundViewport({canvasWidth, canvasHeight})
    },
    configure: (cargs) => {
      if (cargs && cargs.vertexSource) {
        vertexSource = cargs.vertexSource
      }
      if (cargs && cargs.fragmentSource) {
        fragmentSource = cargs.fragmentSource
      }
      if (cargs && cargs.toTexture) {
        toTexture = cargs.toTexture
      }
      if (cargs && cargs.flipY !== undefined) {
        flipY = cargs.flipY
      }
      if (cargs && cargs.mirroredDisplay !== undefined) {
        mirroredDisplay = !!cargs.mirroredDisplay
      }
      if (GLctx_) {
        if (r_) {
          r_.destroy()
        }
        r_ = create({
          GLctx: GLctx_,
          vertexSource,
          fragmentSource,
          toTexture,
          flipY,
          mirroredDisplay,
        })
      }
    },
    setTexOnUpdateProvider: (newProvider) => {
      srcTexOnUpdateProvider_ = newProvider
    },
    setForegroundTexOnUpdateProvider: (newProvider) => {
      srcForegroundTexOnUpdateProvider_ = newProvider
    },
  }
}

let pipelineModuleSingleton = null

const configureSingleton = (args) => {
  if (!pipelineModuleSingleton) {
    pipelineModuleSingleton = basePipelineModule()
  }

  pipelineModuleSingleton.configure(args)
}

const setTexOnUpdateProviderSingleton = (provider) => {
  if (!pipelineModuleSingleton) {
    pipelineModuleSingleton = basePipelineModule()
  }

  pipelineModuleSingleton.setTexOnUpdateProvider(provider)
}

const setForegroundTexOnUpdateProviderSingleton = (provider) => {
  if (!pipelineModuleSingleton) {
    pipelineModuleSingleton = basePipelineModule()
  }

  pipelineModuleSingleton.setForegroundTexOnUpdateProvider(provider)
}

const getPipelineModule = (args): Module => {
  configureSingleton(args)

  return {
    name: pipelineModuleSingleton.name,
    onAttach: pipelineModuleSingleton.onAttach,
    onSessionAttach: pipelineModuleSingleton.onSessionAttach,
    onSessionDetach: pipelineModuleSingleton.onSessionDetach,
    onDetach: pipelineModuleSingleton.onDetach,
    onProcessGpu: pipelineModuleSingleton.onProcessGpu,
    onUpdate: pipelineModuleSingleton.onUpdate,
    onRender: pipelineModuleSingleton.onRender,
    onCanvasSizeChange: pipelineModuleSingleton.onCanvasSizeChange,
    onVideoSizeChange: pipelineModuleSingleton.onVideoSizeChange,
  }
}

// Deprecated at 9.1
function GLRenderer(args) {
  const r_ = (args && args.GLctx) ? create(args) : null
  const m_ = basePipelineModule(args)
  return {
    name: 'deprecatedglrenderer',
    onAttach: m_.onAttach,
    onDetach: m_.onDetach,
    render: renderargs => (r_ ? r_.render(renderargs || {}) : undefined),
    onUpdate: m_.onUpdate,
    onRender: m_.onRender,
    onCanvasSizeChange: m_.onCanvasSizeChange,
    onVideoSizeChange: m_.onVideoSizeChange,
    setRenderTexture: () => { /* no-op */ },
  }
}

// Generates a viewport for drawing that filts the destination with the source without distorting
// the source by center-cropping the source.
function fillTextureViewport(srcWidth, srcHeight, destWidth, destHeight) {
  return constructVideoViewport({
    videoWidth: srcWidth,
    videoHeight: srcHeight,
    canvasWidth: destWidth,
    canvasHeight: destHeight,
  })
}

const GlTextureRenderer = {
  // Gets the current set of WebGL bindings so that they can be restored later.
  getGLctxParameters,

  // Restores the WebGL bindings that were saved with getGLctxParameters.
  setGLctxParameters,

  // Convenience method for getting a Viewport struct that fills a texture or canvas from a source
  // without distortion.
  fillTextureViewport,

  // Gets a pipeline module that draws the camera feed to the canvas.
  pipelineModule: getPipelineModule,

  // Configures the pipeline module that draws the camera feed to the canvas.
  configure: configureSingleton,

  // Creates an object for rendering from a texture to a canvas or another texture.
  create,

  // Sets a provider that passes the texture to draw. This should be a function that take the same
  // inputs as cameraPipelineModule.onUpdate.
  setTextureProvider: setTexOnUpdateProviderSingleton,

  // Sets a provider that passes a list of textures to draw along with an alpha mask for them. This
  // should be a function that take the same inputs as cameraPipelineModule.onUpdate. The function
  // should return: [{
  //   foregroundTexture: WebGLTexture,
  //   foregroundMaskTexture: WebGLTexture
  //   foregroundTextureFlipY?: boolean,
  //   foregroundMaskTextureFlipY?: boolean,
  // }]
  setForegroundTextureProvider: setForegroundTexOnUpdateProviderSingleton,
}

export {
  GlTextureRenderer,

  // Deprecated at version 9.1
  GLRenderer,  // Deprecated at version 9.1
  getGLctxParameters,  // Deprecated at version 9.1
  setGLctxParameters,  // Deprecated at version 9.1
  checkGLError,
}

export type {
  GlRendererPipelineModule,
}
