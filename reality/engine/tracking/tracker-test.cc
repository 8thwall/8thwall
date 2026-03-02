// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":tracker",
    "//c8/geometry:egomotion",
    "//c8/io:capnp-messages",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x2b4e23e1);

#include <gtest/gtest.h>

#include "c8/geometry/egomotion.h"
#include "c8/io/capnp-messages.h"
#include "c8/stats/scope-timer.h"
#include "reality/engine/tracking/tracker.h"

namespace c8 {

// A quaternion sampled from the unit sphere.
constexpr Quaternion Q0(
  0.6413764532829037f, 0.5723576570056439f, -0.4252083875837609f, 0.283268044031922f);

static constexpr c8_PixelPinholeCameraModel INTRINSIC{480, 640, 240.0f, 320.0f, 625.49f, 625.49f};

class TrackerTest : public ::testing::Test {};

TEST_F(TrackerTest, TestFirstFrameFromGyros) {
  ScopeTimer rt("test");
  MutableRootMessage<XrTrackingState> state;
  Tracker tracker;
  DetectionImagePtrMap targets;
  RandomNumbers r;

  TrackingSensorFrame frame;
  frame.intrinsic = INTRINSIC;
  frame.devicePose = Q0;
  frame.srcY = YPlanePixels();  // nullptr

  // Use the tracker to update all state needed to estimate a new camera position.
  auto cam = tracker.track(frame, targets, &r, state.builder());

  auto lastFrame = tracker.getLastKeypoints();
  // Make sure the last frame was allocated.
  EXPECT_NE(lastFrame, nullptr);
  // We don't expect to have any keypoints.
  EXPECT_TRUE(lastFrame->points().empty());
}

TEST_F(TrackerTest, TestC8ToMetric) {
  ScopeTimer rt("test");
  MutableRootMessage<XrTrackingState> state;
  Tracker tracker;
  DetectionImagePtrMap targets;
  RandomNumbers r;

  TrackingSensorFrame frame;
  frame.intrinsic = INTRINSIC;
  frame.devicePose = Q0;
  frame.srcY = YPlanePixels();  // nullptr

  auto camInMetric = tracker.track(frame, targets, &r, state.builder());
}

}  // namespace c8
