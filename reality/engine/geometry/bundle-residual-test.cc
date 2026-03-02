// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "//c8/geometry:egomotion",
    "//c8/geometry:intrinsics",
    "//c8/geometry:worlds",
    "//reality/engine/geometry:bundle-residual",
    "//reality/engine/geometry:bundle-residual-analytic",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x172db773);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <random>

#include "c8/camera/device-infos.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/homography.h"
#include "c8/geometry/intrinsics.h"
#include "c8/geometry/worlds.h"
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

template <typename T>
void cameraToParams(const HMatrix &cam, T *params) {
  // extract results.
  auto t = translation(cam);
  auto r = rotationMat(cam);
  T rot[9];
  // By default, ceres uses column major for rotation matrices.
  int k = 0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      rot[k++] = r(j, i);
    }
  }
  ceres::RotationMatrixToAngleAxis(rot, params);
  params[3] = t.x();
  params[4] = t.y();
  params[5] = t.z();
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
  return cameraMotion(t, r);
}

// Fills in a double array with the camera translation.
std::array<double, 3> translationDouble(const HMatrix &cam) {
  return {
    static_cast<double>(cam(0, 3)), static_cast<double>(cam(1, 3)), static_cast<double>(cam(2, 3))};
}

}  // namespace
std::random_device rd;
auto randGen = std::bind(std::uniform_int_distribution<int>(-180, 180), std::mt19937(59));
auto randDouble = std::bind(std::uniform_real_distribution<double>(0, 10), std::mt19937(rd()));
class BundleResidualTest : public ::testing::Test {
protected:
  void SetUp() override {}

