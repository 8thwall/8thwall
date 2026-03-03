// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)
//

#pragma once

#include <deque>

#include "c8/geometry/parameterized-geometry.h"
#include "c8/hmatrix.h"
#include "c8/quaternion.h"
#include "c8/vector.h"
#include "reality/engine/geometry/observed-point.h"

namespace c8 {

struct PoseEstimationAnalyticalParams {
  // When true, use a loss function to supress outliers.
  bool suppressOutliers = true;
  // Specify a scale value to use inside of the cost function.
  double scale = 1.0;
  // When true, use CameraMovement cost function instead of PositionTarget/RotationTarget combo.
  bool useImageTargetCostFunction = false;
  // When useImageTargetCostFunction is true, scale CameraMovement's weight by this amount.
  double cameraMotionWeight = 0.0f;
  // When true, output logging.
  bool logReport = false;
};

// Stores the residual output of pose estimation.
struct PoseEstimationResidualOutput {
  // The distance between the two rays of a match.
  Vector<double> pointResiduals;
  // The x/y residual for each match.
  Vector<double> pointResidualsX;
  Vector<double> pointResidualsY;
  // Length 6. Pos plus angle-axis rotation.
  Vector<double> camResiduals;

  void clear() {
    pointResiduals.clear();
    pointResidualsX.clear();
    pointResidualsY.clear();
    camResiduals.clear();
  }
};

float computeProjectedMapPointError(
  const Vector<ObservedPoint> &rays, const Vector<HPoint3> &worldPts, const HMatrix &extrinsic);
// same as the ObservedPoint version but takes the positions directly
float computeProjectedMapPointError(
  const Vector<HPoint2> &rays, const Vector<HPoint3> &worldPts, const HMatrix &extrinsic);

float computeProjectedMapPointError(const Vector<HPoint3> &p1, const Vector<HPoint3> &p2);

Vector<ObservedPoint> observedPoints(const Vector<HPoint2> &pts);

// Optimize using SCHUR complement in linear solver
bool bundleAdjustSchur(
  const Vector<ObservedPoint> &observedCameraRay,
  const Vector<int> &obsToWorldPtIndex,
  Vector<HPoint3> *worldPts,
  bool pointsFixed,
  const Vector<int> &obsToCameraIndex,
  Vector<HMatrix> *cameraExtrinsics,
  const Vector<int> &cameraFixed,
  float cameraMotionWeight,
  float *residualError,
  bool logReport = false);

// Optimize the camera's position by comparing the extracted cylinder texture from the search image
// in the target space
bool poseEstimateCurvyTargetSpace(
  const Vector<HPoint2> &targetFeatures,
  const Vector<HPoint2> &liftedFeaturesRays,
  const CurvyImageGeometry &curvyGeom,
  const c8_PixelPinholeCameraModel &intrinsics,
  const float residualScale,
  HMatrix *cameraExtrinsic,
  float *residualError,
  bool logReport);

// Perform pose estimation on a single camera
bool poseEstimationSchur(
  const Vector<ObservedPoint> &observedCameraRay,
  const Vector<HPoint3> &worldPts,
  float cameraMotionWeight,
  HMatrix *cameraExtrinsics,
  bool logReport = false);

// Perform pose estimation on a single camera
bool poseEstimationAnalytical(
  const Vector<ObservedPoint> &observedCameraRay,
  const Vector<HPoint3> &worldPts,
  const std::deque<HMatrix> &lastTwoCameraExtrinsics,
  HMatrix *cameraExtrinsics,
  PoseEstimationAnalyticalParams params,
  PoseEstimationResidualOutput *residualOutput = nullptr);

// Perform pose estimation on a single camera
bool poseEstimationImageTarget(
  const Vector<HPoint2> &camRays,
  const Vector<HPoint2> &targetRays,
  const Vector<float> &weights,
  float cauchyLoss,
  float cameraMotionWeight,
  HMatrix *cameraExtrinsics,
  bool logReport = false);

// Estimate the position of 3d points relative to a model.
// @param modelTransform the pose from the model to the observed points.
bool poseEstimationFull3d(
  const Vector<HPoint3> &observedPoints,
  const Vector<HPoint3> &modelPoints,
  HMatrix *modelTransform);

// Estimates the scale that needs to be applied to the points of a reference model to
// align them with a set of observed points.
bool poseEstimationScale3d(
  const Vector<HPoint3> &observedPoints, const Vector<HPoint3> &modelPoints, float *scale);

// Estimates the scale that needs to be applied to Vio vals to align them with IMU vals.
// @param scale What to multiple the Vio vals by to get IMU vals
bool poseEstimationScale1d(
  const Vector<float> &vioVals,
  const Vector<float> &imuVals,
  float *scale,
  float *accelBiasInWorld);

}  // namespace c8
