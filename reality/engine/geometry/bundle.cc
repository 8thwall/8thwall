// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "bundle.h",
  };
  deps = {
    ":bundle-residual",
    ":bundle-residual-analytic",
    "//c8:hmatrix",
    "//c8:parameter-data",
    "//c8:vector",
    "//c8/geometry:egomotion",
    "//c8/geometry:intrinsics",
    "//c8/geometry:parameterized-geometry",
    "//c8/stats:scope-timer",
    "//reality/engine/geometry/analytical-residual:pose-estimation",
    "@ceres//:ceres",
  };
}
cc_end(0x59313b36);

#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include <cmath>
#include <numeric>

#include "c8/c8-log.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/intrinsics.h"
#include "c8/geometry/parameterized-geometry.h"
#include "c8/parameter-data.h"
#include "c8/stats/scope-timer.h"
#include "ceres/ceres.h"
#include "ceres/rotation.h"
#include "reality/engine/geometry/analytical-residual/pose-estimation.h"
#include "reality/engine/geometry/bundle-residual-analytic.h"
#include "reality/engine/geometry/bundle-residual.h"
#include "reality/engine/geometry/bundle.h"

namespace c8 {
namespace {

struct Settings {
  // Parameters for poseEstimationAnalytical.
  const double rotationRobust;
  const double rotationWt;
  const double positionRobust;
  const double positionWt;
  const double poseEstCauchyLoss;
  const int poseEstMaxNumIterations;
  const double poseEstFunctionTolerance;
  const double poseEstGradientTolerance;
  const double poseEstParameterTolerance;
  const bool useNewPoseEstimationAnalytic;
  const double poseEstHuberLossComposed;
};

const Settings &settings() {
  static const Settings settings_{
    globalParams().getOrSet<double>("Bundle.rotationRobust", 0.3),
    globalParams().getOrSet<double>("Bundle.rotationWt", 0.3),
    globalParams().getOrSet<double>("Bundle.positionRobust", 0.3),
    globalParams().getOrSet<double>("Bundle.positionWt", 0.05),
    globalParams().getOrSet<double>("Bundle.poseEstCauchyLoss", 1e-2),
    globalParams().getOrSet<int>("Bundle.poseEstMaxNumIterations", 100),
    globalParams().getOrSet<double>("Bundle.poseEstFunctionTolerance", 1e-10),
    globalParams().getOrSet<double>("Bundle.poseEstGradientTolerance", 1e-10),
    globalParams().getOrSet<double>("Bundle.poseEstParameterTolerance", 1e-6),
    globalParams().getOrSet<bool>("Bundle.useNewPoseEstimationAnalytic", false),
    globalParams().getOrSet<double>("Bundle.poseEstHuberLossComposed", 0.00088),
  };
  return settings_;
}

constexpr double squared(double d) { return d * d; }

HMatrix cameraParamsToCamMotion(double *camera) {
  // extract results.
  auto translation =
    HMatrixGen::translation(-(float)camera[3], -(float)camera[4], -(float)camera[5]);

  double rot[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  ceres::AngleAxisToRotationMatrix(camera, rot);
  auto rotation = HMatrix{
    {(float)rot[0], (float)rot[1], (float)rot[2], 0.0f},
    {(float)rot[3], (float)rot[4], (float)rot[5], 0.0f},
    {(float)rot[6], (float)rot[7], (float)rot[8], 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {(float)rot[0], (float)rot[3], (float)rot[6], 0.0f},
    {(float)rot[1], (float)rot[4], (float)rot[7], 0.0f},
    {(float)rot[2], (float)rot[5], (float)rot[8], 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}};
  return rotation * translation;
}

template <typename T>
HMatrix paramsToCamera(T *params) {
  // extract results.
  auto t = HMatrixGen::translation((float)params[3], (float)params[4], (float)params[5]);

  // By default, ceres uses column major for rotation matrices.
  T rot[9];
  ceres::AngleAxisToRotationMatrix(params, rot);
  auto r = HMatrix{
    {(float)rot[0], (float)rot[3], (float)rot[6], 0.0f},
    {(float)rot[1], (float)rot[4], (float)rot[7], 0.0f},
    {(float)rot[2], (float)rot[5], (float)rot[8], 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {(float)rot[0], (float)rot[1], (float)rot[2], 0.0f},
    {(float)rot[3], (float)rot[4], (float)rot[5], 0.0f},
    {(float)rot[6], (float)rot[7], (float)rot[8], 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}};
  return t * r;
}

/**
 * Extracts an array to an HMatrix.
 * @param params a 7-element array [r1,r2,r3,t1,t2,t3,s] where [r1,r2,r3] is angle-axis
 * representation of rotation, [t1,t2,t3] is translation, s is scale. TRS convention.
 */
template <typename T>
HMatrix rtsParamsToHMatrix(T *params) {
  // extract results.
  auto t = HMatrixGen::translation((float)params[3], (float)params[4], (float)params[5]);

  // By default, ceres uses column major for rotation matrices.
  T rot[9];
  ceres::AngleAxisToRotationMatrix(params, rot);
  auto r = HMatrix{
    {(float)rot[0], (float)rot[3], (float)rot[6], 0.0f},
    {(float)rot[1], (float)rot[4], (float)rot[7], 0.0f},
    {(float)rot[2], (float)rot[5], (float)rot[8], 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {(float)rot[0], (float)rot[1], (float)rot[2], 0.0f},
    {(float)rot[3], (float)rot[4], (float)rot[5], 0.0f},
    {(float)rot[6], (float)rot[7], (float)rot[8], 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}};
  return t * r * HMatrixGen::scale(static_cast<float>(params[6]));
}

// Fills in a double array with the camera translation.
std::array<double, 3> translationDouble(const HMatrix &cam) {
  return {
    static_cast<double>(cam(0, 3)), static_cast<double>(cam(1, 3)), static_cast<double>(cam(2, 3))};
}

}  // namespace

float computeProjectedMapPointError(
  const Vector<ObservedPoint> &rays, const Vector<HPoint3> &worldPts, const HMatrix &extrinsic) {
  float err = 0.0f;
  for (int i = 0; i < rays.size(); ++i) {
    auto pt = (extrinsic.inv() * worldPts[i]).flatten();

    auto dx = pt.x() - rays[i].position.x();
    auto dy = pt.y() - rays[i].position.y();
    auto dd = dx * dx + dy * dy;
    err += dd;
  }
  return err / rays.size();
}

float computeProjectedMapPointError(
  const Vector<HPoint2> &rays, const Vector<HPoint3> &worldPts, const HMatrix &extrinsic) {
  float err = 0.0f;
  for (int i = 0; i < rays.size(); ++i) {
    auto pt = (extrinsic.inv() * worldPts[i]).flatten();

    auto dx = pt.x() - rays[i].x();
    auto dy = pt.y() - rays[i].y();
    auto dd = dx * dx + dy * dy;
    err += dd;
  }
  return err / rays.size();
}

float computeProjectedMapPointError(const Vector<HPoint3> &p1, const Vector<HPoint3> &p2) {
  float err = 0.0f;
  for (int i = 0; i < p1.size(); ++i) {
    auto dx = p1[i].x() - p2[i].x();
    auto dy = p1[i].y() - p2[i].y();
    auto dz = p1[i].z() - p2[i].z();

    auto dd = dx * dx + dy * dy + dz * dz;
    err += dd;
  }
  return err / p1.size();
}

Vector<ObservedPoint> observedPoints(const Vector<HPoint2> &pts) {
  Vector<ObservedPoint> ob;
  ob.reserve(pts.size());
  for (const auto &pt : pts) {
    ob.push_back({{pt.x(), pt.y()}, 0, 0.0f, 1.0f});
  }
  return ob;
}

// Perform pose estimation on a single camera
bool poseEstimationSchur(
  const Vector<ObservedPoint> &observedCameraRay,
  const Vector<HPoint3> &worldPts,
  float cameraMotionWeight,
  HMatrix *cameraExtrinsic,
  bool logReport) {
  Vector<int> observationToCameraIndex(worldPts.size(), 0);
  Vector<int> observationToWorldPtIndex;
  observationToWorldPtIndex.reserve(worldPts.size());
  for (int i = 0; i < worldPts.size(); ++i) {
    observationToWorldPtIndex.push_back(i);
  }
  Vector<HMatrix> cameraExtrinsics;
  cameraExtrinsics.push_back(*cameraExtrinsic);
  Vector<int> cameraFixed(static_cast<size_t>(1), 0);
  float residualError;
  bool success = bundleAdjustSchur(
    observedCameraRay,
    observationToWorldPtIndex,
    const_cast<Vector<HPoint3> *>(&worldPts),
    true,
    observationToCameraIndex,
    &cameraExtrinsics,
    cameraFixed,
    cameraMotionWeight,
    &residualError,
    logReport);
  *cameraExtrinsic = cameraExtrinsics[0];
  return success;
}

// An alternative local tracker for curvy image target that optimizes in the target space instead
// of the search space.  So far, the results are worse than the currently used local optimizer.
// We plan to come back to this idea to see if improvements can be made.
bool poseEstimateCurvyTargetSpace(
  const Vector<HPoint2> &targetFeatures,
  const Vector<HPoint2> &liftedSearchFeatures,
  const CurvyImageGeometry &curvyGeom,
  const c8_PixelPinholeCameraModel &intrinsics,
  const float residualScale,
  HMatrix *cameraExtrinsicPtr,
  float *residualError,
  bool logReport) {
  if (liftedSearchFeatures.size() == 0) {
    return true;
  }

  auto cameraExtrinsic = *cameraExtrinsicPtr;
  double camera[BundleDesc::N_CAMERA_PARAMS];

  // We are using the camera extrinsics to move from world space to camera space.  Note that
  // RotationMatrixToAngleAxis is in column major order
  double rot[9] = {
    cameraExtrinsic(0, 0),
    cameraExtrinsic(1, 0),
    cameraExtrinsic(2, 0),
    cameraExtrinsic(0, 1),
    cameraExtrinsic(1, 1),
    cameraExtrinsic(2, 1),
    cameraExtrinsic(0, 2),
    cameraExtrinsic(1, 2),
    cameraExtrinsic(2, 2)};

  // convert the 9 rotation parameters into 3 euler angles doubles.
  ceres::RotationMatrixToAngleAxis(rot, camera);

  // extract the translation vectors
  camera[3] = cameraExtrinsic(0, 3);
  camera[4] = cameraExtrinsic(1, 3);
  camera[5] = cameraExtrinsic(2, 3);

  ceres::Problem problem;

  // minimize in the target space
  for (int k = 0; k < targetFeatures.size(); ++k) {
    auto &liftedSearchRay = liftedSearchFeatures[k];

    auto &targetRay = targetFeatures[k];
    ceres::CostFunction *cost_function = new ceres::AutoDiffCostFunction<
      CurvyTargetReprojectionResidual,
      2,                           // number of residuals - scaled x and scaled y
      BundleDesc::N_CAMERA_PARAMS  // 6 dof - euler angle rotation + translation
      >(new CurvyTargetReprojectionResidual(
      targetRay, liftedSearchRay, curvyGeom, intrinsics, residualScale));

    // pass in the camera extrinsics.  We want to minimize the distance of the projected world point
    // from the lifted search image and the target image's ray.
    problem.AddResidualBlock(cost_function, new ceres::HuberLoss(0.5), camera);
  }

  // Encourage smaller camera position movements
  // As of 3/2021 the values here were not individually tuned, rather the weight, rotationWeight,
  // and CauchyLoss values were taken from poseEstimationAnalytical, which was previously tuned.
  ceres::CostFunction *cost_function = new ceres::
    AutoDiffCostFunction<CameraMovement, BundleDesc::N_CAMERA_PARAMS, BundleDesc::N_CAMERA_PARAMS>(
      new CameraMovement(camera, 0.07, 10.0));
  problem.AddResidualBlock(cost_function, new ceres::CauchyLoss(0.03), camera);

  ceres::Solver::Options options;

  options.linear_solver_type = ceres::DENSE_SCHUR;
  options.preconditioner_type = ceres::SCHUR_JACOBI;
  options.use_explicit_schur_complement = true;
  options.max_num_iterations = 100;
  options.function_tolerance = 1e-8;
  options.gradient_tolerance = 1e-8;
  options.parameter_tolerance = 1e-8;
  options.minimizer_progress_to_stdout = true;

  if (!logReport) {
    options.logging_type = ceres::SILENT;
    options.minimizer_progress_to_stdout = false;
  }

  ceres::Solver::Summary summary;
  ceres::Solve(options, &problem, &summary);

  if (residualError) {
    Vector<double> final_residuals;
    problem.Evaluate(ceres::Problem::EvaluateOptions(), NULL, &final_residuals, NULL, NULL);
    *residualError = 0.0f;
    for (int i = 0; i < final_residuals.size(); ++i) {
      *residualError += static_cast<float>(final_residuals[i] * final_residuals[i]);
    }
  }

  if (logReport) {
    C8Log("%s", summary.FullReport().c_str());
  }

  if (summary.IsSolutionUsable()) {
    *cameraExtrinsicPtr = paramsToCamera(camera);
  }

  return summary.IsSolutionUsable();
}

/** Things that have been tested to show better results when it comes to bundle adjustment
 *   - A scale of around 500 in the Snavely Error : a better fit at the same parameter tolerance
 *   - Optimize directly in the pixel space (by incorporating the intrinsic) : no improvement
 * comparing to a scaled snavely
 *   - Hard threshold for camera movement (i.e. lower/upper bounds) : worse than damping the camera
 * movement with CameraMovement cost
 *   - CameraMovement weight is tuned to 100 by: go down until bundle-test starts failing.
 * Go up until the pose update starts lagging
 *
 *  Ideas that have not been tested
 *   - Incorporate feature distance and observation weight for a better fit?
 *
 *  Observations
 *   - When the cost/bounds are too high, the pose react slowly. The correct pose is found after
 * multiple frames. This shows up as a lagged pose update when the phone rotated.
 */
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
  bool logReport) {
  if (cameraExtrinsics->size() == 0 && worldPts->size() == 0) {
    return true;
  }

  if (logReport) {
    C8Log(
      "[bundle] Bundle Adjust Schur %d extrinsics, %d world pts, observedCameraRay=%d, "
      "obsToWorldPtIndex=%d, obsToCameraIndex=%d",
      cameraExtrinsics->size(),
      worldPts->size(),
      observedCameraRay.size(),
      obsToWorldPtIndex.size(),
      obsToCameraIndex.size());
  }

  // All camera extrinsics and mutable world points
  double *parameters;
  double *worldPtsParameterStart;
  {
    // ceres mem setup. Cameras then points.
    parameters =
      new double[BundleDesc::N_CAMERA_PARAMS * cameraExtrinsics->size() + 3 * worldPts->size()];

    // Populate camera parameters
    for (int j = 0; j < cameraExtrinsics->size(); ++j) {
      auto &camEst = cameraExtrinsics->at(j);
      double *camera = &parameters[BundleDesc::N_CAMERA_PARAMS * j];
      double rot[9] = {
        camEst(0, 0),
        camEst(0, 1),
        camEst(0, 2),
        camEst(1, 0),
        camEst(1, 1),
        camEst(1, 2),
        camEst(2, 0),
        camEst(2, 1),
        camEst(2, 2)};
      ceres::RotationMatrixToAngleAxis(rot, camera);
      camera[3] = camEst.inv()(0, 3);
      camera[4] = camEst.inv()(1, 3);
      camera[5] = camEst.inv()(2, 3);
    }

    // Populate worldPt parameters
    worldPtsParameterStart = parameters + BundleDesc::N_CAMERA_PARAMS * cameraExtrinsics->size();
    for (int i = 0; i < worldPts->size(); ++i) {
      auto &worldPt = worldPts->at(i);
      double *pt = worldPtsParameterStart + (3 * i);
      pt[0] = worldPt.x();
      pt[1] = worldPt.y();
      pt[2] = worldPt.z();
    }
  }

  // Create problem
  ceres::Problem problem;
  for (int k = 0; k < observedCameraRay.size(); ++k) {
    const int i = obsToWorldPtIndex[k];
    double *worldPt = worldPtsParameterStart + (3 * i);

    const int j = obsToCameraIndex[k];
    double *camera = &parameters[BundleDesc::N_CAMERA_PARAMS * j];

    auto &ray = observedCameraRay[k];
    ceres::CostFunction *cost_function = new ceres::
      AutoDiffCostFunction<SnavelyReprojectionResidual, 2, BundleDesc::N_CAMERA_PARAMS, 3>(
        new SnavelyReprojectionResidual(ray));
    problem.AddResidualBlock(cost_function, new ceres::HuberLoss(0.5), camera, worldPt);
  }

  if (cameraMotionWeight > 0.f) {
    for (int j = 0; j < cameraExtrinsics->size(); ++j) {
      // Encourage smaller camera position movements
      double *camera = &parameters[BundleDesc::N_CAMERA_PARAMS * j];
      ceres::CostFunction *cost_function = new ceres::AutoDiffCostFunction<
        CameraMovement,
        BundleDesc::N_CAMERA_PARAMS,
        BundleDesc::N_CAMERA_PARAMS>(new CameraMovement(camera, 0.02 * cameraMotionWeight, 10.0));
      problem.AddResidualBlock(cost_function, new ceres::CauchyLoss(0.01), camera);
    }
  }

  ceres::Solver::Options options;
  int fitPt = 0, fitCamera = 0;
  {
    // World points belong to the first elimination group
    // Camera parameters belong to the second elimination group
    // see http://ceres-solver.org/nnls_solving.html#ordering
    options.linear_solver_ordering.reset(new ceres::ParameterBlockOrdering);
    for (int i = 0; i < worldPts->size(); ++i) {
      auto worldPt = worldPtsParameterStart + 3 * i;
      if (pointsFixed) {
        problem.SetParameterBlockConstant(worldPt);
      } else {
        options.linear_solver_ordering->AddElementToGroup(worldPt, 0);
        fitPt++;
      }
    }

    for (int j = 0; j < cameraExtrinsics->size(); ++j) {
      auto camera = parameters + BundleDesc::N_CAMERA_PARAMS * j;
      if (cameraFixed[j]) {
        problem.SetParameterBlockConstant(camera);
      } else {
        options.linear_solver_ordering->AddElementToGroup(camera, 1);
        fitCamera++;
      }
    }
  }
  if (logReport) {
    C8Log(
      "[bundle] pt=%d fitPt=%d cam=%d fitCam=%d",
      worldPts->size(),
      fitPt,
      cameraExtrinsics->size(),
      fitCamera);
  }

  // NOTE(dat): DENSE_SCHUR gives the most speed on a bundle adjustment benchmark
  //            SPARSE_SCHUR gives around the same speed on 100 points. Might be better with more
  //            points.

  // Result on 3/9/2021
  // For bundle adjustment with 8 cameras and 300 points
  //   With DENSE_SCHUR
  // BundleBenchmarkTest/bundleSchur                    74761170 ns   74663222 ns          9
  //   With SPARSE_SCHUR and EigenSparse
  // BundleBenchmarkTest/bundleSchur                    78122369 ns   77937750 ns          8
  //   With ITERATIVE_SCHUR
  // BundleBenchmarkTest/bundleSchur                   103748560 ns  103308600 ns          5

  options.linear_solver_type = ceres::DENSE_SCHUR;
  options.preconditioner_type = ceres::SCHUR_JACOBI;

  // NOTE(dat): Explicit schur complement has not shown better or faster convergence.
  // options.use_explicit_schur_complement = true;

  // NOTE(dat): Residual generally hits a valley in under 50 iterations. 100 iterations is also
  // outside of our time allotment.
  // options.max_num_iterations = 100;
  options.function_tolerance = 1e-8;
  options.gradient_tolerance = 1e-8;
  options.parameter_tolerance = 1e-8;
  options.minimizer_progress_to_stdout = true;
  // options.max_solver_time_in_seconds = fitPt ? 1.0 : 1.0;

  if (!logReport) {
    options.logging_type = ceres::SILENT;
    options.minimizer_progress_to_stdout = false;
  }

  ceres::Solver::Summary summary;
  ceres::Solve(options, &problem, &summary);

  if (residualError) {
    Vector<double> final_residuals;
    problem.Evaluate(ceres::Problem::EvaluateOptions(), NULL, &final_residuals, NULL, NULL);
    *residualError = 0.0f;
    for (int i = 0; i < final_residuals.size(); ++i) {
      *residualError += static_cast<float>(final_residuals[i] * final_residuals[i]);
    }
  }

  if (logReport) {
    C8Log("%s", summary.FullReport().c_str());
  }

  if (summary.IsSolutionUsable()) {
    // extract results.
    // Extract world points
    if (fitPt > 0) {
      for (int i = 0; i < worldPts->size(); ++i) {
        double *pt = &parameters[BundleDesc::N_CAMERA_PARAMS * cameraExtrinsics->size() + 3 * i];
        worldPts->at(i) = HPoint3((float)pt[0], (float)pt[1], (float)pt[2]);
      }
    }

    // Extract camera parameters
    for (int j = 0; j < cameraExtrinsics->size(); ++j) {
      if (cameraFixed[j]) {
        continue;
      }
      double *camera = &parameters[BundleDesc::N_CAMERA_PARAMS * j];
      cameraExtrinsics->at(j) = cameraParamsToCamMotion(camera);
    }
  }

  delete[] parameters;
  return summary.IsSolutionUsable();
}

bool poseEstimationAnalytical(
  const Vector<ObservedPoint> &observedCameraRay,
  const Vector<HPoint3> &worldPts,
  const std::deque<HMatrix> &lastTwoCameraExtrinsics,
  HMatrix *cameraExtrinsics,
  PoseEstimationAnalyticalParams params,
  PoseEstimationResidualOutput *residualOutput) {
  // Find the pre-transform that moves the camera to the origin.
  auto preTransform = translationMat(*cameraExtrinsics).inv();
  // Transform the camera and the points.
  auto solveCam = preTransform * (*cameraExtrinsics);  // Camera location to solve for
  auto solvePts = preTransform * worldPts;             // Points to solve for.
  // Compute the transform to get back to the original space.
  auto postTransform = preTransform.inv();

  double camera[BundleDesc::N_CAMERA_PARAMS];
  {
    // Populate existing camera parameters
    double rot[9] = {
      solveCam(0, 0),
      solveCam(0, 1),
      solveCam(0, 2),
      solveCam(1, 0),
      solveCam(1, 1),
      solveCam(1, 2),
      solveCam(2, 0),
      solveCam(2, 1),
      solveCam(2, 2)};
    ceres::RotationMatrixToAngleAxis(rot, camera);
    camera[3] = solveCam.inv()(0, 3);
    camera[4] = solveCam.inv()(1, 3);
    camera[5] = solveCam.inv()(2, 3);
  }

  // Create problem
  Vector<ceres::ResidualBlockId> ptBlockIds;
  ptBlockIds.reserve(observedCameraRay.size());
  ceres::Problem problem;
  for (int i = 0; i < observedCameraRay.size(); ++i) {
    const auto &solvePt = solvePts[i];
    const auto &ray = observedCameraRay[i];
    if (settings().useNewPoseEstimationAnalytic) {
      ceres::LossFunction *lossFunction = nullptr;
      if (params.suppressOutliers) {
        lossFunction = new ceres::ComposedLoss(
          new ceres::CauchyLoss(settings().poseEstCauchyLoss),
          ceres::TAKE_OWNERSHIP,
          new ceres::HuberLoss(settings().poseEstHuberLossComposed),
          ceres::TAKE_OWNERSHIP);
      }
      ptBlockIds.push_back(problem.AddResidualBlock(
        new analytic::PoseEstimation(solvePt.x(), solvePt.y(), solvePt.z(), ray, params.scale),
        lossFunction,
        camera));
    } else {
      ptBlockIds.push_back(problem.AddResidualBlock(
        new PoseEstimationAnalytic(solvePt.x(), solvePt.y(), solvePt.z(), ray, params.scale),
        params.suppressOutliers ? new ceres::CauchyLoss(settings().poseEstCauchyLoss) : nullptr,
        camera));
    }
  }

  Vector<ceres::ResidualBlockId> camBlockIds;
  if (params.useImageTargetCostFunction) {
    // Encourage smaller camera position movements
    // RotationTarget + PositionTargetAnalytic are more correct here, as PositionTarget optimizes
    // in model space as opposed to view space. But tuning them to get equivalent performance to
    // CameraMovement proved difficult. Values that get close:
    // - RotationTarget: ScaledLoss(ComposedLoss(CauchyLoss(0.01), ScaledLoss(2.0)), 1.0)
    // - PositionTarget: ScaledLoss(ComposedLoss(CauchyLoss(0.01), ScaledLoss(squared(0.05)), 0.5)
    if (params.cameraMotionWeight > 0.f) {
      ceres::CostFunction *cost_function = new ceres::AutoDiffCostFunction<
        CameraMovement,
        BundleDesc::N_CAMERA_PARAMS,
        BundleDesc::N_CAMERA_PARAMS>(
        new CameraMovement(camera, 0.02 * params.cameraMotionWeight, 10.0));
      camBlockIds.push_back(
        problem.AddResidualBlock(cost_function, new ceres::CauchyLoss(0.01), camera));
    }
  } else if (settings().rotationWt > 0.0 || settings().positionWt > 0.0) {
    // Encourage camera positions close to the initial guess.
    // reasonable rotation weight values: (r: 0.1, w: 1), (r: 0.2, w: 1), (r: 0.5, w: 0.2), (r: 0.5,
    // w: 0.5)
    if (settings().rotationWt > 0.0) {
      ceres::CostFunction *rotation_cost =
        new ceres::AutoDiffCostFunction<RotationTarget, 3, BundleDesc::N_CAMERA_PARAMS>(
          new RotationTarget({camera[0], camera[1], camera[2]}));
      camBlockIds.push_back(problem.AddResidualBlock(
        rotation_cost,
        new ceres::ScaledLoss(
          new ceres::CauchyLoss(squared(settings().rotationRobust)),
          settings().rotationWt,
          ceres::TAKE_OWNERSHIP),
        camera));
    }
    if (settings().positionWt > 0.0) {
      // Since we pre-transformed the camera position, the residual is the distance from 0. This
      // distance will be the same in world space and view space regardless of camera rotation.
      ceres::CostFunction *position_cost =
        new ceres::AutoDiffCostFunction<ZeroTarget, 3, BundleDesc::N_CAMERA_PARAMS>(
          new ZeroTarget(3));
      camBlockIds.push_back(problem.AddResidualBlock(
        position_cost,
        new ceres::ScaledLoss(
          new ceres::CauchyLoss(squared(settings().positionRobust)),
          settings().positionWt,
          ceres::TAKE_OWNERSHIP),
        camera));
    }
  } else {
    // Penalize high-frequency translational updates.
    if (lastTwoCameraExtrinsics.size() == 2) {
      auto c1 = translationDouble(lastTwoCameraExtrinsics[0]);
      auto c2 = translationDouble(lastTwoCameraExtrinsics[1]);
      ceres::CostFunction *stability_cost_function = new ceres::
        AutoDiffCostFunction<PoseEstimationTranslationStability, 3, BundleDesc::N_CAMERA_PARAMS>(
          new PoseEstimationTranslationStability(c1, c2));
      camBlockIds.push_back(
        problem.AddResidualBlock(stability_cost_function, new ceres::CauchyLoss(7e-3), camera));
    }

    // Encourage smaller camera position movements
    // Speed up convergence and performance tremendously. Only work on small movements of course.
    // Watch out when you use poseEstimation on a bigger baseline
    ceres::CostFunction *cost_function = new ceres::AutoDiffCostFunction<
      CameraMovement,
      BundleDesc::N_CAMERA_PARAMS,
      BundleDesc::N_CAMERA_PARAMS>(new CameraMovement(camera, 0.07, 10.0));
    camBlockIds.push_back(
      problem.AddResidualBlock(cost_function, new ceres::CauchyLoss(0.03), camera));
  }

  ceres::Solver::Options options;

  // Result on 3/9/2021
  // For bundle adjustment with 8 cameras and 300 points
  //   With DENSE_NORMAL_CHOLESKY (default trust region strat is LM)
  // BundleBenchmarkTest/poseEstimationAnalytical   10082874 ns   10054597 ns         67
  // BundleBenchmarkTest/poseEstimationAnalytical   13612838 ns   13580191 ns         47
  // BundleBenchmarkTest/poseEstimationAnalytical    9537387 ns    9514671 ns         70
  // BundleBenchmarkTest/poseEstimationAnalytical    9914354 ns    9890600 ns         65
  //   With DENSE_NORMAL_CHOLESKY & trust_region_strategy_type = DOGLEG
  // BundleBenchmarkTest/poseEstimationAnalytical   12552601 ns   12510720 ns         50
  // BundleBenchmarkTest/poseEstimationAnalytical   13036353 ns   13005154 ns         52
  // BundleBenchmarkTest/poseEstimationAnalytical   12833301 ns   12808096 ns         52
  //   With DENSE_QR
  // BundleBenchmarkTest/poseEstimationAnalytical   16831174 ns   16724364 ns         44
  // BundleBenchmarkTest/poseEstimationAnalytical   14786001 ns   14754174 ns         46
  // BundleBenchmarkTest/poseEstimationAnalytical   13818333 ns   13780755 ns         49
  //   With DENSE_SCHUR
  // BundleBenchmarkTest/poseEstimationAnalytical   16073180 ns   16023093 ns         43
  // BundleBenchmarkTest/poseEstimationAnalytical   20974456 ns   20924471 ns         34
  // BundleBenchmarkTest/poseEstimationAnalytical   10132048 ns   10095130 ns         69
  //   With ITERATIVE_SCHUR
  // BundleBenchmarkTest/poseEstimationAnalytical   11762680 ns   11739491 ns         57
  // BundleBenchmarkTest/poseEstimationAnalytical    5468377 ns    5453479 ns        119
  // BundleBenchmarkTest/poseEstimationAnalytical   16287866 ns   16253390 ns         41

  options.linear_solver_type = ceres::DENSE_NORMAL_CHOLESKY;
  options.max_num_iterations = settings().poseEstMaxNumIterations;
  options.function_tolerance = settings().poseEstFunctionTolerance;
  options.gradient_tolerance = settings().poseEstGradientTolerance;
  options.parameter_tolerance = settings().poseEstParameterTolerance;
  options.max_solver_time_in_seconds = 1.0;  // TODO(dat): Change to be based on nPoints
  options.logging_type = ceres::PER_MINIMIZER_ITERATION;
  options.minimizer_progress_to_stdout = true;

  if (!params.logReport) {
    options.logging_type = ceres::SILENT;
    options.minimizer_progress_to_stdout = false;
  }

  ceres::Solver::Summary summary;
  ceres::Solve(options, &problem, &summary);

  if (params.logReport) {
    C8Log("Summary %s", summary.FullReport().c_str());
  }

  if (summary.IsSolutionUsable()) {
    *cameraExtrinsics = postTransform * cameraParamsToCamMotion(camera);
  }

  if (residualOutput) {
    residualOutput->clear();

    ceres::Problem::EvaluateOptions options;
    options.residual_blocks = ptBlockIds;
    Vector<double> pointResiduals;
    problem.Evaluate(options, NULL, &pointResiduals, NULL, NULL);

    options.residual_blocks = camBlockIds;
    problem.Evaluate(options, NULL, &residualOutput->camResiduals, NULL, NULL);

    for (int i = 0; i < ptBlockIds.size() * 2; i += 2) {
      // Each residual is the scaled and weighted delta of the distance between the match's
      // x and y components in ray space.
      // (scale * pt.x / pt.z - (scale * u)) * weight,
      // (scale * pt.y / pt.z - (scale * v)) * weight,
      residualOutput->pointResidualsX.push_back(pointResiduals[i]);
      residualOutput->pointResidualsY.push_back(pointResiduals[i + 1]);
      // Using the distance function to represent a combined residual.
      residualOutput->pointResiduals.push_back(
        std::sqrt(
          std::pow(residualOutput->pointResidualsX.back(), 2)
          + std::pow(residualOutput->pointResidualsY.back(), 2)));
    }
  }
  return summary.IsSolutionUsable();
}

bool poseEstimationImageTarget(
  const Vector<HPoint2> &camRays,
  const Vector<HPoint2> &targetRays,
  const Vector<float> &weights,
  float cauchyLoss,
  float cameraMotionWeight,
  HMatrix *cameraExtrinsics,
  bool logReport) {
  double camera[BundleDesc::N_CAMERA_PARAMS];
  {
    // Populate existing camera parameters
    auto camEst = cameraExtrinsics->inv();

    // RotationMatrixToAngleAxis assumes column major data.
    double rot[9];
    int k = 0;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        rot[k++] = camEst(j, i);
      }
    }
    ceres::RotationMatrixToAngleAxis(rot, camera);
    camera[3] = camEst(0, 3);
    camera[4] = camEst(1, 3);
    camera[5] = camEst(2, 3);
  }

  // Create problem
  ceres::Problem problem;
  for (int i = 0; i < camRays.size(); ++i) {
    auto camRay = camRays[i];
    auto targetRay = targetRays[i];
    float weight = weights.empty() ? 1.0f : weights[i];
    // Uncomment to use the AutoDiff version
    // ceres::CostFunction *cost_function = new ceres::
    //   AutoDiffCostFunction<InversePoseEstimationImageTarget, 2, BundleDesc::N_CAMERA_PARAMS>(
    //     new InversePoseEstimationImageTarget(
    //       camRay.x(), camRay.y(), targetRay.x(), targetRay.y(), weight));
    ceres::CostFunction *cost_function = new InversePoseEstimationImageTargetAnalytic(
      camRay.x(), camRay.y(), targetRay.x(), targetRay.y(), weight);
    problem.AddResidualBlock(
      cost_function, cauchyLoss > 0.0f ? new ceres::CauchyLoss(cauchyLoss) : nullptr, camera);
  }

  // Encourage smaller camera position movements
  // Speed up convergence and performance tremendously. Only work on small movements of course.
  // Watch out when you use poseEstimation on a bigger baseline

  if (cameraMotionWeight > 0) {
    ceres::CostFunction *cost_function = new ceres::AutoDiffCostFunction<
      CameraMovement,
      BundleDesc::N_CAMERA_PARAMS,
      BundleDesc::N_CAMERA_PARAMS>(new CameraMovement(camera, 0.02 * cameraMotionWeight, 10.0));
    problem.AddResidualBlock(cost_function, new ceres::CauchyLoss(0.01), camera);
  }

  ceres::Solver::Options options;
  options.linear_solver_type = ceres::DENSE_NORMAL_CHOLESKY;
  options.max_num_iterations = 100;
  options.function_tolerance = 1e-10;
  options.gradient_tolerance = 1e-10;
  options.parameter_tolerance = 1e-6;
  options.max_solver_time_in_seconds = 1.0;  // TODO(dat): Change to be based on nPoints
  options.logging_type = ceres::PER_MINIMIZER_ITERATION;
  options.minimizer_progress_to_stdout = true;

  if (!logReport) {
    options.logging_type = ceres::SILENT;
    options.minimizer_progress_to_stdout = false;
  }

  ceres::Solver::Summary summary;
  ceres::Solve(options, &problem, &summary);

  if (logReport) {
    C8Log("Summary %s", summary.FullReport().c_str());
  }

  if (summary.IsSolutionUsable()) {
    *cameraExtrinsics = paramsToCamera(camera).inv();
  }
  return summary.IsSolutionUsable();
}

bool poseEstimationFull3d(
  const Vector<HPoint3> &observedPoints,
  const Vector<HPoint3> &modelPoints,
  HMatrix *modelTransform) {
  // Convert HMatrix to angle-axis + translation params.
  double camera[BundleDesc::N_CAMERA_PARAMS];
  {
    // RotationMatrixToAngleAxis assumes column major data.
    double rot[9];
    int k = 0;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        rot[k++] = (*modelTransform)(j, i);
      }
    }
    ceres::RotationMatrixToAngleAxis(rot, camera);
    camera[3] = (*modelTransform)(0, 3);
    camera[4] = (*modelTransform)(1, 3);
    camera[5] = (*modelTransform)(2, 3);
  }

