// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)
//
// Unit tests for feature-set-executor.cc

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":feature-set-request-executor",
    "//c8/stats:scope-timer",
    "//reality/engine/api:reality.capnp-cc",
    "//reality/engine/api/response:features.capnp-cc",
    "//reality/engine/tracking:tracker",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x90b672c0);

#include "capnp/message.h"
#include "gtest/gtest.h"
#include "reality/engine/api/reality.capnp.h"
#include "reality/engine/api/response/features.capnp.h"
#include "reality/engine/features/feature-set-request-executor.h"
#include "reality/engine/tracking/tracker.h"

using MutableRealityRequest = c8::MutableRootMessage<c8::RealityRequest>;
using MutableResponseFeatureSet = c8::MutableRootMessage<c8::ResponseFeatureSet>;
using MutableResponsePose = c8::MutableRootMessage<c8::ResponsePose>;

namespace c8 {

static const Vector<HPoint3> MOCK_POINT_CLOUD{
  HPoint3(0.0f, 0.0f, 0.0f),
  HPoint3(1.0f, 0.0f, 0.0f),
  HPoint3(0.0f, 1.0f, 0.0f),
  HPoint3(0.0f, 0.0f, 1.0f),
  HPoint3(1.0f, 1.0f, 0.0f),
  HPoint3(1.0f, 0.0f, 1.0f),
  HPoint3(0.0f, 1.0f, 1.0f),
  HPoint3(1.0f, 1.0f, 1.0f)};

static constexpr float MOCK_POINT_CONFIDENCE = 0.8f;

namespace {

HPoint3 correctC8Position(HPoint3 pt, const ResponsePose::Reader &computedPose) {
  auto initRot = computedPose.getInitialTransform().getRotation();
  auto initPos = computedPose.getInitialTransform().getPosition();
  auto initRotQ = Quaternion(initRot.getW(), initRot.getX(), initRot.getY(), initRot.getZ());
  auto initPosV = HVector3(initPos.getX(), initPos.getY(), initPos.getZ());
  auto initCam = cameraMotion(initPosV, initRotQ);
  return initCam.inv() * pt;
}

HPoint3 correctARCorePosition(HPoint3 pt, const ResponsePose::Reader &computedPose) {
  auto initRot = computedPose.getInitialTransform().getRotation();
  auto initPos = computedPose.getInitialTransform().getPosition();
  auto initRotQ = Quaternion(initRot.getW(), initRot.getX(), initRot.getY(), initRot.getZ());
  auto initPosV = HVector3(initPos.getX(), initPos.getY(), initPos.getZ());
  auto initCam = cameraMotion(initPosV, initRotQ);
  return initCam.inv() * HPoint3(pt.x(), pt.y(), -pt.z());
}

HPoint3 correctARKitPosition(HPoint3 pt, const ResponsePose::Reader &computedPose) {
  auto initRot = computedPose.getInitialTransform().getRotation();
  auto initPos = computedPose.getInitialTransform().getPosition();
  auto initRotQ = Quaternion(initRot.getW(), initRot.getX(), initRot.getY(), initRot.getZ());
  auto initPosV = HVector3(initPos.getX(), initPos.getY(), initPos.getZ());
  auto initCam = cameraMotion(initPosV, initRotQ);
  auto initCamInv = initCam.inv();
  HMatrix invX{
    {-1.0f, 0.0f, 0.0f, 0.0f},
    {0.00f, 1.0f, 0.0f, 0.0f},
    {0.00f, 0.0f, 1.0f, 0.0f},
    {0.00f, 0.0f, 0.0f, 1.0f}};
  return initCamInv * invX * pt;
}

void mockComputedPose(ResponsePose::Builder *computedPose) {
  auto initRot = computedPose->getInitialTransform().getRotation();
  auto initPos = computedPose->getInitialTransform().getPosition();

  initRot.setW(1.0f);
  initRot.setX(4.0f);
  initRot.setY(2.0f);
  initRot.setZ(-5.0f);

  initPos.setX(10.0f);
  initPos.setY(-14.0f);
  initPos.setZ(1.51f);
}

void mockARCorePointCloud(RealityRequest::Builder *request) {
  auto pointCloudBuilder =
    request->getSensors().getARCore().initPointCloud(MOCK_POINT_CLOUD.size());
  for (int i = 0; i < MOCK_POINT_CLOUD.size(); ++i) {
    pointCloudBuilder[i].setId(i);
    pointCloudBuilder[i].setConfidence(MOCK_POINT_CONFIDENCE);
    pointCloudBuilder[i].getPosition().setX(MOCK_POINT_CLOUD[i].x());
    pointCloudBuilder[i].getPosition().setY(MOCK_POINT_CLOUD[i].y());
    pointCloudBuilder[i].getPosition().setZ(MOCK_POINT_CLOUD[i].z());
  }
}

void mockARKitPointCloud(RealityRequest::Builder *request) {
  auto pointCloudBuilder = request->getSensors().getARKit().initPointCloud(MOCK_POINT_CLOUD.size());
  for (int i = 0; i < MOCK_POINT_CLOUD.size(); ++i) {
    pointCloudBuilder[i].setId(i);
    pointCloudBuilder[i].getPosition().setX(MOCK_POINT_CLOUD[i].x());
    pointCloudBuilder[i].getPosition().setY(MOCK_POINT_CLOUD[i].y());
    pointCloudBuilder[i].getPosition().setZ(MOCK_POINT_CLOUD[i].z());
  }
}

void assertPositionsEqual(HPoint3 expected, HPoint3 actual) {
  EXPECT_EQ(expected.x(), actual.x());
  EXPECT_EQ(expected.y(), actual.y());
  EXPECT_EQ(expected.z(), actual.z());
}

class MockTracker : public Tracker {
public:
  virtual void getWorldPoints(ResponseFeatureSet::Builder *response) {
    auto featureSetBuilder = response->initPoints(MOCK_POINT_CLOUD.size());
    for (int i = 0; i < MOCK_POINT_CLOUD.size(); ++i) {
      auto &p = MOCK_POINT_CLOUD[i];
      featureSetBuilder[i].setId(i);
      featureSetBuilder[i].setConfidence(0.5f);
      featureSetBuilder[i].getPosition().setX(p.x());
      featureSetBuilder[i].getPosition().setY(p.y());
      featureSetBuilder[i].getPosition().setZ(p.z());
    }
  }
};

}  // namespace

class FeatureSetRequestExecutorTest : public ::testing::Test {};

TEST_F(FeatureSetRequestExecutorTest, TestARCorePointCloudConversion) {
  ScopeTimer rt("test");
  MutableRealityRequest requestMessage;
  MutableResponsePose poseMessage;
  MutableResponseFeatureSet featureSetMessage;

  auto requestBuilder = requestMessage.builder();
  mockARCorePointCloud(&requestBuilder);

  auto poseBuilder = poseMessage.builder();
  mockComputedPose(&poseBuilder);

  FeatureSetRequestExecutor executor;
  Tracker tracker;

  auto featureSetBuilder = featureSetMessage.builder();
  executor.execute(requestBuilder.asReader(), poseBuilder.asReader(), &tracker, &featureSetBuilder);

  auto featureSetReader = featureSetBuilder.asReader();
  auto requestPoints = requestBuilder.getSensors().getARCore().getPointCloud();
  auto points = featureSetReader.getPoints();
  for (int i = 0; i < points.size(); ++i) {
    auto arCorePosition = requestPoints[i].getPosition();
    auto featurePosition = points[i].getPosition();
    assertPositionsEqual(
      correctARCorePosition(
        HPoint3(arCorePosition.getX(), arCorePosition.getY(), arCorePosition.getZ()),
        poseBuilder.asReader()),
      HPoint3(featurePosition.getX(), featurePosition.getY(), featurePosition.getZ()));
    EXPECT_EQ(requestPoints[i].getConfidence(), points[i].getConfidence());
    EXPECT_EQ(requestPoints[i].getId(), points[i].getId());
  }
}

TEST_F(FeatureSetRequestExecutorTest, TestARKitPointCloudConversion) {
  ScopeTimer rt("test");
  MutableRealityRequest requestMessage;
  MutableResponsePose poseMessage;
  MutableResponseFeatureSet featureSetMessage;

  auto requestBuilder = requestMessage.builder();
  mockARKitPointCloud(&requestBuilder);

  auto poseBuilder = poseMessage.builder();
  mockComputedPose(&poseBuilder);

  FeatureSetRequestExecutor executor;
  Tracker tracker;

  auto featureSetBuilder = featureSetMessage.builder();
  executor.execute(requestBuilder.asReader(), poseBuilder.asReader(), &tracker, &featureSetBuilder);

  auto featureSetReader = featureSetBuilder.asReader();
  auto requestPoints = requestBuilder.getSensors().getARKit().getPointCloud();
  auto points = featureSetReader.getPoints();
  for (int i = 0; i < points.size(); ++i) {
    auto arKitPosition = requestPoints[i].getPosition();
    auto featurePosition = points[i].getPosition();
    assertPositionsEqual(
      correctARKitPosition(
        HPoint3(arKitPosition.getX(), arKitPosition.getY(), arKitPosition.getZ()),
        poseBuilder.asReader()),
      HPoint3(featurePosition.getX(), featurePosition.getY(), featurePosition.getZ()));
    EXPECT_EQ(requestPoints[i].getId(), points[i].getId());
    EXPECT_EQ(1.0f, points[i].getConfidence());
  }
}

TEST_F(FeatureSetRequestExecutorTest, TestC8FeaturePointConversion) {
  ScopeTimer rt("test");
  MutableRealityRequest requestMessage;
  MutableResponsePose poseMessage;
  MutableResponseFeatureSet featureSetMessage;

  auto requestBuilder = requestMessage.builder();

  auto poseBuilder = poseMessage.builder();
  mockComputedPose(&poseBuilder);

  FeatureSetRequestExecutor executor;
  MockTracker tracker;

  auto featureSetBuilder = featureSetMessage.builder();
  executor.execute(requestBuilder.asReader(), poseBuilder.asReader(), &tracker, &featureSetBuilder);

  auto featureSetReader = featureSetBuilder.asReader();
  auto points = featureSetReader.getPoints();
  for (int i = 0; i < points.size(); ++i) {
    auto featurePosition = points[i].getPosition();
    assertPositionsEqual(
      correctC8Position(MOCK_POINT_CLOUD[i], poseBuilder.asReader()),
      HPoint3(featurePosition.getX(), featurePosition.getY(), featurePosition.getZ()));
    EXPECT_EQ(0.5f, points[i].getConfidence());
    EXPECT_EQ(i, points[i].getId());
  }
}

}  // namespace c8
