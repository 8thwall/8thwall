// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/geometry/face-types.h"
#include "c8/geometry/pixel-pinhole-camera-model.h"
#include "c8/hpoint.h"
#include "c8/pixels/opengl/gl-framebuffer.h"
#include "c8/pixels/opengl/gl-quad.h"
#include "c8/pixels/opengl/gl-texture.h"
#include "c8/pixels/opengl/gl.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"
#include "c8/vector.h"
#include "reality/engine/faces/face-roi-shader.h"
#include "reality/engine/render/renderers.h"

namespace c8 {

class FaceRoiRenderer {
public:
  // Get the most recent rendered image.
  RGBA8888PlanePixels pixels() const { return outpix_.pixels(); }

  FaceRenderResult result() const;

  // Check whether any read has yet occured. Once this is true, it's always true.
  bool hasRender() const { return hasRender_; }

  // The texture that the render was drawn to.
  GlTexture dest() const { return dest_.tex().tex(); }

  // Initialize the renderer to process an input with size specified by srcWidth and srcHeight.
  void initialize(FaceRoiShader *shader, int srcWidth, int srcHeight);

  // Enables the use of the WebGL 2 features such as pixel pack buffer.
  void enableWebGl2PixelBuffer();

  // Force a readback of the image render. This is useful to finalize draw calls with DRAW_DEFERRED
  // when there is no imminent next frame to process.
  void readPixels();

  // Configure the renderer to add ROIs for the detected faces on its next draw pass.
  void setDetectedFaces(
    const Vector<DetectedPoints> &globalFaces, const Vector<DetectedPoints> &localFaces);
  // TODO(nathan): remove this version.
  void setDetectedFaces(const Vector<DetectedPoints> &faces);

  // Begin processing the camera texture. If READ_IMMEDIATELY is set, this blocks on materializing
  // the result from the GPU. If DEFER_READ is set, this does not block on the GPU unless a
  // previously requested call to draw is still pending. In that case, it blocks on the result of
  // the previous call.
  void draw(GLuint cameraTexture, GpuReadPixelsOptions options);

  void flipY(bool doFlip) { flipY_ = doFlip ? 1 : 0; }

  // Default constructor.
  // @param roiWidth render the output into a letter box of this dimension
  explicit FaceRoiRenderer(int roiWidth = 128) : roiWidth_(std::min(roiWidth, 192)) {}

  // Default move constructors.
  FaceRoiRenderer(FaceRoiRenderer &&o) = default;
  FaceRoiRenderer &operator=(FaceRoiRenderer &&o) = default;

  // Disallow copying.
  FaceRoiRenderer(const FaceRoiRenderer &) = delete;
  FaceRoiRenderer &operator=(const FaceRoiRenderer &) = delete;

private:
  bool hasRender_ = false;
  bool needsRead_ = false;
  GlVertexArray quad_;
  bool faceFboInitialized_ = false;
  GlFramebufferObject dest_;  // face ROI rendering destination
  RGBA8888PlanePixelBuffer outpix_;
  int drawnTex_ = 0;
  int tex_ = 0;

  // ROIs currently in the pipeline. Next will be applied on the next call to draw. When they are
  // drawn, they move to "drawn" and next is cleared. After they are read back from gpu and they are
  // available in result(), they are moved from "drawn" to faceRois_ and "drawn" is cleared.
  Vector<ImageRoi> nextFaceRois_;
  Vector<ImageRoi> drawnFaceRois_;
  Vector<ImageRoi> faceRois_;

  int inputWidth_ = 0;
  int inputHeight_ = 0;
  // set ROI letterbox square dimension
  int roiWidth_;
  int outputWidth_ = 0;
  int outputHeight_ = 0;
  int texWidth_ = 0;
  int texHeight_ = 0;
  int flipY_ = 0;

  bool usePixelBuffer_ = false;
#ifdef JAVASCRIPT
  GLuint pixelBuffer_ = 0;
#endif
  FaceRoiShader *shaders_ = nullptr;

  void drawTexture(GlTexture cameraTexture);
  void drawStage(GlTexture src, GlFramebufferObject *dest, const GlProgramObject *shader);
};

}  // namespace c8
