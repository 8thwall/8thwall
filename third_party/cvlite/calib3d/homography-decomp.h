// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "third_party/cvlite/core/core.hpp"

namespace c8cv {

/** @brief Decompose a homography matrix to rotation(s), translation(s) and plane normal(s).

@param H The input homography matrix between two images.
@param K The input intrinsic camera calibration matrix.
@param rotations Array of rotation matrices.
@param translations Array of translation matrices.
@param normals Array of plane normal matrices.

This function extracts relative camera motion between two views observing a planar object from the
homography H induced by the plane. The intrinsic camera matrix K must also be provided. The function
may return up to four mathematical solution sets. At least two of the solutions may further be
invalidated if point correspondences are available by applying positive depth constraint (all points
must be in front of the camera). The decomposition method is described in detail in @cite Malis .
 */
CV_EXPORTS_W int decomposeHomographyMat(
  InputArray H,
  InputArray K,
  OutputArrayOfArrays rotations,
  OutputArrayOfArrays translations,
  OutputArrayOfArrays normals);

}  // namespace c8cv
