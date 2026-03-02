// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":bundle",
    "//c8/camera:device-infos",
    "//c8/geometry:intrinsics",
    "@com_google_benchmark//:benchmark",
  };
}
cc_end(0x95c815c2);

#include <benchmark/benchmark.h>

#include "c8/c8-log.h"
#include "c8/camera/device-infos.h"
#include "c8/geometry/intrinsics.h"
#include "c8/stats/scope-timer.h"
#include "ceres/ceres.h"
#include "ceres/rotation.h"
#include "reality/engine/geometry/bundle-residual.h"
#include "reality/engine/geometry/bundle.h"

namespace c8 {
float rand() { return 2.0f * ((std::rand() % 100001) / 100000.0f) - 1.0f; }

class BundleBenchmarkTest : public benchmark::Fixture {
protected:
  void SetUp(const ::benchmark::State &state) override {
    std::srand(10000000);
    answers.clear();
    cameraAnswers.clear();
    observationRays.clear();
    obsToWorldPtIndex.clear();
    worldPts.clear();
    obsToCameraIndex.clear();
    lastTwoCameraExtrinsics.clear();
    cameraExtrinsics.clear();
    cameraFixed.clear();

    // generate data ****

    float maxDistance = 1.0f;  // max distance from answer to estimated world poinrt

    // Previous camera extrinsics. Order is prevCam1 -> prev.
    // We choose these values so that acceleration is zero when cam1 is solved for.
    auto prevCam1 =
      HMatrixGen::translation(3.0, 3.0, 3.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
    auto prevCam2 =
      HMatrixGen::translation(4.0, 4.0, 3.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
    lastTwoCameraExtrinsics.push_back(prevCam1);
    lastTwoCameraExtrinsics.push_back(prevCam2);

    auto cam1 = HMatrixGen::translation(5.0, 5.0, 3.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
    auto cam2 = HMatrixGen::translation(7.0, 6.0, 0.0) * HMatrixGen::rotationD(23.0, 199.0, -25.0);
    cameraAnswers.push_back(cam1);
    cameraAnswers.push_back(cam2);

    for (int i = 0; i < nPoints; ++i) {
      auto answer = HPoint3(rand(), rand(), -10 + 5 * rand());
      answers.push_back(answer);

      // Add noise for inferred world point position
      worldPts.push_back(HPoint3(
        answer.x() + rand() * maxDistance,
        answer.y() + rand() * maxDistance,
        answer.z() + rand() * maxDistance));

      // Create camera observations for world point
      for (int j = 0; j < cameraAnswers.size(); ++j) {
        auto &camera = cameraAnswers[j];
        // This is "true" observed location
        ObservedPoint obs;
        obs.position = (camera.inv() * answer).flatten();
        obs.scale = 0;
        obs.descriptorDist = 0.0f;
        observationRays.push_back(obs);
        obsToWorldPtIndex.push_back(i);
        obsToCameraIndex.push_back(j);
      }
    }
    // Add noise to camera extrinsics
    for (int j = 0; j < cameraAnswers.size(); ++j) {
      if (j == 0) {
        // First camera is fixed
        cameraFixed.push_back(1);
        cameraExtrinsics.push_back(cameraAnswers[j]);
        continue;
      }
      cameraFixed.push_back(0);
      auto r = HMatrixGen::rotationD(rand(), rand(), rand());
      cameraExtrinsics.push_back(r.t() * cameraAnswers[j] * r);
    }
    // end generate data
  }

  // Ground truth
  Vector<HPoint3> answers;
  Vector<HMatrix> cameraAnswers;

  // Data used by bundle adjustment
  Vector<ObservedPoint> observationRays;
  Vector<int> obsToWorldPtIndex;
  Vector<HPoint3> worldPts;
  Vector<int> obsToCameraIndex;
  std::deque<HMatrix> lastTwoCameraExtrinsics;
  Vector<HMatrix> cameraExtrinsics;
  Vector<int> cameraFixed;

  // In real benchmarks, the nPoints is around 50
  int nPoints = 50;
};  // namespace c8

bool SHOW_SUMMARY = false;  // set to see iterations of optimization

BENCHMARK_F(BundleBenchmarkTest, poseEstimationAnalytical)(benchmark::State &state) {
  ScopeTimer rt("test");
  for (auto _ : state) {
    // Get data for the first camera
    Vector<ObservedPoint> observationRays(
      this->observationRays.begin(), this->observationRays.begin() + nPoints);
    Vector<HPoint3> worldPts = this->worldPts;
    HMatrix outputCam = HMatrixGen::i();

    poseEstimationAnalytical(
      observationRays, worldPts, lastTwoCameraExtrinsics, &outputCam, {false, 1.0, false});
  }
}

BENCHMARK_F(BundleBenchmarkTest, poseEstimationAnalyticalWithResiduals)(benchmark::State &state) {
  ScopeTimer rt("test");
  PoseEstimationResidualOutput residualOutput;
  for (auto _ : state) {
    // Get data for the first camera
    Vector<ObservedPoint> observationRays(
      this->observationRays.begin(), this->observationRays.begin() + nPoints);
    Vector<HPoint3> worldPts = this->worldPts;
    HMatrix outputCam = HMatrixGen::i();

    poseEstimationAnalytical(
      observationRays,
      worldPts,
      lastTwoCameraExtrinsics,
      &outputCam,
      {false, 1.0, false},
      &residualOutput);
  }
}

BENCHMARK_F(BundleBenchmarkTest, poseEstimateCurvyTargetSpace)(benchmark::State &state) {
  ScopeTimer rt("test");
  HMatrix cameraFromCylinder =
    HMatrixGen::translation(0.0f, 0.0f, -4.0f) * HMatrixGen::rotationD(0.0f, 7.0f, 0.0f);

  CurvyImageGeometry curvyGeom;
  CurvyImageGeometry fullGeom;
  curvyForTarget(480, 617, {0.3269}, &curvyGeom, &fullGeom);

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
    {139.0f, 407.0f},
    {100.0f, 300.0f}};

  // convert to rays
  auto targetRays = flatten<2>(intrinsics.inv() * extrude<3>(targetPixels));

  // map to local space
  Vector<HPoint3> modelSpacePoints;
  mapToGeometryPoints(curvyGeom, targetPixels, &modelSpacePoints);

  // put from local space to egocentric camera space
  Vector<HPoint2> searchRays = flatten<2>(cameraFromCylinder.inv() * modelSpacePoints);

  for (auto _ : state) {
    HMatrix recoveredCamera =
      HMatrixGen::translation(0.0f + rand() * 1.1f, 0.0f + rand() * 1.1f, -4.0f + rand() * 1.1f)
      * HMatrixGen::rotationD(0.0f + rand() * 5.1f, 7.0f + rand() * 5.1f, 0.0f + rand() * 5.1f);
    float residualError;

    poseEstimateCurvyTargetSpace(
      targetRays, searchRays, curvyGeom, K, 100000.0f, &recoveredCamera, &residualError, false);
  }
}
}  // namespace c8
