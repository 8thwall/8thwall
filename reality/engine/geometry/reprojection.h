// Copyright (c) 2024 Niantic, Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)

#pragma once

#include "c8/hmatrix.h"
#include "c8/hvector.h"
#include "c8/vector.h"

namespace c8 {

// Find inliers with reprojection error, assuming the homogeneous coordinate for worldPts is 1.f
// @param worldPts 3D points in world coordinates.
// @param worldToCam Projection matrix.
// @param rays 2D points in camera coordinates.
// @param sqResidualThresh Squared reprojection error threshold.
// @param inliers Output inliers.
void reprojectionInliers(
  const Vector<HPoint3> &worldPts,
  const HMatrix &worldToCam,
  const Vector<HPoint2> &rays,
  float sqResidualThresh,
  Vector<size_t> *inliers);

}  // namespace c8