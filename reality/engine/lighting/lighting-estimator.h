// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Pooja Bansal (pooja@8thwall.com)
//
// This class implements an algorithm to calculate the exposure value in an image.

#pragma once

#include "c8/pixels/pixels.h"
#include "c8/task-queue.h"
#include "c8/thread-pool.h"

namespace c8 {

class LightingEstimator {
public:
  // Default constructor.
  LightingEstimator() = default;

  // Default move constructors.
  LightingEstimator(LightingEstimator &&) = default;
  LightingEstimator &operator=(LightingEstimator &&) = default;

  // Disallow copying.
  LightingEstimator(const LightingEstimator &) = delete;
  LightingEstimator &operator=(const LightingEstimator &) = delete;

  static float estimateLighting(
    ConstYPlanePixels src,
    TaskQueue *taskQueue,
    ThreadPool *threadPool,
    int numChannels);

  static float scoreHistogram(const std::array<int32_t, 256> &histogram);

private:
};

}  // namespace c8