  HMatrix cam1 = HMatrixGen::translation(2.0, 3.0, 4.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  std::unique_ptr<ceres::CostFunction> autoFunc;
  std::unique_ptr<ceres::CostFunction> analyticFunc;
  std::vector<HPoint3> pt3d;
  std::vector<ObservedPoint> pt2d;
};

TEST_F(BundleResidualTest, testAnalyticalSameAsAuto) {
  HPoint3 pt((float)randGen(), (float)randGen(), (float)randGen());
  pt3d.push_back(pt);
  ObservedPoint obsPt;
  HPoint3 transformedPt = cam1.inv() * pt;
  EXPECT_LT(0, transformedPt.z());
  obsPt.position = transformedPt.flatten();
  obsPt.scale = 2.0f * randDouble();
  obsPt.descriptorDist = randDouble();
  pt2d.push_back(obsPt);

  // Setting up auto derivative problem
  autoFunc.reset(
    new ceres::AutoDiffCostFunction<ReprojectionResidual, 2, BundleDesc::N_CAMERA_PARAMS, 3>(
      new ReprojectionResidual(obsPt)));

  double *worldPts = new double[3]{pt.x(), pt.y(), pt.z()};

  // Setting up analytic derivative problem
  analyticFunc.reset(new PoseEstimationAnalytic(pt.x(), pt.y(), pt.z(), obsPt));

  // An estimated value of camera params
  HMatrix cam2 = HMatrixGen::translation(randDouble(), randDouble(), randDouble())
    * HMatrixGen::rotationD(randGen(), randGen(), randGen());
  double *cameraParams = new double[BundleDesc::N_CAMERA_PARAMS];
  double rotMat[9] = {
    cam2(0, 0),
    cam2(0, 1),
    cam2(0, 2),
    cam2(1, 0),
    cam2(1, 1),
    cam2(1, 2),
    cam2(2, 0),
    cam2(2, 1),
    cam2(2, 2)};
  ceres::RotationMatrixToAngleAxis(rotMat, cameraParams);
  cameraParams[3] = cam2.inv()(0, 3);
  cameraParams[4] = cam2.inv()(1, 3);
  cameraParams[5] = cam2.inv()(2, 3);

  double *funcParameters[2]{cameraParams, worldPts};

  double analyticalResidual[2];
  double analyticalFuncJacobian[2 * BundleDesc::N_CAMERA_PARAMS];
  double *analyticalFuncJacobians[1]{analyticalFuncJacobian};

  double autoFuncResidual[2];
  double autoFuncJacobian[2 * BundleDesc::N_CAMERA_PARAMS];
  double autoFuncJacobianWorldPt[2 * 3];
  double *autoFuncJacobians[2]{autoFuncJacobian, autoFuncJacobianWorldPt};

  analyticFunc->Evaluate(funcParameters, analyticalResidual, analyticalFuncJacobians);

  autoFunc->Evaluate(funcParameters, autoFuncResidual, autoFuncJacobians);

  EXPECT_FLOAT_EQ(analyticalResidual[0], autoFuncResidual[0]);
  EXPECT_FLOAT_EQ(analyticalResidual[1], autoFuncResidual[1]);

  static constexpr float COMPARE_EPSILON = 1e-7;
  EXPECT_THAT((float)analyticalFuncJacobian[0], FloatNear(autoFuncJacobian[0], COMPARE_EPSILON));
  EXPECT_THAT((float)analyticalFuncJacobian[1], FloatNear(autoFuncJacobian[1], COMPARE_EPSILON));
  EXPECT_THAT((float)analyticalFuncJacobian[2], FloatNear(autoFuncJacobian[2], COMPARE_EPSILON));
  EXPECT_THAT((float)analyticalFuncJacobian[3], FloatNear(autoFuncJacobian[3], COMPARE_EPSILON));
  EXPECT_THAT((float)analyticalFuncJacobian[4], FloatNear(autoFuncJacobian[4], COMPARE_EPSILON));
  EXPECT_THAT((float)analyticalFuncJacobian[5], FloatNear(autoFuncJacobian[5], COMPARE_EPSILON));

  EXPECT_THAT((float)analyticalFuncJacobian[6], FloatNear(autoFuncJacobian[6], COMPARE_EPSILON));
  EXPECT_THAT((float)analyticalFuncJacobian[7], FloatNear(autoFuncJacobian[7], COMPARE_EPSILON));
  EXPECT_THAT((float)analyticalFuncJacobian[8], FloatNear(autoFuncJacobian[8], COMPARE_EPSILON));
  EXPECT_THAT((float)analyticalFuncJacobian[9], FloatNear(autoFuncJacobian[9], COMPARE_EPSILON));
  EXPECT_THAT((float)analyticalFuncJacobian[10], FloatNear(autoFuncJacobian[10], COMPARE_EPSILON));
  EXPECT_THAT((float)analyticalFuncJacobian[11], FloatNear(autoFuncJacobian[11], COMPARE_EPSILON));
  // We don't care about the world point jacobian since pose estimation keeps world points fixed.

  delete[] worldPts;
  delete[] cameraParams;
}

// convert HMatrix to cam parameters. This is similar to what is used in SnavelyReprojetionResidual
// The conversion stores camEst.inv() parameters.
void hMatrixToCam(const HMatrix &camEst, double camera[6]) {
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

TEST_F(BundleResidualTest, SnavelyReprojectionResidual) {
  HMatrix cam = HMatrixGen::rotationD(25.f, 15.f, 0.f) * HMatrixGen::translation(1.f, 1.f, -2.f);
  double pt[3] = {1., 2., 3.};
  double camera[6];
  hMatrixToCam(cam, camera);
  HPoint3 ptAsHPoint{
    static_cast<float>(pt[0]), static_cast<float>(pt[1]), static_cast<float>(pt[2])};
  auto ptAsRayInCam = (cam.inv() * ptAsHPoint).flatten();
  // the point is in front of the camera
  EXPECT_GT(ptAsRayInCam.extrude().z(), 0);
  ObservedPoint obs{{ptAsRayInCam.x(), ptAsRayInCam.y()}, 0, 0.f, 1.f};

  double residuals[2];
  SnavelyReprojectionResidual res{obs};
  res(camera, pt, residuals);
  EXPECT_NEAR(residuals[0], 0, 1e-4);
  EXPECT_NEAR(residuals[1], 0, 1e-4);
}

TEST_F(BundleResidualTest, PoseEstimationTranslationStabilityOperator) {
  // Order of extrinsics is c1 -> c2 -> c3.
  auto c1InWorld = HMatrixGen::translation(1.0, 2.0, 2.0) * HMatrixGen::rotationD(50, 1, 90);
  auto c2InWorld = HMatrixGen::translation(1.0, 4.0, 4.0) * HMatrixGen::rotationD(100, 360, 90);
  auto c3InWorld = HMatrixGen::translation(1.0, 6.0, 7.0) * HMatrixGen::rotationD(200, 1, 90);

  // Get current extrinsic in camera space.
  double c3InCamera[BundleDesc::N_CAMERA_PARAMS];
  double rot[9] = {
    c3InWorld(0, 0),
    c3InWorld(0, 1),
    c3InWorld(0, 2),
    c3InWorld(1, 0),
    c3InWorld(1, 1),
    c3InWorld(1, 2),
    c3InWorld(2, 0),
    c3InWorld(2, 1),
    c3InWorld(2, 2)};
  ceres::RotationMatrixToAngleAxis(rot, c3InCamera);
  c3InCamera[3] = c3InWorld.inv()(0, 3);
  c3InCamera[4] = c3InWorld.inv()(1, 3);
  c3InCamera[5] = c3InWorld.inv()(2, 3);

  // Format previous extrinsics for cost function.
  auto c1InWorldDouble = translationDouble(c1InWorld);
  auto c2InWorldDouble = translationDouble(c2InWorld);

  // Evaluate cost function.
  std::array<double, 3> residual;
  PoseEstimationTranslationStability cost(c1InWorldDouble, c2InWorldDouble);
  EXPECT_TRUE(cost(c3InCamera, residual.data()));

  // Acceleration is (0, 0, 1) - note that rotation has no impact, only translation.
  // PoseEstimationTranslationStability uses a weight of .1, so check for (0, 0, .1).
  EXPECT_NEAR(residual[0], 0.0, 1e-7);
  EXPECT_NEAR(residual[1], 0.0, 1e-7);
  EXPECT_NEAR(residual[2], 0.1, 1e-7);
}

TEST_F(BundleResidualTest, testCurvyOptimizationTargetSpace) {
  HMatrix cameraFromCylinder = updateWorldPosition(
    updateWorldPosition(HMatrixGen::i(), HMatrixGen::rotationD(0.0f, 5.0f, 0.0f)),
    HMatrixGen::translation(0, 0, -4.0));

  CurvyImageGeometry curvyGeom;
  CurvyImageGeometry fullGeo;
  curvyForTarget(480, 617, {0.3269}, &curvyGeom, &fullGeo);

  auto K = Intrinsics::rotateCropAndScaleIntrinsics(
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6), 480, 617);
  HMatrix intrinsics = HMatrixGen::intrinsic(K);

  // start with pixels
  Vector<HPoint2> targetPixels = {
    {239.0f, 307.0f},
    {240.0f, 307.0f},
    {139.0f, 207.0f},
    {340.0f, 207.0f},
    {340.0f, 407.0f},
    {139.0f, 407.0f}};

  // convert to rays
  auto targetRays = flatten<2>(intrinsics.inv() * extrude<3>(targetPixels));

  // map to local space
  Vector<HPoint3> modelSpacePoints;
  Vector<HVector3> modelSpaceNormals;
  mapToGeometry(curvyGeom, targetPixels, &modelSpacePoints, &modelSpaceNormals);

  // put from local space to egocentric camera space
  Vector<HPoint2> searchRays = flatten<2>(cameraFromCylinder.inv() * modelSpacePoints);
  Vector<HVector3> searchNormals = cameraFromCylinder.inv() * modelSpaceNormals;

  // Test points for visibility against camera ray normals.  The cylinder is facing -z both in
  // model and world space.
  for (int i = 0; i < searchRays.size(); ++i) {
    HVector3 rayVector{searchRays[i].x(), searchRays[i].y(), -1.0f};
    EXPECT_TRUE(searchNormals[i].dot(rayVector) > 0);
  }

  double cameraParams[BundleDesc::N_CAMERA_PARAMS];
  cameraToParams(cameraFromCylinder, cameraParams);
  double residual[2] = {0.0, 0.0};

  // set the scale to 1.0f so as not to artificially boost the residual
  CurvyTargetReprojectionResidual cost(targetRays[0], searchRays[0], curvyGeom, K, 1.0f);
  EXPECT_TRUE(cost(cameraParams, residual));

  EXPECT_NEAR(residual[0], 0.0f, 1e-8);
  EXPECT_NEAR(residual[1], 0.0f, 1e-8);
}

TEST_F(BundleResidualTest, testEvaluateResidualsNearZero) {
  ObservedPoint pt;
  pt.position = {0.292468f, 0.172493f};
  pt.weight = 1.0;
  pt.descriptorDist = 0;
  pt.scale = 0;
  PoseEstimationAnalytic pea(0.200000, 0.150000, 1.000000, pt);
  double residuals[2];
  double *params[1];
  params[0] = new double[6]{-0.040037, 0.080072, -0.120112, -0.007069, 0.011835, 0.020247};
  EXPECT_TRUE(pea.Evaluate(params, residuals, nullptr));
  delete[] params[0];
  EXPECT_THAT(residuals[0], DoubleNear(0, 1e-6));
  EXPECT_THAT(residuals[1], DoubleNear(0, 1e-6));
}

TEST_F(BundleResidualTest, InversePoseEstimationImageTarget) {
  Vector<HPoint3> pointsOnPlane = Worlds::flatPlaneFromOriginWorld();
  auto camA =
    cameraMotion(HPoint3(0.01f, -0.01f, -0.02f), Quaternion(0.997196f, .02f, -.04f, .06f));
  auto camB = updateWorldPosition(camA, camA);
  auto camPts = flatten<2>(camB.inv() * pointsOnPlane);
  auto imPts = flatten<2>(pointsOnPlane);

  // Tests that points on the z=1 plane give the expected plane normal.
  auto normal = getScaledPlaneNormal({0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f});
  EXPECT_FLOAT_EQ(normal.x(), 0.0f);
  EXPECT_FLOAT_EQ(normal.y(), 0.0f);
  EXPECT_FLOAT_EQ(normal.z(), 1.0f);

  // Tests that points in the scene are on the same expected plane. Tests non-collinear subsets of
  // points by using prime numbers larger than 1x and 2x grid size.
  for (int i = 0; i < pointsOnPlane.size() - 23; ++i) {
    auto ptNormal =
      getScaledPlaneNormal(pointsOnPlane[i], pointsOnPlane[i + 13], pointsOnPlane[i + 23]);
    EXPECT_FLOAT_EQ(ptNormal.x(), normal.x());
    EXPECT_FLOAT_EQ(ptNormal.y(), normal.y());
    EXPECT_FLOAT_EQ(ptNormal.z(), normal.z());
  }

  // Tests that a normally constructed homography matrix using HMatrix and HPoint can achieve zero
  // residual.
  auto H = homographyForPlane(camB, normal);
  auto HI = H.inv();
  auto hpoints = flatten<2>(HI * extrude<3>(camPts));
  for (int i = 0; i < hpoints.size(); ++i) {
    EXPECT_NEAR(hpoints[i].x(), imPts[i].x(), 6e-8);
    EXPECT_NEAR(hpoints[i].y(), imPts[i].y(), 6e-8);
  }

  // Tests that cameraToParams and paramsToCamera are mirrors of each other.
  auto camBInv = camB.inv();
  float params[6];
  cameraToParams(camBInv, params);

  auto camBInvRecon = paramsToCamera(params);
  for (int i = 0; i < 16; ++i) {
    EXPECT_FLOAT_EQ(camBInvRecon.data()[i], camBInv.data()[i]);
  }

  // Tests that the inv33 method computes a valid matrix inverse.
  float H33[9];
  int k = 0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      H33[k++] = H(i, j);
    }
  }

  float H33I[9];
  inv33(H33, H33I);
  k = 0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      EXPECT_FLOAT_EQ(H33I[k++], HI(i, j));
    }
  }

  // Verify that the rotation matrix can be recovered from the parameters.
  float ph[9];
  auto rb = rotationMat(camBInv);
  ceres::AngleAxisToRotationMatrix(params, ceres::RowMajorAdapter3x3(ph));

  k = 0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      EXPECT_FLOAT_EQ(ph[k++], rb(i, j));
    }
  }

  // Verify that we can construct the expected homography matrix.
  ph[2] += params[3];
  ph[5] += params[4];
  ph[8] += params[5];

  float sqscale = 0.0f;
  for (int i = 0; i < 9; i++) {
    sqscale += ph[i] * ph[i];
  }
  float iscale = 1.0f / std::sqrt(sqscale);

  for (int i = 0; i < 9; ++i) {
    ph[i] *= iscale;
    EXPECT_FLOAT_EQ(ph[i], H33[i]) << "Element " << i;
  }

  // Verify that we can compute the inverted homography matrix.
  float m[9];
  inv33(ph, m);

  for (int i = 0; i < 9; ++i) {
    EXPECT_NEAR(m[i], H33I[i], 1e-6) << "Element " << i;
  }

  // Verify that the cost of the params is 0
  for (int i = 0; i < camPts.size(); ++i) {
    InversePoseEstimationImageTarget cost(
      camPts[i].x(), camPts[i].y(), imPts[i].x(), imPts[i].y(), 1.0f);
    float residual[2];
    EXPECT_TRUE(cost(params, residual));
    EXPECT_NEAR(residual[0], 0.0f, 6e-8);
    EXPECT_NEAR(residual[1], 0.0f, 6e-8);
  }
}

