// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "gl-reality-frame.h",
  };
  deps = {
    ":gr8-defs",
    ":gr8-feature-shader",
    "//c8:c8-log",
    "//c8:exceptions",
    "//c8:hpoint",
    "//c8:parameter-data",
    "//c8:string",
    "//c8:vector",
    "//c8/geometry:pixel-pinhole-camera-model",
    "//c8/pixels:gr8-pyramid",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixels",
    "//c8/pixels/opengl:gl-error",
    "//c8/pixels/opengl:gl-headers",
    "//c8/pixels/opengl:gl-quad",
    "//c8/pixels/opengl:gl-texture",
    "//c8/pixels/opengl:gl-framebuffer",
    "//c8/pixels/opengl:gl-program",
    "//c8/stats:scope-timer",
    "//reality/engine/geometry:image-warp",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xf3e5898a);

#include <cmath>

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/parameter-data.h"
#include "c8/pixels/opengl/client-gl.h"
#include "c8/pixels/opengl/gl-error.h"
#include "c8/pixels/opengl/gl-version.h"
#include "c8/stats/scope-timer.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "reality/engine/features/gl-reality-frame.h"
#include "reality/engine/features/gr8-defs.h"
#include "reality/engine/geometry/image-warp.h"

#if C8_OPENGL_VERSION_2
#include "c8/pixels/opengl/glext.h"
#endif

#if JAVASCRIPT
#include <emscripten.h>
#endif

namespace c8 {

namespace {

struct Settings {
  // Size of texture to use for image pyramid. The texture is square.
  int32_t glRealityFramePyramidSize;
};

const Settings &settings() {
  static const Settings settings_{
    globalParams().getOrSet("GlRealityFrame.PyramidSize", 1024),
  };
  return settings_;
}

constexpr bool ALWAYS_FILTER_INPUT = true;

#if C8_OPENGL_VERSION_2
static PFNGLGENVERTEXARRAYSOESPROC glGenVertexArrays = nullptr;
static PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArrays = nullptr;
static PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray = nullptr;
static auto GL_VERTEX_ARRAY_BINDING = GL_VERTEX_ARRAY_BINDING_OES;
#endif

static void lazyInitVertexArrayExtension() {
#if C8_OPENGL_VERSION_2
  if (glGenVertexArrays == nullptr) {
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC)clientGlGetProcAddress("glGenVertexArraysOES");
    glDeleteVertexArrays =
      (PFNGLDELETEVERTEXARRAYSOESPROC)clientGlGetProcAddress("glDeleteVertexArraysOES");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC)clientGlGetProcAddress("glBindVertexArrayOES");
    GL_VERTEX_ARRAY_BINDING = GL_VERTEX_ARRAY_BINDING_OES;
  }
#endif
}

constexpr int roundToPowerOfTwo(int v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return v;
}

constexpr int FILTERED_WIDTH = 720;
constexpr int FILTERED_HEIGHT = 960;

}  // namespace

int32_t GlRealityFrame::pyrSize() { return settings().glRealityFramePyramidSize; }

