// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

// Analytical versions of the ones in bundle-residual

#pragma once
#include "ceres/ceres.h"
#include "reality/engine/geometry/observed-point.h"

namespace c8 {

// TODO(dat): Consider changing 3 and 6 to static constexpr
// Input 6-dimension: rx, ry, rz (rotation in angle-axis format), tx, ty, tz (translation)
// Output 3-dimension: delta_tx, delta_ty, delta_tz delta between the target position and computed
// position
// This cost function try to keep the camera extrinsic such that its position in world coordinate
// is close to v.
class PositionTargetAnalytic : public ceres::SizedCostFunction<3, 6> {
public:
  PositionTargetAnalytic(std::array<double, 3> v) : v_(v){};
  virtual bool Evaluate(
    double const *const *parameters, double *residuals, double **jacobians) const;

private:
  std::array<double, 3> v_;
};

/** Computes the analytical version of TriangulatePointMultiView.
 * Computes the residual of a world point location given its ray space position in one
 * camera, and the camera's projection matrix.  When residuals are combined across cameras, the
 * point can be triangulated by gradient descent.
 *
 * Input 3-dimension: ptx, pty, ptz (the point in world space).
 * Output 2-dimension: the residual in ray space.
 *
 * @param ray The location (in rayspace) of a point in a single camera.
 * @param projection The projection matrix of that camera.
 */
class TriangulatePointMultiviewAnalytic : public ceres::SizedCostFunction<2, 3> {
public:
  TriangulatePointMultiviewAnalytic(std::array<double, 2> ray, std::array<double, 12> projection)
      : ray_(ray), projection_(projection){};
  virtual bool Evaluate(
    double const *const *parameters, double *residuals, double **jacobians) const;

private:
  std::array<double, 2> ray_;
  std::array<double, 12> projection_;
};

class InversePoseEstimationImageTargetAnalytic : public ceres::SizedCostFunction<2, 6> {
public:
  InversePoseEstimationImageTargetAnalytic(double x, double y, double u, double v, double w)
      : x_(x), y_(y), u_(u), v_(v), w_(w) {}
  virtual bool Evaluate(
    double const *const *parameters, double *residuals, double **jacobians) const;

private:
  double x_, y_, u_, v_, w_;
};

/** Perform pose estimation on a single camera.
 *
 * @param scale A scale value used for curvy image targets, based off SnavelyReprojectionResidual.
 * Scale the error value allow us to optimize with a smaller parameter tolerance (~1E-8)
 * without this, we need to optimize at the tolerance of 1E-16 to get a good fit
 * 500 is used for curvy image targets because since 500 is around the focal length in pixel, this
 * puts the parameter tolerance around the tolerance if we were to optimize in pixel space.
 * Experiment has shown that optimizing in pixel space range results in better fit for synthetic
 * data.
 */
class PoseEstimationAnalytic : public ceres::SizedCostFunction<2, 6> {
public:
  PoseEstimationAnalytic(double x, double y, double z, const ObservedPoint &pt, double scale = 1.f);
  virtual bool Evaluate(
    double const *const *parameters, double *residuals, double **jacobians) const;

private:
  double x_, y_, z_, u_, v_, w_, scale_;
};
}  // namespace c8