  // Create problem
  ceres::Problem problem;
  for (int i = 0; i < observedPoints.size(); ++i) {
    auto o = observedPoints[i];
    auto m = modelPoints[i];
    ceres::CostFunction *cost_function =
      new ceres::AutoDiffCostFunction<Full3dPoseEstimation, 3, BundleDesc::N_CAMERA_PARAMS>(
        new Full3dPoseEstimation(m.x(), m.y(), m.z(), o.x(), o.y(), o.z()));

    // NOTE(nb): normally we would add a robust loss function here, but it doesn't seem needed
    // since face mesh points are very constrained.
    problem.AddResidualBlock(cost_function, nullptr, camera);
  }

  ceres::Solver::Options options;
  options.linear_solver_type = ceres::DENSE_NORMAL_CHOLESKY;
  options.max_num_iterations = 100;
  options.function_tolerance = 1e-10;
  options.gradient_tolerance = 1e-10;
  options.parameter_tolerance = 1e-6;
  options.max_solver_time_in_seconds = 0.2;  // TODO(dat): Change to be based on nPoints
  options.logging_type = ceres::PER_MINIMIZER_ITERATION;
  options.minimizer_progress_to_stdout = false;

  ceres::Solver::Summary summary;
  ceres::Solve(options, &problem, &summary);