void GlRealityFrame::initializeLevels(
  int srcWidth, int srcHeight, int outputWidth, int outputHeight) {
  int targetHeight = 640;
  if (outputHeight > 2047 && outputWidth > 2047) {
    texWidth_ = 2048;
    texHeight_ = 2048;
    targetHeight = 1280;
  } else if (outputHeight > 1023 && outputWidth > 1023) {
    texWidth_ = 1024;
    texHeight_ = 1024;
    targetHeight = 640;
  } else if (outputHeight > 511 && outputWidth > 511) {
    texWidth_ = 512;
    texHeight_ = 512;
    targetHeight = 320;
  } else if (outputHeight > 255 && outputWidth > 255) {
    texWidth_ = 256;
    texHeight_ = 256;
    targetHeight = 160;
  } else if (outputHeight > 127 && outputWidth > 127) {
    texWidth_ = 128;
    texHeight_ = 128;
    targetHeight = 80;
  }

  float srcAspect = static_cast<float>(srcWidth) / srcHeight;
  float targetWidth = srcAspect * targetHeight;
  if (targetWidth > targetHeight * 3 / 4) {
    float scale = (targetHeight * 3 / 4) / targetWidth;
    targetWidth *= scale;
    targetHeight *= scale;
  }

  int nLevels = 4;
  levels_.resize(nLevels);
  for (int i = 0; i < nLevels; ++i) {
    float scale = 1.0f / std::pow(1.44, i);
    levels_[i].w = static_cast<int>(targetWidth * scale);
    levels_[i].h = static_cast<int>(targetHeight * scale);
  }

  levels_[0].r = 3;
  levels_[0].c = 3;
  levels_[1].r = 3;
  levels_[1].c = levels_[0].w + 3 + 3;
  std::swap(levels_[1].h, levels_[1].w);
  levels_[1].rotated = true;
  levels_[2].r = levels_[1].h + 3 + 3;
  levels_[2].c = levels_[0].w + 3 + 3;
  levels_[3].r = levels_[2].r;
  levels_[3].c = levels_[2].c + levels_[2].w + 3;

  int roiMaxWidth = (texWidth_ - 15) / 4;
  int roiMaxHeight = texHeight_;
  int roiY = 0;
  for (auto l : levels_) {
    int r = l.r + l.h + 3;
    int h = texHeight_ - r - 3;
    if (h < roiMaxHeight) {
      roiMaxHeight = h;
      roiY = r;
    }
  }

  // NOTE(dat): The ROI width and height has a pre-specified aspect and not related to search image
  // pixel aspect ratio
  int roiHeight = roiMaxHeight * ROI_ASPECT > roiMaxWidth ? roiMaxWidth / ROI_ASPECT : roiMaxHeight;
  int roiWidth = roiMaxWidth / ROI_ASPECT > roiMaxHeight ? roiMaxHeight * ROI_ASPECT : roiMaxWidth;

  roiLayouts_.resize(4);
  for (int i = 0; i < roiLayouts_.size(); ++i) {
    auto &l = roiLayouts_[i];
    l.w = roiWidth;
    l.h = roiHeight;
    l.r = roiY;
    l.c = i * (3 + roiWidth) + 3;
    l.rotated = false;
  }

  outputWidth_ = texWidth_;
  outputHeight_ = texHeight_;
}

void GlRealityFrame::initialize(
  Gr8FeatureShader *shader, int srcWidth, int srcHeight, int rotation) {
  lazyInitVertexArrayExtension();
  int outputWidth = pyrSize();
  int outputHeight = pyrSize();
  bool rotate = srcWidth > srcHeight;
  inputWidth_ = rotate ? srcHeight : srcWidth;
  inputHeight_ = rotate ? srcWidth : srcHeight;

  filteredInputWidth_ = rotate ? FILTERED_HEIGHT : FILTERED_WIDTH;
  filteredInputHeight_ = rotate ? FILTERED_WIDTH : FILTERED_HEIGHT;

  rotation_ = rotation;
  initializeLevels(inputWidth_, inputHeight_, outputWidth, outputHeight);
  // Clear previous memory before allocating a new image.
  outpix_ = RGBA8888PlanePixelBuffer(0, 0);
  outpix_ = RGBA8888PlanePixelBuffer(outputHeight_, outputWidth_);
  quad_ = makeVertexArrayRect();
  GlSubRect subRect{0, 0, outputWidth_, outputHeight_, texWidth_, texHeight_};
  subQuad_ = makeVertexArrayRect(subRect, subRect);
  // The 'filteredInput' framebuffer is a normalized input for our shading stack. It
  // is a power-of-two texture of size 1024x1024, containing an input image of
  // the dimension 960x720 in the upper-left corner. This input is pre-filtered
  // with a 3x3 blur of sigma=0.75px, which is appropriate to downsample to the
  // first pyramid level without introducing moire artifacts. The texture is a
  // linear-interpolation texture, which we can do in sampling for this texture
  // instead of in a shader as we do for the pyramid.
  filteredInput_ = makeLinearRGBA8888Framebuffer(
    roundToPowerOfTwo(filteredInputWidth_), roundToPowerOfTwo(filteredInputHeight_));
  dest1_ = makeNearestRGBA8888Framebuffer(texWidth_, texHeight_);
  dest2_ = makeNearestRGBA8888Framebuffer(texWidth_, texHeight_);
  dest3_ = makeNearestRGBA8888Framebuffer(texWidth_, texHeight_);

  // Manage gr8shader init/cleanup externally.
  gr8Shader_ = shader;
}

