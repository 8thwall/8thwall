// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Akul Santhosh (akulsanthosh@nianticlabs.com)

// WebXR Controller manages the data received from WebXR -
// Creates the capnp messages for the intrinsic and extrinsic data.
// Copies the camera texture from draw to compute context.
// Makes the texture accessible to emscripten.

// This can act as the entry point for different AR experiences, currently
// image targets is implemented. The corresponding cc file is webxr-controller.cc

// @inliner-off
import * as capnp from 'capnp-ts'

import {
  WebXRController,
} from 'reality/engine/api/webxr.capnp'

import {
  GlTextureRenderer,
} from 'reality/app/xr/js/src/gl-renderer'

import {create8wCanvas} from './canvas'

const WebXRControllerFactory = (xrccPromise) => {
  const pipelineModule = () => {
    let xrcc = null
    xrccPromise.then((xrcc_) => { xrcc = xrcc_ })

    let framework_ = null
    let drawCtx_ = null
    let computeCtx_ = null
    let renderer_ = null

    let id_ = null
    let setup_ = false
    let found_ = false

    // reductionFactor is used to reduce the texture size before copying texture between ctx.
    // 4 was chosen as it brings the texture size closer to reduction done in image targets and
    // also considering the performance of reading pixels to copy textures.
    const reductionFactor_ = 4
    let framebuffer_ = null
    let pixels_ = null
    let texture_ = null

    // Dom-overlay canvas for webxr visualization
    const overlayCanvas_ = document.getElementById('xr-overlay')
    const overlayCtx_ = overlayCanvas_.getContext('2d')

    // Scales intrinsic by reduction factor.
    const scaleIntrinsic = (intrinsic) => {
      const {fh, fv, cx, cy} = intrinsic
      return {
        fh: fh / reductionFactor_,
        fv: fv / reductionFactor_,
        cx: cx / reductionFactor_,
        cy: cy / reductionFactor_,
      }
    }

    // Creates a capnp message for the intrinsic received from WebXR.
    const writeIntrinsicMessage = (width, height, intrinsic) => {
      const {fh, fv, cx, cy} = intrinsic
      const message = new capnp.Message()
      const XRFrameData = message.initRoot(WebXRController)

      XRFrameData.setTextureHeight(height)
      XRFrameData.setTextureWidth(width)

      const intrinsicData = XRFrameData.getIntrinsic()
      intrinsicData.setPixelsWidth(width)
      intrinsicData.setPixelsHeight(height)
      intrinsicData.setCenterPointX(cx)
      intrinsicData.setCenterPointY(cy)
      intrinsicData.setFocalLengthHorizontal(fh)
      intrinsicData.setFocalLengthVertical(fv)

      const buffer = message.toArrayBuffer()
      const ptr = xrcc._malloc(buffer.byteLength)
      xrcc.writeArrayToMemory(new Uint8Array(buffer), ptr)
      xrcc._c8EmAsm_initIntrinsic(ptr, buffer.byteLength)
      xrcc._free(ptr)
    }

    // Creates a capnp message for the extrinsic received from WebXR.
    const writeExtrinsicMessage = (extrinsic) => {
      const message = new capnp.Message()
      const XRFrameData = message.initRoot(WebXRController)

      const extrinsicData = XRFrameData.initExtrinsic(16)
      extrinsic.forEach((t, i) => extrinsicData.set(i, extrinsic[i]))

      const buffer = message.toArrayBuffer()
      const ptr = xrcc._malloc(buffer.byteLength)
      xrcc.writeArrayToMemory(new Uint8Array(buffer), ptr)
      xrcc._c8EmAsm_initExtrinsic(ptr, buffer.byteLength)
      xrcc._free(ptr)
    }

    // Read the detected image location capnp message and dispatch an event
    const readDetectedImageLocation = () => {
      const byteBuffer = xrcc.HEAPU8.subarray(
        window._c8.webxrOverlayCanvasPtr,
        window._c8.webxrOverlayCanvasPtr + window._c8.webxrOverlayCanvasSize
      )
      const msg = new capnp.Message(byteBuffer, false).getRoot(WebXRController)
      const position = {
        x: msg.getPlace().getPosition().getX(),
        y: msg.getPlace().getPosition().getY(),
        z: msg.getPlace().getPosition().getZ(),
      }

      const rotation = {
        x: msg.getPlace().getRotation().getX(),
        y: msg.getPlace().getRotation().getY(),
        z: msg.getPlace().getRotation().getZ(),
        w: msg.getPlace().getRotation().getW(),
      }

      const scale = Math.max(msg.getWidthInMeters(), msg.getHeightInMeters()) || 1.0

      const pose = {
        position,
        rotation,
        scale,
      }

      framework_.dispatchEvent('imageupdated', pose)
    }

    const visualize = (width, height, oldWidth) => {
      const byteBuffer = xrcc.HEAPU8.subarray(
        window._c8.webxrOverlayCanvasPtr,
        window._c8.webxrOverlayCanvasPtr + window._c8.webxrOverlayCanvasSize
      )
      const message = new capnp.Message(byteBuffer, false).getRoot(WebXRController)
      const bufferData = message.getCanvasData().toUint8Array()
      const clampedBufferData = new Uint8ClampedArray(bufferData)
      const imageData = new ImageData(clampedBufferData, oldWidth)
      const newCanvas = create8wCanvas('visualize-canvas')
      newCanvas.width = overlayCanvas_.width
      newCanvas.height = overlayCanvas_.height
      newCanvas.getContext('2d').putImageData(imageData, 0, 0)
      overlayCtx_.save()
      overlayCtx_.clearRect(0, 0, overlayCanvas_.width, overlayCanvas_.height)
      overlayCtx_.drawImage(
        newCanvas, 0, 0, width, height, 0, 0, overlayCanvas_.width, overlayCanvas_.height
      )
      overlayCtx_.restore()
    }

    // getNewId() is an emscripten method that returns an id which we can map our texture.
    // WebGL textures don't have a concept of an id, so we cant pass the id directly to C++.
    // So we use this function emscripten uses to manage OpenGL and WebGL objects internally.
    // https://github.com/emscripten-core/emscripten/blob/91a14f6c780eeb520c9f7131a9af8bda6d1ff7dd/src/library_webgl.js#L241
    // Pass the array corresponding to the GL object which here is GL.textures.
    const getNewTextureID = () => xrcc.GL.getNewId(xrcc.GL.textures)

    // Creates the necessary objects required for context transfer.
    const setupCtxTransfer = (width, height) => {
      // Setup the GLTextureRenderer for resizing and flipping the texture.
      renderer_ = GlTextureRenderer.create({
        GLctx: drawCtx_,
        toTexture: {width: width / reductionFactor_, height: height / reductionFactor_},
        flipY: false,  // flips the texture.
      })

      framebuffer_ = drawCtx_.createFramebuffer()
      const texBytes = width * height * 4
      pixels_ = new Uint8Array(texBytes)
      texture_ = computeCtx_.createTexture()
    }

    // Read the image target and copy the image to the texture created in emscripten.
    const imageTargetSetup = () => {
      const imageTarget = document.getElementById('imagetarget')

      // Creates a target loader and returns the id of the underlying texture.
      const imageTargetId = xrcc._c8EmAsm_initImageTargetLoader(
        imageTarget.height,
        imageTarget.width,
        'imagetarget'
      )

      // The id returned from initImageTargetLoader() gives us the texture surface created in
      // emscripten that we can use to bind the target image to be searched.
      const imageTargetTexture = xrcc.GL.textures[imageTargetId]

      // Bind the image to the texture.
      computeCtx_.bindTexture(computeCtx_.TEXTURE_2D, imageTargetTexture)
      computeCtx_.texImage2D(
        computeCtx_.TEXTURE_2D,
        0,
        computeCtx_.RGBA,
        computeCtx_.RGBA,
        computeCtx_.UNSIGNED_BYTE,
        imageTarget
      )

      // Initiate target processing.
      xrcc._c8EmAsm_processNewImageTarget()
    }

    // Creates a new texture in compute ctx and copies the pixels from camera texture.
    const drawToComputeCtxTransfer = (cameraTexture, width, height) => {
      const viewport = {offsetX: 0, offsetY: 0, width, height}

      // Resize and flip the texture first.
      const tex = renderer_.render({renderTexture: cameraTexture, viewport})

      // Read pixels to CPU from the texture.
      drawCtx_.bindTexture(drawCtx_.TEXTURE_2D, tex)
      drawCtx_.bindFramebuffer(drawCtx_.FRAMEBUFFER, framebuffer_)

      drawCtx_.framebufferTexture2D(drawCtx_.FRAMEBUFFER,
        drawCtx_.COLOR_ATTACHMENT0, drawCtx_.TEXTURE_2D, tex, 0)
      drawCtx_.readPixels(0, 0, width, height, drawCtx_.RGBA,
        drawCtx_.UNSIGNED_BYTE, pixels_)

      drawCtx_.bindTexture(drawCtx_.TEXTURE_2D, null)
      drawCtx_.bindFramebuffer(drawCtx_.FRAMEBUFFER, null)

      // Draw the pixels to texture created in compute ctx.
      computeCtx_.bindTexture(computeCtx_.TEXTURE_2D, texture_)

      computeCtx_.texImage2D(drawCtx_.TEXTURE_2D,
        0,
        drawCtx_.RGBA,
        width / reductionFactor_,
        height / reductionFactor_,
        0,
        drawCtx_.RGBA,
        drawCtx_.UNSIGNED_BYTE,
        pixels_)

      computeCtx_.bindTexture(computeCtx_.TEXTURE_2D, null)
    }

    // Callback method called when new camera texture created.
    const textureCreated = ({detail}) => {
      const {
        cameraTexture,
        textureHeight: height,
        textureWidth: width,
        cameraPose: extrinsic,
        renderingFov: intrinsic,
      } = detail

      // Copy the camera texture from draw ctx to compute ctx.
      drawToComputeCtxTransfer(cameraTexture, width, height)

      const reducedWidth = width / reductionFactor_
      const reducedHeight = height / reductionFactor_

      // Create the intrinsic capnp message only once.
      if (!setup_) {
        // Reducing the texture size changes intrinsic.
        const reducedIntrinsic = scaleIntrinsic(intrinsic)
        writeIntrinsicMessage(reducedWidth, reducedHeight, reducedIntrinsic)
        setup_ = true
      }

      // Create the extrinsic capnp message.
      writeExtrinsicMessage(extrinsic)

      // Assign a name and pass the texture to GL.textures array.
      // Emscripten uses this array for managing WebGL and OpenGL textures internally.
      texture_.name = id_
      xrcc.GL.textures[id_] = texture_

      if (!found_) {
        found_ = xrcc._c8EmAsm_detectGlobalLocation(id_, height, width)
      }

      if (found_) {
        found_ = xrcc._c8EmAsm_detectLocalLocation(id_, height, width)
        readDetectedImageLocation()
        // visualize() Call visualize to read capnp canvasData and write it to the dom-overlay
      }

      texture_.name = 0
      xrcc.GL.textures[id_] = null
    }

    const onAttach = ({framework}) => {
      framework_ = framework
    }

    const onStart = ({GLctx, computeCtx}) => {
      computeCtx_ = computeCtx
      drawCtx_ = GLctx

      // Change emscripten GL handle to compute ctx.
      const glHandle = xrcc.GL.registerContext(computeCtx_, computeCtx_.getContextAttributes())
      xrcc.GL.makeContextCurrent(glHandle)

      // Get new id for storing our texture.
      id_ = getNewTextureID()

      // Texture dimensions before resizing are same as screen resolution.
      const width = window.screen.width * window.devicePixelRatio
      const height = window.screen.height * window.devicePixelRatio
      const rotation = width > height ? 90 : 0

      // Setting up the dom-overlay canvas for webxr visualization
      overlayCanvas_.width = width
      overlayCanvas_.height = height

      setupCtxTransfer(width, height)
      imageTargetSetup()

      // Initialize gpu processing for the camera texture.
      xrcc._c8EmAsm_initDetection(width, height, rotation)
    }

    return {
      name: 'webxrcontroller',
      onAttach,
      listeners: [
        {event: 'threejsrendererwebxr.imagecaptured', process: textureCreated},
      ],
      onStart,
    }
  }

  return {
    pipelineModule,
  }
}

export {
  WebXRControllerFactory,
}
