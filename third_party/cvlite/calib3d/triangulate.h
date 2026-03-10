// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "third_party/cvlite/core/core.hpp"
#include "third_party/cvlite/core/core_c.h"

namespace c8cv {

CVAPI(void)
cvTriangulatePoints(
  CvMat *projMatr1, CvMat *projMatr2, CvMat *projPoints1, CvMat *projPoints2, CvMat *points4D);

CVAPI(void)
cvCorrectMatches(CvMat *F, CvMat *points1, CvMat *points2, CvMat *new_points1, CvMat *new_points2);

/** @brief Reconstructs points by triangulation.

@param projMatr1 3x4 projection matrix of the first camera.
@param projMatr2 3x4 projection matrix of the second camera.
@param projPoints1 2xN array of feature points in the first image. In case of c++ version it can
be also a vector of feature points or two-channel matrix of size 1xN or Nx1.
@param projPoints2 2xN array of corresponding points in the second image. In case of c++ version
it can be also a vector of feature points or two-channel matrix of size 1xN or Nx1.
@param points4D 4xN array of reconstructed points in homogeneous coordinates.

The function reconstructs 3-dimensional points (in homogeneous coordinates) by using their
observations with a stereo camera. Projections matrices can be obtained from stereoRectify.

@note
   Keep in mind that all input data should be of float type in order for this function to work.

@sa
   reprojectImageTo3D
 */
CV_EXPORTS_W void triangulatePoints(
  InputArray projMatr1,
  InputArray projMatr2,
  InputArray projPoints1,
  InputArray projPoints2,
  OutputArray points4D);

/** @brief Refines coordinates of corresponding points.

@param F 3x3 fundamental matrix.
@param points1 1xN array containing the first set of points.
@param points2 1xN array containing the second set of points.
@param newPoints1 The optimized points1.
@param newPoints2 The optimized points2.

The function implements the Optimal Triangulation Method (see Multiple View Geometry for details).
For each given point correspondence points1[i] \<-\> points2[i], and a fundamental matrix F, it
computes the corrected correspondences newPoints1[i] \<-\> newPoints2[i] that minimize the geometric
error \f$d(points1[i], newPoints1[i])^2 + d(points2[i],newPoints2[i])^2\f$ (where \f$d(a,b)\f$ is
the geometric distance between points \f$a\f$ and \f$b\f$ ) subject to the epipolar constraint
\f$newPoints2^T * F * newPoints1 = 0\f$ .
 */
CV_EXPORTS_W void correctMatches(
  InputArray F,
  InputArray points1,
  InputArray points2,
  OutputArray newPoints1,
  OutputArray newPoints2);

}  // namespace c8cv
