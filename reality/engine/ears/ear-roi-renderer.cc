// Copyright (c) 2023 Niantic Labs
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)
//
// Renderer for ear landmarks

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "ear-roi-renderer.h",
  };
  deps = {
    ":ear-types",
    "//c8:c8-log",
    "//c8:exceptions",
    "//c8:hpoint",
    "//c8:string",
    "//c8:vector",
    "//c8/geometry:pixel-pinhole-camera-model",
    "//c8/pixels:pixel-buffers",
    "//c8/pixels/opengl:gl-error",
    "//c8/pixels/opengl:gl-headers",
    "//c8/pixels/opengl:gl-quad",
    "//c8/pixels/opengl:gl-texture",
    "//c8/pixels/opengl:gl-framebuffer",
    "//c8/pixels/opengl:gl-program",
    "//c8/pixels:pixels",
    "//c8/stats:scope-timer",
    "//reality/engine/faces:face-geometry",
    "//reality/engine/faces:face-roi-shader",
    "//reality/engine/geometry:image-warp",
    "//reality/engine/render:renderers",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xa976271c);

#include <cmath>

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/pixels/opengl/client-gl.h"
#include "c8/pixels/opengl/gl-error.h"
#include "c8/pixels/opengl/gl-version.h"
#include "c8/stats/scope-timer.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "reality/engine/ears/ear-roi-renderer.h"
#include "reality/engine/ears/ear-types.h"
#include "reality/engine/faces/face-geometry.h"
#include "reality/engine/geometry/image-warp.h"

#if C8_OPENGL_VERSION_2
#include "c8/pixels/opengl/glext.h"
#endif

#if JAVASCRIPT
#include <emscripten.h>
#endif

