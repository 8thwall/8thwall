// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Text fixture for benchmark images.

#pragma once

#include "c8/pixels/pixel-buffer.h"
#include "c8/vector.h"

namespace c8 {

enum class BenchmarkName {
  SIMPLE10_240x320,
  SIMPLE10_480x640,
  LOBBYTOUR10_480x640,
};

class BenchmarkDataset {
public:
  BenchmarkDataset() = delete;

  // Returns the number of images in a benchmark dataset.
  static int size(BenchmarkName name);

  // Reads a benchmark image from disk and returns a Y-plane PixelBuffer.
  static YPlanePixelBuffer loadY(BenchmarkName name, int index);

  // Reads a benchmark image from disk and returns an RGB PixelBuffer.
  static RGB888PlanePixelBuffer loadRGB(BenchmarkName name, int index);

  // Reads a benchmark image from disk and returns an BGR PixelBuffer.
  static BGR888PlanePixelBuffer loadBGR(BenchmarkName name, int index);

  // Reads a benchmark image from disk and returns an RGBA PixelBuffer.
  static RGBA8888PlanePixelBuffer loadRGBA(BenchmarkName name, int index);
};

}  // namespace c8
