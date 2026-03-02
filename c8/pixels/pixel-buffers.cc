// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"pixel-buffers.h"};
  visibility = {
    "//visibility:public",
  };
  deps = {
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixels",
    "//c8/pixels:pixel-transforms",
    "//c8:map",
    "//c8:set",
    "//c8:string",
    "//c8:vector",
  };
}
cc_end(0xe093b06b);

#include "c8/pixels/pixel-buffers.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/set.h"
#include "c8/stats/scope-timer.h"
#include "c8/vector.h"

namespace c8 {

// Copy the image pixels in rhs to the buffer in lhs, reallocating memory if needed.
void copyPixelBuffer(RGBA8888PlanePixelBuffer &lhs, ConstRGBA8888PlanePixels rhs) {
  auto pix = lhs.pixels();
  // Reallocate the destination if needed.
  if (pix.rows() != rhs.rows() || pix.cols() != rhs.cols()) {
    lhs = RGBA8888PlanePixelBuffer(rhs.rows(), rhs.cols());
    pix = lhs.pixels();
  }
  // Copy pixel data to the allocated buffer.
  ScopeTimer t("copy-pixel-buffer");
  copyPixels(rhs, &pix);
}

RGBA8888PlanePixelBuffer clonePixels(ConstRGBA8888PlanePixels pixels) {
  RGBA8888PlanePixelBuffer b;
  copyPixelBuffer(b, pixels);
  return b;
}

// Copy the image depth float pixels in rhs to the buffer in lhs, reallocating memory if needed.
void copyDepthFloatPixelBuffer(DepthFloatPixelBuffer &lhs, ConstDepthFloatPixels rhs) {
  auto pix = lhs.pixels();
  // Reallocate the destination if needed.
  if (pix.rows() != rhs.rows() || pix.cols() != rhs.cols()) {
    lhs = DepthFloatPixelBuffer(rhs.rows(), rhs.cols());
    pix = lhs.pixels();
  }
  // Copy pixel data to the allocated buffer.
  ScopeTimer t("copy-float-pixel-buffer");
  copyFloatPixels(rhs, &pix);
}

void copyPixelBufferSubImage(
  ConstRGBA8888PlanePixels srcPixels,
  int srcX,
  int srcY,
  int dstX,
  int dstY,
  int subImageWidth,
  int subImageHeight,
  RGBA8888PlanePixelBuffer &dstBuffer) {
  auto srcPix = srcPixels.pixels();
  auto srcRowBytes = srcPixels.rowBytes();
  auto srcPtr = srcPix + srcY * srcRowBytes + 4 * srcX;
  auto srcRowPixCount = srcPixels.cols() - srcX;
  auto srcRowCount = srcPixels.rows() - srcY;

  auto dstPix = dstBuffer.pixels().pixels();
  auto dstRowBytes = dstBuffer.pixels().rowBytes();
  auto dstPtr = dstPix + dstY * dstRowBytes + 4 * dstX;
  auto dstRowPixCount = dstBuffer.pixels().cols() - dstX;
  auto dstRowCount = dstBuffer.pixels().rows() - dstY;

  auto copyRowPixCount = std::min(subImageWidth, std::min(srcRowPixCount, dstRowPixCount));
  auto copyRowBytes = 4 * copyRowPixCount;
  auto copyRows = std::min(subImageHeight, std::min(srcRowCount, dstRowCount));

  for (int r = 0; r < copyRows; ++r) {
    std::memcpy(dstPtr, srcPtr, copyRowBytes);

    srcPtr += srcRowBytes;
    dstPtr += dstRowBytes;
  }
}

}  // namespace c8
