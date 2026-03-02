// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "//reality/engine/geometry:bundle-residual",
    "//reality/engine/geometry:bundle-residual-analytic",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xfccec9b7);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <random>

#include "ceres/ceres.h"
#include "ceres/rotation.h"
#include "reality/engine/geometry/bundle-residual-analytic.h"
#include "reality/engine/geometry/bundle-residual.h"

using testing::DoubleNear;
using testing::Eq;
using testing::FloatNear;
using testing::Pointwise;

namespace c8 {

namespace {

std::random_device rd;
auto randGen = std::bind(std::uniform_int_distribution<int>(-180, 180), std::mt19937(59));
auto randDouble = std::bind(std::uniform_real_distribution<double>(-2, 2), std::mt19937(rd()));

}  // namespace

class BundleResidualAnalyticTest : public ::testing::Test {
protected:
  void SetUp() override {}
};

TEST_F(BundleResidualAnalyticTest, testPositionTargetAnalyticSameAsAutoLarge) {
  constexpr int TARGET_DIM = 3;
  const double cameraParams[6]{
    randDouble(), randDouble(), randDouble(), randDouble(), randDouble(), randDouble()};
  std::array<double, TARGET_DIM> v = {-cameraParams[3], -cameraParams[4], -cameraParams[5]};

  // Setting up auto version of the problem
  std::unique_ptr<ceres::CostFunction> autoFunc{
    new ceres::AutoDiffCostFunction<PositionTarget, TARGET_DIM, BundleDesc::N_CAMERA_PARAMS>(
      new PositionTarget(v))};

  // Analytic version of the problem
  PositionTargetAnalytic analyticFunc{v};

  const double *funcParameters[1]{cameraParams};

  double autoFuncResidual[TARGET_DIM];
  double autoFuncJacobian[TARGET_DIM * BundleDesc::N_CAMERA_PARAMS];
  double *autoFuncJacobians[1]{autoFuncJacobian};
  autoFunc->Evaluate(funcParameters, autoFuncResidual, autoFuncJacobians);

  double analyticalResidual[TARGET_DIM];
  double analyticalFuncJacobian[TARGET_DIM * BundleDesc::N_CAMERA_PARAMS];
  double *analyticalFuncJacobians[1]{analyticalFuncJacobian};
  analyticFunc.Evaluate(funcParameters, analyticalResidual, analyticalFuncJacobians);

  for (size_t i = 0; i < TARGET_DIM; i++) {
    EXPECT_NEAR(analyticalResidual[i], autoFuncResidual[i], 1e-6);
  }

  for (size_t i = 0; i < TARGET_DIM * BundleDesc::N_CAMERA_PARAMS; i++) {
    EXPECT_NEAR(analyticalFuncJacobian[i], autoFuncJacobian[i], 1e-5) << i;
  }
}

TEST_F(BundleResidualAnalyticTest, testPositionTargetAnalyticSameAsAutoSmall) {
  constexpr int TARGET_DIM = 3;
  double cameraParams[6] = {
    1e-6 * randDouble(),
    1e-6 * randDouble(),
    1e-6 * randDouble(),
    randDouble(),
    randDouble(),
    randDouble()};
  std::array<double, TARGET_DIM> v = {-cameraParams[3], -cameraParams[4], -cameraParams[5]};

  // Setting up auto version of the problem
  std::unique_ptr<ceres::CostFunction> autoFunc{
    new ceres::AutoDiffCostFunction<PositionTarget, TARGET_DIM, BundleDesc::N_CAMERA_PARAMS>(
      new PositionTarget(v))};

  // Analytic version of the problem
  PositionTargetAnalytic analyticFunc{v};

  double *funcParameters[1]{cameraParams};

  double analyticalResidual[TARGET_DIM];
  double analyticalFuncJacobian[TARGET_DIM * BundleDesc::N_CAMERA_PARAMS];
  double *analyticalFuncJacobians[1]{analyticalFuncJacobian};

  double autoFuncResidual[TARGET_DIM];
  double autoFuncJacobian[TARGET_DIM * BundleDesc::N_CAMERA_PARAMS];
  double *autoFuncJacobians[1]{autoFuncJacobian};

  analyticFunc.Evaluate(funcParameters, analyticalResidual, analyticalFuncJacobians);

  autoFunc->Evaluate(funcParameters, autoFuncResidual, autoFuncJacobians);

  for (size_t i = 0; i < TARGET_DIM; i++) {
    EXPECT_NEAR(analyticalResidual[i], autoFuncResidual[i], 1e-6);
  }

  for (size_t i = 0; i < TARGET_DIM * BundleDesc::N_CAMERA_PARAMS; i++) {
    EXPECT_NEAR(analyticalFuncJacobian[i], autoFuncJacobian[i], 1e-5) << i;
  }
}

TEST_F(BundleResidualAnalyticTest, testPositionTargetAnalyticWithDirectAngleAxisRotate) {
  constexpr int TARGET_DIM = 3;
  std::array<double, 3> positionTarget = {0., 0., 0.};
  PositionTargetAnalytic posTarget(positionTarget);
  // These values are captured in one of the runs. See bundle-residual-test
  double camParams[6] = {
    -0.0036569173298607125,
    2.8092244981738665,
    -0.75110418817873481,
    0.18389122188091278,
    -0.3818168640136718,
    -0.52198320627212524};

  double residuals[3];
  double *jacobians[1]{nullptr};
  double *funcParameters[1]{camParams};
  posTarget.Evaluate(funcParameters, residuals, jacobians);

  double gtResiduals[3] = {-0.038702315185167771, -0.063887393558295202, 0.6681969009983908};
  for (size_t i = 0; i < TARGET_DIM; i++) {
    EXPECT_NEAR(residuals[i], gtResiduals[i], 1e-6);
  }
}

constexpr int IMAGE_TARGET_DIM = 2;
void inversePoseEstimationImageTargetVsAuto(
  const double (&cameraParams)[BundleDesc::N_CAMERA_PARAMS],
  const std::array<double, IMAGE_TARGET_DIM * 2> &v,
  double w) {
  // Setting up auto version of the problem
  std::unique_ptr<ceres::CostFunction> autoFunc{new ceres::AutoDiffCostFunction<
    InversePoseEstimationImageTarget,
    IMAGE_TARGET_DIM,
    BundleDesc::N_CAMERA_PARAMS>(new InversePoseEstimationImageTarget(v[0], v[1], v[2], v[3], w))};

  const double *funcParameters[1]{cameraParams};

  double autoFuncResidual[IMAGE_TARGET_DIM];
  double autoFuncJacobian[IMAGE_TARGET_DIM * BundleDesc::N_CAMERA_PARAMS];
  double *autoFuncJacobians[1]{autoFuncJacobian};
  autoFunc->Evaluate(funcParameters, autoFuncResidual, autoFuncJacobians);

  InversePoseEstimationImageTargetAnalytic analyticFunc{v[0], v[1], v[2], v[3], w};
  double analyticalResidual[IMAGE_TARGET_DIM];
  double analyticalFuncJacobian[IMAGE_TARGET_DIM * BundleDesc::N_CAMERA_PARAMS];
  double *analyticalFuncJacobians[1]{analyticalFuncJacobian};
  analyticFunc.Evaluate(funcParameters, analyticalResidual, analyticalFuncJacobians);

  for (size_t i = 0; i < IMAGE_TARGET_DIM; i++) {
    EXPECT_NEAR(analyticalResidual[i], autoFuncResidual[i], 1e-6);
  }

  for (size_t i = 0; i < IMAGE_TARGET_DIM * BundleDesc::N_CAMERA_PARAMS; i++) {
    EXPECT_NEAR(analyticalFuncJacobian[i], autoFuncJacobian[i], 1e-5) << i;
  }
}

TEST_F(BundleResidualAnalyticTest, testInversePoseEstimationTargetAnalyticSameAsAutoSmall) {
  const double cameraParams[6]{0.1e-6, 0.2e-6, 0.3e-6, 1., 2., 3.};
  std::array<double, IMAGE_TARGET_DIM * 2> v = {
    randDouble(), randDouble(), randDouble(), randDouble()};

  inversePoseEstimationImageTargetVsAuto(cameraParams, v, 1.0);
}

TEST_F(BundleResidualAnalyticTest, testInversePoseEstimationTargetAnalyticSameAsAutoLarge) {
  // The rotation values correspond to
  // R =
  //[  0.9090884, -0.3195928,  0.2672426;
  // 0.3564488,  0.9287450, -0.1018674;
  //-0.2156442,  0.1878648,  0.9582298 ]
  // Q[x,y,z,w] = [ 0.0743532, 0.1239221, 0.1734909, 0.9741744 ]
  const double cameraParams[6]{0.15, 0.25, 0.35, 1., 2., 3.};

  std::array<double, IMAGE_TARGET_DIM * 2> v = {
    randDouble(), randDouble(), randDouble(), randDouble()};

  inversePoseEstimationImageTargetVsAuto(cameraParams, v, 1.0);
}

}  // namespace c8
