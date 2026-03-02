// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/hmatrix.h"
#include "c8/hvector.h"

namespace c8 {

// Fit a 3x3 matrix to predict pred from obs using ridge regression. The regularizer term allows for
// graceful degredation in the face of collinear data by resolving weight matrix ties in favor of
// weights that are closer to 0.  Setting the regularizer to 0 will result in pure analytical linear
// reguression, but could throw a runtime error if the observations are collinear.
HMatrix fitLinear33(const Vector<HVector3> &obs, const Vector<HVector3> &pred, float reg = 1e-5f);

// Compute the residual of a fit between observation an prediction. This is the sum square distance
// of the recored prediction (fit * obs) and the ground truth prediction.
float residual(const HMatrix &fit, const Vector<HVector3> &obs, const Vector<HVector3> &pred);

}  // namespace c8