void GlRealityFrame::enableWebGl2PixelBuffer() {
#ifdef JAVASCRIPT
  usePixelBuffer_ = true;
  glGenBuffers(1, &pixelBuffer_);
  EM_ASM_(
    {
      Module.ctx.bindBuffer(Module.ctx.PIXEL_PACK_BUFFER, GL.buffers[$0]);
      const bufferInit = new Uint8Array($1);
      Module.ctx.bufferData(Module.ctx.PIXEL_PACK_BUFFER, bufferInit, Module.ctx.DYNAMIC_READ, 0);
      Module.ctx.bindBuffer(Module.ctx.PIXEL_PACK_BUFFER, null);
    },
    pixelBuffer_,
    outputWidth_ * outputHeight_ * 4);
#else
  C8_THROW("Using pixel buffer outside of JavaScript not implemented");
#endif
}

void GlRealityFrame::drawPyramidBlurStage(GlTexture src, GlFramebufferObject *dest, int level) {
  const GlProgramObject *shader = gr8Shader_->shaderPyramidBlur();
  src.bind();

  gr8Shader_->bindAndSetParams(shader);

  dest->bind();
  checkGLError("[gl-reality-frame] bind framebuffer");

  GlVertexArray &quadToDraw = quad_;

  // Load vertex data
  quadToDraw.bind();
  glFrontFace(GL_CCW);
  checkGLError("[gl-reality-frame] bind quad");

  // Only render to the image level that we are blurring.
  auto l = levels_[level];
  glViewport(l.c, l.r, l.w, l.h);
  glUniform4f(shader->location("roi"), l.c, l.r, l.w, l.h);
  glUniform2f(shader->location("scale"), 1.0 / texWidth_, 1.0 / texHeight_);
  quadToDraw.drawElements();
  checkGLError("[gl-reality-frame] draw elements (pyramid blur)");
}

void GlRealityFrame::drawPyramidDecimateStage(GlTexture src, GlFramebufferObject *dest, int level) {
  const GlProgramObject *shader = gr8Shader_->shaderPyramidDecimate();
  src.bind();

  gr8Shader_->bindAndSetParams(shader);

  dest->bind();
  checkGLError("[gl-reality-frame] bind framebuffer");

  GlVertexArray &quadToDraw = quad_;

  // Load vertex data
  quadToDraw.bind();
  glFrontFace(GL_CCW);
  checkGLError("[gl-reality-frame] bind quad");

  // Sample from the  current ROI.
  auto ol = levels_[level];
  glUniform4f(shader->location("roi"), ol.c, ol.r, ol.w, ol.h);

  // Only render to the next image level viewport.
  auto l = levels_[level + 1];
  glViewport(l.c, l.r, l.w, l.h);
  glUniform2f(shader->location("scale"), 1.0 / texWidth_, 1.0 / texHeight_);

  auto oShaderRotation = ol.rotated ? rotation_ - 90 : rotation_;
  auto shaderRotation = l.rotated ? rotation_ - 90 : rotation_;
  auto rotationDiff = shaderRotation - oShaderRotation;
  glUniform1i(shader->location("rotation"), rotationDiff);

  quadToDraw.drawElements();
  checkGLError("[gl-reality-frame] draw elements (pyramid blur)");
}
void GlRealityFrame::drawInputStage(GlTexture src, GlFramebufferObject *dest) {
  const GlProgramObject *shader = gr8Shader_->shaderInput();
  src.bind();

  gr8Shader_->bindAndSetParams(shader);

  dest->bind();
  checkGLError("[gl-reality-frame] bind framebuffer");

  // Load vertex data
  quad_.bind();
  glFrontFace(GL_CCW);
  checkGLError("[gl-reality-frame] bind quad");

  glUniform2f(shader->location("scale"), 1.0 / inputWidth_, 1.0 / inputHeight_);

  // Draw the filtered input.
  glViewport(0, 0, filteredInputWidth_, filteredInputHeight_);
  quad_.drawElements();
  checkGLError("[gl-reality-frame] draw elements (input filter)");

  quad_.unbind();
  checkGLError("[gl-reality-frame] quadToDraw unbind");
}

