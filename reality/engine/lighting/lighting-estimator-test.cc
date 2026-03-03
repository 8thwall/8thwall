// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Pooja Bansal (pooja@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":lighting-estimator",
    "//c8:task-queue",
    "//c8:thread-pool",
    "//c8/pixels:pixels",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xf7f0e447);

#include <array>

#include "c8/pixels/pixels.h"
#include "gtest/gtest.h"
#include "reality/engine/lighting/lighting-estimator.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

const uint8_t srcData[] = {
  1,   2,   26, 50, 0,   0,    // first row
  63,  201, 56, 45, 124, 250,  // second row
  123, 45,  7,  23, 200, 87,   // third row
  100, 200, 60, 5,  6,   150,  // fourth row
  34,  57,  2,  5,  78,  87    // fifth row
};

class LightingEstimatorTest : public ::testing::Test {};

TEST_F(LightingEstimatorTest, TestEstimateLighting) {
  ConstYPlanePixels src(5, 5, 6, srcData);
  ScopeTimer rt("test");

  TaskQueue taskQueue;
  ThreadPool threadPool(1);
  float val =
    LightingEstimator::estimateLighting(src, &taskQueue, &threadPool, 1);
  EXPECT_TRUE(true);
  EXPECT_FLOAT_EQ(-0.36f, val);
}

}  // namespace c8
