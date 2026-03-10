// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// A class that owns the memory for pixels.

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "c8/pixels/pixels.h"

namespace c8 {

template <class PixelsType, int bytesPerPix>
class PixelBuffer {
public:
  // Accessor for pixels.
  PixelsType pixels() const noexcept { return PixelsType(rows_, cols_, rowBytes_, pixels_); }

  // Explicit constructors.
  PixelBuffer() = default;  // Create empty, and then move.

  PixelBuffer(int rows, int cols, int rowBytesHint) noexcept : rows_(rows), cols_(cols) {
    // TODO(mc): Figure out what needs to change to allow the following to work:
    /*
    // Let's ensure each row is aligned to max_align_t as well.
    constexpr auto alignment = alignof(std::max_align_t);
    rowBytes_ = ((rowBytesHint + alignment - 1) / alignment) * alignment;
    */
    rowBytes_ = rowBytesHint;

    size_t allocSize = rows_ * rowBytes_;

    if (allocSize > 0) {
      // C++ standard guarantees unsigned char allocation new will be aligned to max_align_t.
      pixels_ = static_cast<uint8_t *>(new unsigned char[allocSize]);
    }
  }
  PixelBuffer(int rows, int cols) noexcept : PixelBuffer(rows, cols, bytesPerPix * cols) {}

  ~PixelBuffer() {
    if (pixels_ != nullptr) {
      delete[] pixels_;
    }
  }

  // Move constructors.
  PixelBuffer(PixelBuffer &&b) {
    std::memcpy(this, &b, sizeof(PixelBuffer));
    std::memset(&b, 0, sizeof(PixelBuffer));
  };

  PixelBuffer &operator=(PixelBuffer &&b) {
    if (pixels_ != nullptr) {
      delete[] pixels_;
    }
    std::memcpy(this, &b, sizeof(PixelBuffer));
    std::memset(&b, 0, sizeof(PixelBuffer));
    return *this;
  };

  // Disallow copying.
  PixelBuffer(const PixelBuffer &) = delete;
  PixelBuffer &operator=(const PixelBuffer &) = delete;

private:
  int rows_ = 0;
  int cols_ = 0;
  int rowBytes_ = 0;
  uint8_t *pixels_ = nullptr;
};

template <class PixelsType, int elementsPerPix>
class PixelBuffer32 {
public:
  // Accessor for pixels.
  PixelsType pixels() const noexcept { return PixelsType(rows_, cols_, rowElements_, pixels_); }

  // Explicit constructors.
  PixelBuffer32() = default;  // Create empty, and then move.

  PixelBuffer32(int rows, int cols, int rowElements) noexcept : rows_(rows), cols_(cols) {
    rowElements_ = rowElements;

    size_t allocSize = rows_ * rowElements_;

    if (allocSize > 0) {
      // C++ standard guarantees unsigned char allocation new will be aligned to max_align_t.
      pixels_ = static_cast<uint32_t *>(new uint32_t[allocSize]);
    }
  }
  PixelBuffer32(int rows, int cols) noexcept : PixelBuffer32(rows, cols, elementsPerPix * cols) {}

  ~PixelBuffer32() {
    if (pixels_ != nullptr) {
      delete[] pixels_;
    }
  }

  // Move constructors.
  PixelBuffer32(PixelBuffer32 &&b) {
    std::memcpy(this, &b, sizeof(PixelBuffer32));
    std::memset(&b, 0, sizeof(PixelBuffer32));
  };

  PixelBuffer32 &operator=(PixelBuffer32 &&b) {
    if (pixels_ != nullptr) {
      delete[] pixels_;
    }
    std::memcpy(this, &b, sizeof(PixelBuffer32));
    std::memset(&b, 0, sizeof(PixelBuffer32));
    return *this;
  };

  // Disallow copying.
  PixelBuffer32(const PixelBuffer32 &) = delete;
  PixelBuffer32 &operator=(const PixelBuffer32 &) = delete;

private:
  int rows_ = 0;
  int cols_ = 0;
  int rowElements_ = 0;
  uint32_t *pixels_ = nullptr;
};

template <class PixelsType, int elementsPerPix>
class FloatPixelBuffer {
public:
  // Accessor for pixels.
  PixelsType pixels() const noexcept { return PixelsType(rows_, cols_, rowElements_, pixels_); }

  // Explicit constructors.
  FloatPixelBuffer() = default;  // Create empty, and then move.

  FloatPixelBuffer(int rows, int cols, int rowElementsHint) noexcept : rows_(rows), cols_(cols) {
    // TODO(mc): Figure out what needs to change to allow the following to work:
    /*
    // Let's ensure each row is aligned to max_align_t as well.
    constexpr auto alignment = alignof(std::max_align_t);
    rowElements_ = ((rowElementsHint + alignment - 1) / alignment) * alignment;
    */
    rowElements_ = rowElementsHint;

    size_t allocSize = rows_ * rowElements_;
    if (allocSize > 0) {
      // C++ standard guarantees unsigned char allocation new will be aligned to max_align_t.
      pixels_ = new float[allocSize];
    }
  }
  FloatPixelBuffer(int rows, int cols) noexcept
      : FloatPixelBuffer(rows, cols, elementsPerPix * cols) {}

  ~FloatPixelBuffer() {
    if (pixels_ != nullptr) {
      delete[] pixels_;
    }
  }

  // Move constructors.
  FloatPixelBuffer(FloatPixelBuffer &&b) {
    std::memcpy(this, &b, sizeof(FloatPixelBuffer));
    std::memset(&b, 0, sizeof(FloatPixelBuffer));
  };

  FloatPixelBuffer &operator=(FloatPixelBuffer &&b) {
    if (pixels_ != nullptr) {
      delete[] pixels_;
    }
    std::memcpy(this, &b, sizeof(FloatPixelBuffer));
    std::memset(&b, 0, sizeof(FloatPixelBuffer));
    return *this;
  };

  // Disallow copying.
  FloatPixelBuffer(const FloatPixelBuffer &) = delete;
  FloatPixelBuffer &operator=(const FloatPixelBuffer &) = delete;

private:
  int rows_ = 0;
  int cols_ = 0;
  int rowElements_ = 0;
  float *pixels_ = nullptr;
};

typedef PixelBuffer<YPlanePixels, 1> YPlanePixelBuffer;
typedef PixelBuffer<UPlanePixels, 1> UPlanePixelBuffer;
typedef PixelBuffer<VPlanePixels, 1> VPlanePixelBuffer;
typedef PixelBuffer<UVPlanePixels, 2> UVPlanePixelBuffer;
typedef PixelBuffer<BGR888PlanePixels, 3> BGR888PlanePixelBuffer;
typedef PixelBuffer<RGB888PlanePixels, 3> RGB888PlanePixelBuffer;
typedef PixelBuffer<RGBA8888PlanePixels, 4> RGBA8888PlanePixelBuffer;
typedef PixelBuffer<BGRA8888PlanePixels, 4> BGRA8888PlanePixelBuffer;
typedef PixelBuffer<YUVA8888PlanePixels, 4> YUVA8888PlanePixelBuffer;

typedef PixelBuffer32<R32PlanePixels, 1> R32PlanePixelBuffer;
typedef PixelBuffer32<RGBA32PlanePixels, 4> RGBA32PlanePixelBuffer;

typedef FloatPixelBuffer<DepthFloatPixels, 1> DepthFloatPixelBuffer;
typedef FloatPixelBuffer<OneChannelFloatPixels, 1> OneChannelFloatPixelBuffer;
}  // namespace c8
