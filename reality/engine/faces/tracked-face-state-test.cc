// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":tracked-face-state",
    "//c8/geometry:facemesh-data",
    "//c8/geometry:intrinsics",
    "@com_google_googletest//:gtest_main",
  };
  linkstatic = 1;
}
cc_end(0x71dd3b1e);

#include <cmath>
#include <cstdio>

#include "c8/geometry/facemesh-data.h"
#include "c8/geometry/intrinsics.h"
#include "c8/stats/scope-timer.h"
#include "gtest/gtest.h"
#include "reality/engine/faces/tracked-face-state.h"

namespace c8 {

class FaceTrackerTest : public ::testing::Test {};

TEST_F(FaceTrackerTest, TestLocate) {
  ScopeTimer rt("test");

  ImageRoi roi{
    ImageRoi::Source::FACE,
    2,
    "",
    {
      // roi
      {3.631096, 0.000000, 0.000000, -1.445287},
      {0.000000, 4.841462, 0.000000, 2.579028},
      {0.000000, 0.000000, 1.000000, 0.000000},
      {0.000000, 0.000000, 0.000000, 1.000000},
    }};

  DetectedPoints p{
    1.0f,                            // confidence
    0,                               // detectedClass
    {0.0f, 192.0f, 192.0f, 192.0f},  // viewport
    roi,                             // roi
    {
      // boundingBox
      {.25f, 0.5f},  // upper left
      {.75f, 0.5f},  // upper right
      {.25f, 1.0f},  // lower left
      {1.0f, 1.0f},  // lower right
    },
    FACEMESH_SAMPLE_OUTPUT,                                       // points
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6)  // intrinsics
  };

  TrackedFaceState s;
  auto tracked = s.locateFace(p);

  // The first time locateFace is called, the status should be FOUND.
  EXPECT_EQ(478, tracked.vertices.size());
  EXPECT_EQ(478, tracked.uvsInCameraSpace.size());
  EXPECT_EQ(2, tracked.id);
  EXPECT_EQ(Face3d::TrackingStatus::FOUND, tracked.status);
  EXPECT_EQ(Face3d::TrackingStatus::FOUND, s.status());

  // If mark is called and then locateFace is called again, the status should be UPDATED.
  s.markFrame();
  tracked = s.locateFace(p);

  EXPECT_EQ(478, tracked.vertices.size());
  EXPECT_EQ(478, tracked.uvsInCameraSpace.size());
  EXPECT_EQ(2, tracked.id);
  EXPECT_EQ(Face3d::TrackingStatus::UPDATED, tracked.status);
  EXPECT_EQ(Face3d::TrackingStatus::UPDATED, s.status());

  // If mark is called but then locateFace is not called, the status should be LOST.
  s.markFrame();
  EXPECT_EQ(Face3d::TrackingStatus::LOST, s.status());
}

}  // namespace c8
