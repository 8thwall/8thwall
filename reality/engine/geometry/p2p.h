// Copyright (c) 2024 Niantic, Inc.
// Original Author: Haomin Zhu (hzhu@nianticlabs.com)

#pragma once

#include "c8/hmatrix.h"
#include "c8/hpoint.h"

namespace c8 {

// P2PImpl computes the camera pose from two 3D points and their corresponding 2D camRays.
// @param trackingPose, the transform from camera to tracking coordinates
// Note: camRay1 and camRay2 should not have same u/x or v/y values, which will cause degeration of
// the problem. Same u/x will not lead to correct solution while same v/y will cause zero
// determinant of the M matrix and will be unable to solve the problem.
bool p2pImpl(
  const HPoint3 &worldPt1,
  const HPoint3 &worldPt2,
  const HPoint2 &camRay1,
  const HPoint2 &camRay2,
  const c8::HMatrix &trackingPose,
  c8::HMatrix *worldToCam1,
  c8::HMatrix *worldToCam2);

}  // namespace c8