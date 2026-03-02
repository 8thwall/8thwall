// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "face-roi-renderer.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:exceptions",
    "//c8:hpoint",
    "//c8:string",
    "//c8:vector",
    "//c8/geometry:face-types",
    "//c8/geometry:pixel-pinhole-camera-model",
    "//c8/pixels:pixel-buffer",
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
cc_end(0xa9b37197);

#include <cmath>

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/pixels/opengl/client-gl.h"
#include "c8/pixels/opengl/gl-error.h"
#include "c8/pixels/opengl/gl-version.h"
#include "c8/stats/scope-timer.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "reality/engine/faces/face-geometry.h"
#include "reality/engine/faces/face-roi-renderer.h"
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

ImageViewport viewportForFullImage(int videoWidth, int videoHeight, int roiWidth) {
  // Construct a viewport which is a sub-region of the 128x128 letterbox geometry that
  // blazeface wants. This 128 by 128 region is in the upper right of the destination texture.
  int targetWidth = videoWidth * roiWidth / videoHeight;
  int targetHeight = roiWidth;
  if (targetWidth > roiWidth) {
    targetWidth = roiWidth;
    targetHeight = videoHeight * roiWidth / videoWidth;
  }

  // For example, if videoHeight is 960, and videoWidth is 720, then
  // {
  //   offsetX: 16,
  //   offsetY: 0,
  //   width: 96,
  //   hieght: 128,
  // }
  int halfRoiWidth = roiWidth / 2;
  return {
    static_cast<float>(halfRoiWidth - targetWidth / 2),
    static_cast<float>(halfRoiWidth - targetHeight / 2),
    static_cast<float>(targetWidth),
    static_cast<float>(targetHeight),
  };
}

ImageViewport viewportForCrop(int i) {
  // Select out a 192x192 sub-region of the destination texture.
  float offsetX = 0;
  float offsetY = 0;

  // ROI 0 gets drawn to the upper right (192, 0)
  if (i == 0) {
    offsetX = 192;
  }

  // ROI 1 gets drawn to the lower left (192, 0)
  if (i == 1) {
    offsetY = 192;
  }

  // ROI 2 gets drawn to the lower right (192, 192)
  if (i == 2) {
    offsetX = 192;
    offsetY = 192;
  }

  // i == 3 and greater go where the letterbox crop normally goes, at 0, 0

  // All ROIs get drawn with width/heigh 192, 192.
  return {
    offsetX,
    offsetY,
    192,
    192,
  };
}

}  // namespace

void FaceRoiRenderer::initialize(FaceRoiShader *shader, int srcWidth, int srcHeight) {
  C8Log("[face-roi-renderer] initialize(%p, %d, %d)", shader, srcWidth, srcHeight);
  inputWidth_ = srcWidth;
  inputHeight_ = srcHeight;

  texWidth_ = 512;
  texHeight_ = 512;
  outputWidth_ = 2 * 192;
  outputHeight_ = 2 * 192;

  // Clear previous memory before allocating a new image.
  outpix_ = RGBA8888PlanePixelBuffer(0, 0);
  outpix_ = RGBA8888PlanePixelBuffer(outputHeight_, outputWidth_);
  quad_ = makeVertexArrayRect();
  faceFboInitialized_ = false;

  // Manage gr8shader init/cleanup externally.
  shaders_ = shader;
}

