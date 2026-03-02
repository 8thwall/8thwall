// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "//c8:c8-log",
    "//c8:random-numbers",
    "//c8/geometry:egomotion",
    "//c8/geometry:intrinsics",
    "//c8/geometry:worlds",
    "//reality/engine/geometry:bundle",
    "//reality/engine/geometry:bundle-residual",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xe3500190);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/c8-log.h"
#include "c8/camera/device-infos.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/intrinsics.h"
#include "c8/geometry/worlds.h"
#include "c8/random-numbers.h"
#include "c8/stats/scope-timer.h"
#include "reality/engine/geometry/bundle-residual.h"
#include "reality/engine/geometry/bundle.h"

using testing::Eq;
using testing::FloatNear;
using testing::Pointwise;

namespace c8 {
// return a float between -1 and 1
float rand() { return 2.0f * ((std::rand() % 100001) / 100000.0f) - 1.0f; }

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsMatrix(const HMatrix &matrix, float threshold = 1e-6) {
  return Pointwise(AreWithin(threshold), matrix.data());
}

float computeAbsoluteError(const Vector<HPoint3> &groundTruthPts, const Vector<HPoint3> worldPts) {
  float err = 0.0f;
  for (int i = 0; i < groundTruthPts.size(); ++i) {
    auto &groundTruth = groundTruthPts[i];
    auto &worldPt = worldPts[i];
    auto dx = groundTruth.x() - worldPt.x();
    auto dy = groundTruth.y() - worldPt.y();
    auto dz = groundTruth.z() - worldPt.z();
    auto e = dx * dx + dy * dy + dz * dz;
    err += e;
  }
  return err / groundTruthPts.size();
}

float computeCameraError(
  const Vector<HPoint3> &groundTruthPts,
  const Vector<HMatrix> &groundTruthCams,
  const Vector<HMatrix> &cameraExtrinsics) {
  float err = 0.0f;
  for (int j = 0; j < groundTruthCams.size(); ++j) {
    auto &cam = groundTruthCams[j];
    auto &est = cameraExtrinsics[j];
    for (int i = 0; i < groundTruthPts.size(); ++i) {
      auto &answer = groundTruthPts[i];
      auto ansPt = (cam.inv() * answer).flatten();
      auto estPt = (est.inv() * answer).flatten();
      auto dx = ansPt.x() - estPt.x();
      auto dy = ansPt.y() - estPt.y();
      auto e = dx * dx + dy * dy;
      err += e;
    }
  }
  return err / groundTruthPts.size() / groundTruthCams.size();
}

Vector<ObservedPoint> observePoints(const Vector<HPoint3> &pts, const HMatrix &camera, float wt) {
  Vector<ObservedPoint> observationRays;
  observationRays.reserve(pts.size());
  for (auto pt : pts) {
    // No distortion in our observation
    ObservedPoint obs;
    obs.position = (camera.inv() * pt).flatten();
    obs.scale = 0;
    obs.descriptorDist = 0.0f;
    obs.weight = wt;
    observationRays.push_back(obs);
  }
  return observationRays;
}

float computeReprojectionError(
  const Vector<HPoint3> worldPts, const Vector<ObservedPoint> obsRays, const HMatrix &cam) {
  float err = 0.0f;
  for (int j = 0; j < worldPts.size(); ++j) {
    auto &pt3d = worldPts[j];
    auto &pt2 = obsRays[j].position;

    auto estPt = (cam.inv() * pt3d).flatten();
    auto dx = estPt.x() - pt2.x();
    auto dy = estPt.y() - pt2.y();
    auto e = dx * dx + dy * dy;
    err += e;
  }
  return err / worldPts.size();
}

float computeReprojectionError(
  const Vector<HPoint3> worldPts, const Vector<HPoint2> obsRays, const HMatrix &cam) {
  float err = 0.0f;
  for (int j = 0; j < worldPts.size(); ++j) {
    auto &pt3d = worldPts[j];
    auto &pt2 = obsRays[j];

    auto estPt = (cam.inv() * pt3d).flatten();
    auto dx = estPt.x() - pt2.x();
    auto dy = estPt.y() - pt2.y();
    auto e = dx * dx + dy * dy;
    err += e;
  }
  return err / worldPts.size();
}

class BundleTest : public ::testing::Test {
protected:
  void SetUp() override {
    std::srand(10000000);
    groundTruthPts.clear();
    groundTruthCams.clear();
    groundTruthObsRays.clear();
    groundTruthObsToPtIndex.clear();
    noisyPts.clear();
    groundTruthObsToCamIndex.clear();
    noisyCams.clear();
    cameraFixed.clear();

    // Number of points.
    const int nPoints = 100;
    // Max distance from ground truth point to noisy point along x, y, and z axes.
    const float maxDistance = 1.0f;

    // Create two ground truth cameras.
    auto cam1 = HMatrixGen::translation(5.0, 5.0, 3.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
    auto cam2 = HMatrixGen::translation(7.0, 6.0, 0.0) * HMatrixGen::rotationD(23.0, 199.0, -25.0);
    groundTruthCams.push_back(cam1);
    groundTruthCams.push_back(cam2);

    for (int i = 0; i < nPoints; ++i) {
      // Create ground truth point.
      auto groundTruthPt = HPoint3(rand(), rand(), -10 + 5 * rand());
      groundTruthPts.push_back(groundTruthPt);

      // Add noise to ground truth point.
      noisyPts.push_back(HPoint3(
        groundTruthPt.x() + rand() * maxDistance,
        groundTruthPt.y() + rand() * maxDistance,
        groundTruthPt.z() + rand() * maxDistance));

      // For each ground truth camera, observe the ground truth point.
      for (int j = 0; j < groundTruthCams.size(); ++j) {
        ObservedPoint obs;
        obs.position = (groundTruthCams[j].inv() * groundTruthPt).flatten();
        obs.scale = 0;
        obs.descriptorDist = 0.0f;
        obs.weight = 1.0f;
        groundTruthObsRays.push_back(obs);
        groundTruthObsToPtIndex.push_back(i);
        groundTruthObsToCamIndex.push_back(j);
      }
    }

    // For each ground truth camera other than the first, add noise to the rotation.
    for (int j = 0; j < groundTruthCams.size(); ++j) {
      if (j == 0) {
        // First camera is fixed
        cameraFixed.push_back(1);
        noisyCams.push_back(groundTruthCams[j]);
        continue;
      }
      cameraFixed.push_back(0);
      auto r = HMatrixGen::rotationD(rand(), rand(), rand());
      noisyCams.push_back(r.t() * groundTruthCams[j] * r);
    }

    // For the first ground truth camera we compute perfect and noisy observations of ground truth
    // points.
    for (int j = 0; j < groundTruthPts.size(); ++j) {
      // Perfect observation of the ground truth point by the ground truth cam.
      ObservedPoint groundTruth;
      groundTruth.scale = 0;
      groundTruth.descriptorDist = 0.0f;
      groundTruth.weight = 1.0f;
      groundTruth.position = (groundTruthCams[0].inv() * groundTruthPts[j]).flatten();
      firstCamGroundTruthObsRays.push_back(groundTruth);

      // Add noise to the ground truth observation's ray.
      ObservedPoint noisy;
      noisy.weight = 1.0f;
      noisy.scale = 0;
      noisy.descriptorDist = 0.0f;
      if (j % 10 != 0) {  // 10% of the points have random errors
        noisy.position = groundTruth.position;
      } else {
        noisy.position =
          HPoint2(groundTruth.position.x() + rand(), groundTruth.position.y() + rand());
      }
      firstCamNoisyObsRays.push_back(noisy);
      firstCamObsToPtIndex.push_back(j);
      firstCamObsToCamIndex.push_back(0);
    }
  }

  // The ground truth of the 100 points in world space.
  Vector<HPoint3> groundTruthPts;
  // The ground truth of 2 extrinsics in world space.
  Vector<HMatrix> groundTruthCams;

  // A noisy version of groundTruthPts.
  Vector<HPoint3> noisyPts;
  // A noisy version of groundTruthCams. The first camera has no noise added.
  Vector<HMatrix> noisyCams;
  // Whether each camera in noisyCams is fixed. Only the first camera is fixed.
  Vector<int> cameraFixed;

  // Rays from each camera in groundTruthCams to each point in groundTruthPts.
  Vector<ObservedPoint> groundTruthObsRays;
  // Indexes an observation in groundTruthObsRays to its point in groundTruthPts.
  // Size is 200, follows pattern [0, 0, 1, 1, 2, 2...]
  Vector<int> groundTruthObsToPtIndex;
  // Indexes an observation in groundTruthObsRays to its camera in groundTruthCams.
  // Size is 200, follows pattern [0, 1, 0, 1, 0, 1...]
  Vector<int> groundTruthObsToCamIndex;

  // Rays from the first camera in groundTruthCams to each point in groundTruthPts.
  Vector<ObservedPoint> firstCamGroundTruthObsRays;
  // Rays from the first camera in groundTruthCams to each point in groundTruthPts, with noise added
  // to the ray.
  Vector<ObservedPoint> firstCamNoisyObsRays;
  // Indexes a firstCam* observation in to its point in groundTruthPts.
  // Size 100 vector [0, 1, 2, 3, ..., 99] (because we have a single camera and 100 points).
  Vector<int> firstCamObsToPtIndex;
  // Indexes a firstCam* observation in to its camera in groundTruthCams.
  // Size 100 vector of zeroes (because all observations are to the first cam).
  Vector<int> firstCamObsToCamIndex;
};

TEST_F(BundleTest, BundleSchurOneDTranslation) {
  ScopeTimer rt("test");
  Vector<HPoint3> pointsOnPlane = Worlds::flatPlaneFromOriginWorld();
  HMatrix camera = HMatrixGen::translation(0.01f, 0.00f, 0.00f);
  auto observationRays = observePoints(pointsOnPlane, camera, 1e0);
  auto reconstructionCam = HMatrixGen::i();
  EXPECT_TRUE(poseEstimationSchur(observationRays, pointsOnPlane, 1.f, &reconstructionCam, false));
  EXPECT_NEAR(
    computeReprojectionError(pointsOnPlane, observationRays, reconstructionCam), 0.0, 1e-7);
  EXPECT_THAT(reconstructionCam.data(), equalsMatrix(camera));
}

TEST_F(BundleTest, BundleSchurTwoDTranslation) {
  ScopeTimer rt("test");
  Vector<HPoint3> pointsOnPlane = Worlds::flatPlaneFromOriginWorld();
  HMatrix camera = HMatrixGen::translation(0.01f, 0.01f, 0.00f);
  auto observationRays = observePoints(pointsOnPlane, camera, 1e0);
  auto reconstructionCam = HMatrixGen::i();
  EXPECT_TRUE(poseEstimationSchur(observationRays, pointsOnPlane, 1.f, &reconstructionCam, false));
  EXPECT_NEAR(
    computeReprojectionError(pointsOnPlane, observationRays, reconstructionCam), 0.0, 1e-7);
  EXPECT_THAT(reconstructionCam.data(), equalsMatrix(camera));
}

TEST_F(BundleTest, BundleSchurFullMotion) {
  ScopeTimer rt("test");
  Vector<HPoint3> pointsOnPlane = Worlds::flatPlaneFromOriginWorld();
  HMatrix camera =
    cameraMotion(HPoint3(0.01f, -0.01f, -0.02f), Quaternion(0.997196f, .02f, -.04f, .06f));
  auto observationRays = observePoints(pointsOnPlane, camera, 1e0);
  auto reconstructionCam = HMatrixGen::i();
  EXPECT_TRUE(poseEstimationSchur(observationRays, pointsOnPlane, 1.f, &reconstructionCam, false));
  EXPECT_NEAR(
    computeReprojectionError(pointsOnPlane, observationRays, reconstructionCam), 0.0, 1e-7);
  EXPECT_THAT(reconstructionCam.data(), equalsMatrix(camera, 5e-4));
}

TEST_F(BundleTest, SolveTwoDTranslation) {
  ScopeTimer rt("test");
  Vector<HPoint3> pointsOnPlane = Worlds::flatPlaneFromOriginWorld();
  // Order of extrinsics is cam1 -> cam2 -> camera.
  HMatrix cam1 =
    HMatrixGen::translation(-0.03f, -0.01f, 0.0f) * HMatrixGen::rotationD(50.f, 1.f, 90.f);
  HMatrix cam2 =
    HMatrixGen::translation(-0.01f, 0.0f, 0.0f) * HMatrixGen::rotationD(100.f, 360.f, 90.f);
  HMatrix camera = HMatrixGen::translation(0.01f, 0.01f, 0.00f);
  auto observationRays = observePoints(pointsOnPlane, camera, 1e4);
  auto reconstructionCam = HMatrixGen::i();
  EXPECT_TRUE(poseEstimationAnalytical(
    observationRays, pointsOnPlane, {cam1, cam2}, &reconstructionCam, {false, 1.0, false}));
  EXPECT_NEAR(
    computeReprojectionError(pointsOnPlane, observationRays, reconstructionCam), 0.0, 1e-7);
  EXPECT_THAT(reconstructionCam.data(), equalsMatrix(camera));
}

TEST_F(BundleTest, SolveFullMotion) {
  ScopeTimer rt("test");
  Vector<HPoint3> pointsOnPlane = Worlds::flatPlaneFromOriginWorld();
  // Order of extrinsics is cam1 -> cam2 -> camera.
  HMatrix camera =
    cameraMotion(HPoint3(0.01f, -0.01f, -0.02f), Quaternion(0.997196f, .02f, -.04f, .06f));
  HMatrix cam1 = HMatrixGen::translation(-1.0f, 0.0f, -20.0f) * camera;
  HMatrix cam2 = HMatrixGen::translation(-2.0f, 0.0f, -10.0f) * camera;
  auto observationRays = observePoints(pointsOnPlane, camera, 1e4);
  auto reconstructionCam = HMatrixGen::i();
  EXPECT_TRUE(poseEstimationAnalytical(
    observationRays, pointsOnPlane, {cam1, cam2}, &reconstructionCam, {false, 1.0, false}));
  EXPECT_NEAR(
    computeReprojectionError(pointsOnPlane, observationRays, reconstructionCam), 0.0, 1e-7);
  EXPECT_THAT(reconstructionCam.data(), equalsMatrix(camera, 3e-6));
}

TEST_F(BundleTest, BundleAnalyticalOneDTranslation) {
  ScopeTimer rt("test");
  Vector<HPoint3> pointsOnPlane = Worlds::flatPlaneFromOriginWorld();
  // Order of extrinsics is cam1 -> cam2 -> camera.
  HMatrix cam1 =
    HMatrixGen::translation(-2.0f, -0.01f, 0.0f) * HMatrixGen::rotationD(50.f, 1.f, 90.f);
  HMatrix cam2 =
    HMatrixGen::translation(-1.0f, 0.0f, 0.0f) * HMatrixGen::rotationD(100.f, 360.f, 90.f);
  HMatrix camera = HMatrixGen::translation(0.00f, 0.01f, 0.00f);
  auto observationRays = observePoints(pointsOnPlane, camera, 1e4);
  auto reconstructionCam = HMatrixGen::i();
  EXPECT_TRUE(poseEstimationAnalytical(
    observationRays, pointsOnPlane, {cam1, cam2}, &reconstructionCam, {false, 1.0, false}));
  EXPECT_NEAR(
    computeReprojectionError(pointsOnPlane, observationRays, reconstructionCam), 0.0, 1e-7);
  EXPECT_THAT(reconstructionCam.data(), equalsMatrix(camera));
}

TEST_F(BundleTest, SolveImageTargets) {
  ScopeTimer rt("test");
  Vector<HPoint3> pointsOnPlane = Worlds::flatPlaneFromOriginWorld();
  auto camA =
    cameraMotion(HPoint3(0.01f, -0.01f, -0.02f), Quaternion(0.997196f, .02f, -.04f, .06f));
  auto camB = updateWorldPosition(camA, camA);
  auto camRays = flatten<2>(camB.inv() * pointsOnPlane);
  auto targetRays = flatten<2>(pointsOnPlane);
  auto estCam = camA;
  EXPECT_TRUE(poseEstimationImageTarget(camRays, targetRays, {}, 1e-2f, 0, &estCam, false));
  auto estPts = flatten<2>(estCam.inv() * pointsOnPlane);
  for (int i = 0; i < estPts.size(); ++i) {
    EXPECT_NEAR(camRays[i].x(), estPts[i].x(), 6e-8);
    EXPECT_NEAR(camRays[i].y(), estPts[i].y(), 6e-8);
  }
  EXPECT_THAT(estCam.data(), equalsMatrix(camB, 2e-7));
}

TEST_F(BundleTest, SolveFull3d) {
  ScopeTimer rt("test");
  Vector<HPoint3> pointsOnFace = Worlds::sparseFaceMeshWorld();
  auto offset =
    cameraMotion(HPoint3(0.01f, -0.01f, -0.02f), Quaternion(0.997196f, .02f, -.04f, .06f));

  Vector<HPoint3> pointsObserved = offset * pointsOnFace;

  auto estCam = HMatrixGen::i();
  EXPECT_TRUE(poseEstimationFull3d(pointsObserved, pointsOnFace, &estCam));

  EXPECT_THAT(estCam.data(), equalsMatrix(offset, 7e-8));
}

TEST_F(BundleTest, SolveScale3dNoError) {
  ScopeTimer rt("test");
  Vector<HPoint3> pointsOnPlane = Worlds::flatPlaneFromOriginWorld();
  Vector<HPoint3> pointsObserved = pointsOnPlane;

  float scale = .5f;
  EXPECT_TRUE(poseEstimationScale3d(pointsObserved, pointsOnPlane, &scale));
  EXPECT_NEAR(scale, 1.f, 1e-4);
}

TEST_F(BundleTest, SolveScale3dScaleError) {
  ScopeTimer rt("test");
  Vector<HPoint3> pointsOnPlane = Worlds::flatPlaneFromOriginWorld();
  float scaleFactor = 1.0f / 0.9f;
  auto offset = HMatrixGen::scale(scaleFactor, scaleFactor, scaleFactor);

  Vector<HPoint3> pointsObserved = offset * pointsOnPlane;

  float scale = .5f;
  EXPECT_TRUE(poseEstimationScale3d(pointsObserved, pointsOnPlane, &scale));
  // pointsObserved are 1.1 times bigger than pointsOnPlane, so scale returned is .9
  EXPECT_NEAR(scale, .9f, 1e-4);
}

TEST_F(BundleTest, PoseEstimateCurvyTargetSpace) {
  ScopeTimer rt("test");

  HMatrix cameraFromCylinder =
    HMatrixGen::translation(0.0f, 0.0f, -4.0f) * HMatrixGen::rotationD(0.0f, 7.0f, 0.0f);

  HMatrix recoveredCamera =
    HMatrixGen::translation(0.0f + rand() * 1.1f, 0.0f + rand() * 1.1f, -4.0f + rand() * 1.1f)
    * HMatrixGen::rotationD(0.0f + rand() * 5.1f, 7.0f + rand() * 5.1f, 0.0f + rand() * 5.1f);

  CurvyImageGeometry curvyGeom;
  CurvyImageGeometry fullGeom;
  curvyForTarget(480, 617, {0.3269}, &curvyGeom, &fullGeom);

  c8_PixelPinholeCameraModel K = Intrinsics::rotateCropAndScaleIntrinsics(
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6), 480, 617);
  HMatrix intrinsics = HMatrixGen::intrinsic(K);