namespace c8 {

namespace {

#if C8_OPENGL_VERSION_2
static auto glGenVertexArrays =
  (PFNGLGENVERTEXARRAYSOESPROC)clientGlGetProcAddress("glGenVertexArraysOES");
static auto glDeleteVertexArrays =
  (PFNGLDELETEVERTEXARRAYSOESPROC)clientGlGetProcAddress("glDeleteVertexArraysOES");
static auto glBindVertexArray =
  (PFNGLBINDVERTEXARRAYOESPROC)clientGlGetProcAddress("glBindVertexArrayOES");
static auto GL_VERTEX_ARRAY_BINDING = GL_VERTEX_ARRAY_BINDING_OES;
#endif

}  // namespace

// Each face will be rendered into one column, with left ear on top and right ear on the bottom
// returns {offsetX, offsetY, width, height}
ImageViewport earViewportForCrop(int i) {
  // Select out a 112x160 sub-region of the destination texture.
  const int faceOffset = i / 2;
  const int earOffset = i % 2;
  int offsetX = faceOffset * EAR_LANDMARK_DETECTION_INPUT_WIDTH;
  int offsetY = earOffset * EAR_LANDMARK_DETECTION_INPUT_HEIGHT;

  // i == 6 and greater go where the letterbox crop normally goes, at 0, 0
  if (i >= 6) {
    offsetX = offsetY = 0;
  }

  // All ROIs get drawn with width/heigh 112, 160.
  return {
    static_cast<float>(offsetX),
    static_cast<float>(offsetY),
    static_cast<float>(EAR_LANDMARK_DETECTION_INPUT_WIDTH),
    static_cast<float>(EAR_LANDMARK_DETECTION_INPUT_HEIGHT),
  };
}

void EarRoiRenderer::initialize(FaceRoiShader *shader, int srcWidth, int srcHeight) {
  C8Log("[ear-roi-renderer] initialize(%p, %d, %d)", shader, srcWidth, srcHeight);
  inputWidth_ = srcWidth;
  inputHeight_ = srcHeight;

  texWidth_ = 512;
  texHeight_ = 512;
  // we are tracking maximum 3 faces simultaneously
  outputWidth_ = 3 * EAR_LANDMARK_DETECTION_INPUT_WIDTH;
  outputHeight_ = 2 * EAR_LANDMARK_DETECTION_INPUT_HEIGHT;

  // Clear previous memory before allocating a new image.
  outpix_ = RGBA8888PlanePixelBuffer(0, 0);
  outpix_ = RGBA8888PlanePixelBuffer(outputHeight_, outputWidth_);
  quad_ = makeVertexArrayRect();
  dest1_ = makeNearestRGBA8888Framebuffer(texWidth_, texHeight_);

  // Manage gr8shader init/cleanup externally.
  shaders_ = shader;
}

void EarRoiRenderer::enableWebGl2PixelBuffer() {
#ifdef JAVASCRIPT
  usePixelBuffer_ = true;
  glGenBuffers(1, &pixelBuffer_);
  EM_ASM_(
    {
      Module.ctx.bindBuffer(Module.ctx.PIXEL_PACK_BUFFER, GL.buffers[$0]);
      bufferInit = new Uint8Array($1);
      Module.ctx.bufferData(Module.ctx.PIXEL_PACK_BUFFER, bufferInit, Module.ctx.DYNAMIC_READ, 0);
      Module.ctx.bindBuffer(Module.ctx.PIXEL_PACK_BUFFER, null);
    },
    pixelBuffer_,
    outputWidth_ * outputHeight_ * 4);
#else
  C8_THROW("Using pixel buffer outside of JavaScript not implemented");
#endif
}

void EarRoiRenderer::setDetectedEars(const Vector<DetectedPoints> &ears) {
  nextEarRois_.clear();
  nextEarRois_.reserve(ears.size());
  for (const auto &ear : ears) {
    nextEarRois_.push_back(detectionRoiNoPadding(ear));
    // Preserve the ROI id.
    nextEarRois_.back().faceId = ear.roi.faceId;
  }
};

void EarRoiRenderer::drawStage(
  GlTexture src, GlFramebufferObject *dest, const GlProgramObject *shader) {
  src.bind();

  shaders_->bind(shader);

  dest->bind();
  checkGLError("[ear-roi-renderer] bind framebuffer");

  glViewport(0, 0, texWidth_, texHeight_);
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  GlVertexArray &quadToDraw = quad_;

  // Load vertex data
  quadToDraw.bind();
  glFrontFace(GL_CCW);
  checkGLError("[ear-roi-renderer] bind quad");

  // Set shader warp back to identity.
  HMatrix imat = HMatrixGen::i();
  glUniformMatrix4fv(shader->location("mvp"), 1, GL_FALSE, imat.data().data());
  glUniform1i(shader->location("flipY"), flipY_);
  glUniform1i(shader->location("rotation"), 0);
  // draw local ears
  for (size_t i = 0; i < nextEarRois_.size(); ++i) {
    const auto *mvp = nextEarRois_[i].warp.data().begin();
    glUniformMatrix4fv(shader->location("mvp"), 1, GL_FALSE, mvp);
    glUniform1i(shader->location("rotation"), 0);
    auto v = earViewportForCrop(i);
    glViewport(v.x, v.y, v.w, v.h);
    quadToDraw.drawElements();
    checkGLError("[ear-roi-renderer] draw elements (full)");
  }

  quadToDraw.unbind();
  checkGLError("[ear-roi-renderer] quadToDraw unbind");
}

void EarRoiRenderer::readPixels() {
  if (!needsRead_) {
    return;
  }
  ScopeTimer t("ear-renderer-read-pixels");

  needsRead_ = false;

#ifdef JAVASCRIPT
  if (usePixelBuffer_) {
    EM_ASM_({
      if (Module.ctx.fenceSync && Module.ctx.__glSync_) {
        // NOTE(christoph): This was causing a lag too frequently so we will continue depending on
        // Chrome to guard the getBufferSubData itself.
        // Sometimes chrome seems to spin forever here, so add a 300ms timeout as a safety valve.
        // Module.ctx.__then_ = Date.now() + 300;

        // Spin and wait on the fence to prevent chrome from printing a warning to the console
        // 'READ-usage buffer was read back without waiting on a fence.'
        // while (Module.ctx.clientWaitSync(Module.ctx.__glSync_, 0, 0) ==
        // Module.ctx.TIMEOUT_EXPIRED
        //    && Date.now() < Module.ctx.__then_) {
        //   // continue
        // }

        Module.ctx.clientWaitSync(Module.ctx.__glSync_, 0, 0);
        Module.ctx.deleteSync(Module.ctx.__glSync_);
        Module.ctx.__glSync_ = undefined;
        // Module.ctx.__then_ = undefined;
      }
    });
  }
#endif

  // Copy data to byte array
  glActiveTexture(GL_TEXTURE0);
  dest1_.bind();
  dest1_.tex().bind();

  auto o = outpix_.pixels();

  if (usePixelBuffer_) {
#ifdef JAVASCRIPT
    EM_ASM_(
      {
        Module.ctx.bindBuffer(Module.ctx.PIXEL_PACK_BUFFER, GL.buffers[$2]);
        Module.ctx.getBufferSubData(Module.ctx.PIXEL_PACK_BUFFER, 0, HEAPU8.subarray($0, $0 + $1));
        Module.ctx.bindBuffer(Module.ctx.PIXEL_PACK_BUFFER, null);
      },
      o.pixels(),
      o.rows() * o.rowBytes(),
      pixelBuffer_);
#else
    C8_THROW("Using pixel buffer outside of JavaScript not implemented");
#endif
  } else {
    glReadPixels(0, 0, o.cols(), o.rows(), GL_RGBA, GL_UNSIGNED_BYTE, o.pixels());
  }

  checkGLError("[ear-roi-renderer] read pixels");
  hasRender_ = true;
  earRois_ = drawnEarRois_;
  tex_ = drawnTex_;
  drawnEarRois_.clear();
  drawnTex_ = 0;
}

void EarRoiRenderer::drawTexture(GlTexture cameraTexture) {
  ScopeTimer t("ear-renderer-draw-tex");

  drawStage(cameraTexture, &dest1_, shaders_->shader1());

  // Copy data to byte array
  if (usePixelBuffer_) {
#ifdef JAVASCRIPT
    dest1_.bind();
    glActiveTexture(GL_TEXTURE0);
    dest1_.tex().bind();

    EM_ASM_(
      {
        Module.ctx.bindBuffer(Module.ctx.PIXEL_PACK_BUFFER, GL.buffers[$0]);
        Module.ctx.readPixels(0, 0, $1, $2, Module.ctx.RGBA, Module.ctx.UNSIGNED_BYTE, 0);
        Module.ctx.bindBuffer(Module.ctx.PIXEL_PACK_BUFFER, null);
      },
      pixelBuffer_,
      outputWidth_,
      outputHeight_);
#else
    C8_THROW("Using pixel buffer outside of JavaScript not implemented");
#endif
  }

#ifdef JAVASCRIPT
  if (usePixelBuffer_) {
    EM_ASM_({
      if (Module.ctx.fenceSync) {
        Module.ctx.__glSync_ = Module.ctx.fenceSync(Module.ctx.SYNC_GPU_COMMANDS_COMPLETE, 0);
      }
    });
  }
#endif

  glFlush();

  needsRead_ = true;
  drawnEarRois_ = nextEarRois_;
  drawnTex_ = cameraTexture.texture;
  nextEarRois_.clear();
}

EarRenderResult EarRoiRenderer::result() const {
  auto op = outpix_.pixels();

  EarRenderResult r{
    tex_,  // srcTex
    op,    // rawPixels,
    {},    // earDetectImages
  };

  for (int i = 0; i < earRois_.size(); ++i) {
    auto vp = earViewportForCrop(i);
    r.earDetectImages.push_back({vp, imageForViewport(op, vp), earRois_[i]});
  }

  return r;
}

void EarRoiRenderer::draw(GLuint cameraTexture, GpuReadPixelsOptions options) {
  GLint restoreActiveTexture = 0;
  GLint restoreTexture = 0;
  GLint restoreProgram = 0;
  GLint restoreElementBuffer = 0;
  GLint restoreVertexArray = 0;
  GLint restoreArrayBuffer = 0;
  GLint restoreFrameBuffer = 0;
  GLint restoreViewport[4];
  GLfloat restoreClearColor[4];

  if (isRestoreStateGpuReadPixels(options)) {
    ScopeTimer t("set-restore-bindings-gl");
    glGetIntegerv(GL_VIEWPORT, restoreViewport);
    glGetFloatv(GL_COLOR_CLEAR_VALUE, restoreClearColor);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &restoreFrameBuffer);
    glGetIntegerv(GL_CURRENT_PROGRAM, &restoreProgram);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &restoreElementBuffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &restoreVertexArray);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &restoreArrayBuffer);
    glGetIntegerv(GL_ACTIVE_TEXTURE, &restoreActiveTexture);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &restoreTexture);
    checkGLError("[ear-roi-renderer] cache gl state");
  }

  auto ct = wrapRGBA8888Texture(cameraTexture, inputWidth_, inputHeight_);

  if (isDeferredGpuReadPixels(options)) {
    readPixels();
    drawTexture(ct);
  } else {
    drawTexture(ct);
    readPixels();
  }

  if (isRestoreStateGpuReadPixels(options)) {
    ScopeTimer t("restore-bindings");
    glActiveTexture(restoreActiveTexture);
    glBindBuffer(GL_ARRAY_BUFFER, restoreArrayBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, restoreFrameBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, restoreElementBuffer);
    glBindVertexArray(restoreVertexArray);
    glUseProgram(restoreProgram);
    glBindTexture(GL_TEXTURE_2D, restoreTexture);
    glViewport(restoreViewport[0], restoreViewport[1], restoreViewport[2], restoreViewport[3]);
    glClearColor(
      restoreClearColor[0], restoreClearColor[1], restoreClearColor[2], restoreClearColor[3]);
    checkGLError("[ear-roi-renderer] restore bindings");
  }
}

}  // namespace c8
