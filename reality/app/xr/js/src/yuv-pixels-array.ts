// Copyright (c) 2025 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)
//
// Copied from internalqa/omniscope to be surfaced in the regular engine
// pipeline module that convert RGBA camera data into YUVA camera data
//
import {GlTextureRenderer} from './gl-renderer'

declare const XR8: any

// See texture-transform for the original version of this code, @see RGB_TO_YUV_FRAGMENT_CODE
const yuvConvertFragmentSource =
  'precision mediump float;\n' +
  'varying vec2 texUv;\n' +
  'uniform sampler2D sampler;\n' +
  'void main() {\n' +
    'vec4 color = texture2D(sampler, texUv);\n' +
    'float y = .299 * color.r + .587 * color.g + .114 * color.b;\n' +
    'gl_FragColor.r = y;\n' +
    'gl_FragColor.g = -0.169 * color.r - .331 * color.g + .5 * color.b + 0.5;\n' +
    'gl_FragColor.b = 0.5 * color.r - .419 * color.g - .081 * color.b + 0.5;\n' +
    'gl_FragColor.a = color.a;\n' +
  '}\n'

const outputSize = ({src, config}: any) => {
  if (!src.videoWidth || !src.videoHeight) {
    return {width: 0, height: 0}
  }

  if (config.maxDimension > 0) {
    if (src.videoWidth > src.videoHeight) {
      return {
        width: config.maxDimension,
        height: Math.max(Math.round((src.videoHeight * config.maxDimension) / src.videoWidth), 1),
      }
    } else {
      return {
        width: Math.max(Math.round((src.videoWidth * config.maxDimension) / src.videoHeight), 1),
        height: config.maxDimension,
      }
    }
  }

  if (config.width && config.height) {
    return {width: config.width, height: config.height}
  }

  return {width: src.videoWidth, height: src.videoHeight}
}

// A pipeline module that provides the camera texture as an array of YUVA that can be used for CPU
// image processing.
//
// Arguments:
//   maxDimension: [Optional] The size in pixels of the longest dimension of the output image. The
//     shorter dimension will be scaled relative to the size of the camera input so that the image
//     is resized without cropping or distortion.
//   width: [Optional] Width of the output image. Ignored if maxDimension is set.
//   height: [Optional] Height of the output image. Ignored if maxDimension is set.
//
// Produces:
//   processGpuResult.yuvpixelsarray: {
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
  let pixels_ = null

  const config = {
    maxDimension: 640,
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

  const updateSize = ({videoWidth, videoHeight}: {videoWidth: number, videoHeight: number}) => {
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
    pixels_ = new Uint8Array(dest.width * dest.height * 4)

    renderer_ = GlTextureRenderer.create({
      GLctx: computeCtx_,
      fragmentSource: yuvConvertFragmentSource,
      toTexture: {width: dest.width, height: dest.height},
      flipY: true,
    })
  }

  return {
    name: 'yuvpixelsarray',
    onAttach: ({computeCtx, videoWidth, videoHeight}) => {
      attached_ = true
      computeCtx_ = computeCtx
      updateSize({videoWidth, videoHeight})
      GlTextureRenderer.setTextureProvider(
        ({processGpuResult}) => (processGpuResult.yuvpixelsarray
          ? processGpuResult.yuvpixelsarray.srcTex
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
        computeCtx_.readPixels(
          0,
          0,
          dest.width,
          dest.height,
          computeCtx_.RGBA,
          computeCtx_.UNSIGNED_BYTE,
          pixels_
        )

        // all of our YuvPixelsArray operations are done using computeCtx_. If we are to provide
        // the texture so that GLTextureRenderer and the user can render using it, then we need to
        // get the drawContext version of the texture.
        const drawCtxDisplayTex = XR8.drawTexForComputeTex(displayTex_)
        retval = {
          cols: dest.width,
          rows: dest.height,
          rowBytes: dest.width * 4,
          pixels: pixels_,
          srcTex: drawCtxDisplayTex,
        }
      }

      const viewport = {offsetX: 0, offsetY: 0, width: dest.width, height: dest.height}
      renderTex_ = renderer_.render({renderTexture: computeTexture, viewport})
      displayTex_ = computeTexture

      return retval
    },
  }
}

const YuvPixelsArray = {
  pipelineModule,
}

export {
  YuvPixelsArray,
}
