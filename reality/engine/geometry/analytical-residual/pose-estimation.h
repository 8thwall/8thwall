// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#pragma once

#include "ceres/ceres.h"
#include "reality/engine/geometry/observed-point.h"

namespace c8 {
namespace analytic {

/** Perform pose estimation on a single camera.
 * This method is designed to replace PoseEstimationAnalytic in bundle-residual-analytic. When pose-estimation.py
 * has auto-memoize then we can switch it over.
 *
 * @param scale A scale value used for curvy image targets, based off SnavelyReprojectionResidual.
 * Scale the error value allow us to optimize with a smaller parameter tolerance (~1E-8)
 * without this, we need to optimize at the tolerance of 1E-16 to get a good fit
 * 500 is used for curvy image targets because since 500 is around the focal length in pixel, this
 * puts the parameter tolerance around the tolerance if we were to optimize in pixel space.
 * Experiment has shown that optimizing in pixel space range results in better fit for synthetic
 * data.
 */
class PoseEstimation: public ceres::SizedCostFunction<2, 6> {
public:
  PoseEstimation(double x, double y, double z, const ObservedPoint &pt, double scale = 1.f);

  virtual bool Evaluate(
    double const *const *parameters, double *residuals, double **jacobians) const;

private:
  double x_, y_, z_, u_, v_, w_, scale_;
};

}
}
