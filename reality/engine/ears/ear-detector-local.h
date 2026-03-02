// Copyright (c) 2023 Niantic Labs
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#pragma once

#include "c8/geometry/face-types.h"
#include "c8/vector.h"
#include "reality/engine/deepnets/tflite-interpreter.h"

namespace c8 {

constexpr size_t EAR_LOCAL_CACHE_SIZE = 3000000;

// EarDetectorLocal provides an abstraction layer above the deep net model for analyzing ears
// in detail from a high res region of interest.
class EarDetectorLocal {
public:
  // Construct from a tflite model.
  EarDetectorLocal(const uint8_t *modelData, int modelSize)
      : interpreter_(modelData, modelSize, EAR_LOCAL_CACHE_SIZE, NUM_THREADS) {}
  EarDetectorLocal(const Vector<uint8_t> &modelData)
      : interpreter_(modelData, EAR_LOCAL_CACHE_SIZE, NUM_THREADS) {}

  // Default move constructors.
  EarDetectorLocal(EarDetectorLocal &&) = default;
  EarDetectorLocal &operator=(EarDetectorLocal &&) = default;

  // Disallow copying.
  EarDetectorLocal(const EarDetectorLocal &) = delete;
  EarDetectorLocal &operator=(const EarDetectorLocal &) = delete;

  // Detect ears in a zoomed-in, stacked image with left ear image on the top and
  // right ear image on the bottom.
  // The srcLeft should be w112xh160.
  // The srcRight should be w112xh160 as well.
  Vector<DetectedPoints> analyzeEars(
    const RenderedSubImage &srcLeft,
    const RenderedSubImage &srcRight,
    c8_PixelPinholeCameraModel k);

private:
  static constexpr int NUM_THREADS = 2;
  TFLiteInterpreter interpreter_;
};

}  // namespace c8