  if (summary.IsSolutionUsable()) {
    *modelTransform = paramsToCamera(camera);
  }
  return summary.IsSolutionUsable();
}

bool poseEstimationScale3d(
  const Vector<HPoint3> &observedPoints, const Vector<HPoint3> &modelPoints, float *scale_) {
  if (observedPoints.empty() || modelPoints.empty()) {
    return false;
  }
  double scale[1];
  scale[0] = *scale_;

  // Create problem
  ceres::Problem problem;
  for (int i = 0; i < observedPoints.size(); ++i) {
    auto o = observedPoints[i];
    auto m = modelPoints[i];
    ceres::CostFunction *cost_function = new ceres::AutoDiffCostFunction<
      Scale3dPoseEstimation,
      3,  // x,y,z residuals
      1   // scale
      >(new Scale3dPoseEstimation(m.x(), m.y(), m.z(), o.x(), o.y(), o.z()));

    problem.AddResidualBlock(cost_function, new ceres::CauchyLoss(1e-4f), scale);
  }

  ceres::Solver::Options options;
  options.linear_solver_type = ceres::DENSE_NORMAL_CHOLESKY;
  options.max_num_iterations = 100;
  options.function_tolerance = 1e-10;
  options.gradient_tolerance = 1e-10;
  options.parameter_tolerance = 1e-6;
  options.max_solver_time_in_seconds = 0.2;
  options.logging_type = ceres::SILENT;
  options.minimizer_progress_to_stdout = false;

  ceres::Solver::Summary summary;
  ceres::Solve(options, &problem, &summary);

  if (summary.IsSolutionUsable()) {
    *scale_ = scale[0];
  }
  return summary.IsSolutionUsable();
}

