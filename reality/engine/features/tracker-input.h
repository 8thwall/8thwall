// Copyright (c) 2026 8th Wall, Inc.
// Original Author: Paris Morgan (parismorgan@nianticlabs.com)

#pragma once

#include "reality/engine/features/frame-point.h"

namespace c8 {
struct TrackerInput {
  FrameWithPoints framePoints;
  Vector<FrameWithPoints> roiPoints;
  Quaternion pose;
  int64_t timeNanos;

  TrackerInput(c8_PixelPinholeCameraModel intrinsic, Quaternion xrDevicePose, int64_t timeNanos = 0)
      : framePoints(intrinsic), pose(xrDevicePose), timeNanos(timeNanos) {
    framePoints.setXRDevicePose(pose);
  }
};

}  // namespace c8
