// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":hit-test-performer",
    "//c8/protolog:api-limits",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x11df7bc7);

#include "c8/protolog/api-limits.h"
#include "reality/engine/hittest/hit-test-performer.h"

#include "gtest/gtest.h"

using MutableXrHitTestRequest = c8::MutableRootMessage<c8::XrHitTestRequest>;
using MutableXrHitTestResponse = c8::MutableRootMessage<c8::XrHitTestResponse>;
using MutableRealityResponse = c8::MutableRootMessage<c8::RealityResponse>;

namespace c8 {

namespace {
static void mockRealityResponseCamera(ResponseCamera::Builder *camera) {
  camera->getExtrinsic().getRotation().setW(1.0f);
  camera->getExtrinsic().getRotation().setX(0.0f);
  camera->getExtrinsic().getRotation().setY(0.0f);
  camera->getExtrinsic().getRotation().setZ(0.0f);

  camera->getExtrinsic().getPosition().setX(0.0f);
  camera->getExtrinsic().getPosition().setY(0.0f);
  camera->getExtrinsic().getPosition().setZ(0.0f);

  // Initialize with Intrinsics::unitTestCamera() values.
  auto m = camera->getIntrinsic().initMatrix44f(16);
  m.set(0 + 0 * 4, 5.0f);
  m.set(0 + 1 * 4, 0.0f);
  m.set(0 + 2 * 4, 3.0f);
  m.set(0 + 3 * 4, 0.0f);
  m.set(1 + 0 * 4, 0.0f);
  m.set(1 + 1 * 4, -5.0f);
  m.set(1 + 2 * 4, 2.0f);
  m.set(1 + 3 * 4, 0.0f);
  m.set(2 + 0 * 4, 0.0f);
  m.set(2 + 1 * 4, 0.0f);
  m.set(2 + 2 * 4, 1.0f);
  m.set(2 + 3 * 4, 0.0f);
  m.set(3 + 0 * 4, 0.0f);
  m.set(3 + 1 * 4, 0.0f);
  m.set(3 + 2 * 4, 0.0f);
  m.set(3 + 3 * 4, 1.0f);
}
}  // namespace

class HitTestPerformerTest : public ::testing::Test {};

TEST_F(HitTestPerformerTest, TestHitTestWithNoFeaturePointsNoSurfaces) {
  ScopeTimer rt("test");
  MutableXrHitTestRequest hitTestRequest;
  MutableXrHitTestResponse hitTestResponse;
  MutableRealityResponse realityResponse;
  HitTestPerformer hitTestPerformer;

  auto realityResponseBuilder = realityResponse.builder();
  auto camera = realityResponseBuilder.getXRResponse().getCamera();
  mockRealityResponseCamera(&camera);

  auto includedTypes = hitTestRequest.builder().initIncludedTypes(1);
  includedTypes.set(0, XrHitTestResult::ResultType::FEATURE_POINT);

  // Init with zero feature points.
  realityResponseBuilder.getFeatureSet().initPoints(0);

  auto hitTestResponseBuilder = hitTestResponse.builder();
  hitTestPerformer.performHitTest(
    realityResponse.reader().getXRResponse().getCamera(),
    realityResponse.reader().getFeatureSet(),
    realityResponse.reader().getXRResponse().getSurfaces(),
    hitTestRequest.reader(),
    &hitTestResponseBuilder);

  auto resultReader = hitTestResponse.reader();
  EXPECT_FALSE(resultReader.hasHits());
}

TEST_F(HitTestPerformerTest, TestHitTestWithFeaturePointsNoSurfaces) {
  ScopeTimer rt("test");
  MutableXrHitTestRequest hitTestRequest;
  MutableXrHitTestResponse hitTestResponse;
  MutableRealityResponse realityResponse;
  HitTestPerformer hitTestPerformer;

  auto realityResponseBuilder = realityResponse.builder();
  auto camera = realityResponseBuilder.getXRResponse().getCamera();
  mockRealityResponseCamera(&camera);

  // Init with zero feature points.
  realityResponseBuilder.getFeatureSet().initPoints(1);
  realityResponseBuilder.getFeatureSet().getPoints()[0].getPosition().setX(0.0f);
  realityResponseBuilder.getFeatureSet().getPoints()[0].getPosition().setY(0.0f);
  realityResponseBuilder.getFeatureSet().getPoints()[0].getPosition().setZ(0.0f);

  auto includedTypes = hitTestRequest.builder().initIncludedTypes(1);
  includedTypes.set(0, XrHitTestResult::ResultType::FEATURE_POINT);

  auto hitTestResponseBuilder = hitTestResponse.builder();
  hitTestPerformer.performHitTest(
    realityResponse.reader().getXRResponse().getCamera(),
    realityResponse.reader().getFeatureSet(),
    realityResponse.reader().getXRResponse().getSurfaces(),
    hitTestRequest.reader(),
    &hitTestResponseBuilder);

  auto resultReader = hitTestResponse.reader();
  EXPECT_TRUE(resultReader.hasHits());
  EXPECT_EQ(resultReader.getHits().size(), 1);
  EXPECT_EQ(resultReader.getHits()[0].getType(), XrHitTestResult::ResultType::FEATURE_POINT);
}

TEST_F(HitTestPerformerTest, TestRotatePtsInFrontOfCamera) {
  Vector<HPoint3> surfacePts{
    HPoint3(-.25f, 0.0f, 3.0f), HPoint3(1.0f, 1.0f, 3.0f), HPoint3(1.0f, -1.0f, 3.0f)};
  HMatrix cam =
    cameraMotion(HPoint3(0.0f, 0.0f, 0.0f), Quaternion::fromEulerAngleDegrees(15.0f, 25.0f, 45.0f));

  HMatrix r = HMatrixGen::i();
  float d;
  Vector<HPoint3> rotatedPts;
  HitTestPerformer::rotatePointsToFrontOfCamera(surfacePts, cam, &rotatedPts, &d, &r);

  EXPECT_FLOAT_EQ(-3.0f, rotatedPts[0].z());
  EXPECT_FLOAT_EQ(-3.0f, rotatedPts[1].z());
  EXPECT_FLOAT_EQ(-3.0f, rotatedPts[2].z());
}

}  // namespace c8
