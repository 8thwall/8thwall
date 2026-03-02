// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// A GlPixelBuffer is a pixel buffer that is managed by OpenGL. It can be mapped into client memory
// and then accesed like a normal CPU PixelBuffer. Under the hood it is a pixel buffer object,
// combined with an 8th Wall Pixels header. This is currently designed only for reading from the GPU
// into CPU.

#pragma once

#include "c8/pixels/opengl/gl-buffer-object.h"
#include "c8/pixels/opengl/gl.h"
#include "c8/pixels/opengl/gl-version.h"
#include "c8/pixels/pixels.h"

#if C8_OPENGL_VERSION_3

namespace c8 {

template <class PixelsType, int bytesPerPix>
class GlPixelBuffer {
public:
  // Accessor for pixels. Only use these pixels while the GlPixelBuffer is mapped into client
  // memory.
  PixelsType pixels() const noexcept { return PixelsType(rows_, cols_, rowBytes_, pixels_); }

  GlPixelBuffer() noexcept
      : rows_(0),
        cols_(0),
        rowBytes_(0),
        pbo_(nullptr),
        pixels_(nullptr) {}  // Create empty and then move.

  GlPixelBuffer(int rows, int cols, int rowBytes) noexcept
      : rows_(rows), cols_(cols), rowBytes_(rowBytes), pbo_(), pixels_(nullptr) {
    pbo_.set(GL_PIXEL_PACK_BUFFER, rowBytes_ * rows_, nullptr, GL_DYNAMIC_READ);
  }

  ~GlPixelBuffer() = default;

  void bind() const noexcept { return pbo_.bind(); }
  void unbind() const noexcept { return pbo_.unbind(); }

  // Call this when you would like access to the pixels. The buffer should be bound before you do.
  void map() const noexcept {
    pixels_ = reinterpret_cast<uint8_t *>(pbo_.mapBuffer(GL_MAP_READ_BIT));
  }

  // Call this as soon as you are finished with the pixels access, so the GPU can use it again.
  void unmap() const noexcept {
    pixels_ = nullptr;
    pbo_.unmapBuffer();
  }

  // Move constructors.
  GlPixelBuffer(GlPixelBuffer &&) = default;
  GlPixelBuffer &operator=(GlPixelBuffer &&) = default;

  // Disallow copying.
  GlPixelBuffer(const GlPixelBuffer &) = delete;
  GlPixelBuffer &operator=(const GlPixelBuffer &) = delete;

private:
  int rows_;
  int cols_;
  int rowBytes_;
  GlBufferObject pbo_;
  mutable uint8_t *pixels_;
};

using GlYPlanePixelBuffer = GlPixelBuffer<YPlanePixels, 1>;
using GlUVPlanePixelBuffer = GlPixelBuffer<UVPlanePixels, 2>;
using GlBGR888PlanePixelBuffer = GlPixelBuffer<BGR888PlanePixels, 3>;
using GlRGB888PlanePixelBuffer = GlPixelBuffer<RGB888PlanePixels, 3>;
using GlRGBA8888PlanePixelBuffer = GlPixelBuffer<RGBA8888PlanePixels, 4>;
using GlYUVA8888PlanePixelBuffer = GlPixelBuffer<YUVA8888PlanePixels, 4>;

}  // namespace c8

#endif  // C8_OPENGL_VERSION_3