void GlRealityFrame::drawInitialStage(
  GlTexture src, GlTexture roiSrc, GlFramebufferObject *dest, const GlProgramObject *shader) {
  src.bind();

  gr8Shader_->bindAndSetParams(shader);

  dest->bind();
  checkGLError("[gl-reality-frame] bind framebuffer");

  // Load vertex data
  quad_.bind();
  glFrontFace(GL_CCW);
  checkGLError("[gl-reality-frame] bind quad");

  HMatrix imat = HMatrixGen::i();
  // Set shader warp back to identity.
  glUniformMatrix4fv(shader->location("mvp"), 1, GL_FALSE, imat.data().data());
  glUniform1i(shader->location("clear"), 0);
  glUniform1i(shader->location("flipY"), flipY_);

  glUniform2f(shader->location("scale"), 1.0 / src.width(), 1.0 / src.height());

  if (shouldFilterInput()) {
    glUniform2f(shader->location("pixScale"), filteredInputWidth_, filteredInputHeight_);
  } else {
    glUniform2f(shader->location("pixScale"), inputWidth_, inputHeight_);
  }

  // Draw the first pyramid level.
  auto l = levels_[0];
  auto shaderRotation = l.rotated ? rotation_ - 90 : rotation_;
  glUniform1i(shader->location("rotation"), shaderRotation);
  glViewport(l.c, l.r, l.w, l.h);
  quad_.drawElements();
  checkGLError("[gl-reality-frame] draw elements (pyramid)");

  // If any ROIs have been specified, draw them too.
  if (!nextRois_.empty() || !roiLayouts_.empty()) {
    roiSrc.bind();
    glUniform2f(shader->location("scale"), 1.0 / roiSrc.width(), 1.0 / roiSrc.height());
    glUniform2f(shader->location("pixScale"), inputWidth_, inputHeight_);
  }
  int i = 0;
  for (; i < nextRois_.size(); ++i) {
    const auto &r = nextRois_[i];
    auto l = r.layout;
    auto shaderRotation = l.rotated ? rotation_ - 90 : rotation_;
    glUniform1i(shader->location("rotation"), shaderRotation);
    glViewport(l.c, l.r, l.w, l.h);

    // Draw over the full quad to clear it from the previous frame.
    glUniformMatrix4fv(shader->location("mvp"), 1, GL_FALSE, imat.data().data());
    glUniform1i(shader->location("clear"), 1);
    quad_.drawElements();

    // Draw the ROI specified by warp or curvy geometry
    glUniformMatrix4fv(shader->location("mvp"), 1, GL_FALSE, r.roi.warp.data().data());
    glUniform1i(shader->location("clear"), 0);

    bool isCurvy = r.roi.source == ImageRoi::Source::CURVY_IMAGE_TARGET;
    glUniform1i(shader->location("isCurvy"), isCurvy);
    if (isCurvy) {
      // Adjust dimensions for landscape image targets (including isRotated=true).
      CurvyImageGeometry geom = r.roi.geom;
      bool isRotated = false;
      if (geom.srcCols > geom.srcRows) {
        isRotated = true;
        std::swap(geom.srcRows, geom.srcCols);
      }
      float sx = (geom.activationRegion.right - geom.activationRegion.left) / (geom.srcCols - 1);
      float sy = 1.0f / (geom.srcRows - 1);
      float shift = geom.activationRegion.left;

      // For non 3x4 image targets, the lifted image target won't take up the full viewport of the
      // quad. Compute the full size of the quad in pixels so that the lifted image target can
      // mantain its aspect ratio and only draw to part.
      float aspectRatio = 1.f * r.layout.w / r.layout.h;
      float roiRows = (static_cast<float>(geom.srcCols) / geom.srcRows > aspectRatio)
        ? geom.srcCols / aspectRatio
        : geom.srcRows;
      float roiCols = (static_cast<float>(geom.srcCols) / geom.srcRows > aspectRatio)
        ? geom.srcCols
        : aspectRatio * geom.srcRows;

      // Other layouts use a vertex shader to pre-rotate the image, but we don't pre-rotate except
      // in the case of isRotated=true images, when we need to rotate 90 degrees clockwise.
      glUniform1i(shader->location("rotation"), isRotated ? 90 : 0);
      // But we still need to do a post-rotation after our geometry mapping.
      glUniform1i(shader->location("curvyPostRotation"), shaderRotation);
      glUniform2f(shader->location("targetDims"), geom.srcCols, geom.srcRows);
      // Dimension of ROI used in search. Almost always 480x640 regardless of target
      // pixel dimension or camera pixel dimension.
      glUniform2f(shader->location("roiDims"), roiCols, roiRows);
      // Search dims = dimension of the search camera feed. Default to 480x640 but is 360x640 for
      // Google Pixel camera.
      glUniform2f(shader->location("searchDims"), levels_[0].w, levels_[0].h);
      glUniformMatrix4fv(
        shader->location("intrinsics"), 1, GL_FALSE, r.roi.intrinsics.data().data());
      glUniformMatrix4fv(
        shader->location("globalPoseInverse"), 1, GL_FALSE, r.roi.globalPose.inv().data().data());
      glUniform2f(shader->location("scales"), sx, sy);
      glUniform1f(shader->location("shift"), shift);
      glUniform1f(shader->location("radius"), r.roi.geom.radius);
      glUniform1f(shader->location("radiusBottom"), r.roi.geom.radiusBottom);
    }
    quad_.drawElements();

    checkGLError("[gl-reality-frame] draw elements (pyramid)");
  }
  for (; i < roiLayouts_.size(); ++i) {
    auto l = roiLayouts_[i];
    auto shaderRotation = l.rotated ? rotation_ - 90 : rotation_;
    glUniform1i(shader->location("rotation"), shaderRotation);
    glViewport(l.c, l.r, l.w, l.h);

    // Draw over the full quad to clear it from the previous frame.
    glUniformMatrix4fv(shader->location("mvp"), 1, GL_FALSE, imat.data().data());
    glUniform1i(shader->location("clear"), 1);
    quad_.drawElements();
  }
  quad_.unbind();
  checkGLError("[gl-reality-frame] quadToDraw unbind");
}

