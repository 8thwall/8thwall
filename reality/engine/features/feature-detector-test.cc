// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":feature-detector",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixel-transforms",
    "//c8/pixels:pixels",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x96093ebe);

#include "reality/engine/features/feature-detector.h"

#include "gtest/gtest.h"

#include "c8/vector.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/pixels/pixels.h"
#include "c8/stats/scope-timer.h"

#include <iostream>

namespace c8 {

class FeatureDetectorTest : public ::testing::Test {};

TEST_F(FeatureDetectorTest, TestFeatures) {
  ScopeTimer rt("test");
  // Create a frames that is large enough for point detection to work. This will look something like
  // 11111
  // 11011
  // 11111
  // with a single salient corner pont.
  YPlanePixelBuffer frame1Buffer(80, 80);
  auto frame1 = frame1Buffer.pixels();
  fill(255, &frame1);
  auto frame1Crop = crop(frame1, 1, 1, 39, 39);
  fill(0, &frame1Crop);

  FeatureDetector detector;

  FrameWithPoints keyPoints({80, 80, 40.0f, 40.0f, 600.0f, 600.0f});

  detector.detectFeatures(frame1, &keyPoints);

  EXPECT_GE(keyPoints.points().size(), 1);
  EXPECT_EQ(39.0f, std::round(keyPoints.pixels()[0].x()));
  EXPECT_EQ(39.0f, std::round(keyPoints.pixels()[0].y()));
}

}  // namespace c8
