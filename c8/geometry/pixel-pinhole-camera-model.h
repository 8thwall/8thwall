// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <cstdint>
#include <sstream>

#include "c8/string.h"
#include "c8/string/format.h"

// This is parallel to the PixelPinholeCameraModel capnp struct; keeping it out of the c8 namespace
// avoids namespace conflicts.
class c8_PixelPinholeCameraModel {
public:
  // Aggregate type, don't add user constructors.

  int32_t pixelsWidth = 0;
  int32_t pixelsHeight = 0;
  float centerPointX = 0.0f;
  float centerPointY = 0.0f;
  // Set default focal length to 1 so that this turns into an identity intrinsic matrix.
  float focalLengthHorizontal = 1.0f;
  float focalLengthVertical = -1.0f;

  c8::String toString() const noexcept {
    return c8::format(
      "(w: %d, h: %d, cx: %04.2f, cy: %04.2f, fh: %04.2f, fv: %04.2f)",
      pixelsWidth,
      pixelsHeight,
      centerPointX,
      centerPointY,
      focalLengthHorizontal,
      focalLengthVertical);
  }

private:
};