void GlRealityFrame::drawStage(
  GlTexture src, GlFramebufferObject *dest, const GlProgramObject *shader) {
  src.bind();

  gr8Shader_->bindAndSetParams(shader);

  dest->bind();
  checkGLError("[gl-reality-frame] bind framebuffer");

  // Load vertex data
  subQuad_.bind();
  glFrontFace(GL_CCW);
  checkGLError("[gl-reality-frame] bind quad");

  glViewport(0, 0, texWidth_, texHeight_);
  glUniform2f(shader->location("scale"), 1.0 / texWidth_, 1.0 / texHeight_);
  glUniform1i(shader->location("skipSubpixel"), skipSubpixel_ ? 1 : 0);
  subQuad_.drawElements();
  checkGLError("[gl-reality-frame] draw elements (no pyramid)");
  subQuad_.unbind();
  checkGLError("[gl-reality-frame] quadToDraw unbind");
}

void GlRealityFrame::readPixels() {
  if (!needsRead_) {
    return;
  }
  ScopeTimer t("pyr-read-pixels");

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
  dest3_.bind();
  dest3_.tex().bind();

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

  checkGLError("[gl-reality-frame] read pixels");
  hasPyramid_ = true;
  rois_ = drawnRois_;
  drawnRois_.clear();
}

bool GlRealityFrame::shouldFilterInput() const {
  return ALWAYS_FILTER_INPUT
    || (inputWidth_ >= filteredInputWidth_ && inputHeight_ >= filteredInputHeight_);
}

