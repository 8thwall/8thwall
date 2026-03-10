// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)
//
// Camera related data providers
//
/* global XR8:readonly */
import {GlTextureRenderer} from './gl-renderer'

const luminanceFragmentSource =
  'precision mediump float;\n' +
  'varying vec2 texUv;\n' +
  'uniform sampler2D sampler;\n' +
  'void main() {\n' +
  '  vec4 Color = texture2D(sampler, texUv);\n' +
  '  vec3 lum = vec3(0.299, 0.587, 0.114);\n' +
  '  gl_FragColor = vec4(vec3(dot(Color.rgb, lum)), Color.a);\n' +
  '}\n'

const outputSize = ({src, config}) => {
  if (!src.videoWidth || !src.videoHeight) {
    return {width: 0, height: 0}
  }

  if (config.maxDimension > 0) {
    if (src.videoWidth > src.videoHeight) {
      return {
        width: config.maxDimension,
        height: Math.max(Math.round(src.videoHeight * config.maxDimension / src.videoWidth), 1),
      }
    } else {
      return {
        width: Math.max(Math.round(src.videoWidth * config.maxDimension / src.videoHeight), 1),
        height: config.maxDimension,
      }
    }
  }

  if (config.width && config.height) {
    return {width: config.width, height: config.height}
  }

  return {width: src.videoWidth, height: src.videoHeight}
}

// A pipeline module that provides the camera texture as an array of RGBA or grayscale pixel values
// that can be used for CPU image processing.
//
// Arguments:
//   luminance: [Optional] If true, output grayscale instead of RGBA.
//   maxDimension: [Optinal] The size in pixels of the longest dimension of the output image. The
//     shorter dimension will be scaled relative to the size of the camera input so that the image
//     is resized without cropping or distortion.
//   width: [Optional] Width of the output image. Ignored if maxDimension is set.
//   height: [Optional] Height of the output image. Ignored if maxDimension is set.
//
// Produces:
//   processGpuResult.camerapixelarray: {
//     cols: Width in pixels of the output image.
//     rows: Height in pixels of the output image.
//     rowBytes: Number of bytes per row of the output image.
//     pixels: A UInt8Array of pixel data.
//     srcTex: A texture containing the source image for the returned pixels.
//   }
function pipelineModule(_config = {}) {
  let renderer_ = null
  let renderTex_ = null  // owned by renderer
  let displayTex_ = null  // owned by pipeline
  let computeCtx_ = null
  let attached_ = false

  const config = {
    luminance: false,
    maxDimension: 0,
    width: 0,
    height: 0,
    ..._config,
  }

  const src = {
    videoWidth: 0,
    videoHeight: 0,
  }

  const dest = {
    width: 0,
    height: 0,
  }

  const updateSize = ({videoWidth, videoHeight}) => {
    if (!attached_) {
      return
    }

    if (videoWidth === src.videoWidth && videoHeight === src.videoHeight) {
      return
    }

    src.videoWidth = videoWidth
    src.videoHeight = videoHeight

    const dsize = outputSize({src, config})

    if (!dsize.width || !dsize.height) {
      return
    }

    if (renderer_ && dsize.width === dest.width && dsize.height === dest.height) {
      return
    }

    dest.width = dsize.width
    dest.height = dsize.height

    if (config.luminance) {
      renderer_ = GlTextureRenderer.create({
        GLctx: computeCtx_,
        fragmentSource: luminanceFragmentSource,
        toTexture: {width: dest.width, height: dest.height},
        flipY: true,
      })
    } else {
      renderer_ = GlTextureRenderer.create({
        GLctx: computeCtx_,
        toTexture: {width: dest.width, height: dest.height},
        flipY: true,
      })
    }
  }

  return {
    name: 'camerapixelarray',
    onAttach: ({computeCtx, videoWidth, videoHeight}) => {
      attached_ = true
      computeCtx_ = computeCtx
      updateSize({videoWidth, videoHeight})
      GlTextureRenderer.setTextureProvider(
        ({processGpuResult}) => (processGpuResult.camerapixelarray
          ? processGpuResult.camerapixelarray.srcTex
          : null
        )
      )
    },
    onDetach: () => {
      if (renderer_) {
        renderer_.destroy()
      }
      renderer_ = null
      renderTex_ = null  // owned by renderer
      displayTex_ = null  // owned by pipeline
      computeCtx_ = null
      attached_ = false
      src.videoWidth = 0
      src.videoHeight = 0
      dest.width = 0
      dest.height = 0
      GlTextureRenderer.setTextureProvider(null)
    },
    onDeviceOrientationChange: ({videoWidth, videoHeight}) => updateSize({videoWidth, videoHeight}),
    onVideoSizeChange: ({videoWidth, videoHeight}) => updateSize({videoWidth, videoHeight}),
    onProcessGpu: ({frameStartResult}) => {
      const {textureWidth, textureHeight, computeTexture} = frameStartResult
      if (!computeTexture) {
        return undefined
      }

      updateSize({videoWidth: textureWidth, videoHeight: textureHeight})

      if (!renderer_) {
        return undefined
      }

      let retval = {}
      if (renderTex_) {
        computeCtx_.bindFramebuffer(computeCtx_.FRAMEBUFFER, renderer_.framebuffer())

        const cols = dest.width
        const rows = dest.height
        let pixels = new Uint8Array(cols * rows * 4)
        let rowBytes = cols * 4

        computeCtx_.readPixels(
          0,
          0,
          cols,
          rows,
          computeCtx_.RGBA,
          computeCtx_.UNSIGNED_BYTE,
          pixels
        )

        if (config.luminance) {
          const pix = new Uint8Array(cols * rows)
          for (let i = 0; i < cols * rows; i++) {
            pix[i] = pixels[4 * i]
          }
          pixels = pix
          rowBytes = cols
        }

        // all of our CameraPixelArray operations are done using computeCtx_. If we are to provide
        // the texture so that GLTextureRenderer and the user can render using it, then we need to
        // get the drawContext version of the texture.
        const drawCtxDisplayTex = XR8.drawTexForComputeTex(displayTex_)
        retval = {rows, cols, rowBytes, pixels, srcTex: drawCtxDisplayTex}
      }

      const viewport = {offsetX: 0, offsetY: 0, width: dest.width, height: dest.height}
      renderTex_ = renderer_.render({renderTexture: computeTexture, viewport})
      displayTex_ = computeTexture

      return retval
    },
  }
}

const CameraPixelArray = {
  pipelineModule,
}

const CameraPixelArrayModule = CameraPixelArray.pipelineModule

export {
  CameraPixelArray,
  CameraPixelArrayModule,
}
