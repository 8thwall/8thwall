// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"

namespace c8 {

void copyPixelBuffer(RGBA8888PlanePixelBuffer &lhs, ConstRGBA8888PlanePixels rhs);
RGBA8888PlanePixelBuffer clonePixels(ConstRGBA8888PlanePixels pixels);

void copyDepthFloatPixelBuffer(DepthFloatPixelBuffer &lhs, ConstDepthFloatPixels rhs);

// copy pixels from srcPixels with offset (srcX, srcY) to dstBuffer with offset (dstX, dstY)
// for width and height
void copyPixelBufferSubImage(
  ConstRGBA8888PlanePixels srcPixels,
  int srcX,
  int srcY,
  int dstX,
  int dstY,
  int subImageWidth,
  int subImageHeight,
  RGBA8888PlanePixelBuffer &dstBuffer);

}  // namespace c8