TEST_F(BundleResidualTest, YawOnlyPoseEstimation) {
  HPoint3 p3(3.0f, 1.0f, -2.0f);
  auto trueCam =
    HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto obs = (trueCam.inv() * p3).flatten();

  auto q = Quaternion::fromHMatrix(
    updateWorldPosition(HMatrixGen::yRadians(-0.1f), HMatrixGen::rotationD(12.0, 190.0, -15.0)));

  YawOnlyPoseEstimation cost(q.w(), q.x(), q.y(), q.z(), p3.x(), p3.y(), p3.z(), obs.x(), obs.y());
  double p[4] = {5.0, 5.0, 19.0, 0.1};
  double r[2];
  EXPECT_TRUE(cost(p, r));
  EXPECT_NEAR(r[0], 0.0, 7e-7);
  EXPECT_NEAR(r[1], 0.0, 7e-7);
}

TEST_F(BundleResidualTest, Full3dPoseEstimation) {
  // Create a point in a model space.
  HPoint3 mPt(3.0f, 1.0f, -2.0f);

  // Transform that point into a new space.
  auto cam = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto oPt = cam * mPt;

  // Propose camera parameters with the known transform.
  double params[6];
  cameraToParams(cam, params);

  // Construct a cost function from our data point.
  Full3dPoseEstimation cost(mPt.x(), mPt.y(), mPt.z(), oPt.x(), oPt.y(), oPt.z());

  // Compute the residual.
  double residual[3];
  EXPECT_TRUE(cost(params, residual));

  // Check that the residual is 0 on all dimensions.
  EXPECT_NEAR(residual[0], 0.0, 5e-7);
  EXPECT_NEAR(residual[1], 0.0, 5e-7);
  EXPECT_NEAR(residual[2], 0.0, 5e-7);
}