void FaceRoiRenderer::enableWebGl2PixelBuffer() {
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

// Configure the renderer to add ROIs for the detected faces on its next draw pass.
void FaceRoiRenderer::setDetectedFaces(
  const Vector<DetectedPoints> &globalFaces, const Vector<DetectedPoints> &localFaces) {
  auto detectedFaces = localFaces;
  detectedFaces.insert(detectedFaces.end(), globalFaces.begin(), globalFaces.end());

  setDetectedFaces(detectedFaces);
};

void FaceRoiRenderer::setDetectedFaces(const Vector<DetectedPoints> &faces) {
  nextFaceRois_.clear();
  nextFaceRois_.reserve(faces.size());
  for (const auto &face : faces) {
    nextFaceRois_.push_back(detectionRoi(face));
    // Preserve the ROI id.
    nextFaceRois_.back().faceId = face.roi.faceId;
  }
};

void FaceRoiRenderer::drawStage(
  GlTexture src, GlFramebufferObject *dest, const GlProgramObject *shader) {
  src.bind();

  shaders_->bind(shader);

  dest->bind();
  checkGLError("[face-roi-renderer] bind framebuffer");

  glViewport(0, 0, texWidth_, texHeight_);
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  GlVertexArray &quadToDraw = quad_;

  // Load vertex data
  quadToDraw.bind();
  glFrontFace(GL_CCW);
  checkGLError("[face-roi-renderer] bind quad");

  // Set shader warp back to identity.

  // Always draw the main image.
  HMatrix imat = HMatrixGen::i();
  glUniformMatrix4fv(shader->location("mvp"), 1, GL_FALSE, imat.data().data());
  glUniform1i(shader->location("flipY"), flipY_);
  glUniform1i(shader->location("rotation"), 0);
  auto v = viewportForFullImage(inputWidth_, inputHeight_, roiWidth_);
  glViewport(v.x, v.y, v.w, v.h);
  quadToDraw.drawElements();
  checkGLError("[face-roi-renderer] draw elements (full)");

  for (int i = 0; i < nextFaceRois_.size(); ++i) {
    const auto *mvp = nextFaceRois_[i].warp.data().begin();
    glUniformMatrix4fv(shader->location("mvp"), 1, GL_FALSE, mvp);
    glUniform1i(shader->location("rotation"), 0);
    auto v = viewportForCrop(i);
    glViewport(v.x, v.y, v.w, v.h);
    quadToDraw.drawElements();
    checkGLError("[face-roi-renderer] draw elements (full)");
  }

  quadToDraw.unbind();
  checkGLError("[face-roi-renderer] quadToDraw unbind");
}

void FaceRoiRenderer::readPixels() {
  if (!needsRead_) {
    return;
  }
  ScopeTimer t("face-renderer-read-pixels");

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
  dest_.bind();
  dest_.tex().bind();

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

  checkGLError("[face-roi-renderer] read pixels");
  hasRender_ = true;
  faceRois_ = drawnFaceRois_;
  tex_ = drawnTex_;
  drawnFaceRois_.clear();
  drawnTex_ = 0;
}

void FaceRoiRenderer::drawTexture(GlTexture cameraTexture) {
  ScopeTimer t("face-renderer-draw-tex");

  // initialize face dest FBO after camera textures are initialized
  if (!faceFboInitialized_) {
    dest_ = makeNearestRGBA8888Framebuffer(texWidth_, texHeight_);
    faceFboInitialized_ = true;
  }

  drawStage(cameraTexture, &dest_, shaders_->shader1());

  // Copy data to byte array
  if (usePixelBuffer_) {
#ifdef JAVASCRIPT
    dest_.bind();
    glActiveTexture(GL_TEXTURE0);
    dest_.tex().bind();

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
  drawnFaceRois_ = nextFaceRois_;
  drawnTex_ = cameraTexture.texture;
  nextFaceRois_.clear();
}

FaceRenderResult FaceRoiRenderer::result() const {
  auto fvp = viewportForFullImage(inputWidth_, inputHeight_, roiWidth_);
  auto op = outpix_.pixels();

  FaceRenderResult r{
    tex_,  // srcTex
    op,    // rawPixels,
    {fvp,
     imageForViewport(op, fvp),
     {ImageRoi::Source::FACE, 0, "", HMatrixGen::i()}},  // faceDetectImage
    {},                                                  // faceMeshImages
  };

  for (int i = 0; i < faceRois_.size(); ++i) {
    auto vp = viewportForCrop(i);
    r.faceMeshImages.push_back({vp, imageForViewport(op, vp), faceRois_[i]});
  }

  return r;
}

void FaceRoiRenderer::draw(GLuint cameraTexture, GpuReadPixelsOptions options) {
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
    checkGLError("[face-roi-renderer] cache gl state");
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
    checkGLError("[face-roi-renderer] restore bindings");
  }
}

}  // namespace c8