  // start with pixels
  Vector<HPoint2> targetPixels = {
    {239.0f, 307.0f},
    {240.0f, 307.0f},
    {139.0f, 207.0f},
    {340.0f, 207.0f},
    {340.0f, 407.0f},
    {139.0f, 407.0f},
    {100.0f, 300.0f}};

  // convert to rays
  auto targetRays = flatten<2>(intrinsics.inv() * extrude<3>(targetPixels));

  // map to local space
  Vector<HPoint3> modelSpacePoints;
  mapToGeometryPoints(curvyGeom, targetPixels, &modelSpacePoints);

  // put from local space to egocentric camera space
  Vector<HPoint2> searchRays = flatten<2>(cameraFromCylinder.inv() * modelSpacePoints);

  float residualError;
  bool logReport = false;
  bool success = poseEstimateCurvyTargetSpace(
    targetRays, searchRays, curvyGeom, K, 1e6, &recoveredCamera, &residualError, logReport);

  EXPECT_THAT(true, success);

  // the residual is around 1.14, which is high
  EXPECT_NEAR(residualError, 1.0f, 1.0f);

  EXPECT_THAT(recoveredCamera.data(), equalsMatrix(cameraFromCylinder, 2e-6));

  if (logReport) {
    C8Log("residualError: %f", residualError);
    C8Log("cameraFromCylinder:\n %s", cameraFromCylinder.toString().c_str());
    C8Log("recoveredCamera:\n %s", recoveredCamera.toString().c_str());
  }
}
}  // namespace c8