void GlRealityFrame::drawTexture(GlTexture cameraTexture) {
  ScopeTimer t("pyr-draw-tex");

  if (shouldFilterInput()) {
    // Create the filtered input image.
    drawInputStage(cameraTexture, &filteredInput_);
    drawInitialStage(filteredInput_.tex().tex(), cameraTexture, &dest1_, gr8Shader_->shader1());
  } else {
    // Take a smaller input image as-is.
    drawInitialStage(cameraTexture, cameraTexture, &dest1_, gr8Shader_->shader1());
  }

  for (int i = 0; i < 3; ++i) {
    drawPyramidBlurStage(dest1_.tex().tex(), &dest2_, i);
    drawPyramidDecimateStage(dest2_.tex().tex(), &dest1_, i);
  }

#if C8_USE_ALTERNATE_FEATURE_SCORE
  drawStage(dest1_.tex().tex(), &dest2_, gr8Shader_->shaderGradient());
  drawStage(dest2_.tex().tex(), &dest3_, gr8Shader_->shaderMomentX());
  drawStage(dest3_.tex().tex(), &dest1_, gr8Shader_->shaderMomentY());
#endif

  drawStage(dest1_.tex().tex(), &dest2_, gr8Shader_->shader2());
  drawStage(dest2_.tex().tex(), &dest3_, gr8Shader_->shader3());

  // Copy data to byte array
  if (usePixelBuffer_) {
#ifdef JAVASCRIPT
    dest3_.bind();
    glActiveTexture(GL_TEXTURE0);
    dest3_.tex().bind();

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

  if (GlRealityFrame::forceFinish_) {
    // TODO(nb): these don't seem to have the right effect.
    glFinish();
  }
  needsRead_ = true;
  drawnRois_ = nextRois_;
  nextRois_.clear();
}

void GlRealityFrame::addNextDrawRoi(const ImageRoi &roi) {
  auto idx = nextRois_.size();
  if (idx >= roiLayouts_.size()) {
    return;
  }
  nextRois_.push_back({roi, roiLayouts_[idx]});
}

void GlRealityFrame::draw(
  GLuint cameraTexture, GlRealityFrame::Options options) {
  GLint restoreActiveTexture = 0;
  GLint restoreTexture = 0;
  GLint restoreProgram = 0;
  GLint restoreElementBuffer = 0;
  GLint restoreVertexArray = 0;
  GLint restoreArrayBuffer = 0;
  GLint restoreFrameBuffer = 0;
  GLint restoreViewport[4];
  bool blendEnabled = false;

  if (isRestoreState(options)) {
    ScopeTimer t("set-restore-bindings-gl");
    blendEnabled = glIsEnabled(GL_BLEND);
    glGetIntegerv(GL_VIEWPORT, restoreViewport);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &restoreFrameBuffer);
    glGetIntegerv(GL_CURRENT_PROGRAM, &restoreProgram);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &restoreElementBuffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &restoreVertexArray);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &restoreArrayBuffer);
    glGetIntegerv(GL_ACTIVE_TEXTURE, &restoreActiveTexture);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &restoreTexture);
    checkGLError("[gl-reality-frame] cache gl state");
    glDisable(GL_BLEND);
  }

  auto ct = wrapRGBA8888Texture(cameraTexture, inputWidth_, inputHeight_);

  if (isDeferred(options)) {
    readPixels();
    drawTexture(ct);
  } else {
    drawTexture(ct);
    readPixels();
  }

  if (isRestoreState(options)) {
    ScopeTimer t("restore-bindings");
    glActiveTexture(restoreActiveTexture);
    glBindBuffer(GL_ARRAY_BUFFER, restoreArrayBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, restoreFrameBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, restoreElementBuffer);
    glBindVertexArray(restoreVertexArray);
    glUseProgram(restoreProgram);
    glBindTexture(GL_TEXTURE_2D, restoreTexture);
    glViewport(restoreViewport[0], restoreViewport[1], restoreViewport[2], restoreViewport[3]);
    if (blendEnabled) {
      glEnable(GL_BLEND);
    }
    checkGLError("[gl-reality-frame] restore bindings");
  }
}

void GlRealityFrame::addNextDrawHiResScans(c8_PixelPinholeCameraModel intrinsics, HPoint2 scanRay) {
  int startAt = nextRois_.size();
  for (int i = startAt; i < roiLayouts_.size(); ++i) {
    int pyrscale = (i - startAt) + 1;
    float scale = 1.0f - std::pow(1.44f, 2 + pyrscale);
    auto roi = glImageTargetWarp(
      intrinsics, intrinsics, HMatrixGen::translation(scanRay.x(), scanRay.y(), scale));
    // Note: We give this ROI a name so that it does match with an empty string in visualization.
    // Low chance of collision with a user-defined image target.
    addNextDrawRoi({ImageRoi::Source::HIRES_SCAN, -pyrscale, "INTERNAL_HIRES_SCAN_ROI", roi});
  }
}

bool GlRealityFrame::forceFinish_ = false;

}  // namespace c8