TEST_F(BundleResidualTest, Full3dWithScalePoseEstimationSimple) {
  // Create a point in a model space.
  HPoint3 mPt(3.0f, 1.0f, -2.0f);

  // Transform that point into a new space.
  auto cam = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto oPt = cam * mPt;

  // Propose camera parameters with the known transform.
  double params[BundleDesc::N_CAMERA_WITH_SCALE_PARAMS];
  cameraToParams(cam, params);
  params[6] = 1.0;

  // Construct a cost function from our data point.
  Full3dWithScalePoseEstimation cost(mPt.x(), mPt.y(), mPt.z(), oPt.x(), oPt.y(), oPt.z());

  // Compute the residual.
  double residual[3];
  EXPECT_TRUE(cost(params, residual));

  // Check that the residual is 0 on all dimensions.
  EXPECT_NEAR(residual[0], 0.0, 5e-7);
  EXPECT_NEAR(residual[1], 0.0, 5e-7);
  EXPECT_NEAR(residual[2], 0.0, 5e-7);
}

TEST_F(BundleResidualTest, Full3dWithScalePoseEstimation) {
  // Create a point in a model space.
  HPoint3 mPt(3.0f, 1.0f, -2.0f);

  // Transform that point into a new space.
  auto scale = 2.f;
  // We don't make scale part of cam b/c then we'll have to extract the rotational component
  // differently.
  auto cam = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto oPt = cam * HMatrixGen::scale(scale) * mPt;

  // Propose camera parameters with the known transform.
  double params[BundleDesc::N_CAMERA_WITH_SCALE_PARAMS];
  cameraToParams(cam, params);
  params[6] = scale;

  // Construct a cost function from our data point.
  Full3dWithScalePoseEstimation cost(mPt.x(), mPt.y(), mPt.z(), oPt.x(), oPt.y(), oPt.z());

  // Compute the residual.
  double residual[3];
  EXPECT_TRUE(cost(params, residual));

  // Check that the residual is 0 on all dimensions.
  EXPECT_NEAR(residual[0], 0.0, 2e-6);
  EXPECT_NEAR(residual[1], 0.0, 2e-6);
  EXPECT_NEAR(residual[2], 0.0, 2e-6);
}

}  // namespace c8
