// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Lynn Dang (lynn@8thwall.com)
//
// Operations that produce images resizing from images. Performs letterboxing if the image aspect
// ratio doesn't match.

#pragma once

#include "c8/pixels/opengl/gl-texture.h"
#include "c8/pixels/pixels.h"
#include "c8/pixels/render/renderer.h"

namespace c8 {

class GpuPixelsResizer {
public:
  // Default constructor.
  GpuPixelsResizer();

  // Default move constructors.
  GpuPixelsResizer(GpuPixelsResizer &&) = default;
  GpuPixelsResizer &operator=(GpuPixelsResizer &&) = default;

  // Disallow copying.
  GpuPixelsResizer(const GpuPixelsResizer &) = delete;
  GpuPixelsResizer &operator=(const GpuPixelsResizer &) = delete;

  // Resizes the image using gpu.
  RGBA8888PlanePixelBuffer resizeOnGpu(ConstRGBA8888PlanePixels pix, int rows, int cols);

  // Deferred image resizer.
  // Set that we will draw the next image.
  void drawNextImage() { shouldDraw_ = true; }
  // If we should draw, begin constructing the request by rendering the texture to an image buffer.
  void draw(GlTexture tex, int desiredWidth, int desiredHeight);

  // If we started to draw an image, read and save the image locally.
  void read();

  // Returns true if we there is an image to claim that we previously read().
  bool hasImage() const { return hasImage_; }
  // Claim the image we saved locally in read().
  RGBA8888PlanePixels claimImage();

private:
  std::unique_ptr<Renderer> renderer_;

  std::unique_ptr<Scene> scene_;  // For image resizing.

  // Holding scene results so we don't allocate every frame
  std::unique_ptr<RGBA8888PlanePixelBuffer> imageBuffer_;
  std::unique_ptr<RGBA8888PlanePixelBuffer> scratch_;

  bool shouldDraw_ = false;
  bool shouldRead_ = false;
  bool hasImage_ = false;

  int curInputHeight_ = 0;
  int curInputWidth_ = 0;
};

}  // namespace c8
