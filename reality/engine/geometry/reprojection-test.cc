// Copyright (c) 2024 Niantic, Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":reprojection",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xf2db4047);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "reality/engine/geometry/reprojection.h"

using testing::Eq;
using testing::FloatNear;
using testing::Pointwise;

namespace c8 {

class ReprojectionTest : public ::testing::Test {};

TEST_F(ReprojectionTest, TestReprojectionInliers) {
  HMatrix worldToCam = HMatrixGen::translation(0.0f, 0.0f, -1.0f);

  Vector<HPoint3> points{
    {0.54342354f, 0.65875207f, 0.89720247f},
    {0.51389245f, 0.97631843f, 0.48467117f},
    {0.25009358f, 0.71512308f, 0.3240954f},
    {0.99413048f, 0.50010172f, 0.12262631f},
    {0.12345678f, 0.87654321f, -2.0f}};

  Vector<HPoint3> ptsInCam;
  worldToCam.inv().times(points, &ptsInCam);

  Vector<HPoint2> rays;
  flatten<2>(ptsInCam, &rays);

  // Add some noise to the rays 0 and 2, so they become outliers.
  rays[0] = HPoint2{rays[0].x() + 1e-2f, rays[0].y() - 1e-2f};
  rays[2] = HPoint2{rays[2].x() - 1e-2f, rays[2].y() + 1e-2f};

  Vector<size_t> inliers;
  reprojectionInliers(points, worldToCam.inv(), rays, 1e-3f * 1e-3f, &inliers);

  // Indices 0 and 2 have high residuals, so they should be excluded.
  // Index 4 is behind the camera, so it should be excluded.
  // Hence, only points 1 and 3 should be inliers.
  EXPECT_THAT(inliers, Eq(Vector<size_t>{1, 3}));
}

}  // namespace c8