bool poseEstimationScale1d(
  const Vector<float> &vioVals,
  const Vector<float> &imuVals,
  float *scale,
  float *accelBiasInWorld) {
  if (vioVals.empty() || imuVals.empty()) {
    return false;
  }
  double params[2];
  params[0] = *scale;
  params[1] = *accelBiasInWorld;

  // Create problem
  ceres::Problem problem;
  for (int i = 0; i < vioVals.size(); ++i) {
    ceres::CostFunction *cost_function = new ceres::AutoDiffCostFunction<
      Scale1dPoseEstimation,
      1,  // residual
      2   // scale, bias
      >(new Scale1dPoseEstimation(vioVals[i], imuVals[i]));

    problem.AddResidualBlock(cost_function, new ceres::CauchyLoss(1e-3f), params);
  }

  ceres::Solver::Options options;
  options.linear_solver_type = ceres::DENSE_NORMAL_CHOLESKY;
  options.max_num_iterations = 100;
  options.function_tolerance = 1e-10;
  options.gradient_tolerance = 1e-10;
  options.parameter_tolerance = 1e-6;
  options.max_solver_time_in_seconds = 0.2;
  options.logging_type = ceres::SILENT;
  options.minimizer_progress_to_stdout = false;

  ceres::Solver::Summary summary;
  ceres::Solve(options, &problem, &summary);

  if (summary.IsSolutionUsable()) {
    *scale = params[0];
    *accelBiasInWorld = params[1];
  }
  return summary.IsSolutionUsable();
}

}  // namespace c8
