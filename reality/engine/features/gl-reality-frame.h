// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/geometry/pixel-pinhole-camera-model.h"
#include "c8/hpoint.h"
#include "c8/pixels/gr8-pyramid.h"
#include "c8/pixels/opengl/gl-framebuffer.h"
#include "c8/pixels/opengl/gl-quad.h"
#include "c8/pixels/opengl/gl-texture.h"
#include "c8/pixels/opengl/gl.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"
#include "c8/vector.h"
#include "reality/engine/features/gr8-feature-shader.h"

namespace c8 {

class GlRealityFrame {
public:
  enum class Options {
    UNSPECIFIED_OPTIONS,
    // Delay readback of the pyramid texture until the next call to draw or readPixels.
    DEFER_READ,
    // Set a new texture and read it back immediately.
    READ_IMMEDIATELY,
    // Like DEFER_READ but also query and restore opengl state (slow but safe).
    DEFER_READ_RESTORE_STATE,
    // Like READ_IMMEDIATELY but also query and restore opengl state (slow but safe).
    READ_IMMEDIATELY_RESTORE_STATE
  };

  static constexpr bool isDeferred(Options o) {
    return o == Options::DEFER_READ || o == Options::DEFER_READ_RESTORE_STATE;
  }

  static constexpr bool isRestoreState(Options o) {
    return o == Options::DEFER_READ_RESTORE_STATE || o == Options::READ_IMMEDIATELY_RESTORE_STATE;
  }

  // Get the most recent pyramid image.
  Gr8Pyramid pyramid() const { return Gr8Pyramid{outpix_.pixels(), levels_, rois_}; }

  // Check whether any read has yet occured. Once this is true, it's always true.
  bool hasPyramid() const { return hasPyramid_; }

  // The texture that the pyramid was drawn to.
  GlTexture dest() const { return dest3_.tex().tex(); }

  // Initialize the pyramid to process an input with size specified by srcWidth and srcHeight. The
  // output pyramid image will be less than or equal to outputWidth, outputHeight.
  void initialize(Gr8FeatureShader *shader, int srcWidth, int srcHeight, int rotation);

  // Enables the use of the WebGL 2 features such as pixel pack buffer.
  void enableWebGl2PixelBuffer();

  // Force a readback of the image pyramid. This is useful to finalize draw calls with DRAW_DEFERRED
  // when there is no imminent next frame to process.
  void readPixels();

  // Add an ROI that will be set on the next call to draw. After the buffer is read back, this will
  // be cleared and must be set again if still desired.
  void addNextDrawRoi(const ImageRoi &roi);

  // Fills the remaining ROI slots on the next draw with scans along a ray at increasing resolution.
  void addNextDrawHiResScans(c8_PixelPinholeCameraModel intrinsics, HPoint2 scanRay);

  // Begin processing the camera texture. If READ_IMMEDIATELY is set, this blocks on materializing
  // the result from the GPU. If DEFER_READ is set, this does not block on the GPU unless a
  // previously requested call to draw is still pending. In that case, it blocks on the result of
  // the previous call.
  void draw(GLuint cameraTexture, GlRealityFrame::Options options);

  // TODO(nb): remove
  void setFeatureGain(float gain) { gr8Shader_->setFeatureGain(gain); }
  void setFeatureParams(
    float robustGr8, float scoreThresh, bool thresholdClamp, float nmsTolerance) {
    gr8Shader_->setFeatureParams(robustGr8, scoreThresh, thresholdClamp, nmsTolerance);
  }

  void flipY(bool doFlip) { flipY_ = doFlip ? 1 : 0; }

  void skipSubpixel(bool value) { skipSubpixel_ = value; }

  // Default constructor.
  GlRealityFrame() = default;

  // Default move constructors.
  GlRealityFrame(GlRealityFrame &&o) = default;
  GlRealityFrame &operator=(GlRealityFrame &&o) = default;

  // Disallow copying.
  GlRealityFrame(const GlRealityFrame &) = delete;
  GlRealityFrame &operator=(const GlRealityFrame &) = delete;

  // Force calls to glFlush and glFinish after all rendering commands are issued.
  static void setForceFinish(bool force) { forceFinish_ = force; }

  // Query the size of image buffer that will hold the pyramid (square).
  static int32_t pyrSize();

private:
  bool shouldFilterInput() const;
  bool hasPyramid_ = false;
  bool needsRead_ = false;
  GlVertexArray quad_;
  GlVertexArray subQuad_;
  GlFramebufferObject filteredInput_;
  GlFramebufferObject dest1_;
  GlFramebufferObject dest2_;
  GlFramebufferObject dest3_;
  RGBA8888PlanePixelBuffer outpix_;
  Vector<LevelLayout> levels_;
  Vector<LevelLayout> roiLayouts_;
  Vector<ROI> nextRois_;
  Vector<ROI> drawnRois_;
  Vector<ROI> rois_;
  int inputWidth_ = 0;
  int inputHeight_ = 0;
  int filteredInputWidth_ = 0;
  int filteredInputHeight_ = 0;
  int outputWidth_ = 0;
  int outputHeight_ = 0;
  int texWidth_ = 0;
  int texHeight_ = 0;
  int rotation_ = 0;
  int flipY_ = 0;
  bool skipSubpixel_ = false;

  bool usePixelBuffer_ = false;
#ifdef JAVASCRIPT
  GLuint pixelBuffer_ = 0;
#endif
  Gr8FeatureShader *gr8Shader_ = nullptr;

  static bool forceFinish_;

  void drawTexture(GlTexture cameraTexture);
  void drawInputStage(GlTexture src, GlFramebufferObject *dest);
  void drawInitialStage(
    GlTexture src, GlTexture roiSrc, GlFramebufferObject *dest, const GlProgramObject *shader);
  void drawStage(GlTexture src, GlFramebufferObject *dest, const GlProgramObject *shader);
  void drawPyramidBlurStage(GlTexture src, GlFramebufferObject *dest, int level);
  void drawPyramidDecimateStage(GlTexture src, GlFramebufferObject *dest, int level);
  void initializeLevels(int srcWidth, int srcHeight, int outputWidth, int outputHeight);
};

}  // namespace c8
