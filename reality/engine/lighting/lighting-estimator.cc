// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Pooja Bansal (pooja@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//reality/engine/executor:__subpackages__",
    "//reality/engine/lighting:__subpackages__",
    "//reality/quality/visualization:__subpackages__",
  };
  hdrs = {
    "lighting-estimator.h",
  };
  deps = {
    "//c8:task-queue",
    "//c8:thread-pool",
    "//c8/pixels:pixel-transforms",
    "//c8/pixels:pixels",
    "//c8/stats:scope-timer",
  };
}
cc_end(0xef1bb473);

#include "reality/engine/lighting/lighting-estimator.h"

#include <array>
#include "c8/pixels/pixel-transforms.h"
#include "c8/pixels/pixels.h"
#include "c8/task-queue.h"
#include "c8/thread-pool.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

// Estimate the exposure of an image by counting the near black and near white pixels.
float LightingEstimator::estimateLighting(
  ConstYPlanePixels src,
  TaskQueue *taskQueue,
  ThreadPool *threadPool,
  int numChannels) {
#if defined(__ARM_NEON__)
  if (numChannels > 1) {
    return estimateExposureScore(src, taskQueue, threadPool, numChannels);
  } else {
    return estimateExposureScoreNEON(src);
  }
#else
  return estimateExposureScore(src, taskQueue, threadPool, numChannels);
#endif  // defined(__ARM_NEON__)
}
}  // namespace c8
