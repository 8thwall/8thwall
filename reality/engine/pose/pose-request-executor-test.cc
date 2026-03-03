// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":pose-request-executor",
    "//c8:random-numbers",
    "//c8/io:capnp-messages",
    "//reality/engine/api:reality.capnp-cc",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xd0727438);

#include <gtest/gtest.h>

#include <random>

#include "c8/geometry/device-pose.h"
#include "c8/hpoint.h"
#include "c8/io/capnp-messages.h"
#include "capnp/message.h"
#include "reality/engine/api/reality.capnp.h"
#include "reality/engine/api/response/pose.capnp.h"
#include "reality/engine/pose/pose-request-executor.h"

namespace c8 {

namespace {

static constexpr c8_PixelPinholeCameraModel INTRINSIC{480, 640, 240.0f, 320.0f, 625.49f, 625.49f};
static constexpr DeviceInfos::DeviceModel DEVICE_MODEL = DeviceInfos::DeviceModel::NOT_SPECIFIED;
static constexpr char DEVICE_MANUFACTURER[] = "";
constexpr Quaternion Q0(
  0.6413764532829037f, 0.5723576570056439f, -0.4252083875837609f, 0.283268044031922f);

}  // namespace

class PoseRequestExecutorTest : public ::testing::Test {
protected:
  MutableRootMessage<RequestFlags> flagsMessage;
  MutableRootMessage<RequestSensor> lastRequestMessage;
  MutableRootMessage<RealityResponse> lastResponseMessage;
  MutableRootMessage<DeprecatedResponseFeatures> featuresMessage;
  MutableRootMessage<ResponsePose> responseMessage;
  MutableRootMessage<ResponsePose> expectedResponseMessage;
  MutableRootMessage<RealityRequest> realityRequestMessage;
  DetectionImageMap targets;
  RandomNumbers random;

  virtual ~PoseRequestExecutorTest() throw() {}
};

TEST_F(PoseRequestExecutorTest, TestPose) {
  Tracker tracker;
  ScopeTimer rt("test");
  int64_t lastEventTimeMicros = 1;
  int64_t currentEventTimeMicros = 2;
  auto flagsBuilder = flagsMessage.builder();
  auto lastRequestBuilder = lastRequestMessage.builder();
  auto lastResponseBuilder = lastResponseMessage.builder();
  auto requestBuilder = realityRequestMessage.builder().getSensors();
  auto featuresBuilder = featuresMessage.builder();
  auto responseBuilder = responseMessage.builder();
  auto expectedResponseBuilder = expectedResponseMessage.builder();

  requestBuilder.getPose().getDevicePose().setW(1.0f);
  requestBuilder.getPose().getSensorRotationToPortrait().setW(1.0f);

  expectedResponseBuilder.getStatus();

  PoseRequestExecutor executor;
  executor.execute(
    lastEventTimeMicros,
    currentEventTimeMicros,
    DEVICE_MODEL,
    DEVICE_MANUFACTURER,
    INTRINSIC,
    flagsBuilder.asReader(),
    lastRequestBuilder.asReader(),
    lastResponseBuilder.asReader(),
    realityRequestMessage.reader().getAppContext(),
    requestBuilder.asReader(),
    realityRequestMessage.reader().getDebugData(),
    featuresBuilder.asReader(),
    &responseBuilder,
    &tracker,
    &targets,
    &random);

  EXPECT_TRUE(responseBuilder.hasStatus());
  EXPECT_FALSE(responseBuilder.getStatus().hasError());

  // NOTE(paris): Disabled when yaw correction was disabled.
  // EXPECT_FLOAT_EQ(0.7071067811865476f, responseBuilder.getTransform().getRotation().getW());
  EXPECT_TRUE(responseBuilder.getTransform().hasRotation());
  EXPECT_TRUE(responseBuilder.getTransform().hasPosition());
}

TEST_F(PoseRequestExecutorTest, TestDisplacement) {
  ScopeTimer rt("test");
  Tracker tracker;
  int64_t lastEventTimeMicros = 1000000;
  int64_t currentEventTimeMicros = 2000000;
  auto lastRequestBuilder = lastRequestMessage.builder();
  auto lastResponseBuilder = lastResponseMessage.builder();
  auto requestBuilder = realityRequestMessage.builder().getSensors();
  auto responseBuilder = responseMessage.builder();

  requestBuilder.getPose().getDeviceAcceleration().setX(1.0f);
  requestBuilder.getPose().getDeviceAcceleration().setY(1.0f);
  requestBuilder.getPose().getDeviceAcceleration().setZ(1.0f);
  requestBuilder.getPose().getSensorRotationToPortrait().setW(1.0f);

  PoseRequestExecutor executor;
  executor.execute(
    lastEventTimeMicros,
    currentEventTimeMicros,
    DEVICE_MODEL,
    DEVICE_MANUFACTURER,
    INTRINSIC,
    flagsMessage.reader(),
    lastRequestBuilder.asReader(),
    lastResponseBuilder.asReader(),
    realityRequestMessage.reader().getAppContext(),
    requestBuilder.asReader(),
    realityRequestMessage.reader().getDebugData(),
    featuresMessage.reader(),
    &responseBuilder,
    &tracker,
    &targets,
    &random);

  EXPECT_EQ(1.0f, responseBuilder.getVelocity().getPosition().getX());
  EXPECT_EQ(1.0f, responseBuilder.getVelocity().getPosition().getY());
  EXPECT_EQ(-1.0f, responseBuilder.getVelocity().getPosition().getZ());
  // TODO(nb): Make this work again.
  EXPECT_EQ(0.0f, responseBuilder.getTransform().getPosition().getX());  // 2.5f
  EXPECT_EQ(0.0f, responseBuilder.getTransform().getPosition().getY());  // 2.5f
  EXPECT_EQ(0.0f, responseBuilder.getTransform().getPosition().getZ());  // -2.5f
}

TEST_F(PoseRequestExecutorTest, TestDisplacementShort) {
  // NOTE: The outputs of this test reflect the current code and are not known to be the correct
  // answers.  This helps prevent accidental regressions from refactorings, but at some points the
  // results should be carefully vetted against what should happen.
  ScopeTimer rt("test");
  Tracker tracker;
  int64_t lastEventTimeMicros = 1000000;
  int64_t currentEventTimeMicros = 1016000;
  auto lastRequestBuilder = lastRequestMessage.builder();
  auto lastResponseBuilder = lastResponseMessage.builder();
  auto requestBuilder = realityRequestMessage.builder().getSensors();
  auto responseBuilder = responseMessage.builder();

  requestBuilder.getPose().getDeviceAcceleration().setX(-1.0f);
  requestBuilder.getPose().getDeviceAcceleration().setY(2.0f);
  requestBuilder.getPose().getDeviceAcceleration().setZ(5.0f);
  requestBuilder.getPose().getSensorRotationToPortrait().setW(1.0f);

  PoseRequestExecutor executor;
  executor.execute(
    lastEventTimeMicros,
    currentEventTimeMicros,
    DEVICE_MODEL,
    DEVICE_MANUFACTURER,
    INTRINSIC,
    flagsMessage.reader(),
    lastRequestBuilder.asReader(),
    lastResponseBuilder.asReader(),
    realityRequestMessage.reader().getAppContext(),
    requestBuilder.asReader(),
    realityRequestMessage.reader().getDebugData(),
    featuresMessage.reader(),
    &responseBuilder,
    &tracker,
    &targets,
    &random);

  EXPECT_FLOAT_EQ(-0.016f, responseBuilder.getVelocity().getPosition().getX());
  EXPECT_FLOAT_EQ(0.032f, responseBuilder.getVelocity().getPosition().getY());
  EXPECT_FLOAT_EQ(-0.08f, responseBuilder.getVelocity().getPosition().getZ());
  // TODO(nb): Make this work again.
  EXPECT_FLOAT_EQ(0.0f, responseBuilder.getTransform().getPosition().getX());  // -0.00064f
  EXPECT_FLOAT_EQ(0.0f, responseBuilder.getTransform().getPosition().getY());  // 0.00128f
  EXPECT_FLOAT_EQ(0.0f, responseBuilder.getTransform().getPosition().getZ());  // -0.00320f
}

TEST_F(PoseRequestExecutorTest, TestDisplacementLong) {
  // NOTE: The outputs of this test reflect the current code and are not known to be the correct
  // answers.  This helps prevent accidental regressions from refactorings, but at some points the
  // results should be carefully vetted against what should happen.
  ScopeTimer rt("test");
  Tracker tracker;
  int64_t lastEventTimeMicros = 1000000;
  int64_t currentEventTimeMicros = 4000000;
  auto lastRequestBuilder = lastRequestMessage.builder();
  auto lastResponseBuilder = lastResponseMessage.builder();
  auto requestBuilder = realityRequestMessage.builder().getSensors();
  auto responseBuilder = responseMessage.builder();

  requestBuilder.getPose().getDeviceAcceleration().setX(-1.0f);
  requestBuilder.getPose().getDeviceAcceleration().setY(2.0f);
  requestBuilder.getPose().getDeviceAcceleration().setZ(5.0f);
  requestBuilder.getPose().getSensorRotationToPortrait().setW(1.0f);

  PoseRequestExecutor executor;
  executor.execute(
    lastEventTimeMicros,
    currentEventTimeMicros,
    DEVICE_MODEL,
    DEVICE_MANUFACTURER,
    INTRINSIC,
    flagsMessage.reader(),
    lastRequestBuilder.asReader(),
    lastResponseBuilder.asReader(),
    realityRequestMessage.reader().getAppContext(),
    requestBuilder.asReader(),
    realityRequestMessage.reader().getDebugData(),
    featuresMessage.reader(),
    &responseBuilder,
    &tracker,
    &targets,
    &random);

  EXPECT_EQ(-3.0f, responseBuilder.getVelocity().getPosition().getX());
  EXPECT_EQ(6.0f, responseBuilder.getVelocity().getPosition().getY());
  EXPECT_EQ(-15.0f, responseBuilder.getVelocity().getPosition().getZ());
  // TODO(nb): Make this work again.
  EXPECT_EQ(0.0f, responseBuilder.getTransform().getPosition().getX());  // -22.5f
  EXPECT_EQ(0.0f, responseBuilder.getTransform().getPosition().getY());  // 45.0f
  EXPECT_EQ(0.0f, responseBuilder.getTransform().getPosition().getZ());  // -112.5f
}

TEST_F(PoseRequestExecutorTest, TestDragForce) {
  ScopeTimer rt("test");
  Tracker tracker;
  int64_t lastEventTimeMicros = 1000000;
  int64_t currentEventTimeMicros = 2000000;
  auto lastRequestBuilder = lastRequestMessage.builder();
  auto lastResponseBuilder = lastResponseMessage.builder();
  auto requestBuilder = realityRequestMessage.builder().getSensors();
  auto responseBuilder = responseMessage.builder();

  lastResponseBuilder.getPose().getVelocity().getPosition().setX(1.0f);
  lastResponseBuilder.getPose().getVelocity().getPosition().setY(1.0f);
  lastResponseBuilder.getPose().getVelocity().getPosition().setZ(1.0f);

  PoseRequestExecutor executor;
  executor.execute(
    lastEventTimeMicros,
    currentEventTimeMicros,
    DEVICE_MODEL,
    DEVICE_MANUFACTURER,
    INTRINSIC,
    flagsMessage.reader(),
    lastRequestBuilder.asReader(),
    lastResponseBuilder.asReader(),
    realityRequestMessage.reader().getAppContext(),
    requestBuilder.asReader(),
    realityRequestMessage.reader().getDebugData(),
    featuresMessage.reader(),
    &responseBuilder,
    &tracker,
    &targets,
    &random);

  EXPECT_EQ(0.0f, responseBuilder.getVelocity().getPosition().getX());
  EXPECT_EQ(0.0f, responseBuilder.getVelocity().getPosition().getY());
  EXPECT_EQ(0.0f, responseBuilder.getVelocity().getPosition().getZ());
}

TEST_F(PoseRequestExecutorTest, TestDisplacementStartingPose) {
  // NOTE: The outputs of this test reflect the current code and are not known to be the correct
  // answers.  This helps prevent accidental regressions from refactorings, but at some points the
  // results should be carefully vetted against what should happen.
  ScopeTimer rt("test");
  Tracker tracker;
  int64_t lastEventTimeMicros = 1000000;
  int64_t currentEventTimeMicros = 1016000;
  auto lastRequestBuilder = lastRequestMessage.builder();
  auto lastResponseBuilder = lastResponseMessage.builder();
  auto requestBuilder = realityRequestMessage.builder().getSensors();
  auto responseBuilder = responseMessage.builder();

  lastResponseBuilder.getPose().getTransform().getPosition().setX(11.0f);
  lastResponseBuilder.getPose().getTransform().getPosition().setY(-12.0f);
  lastResponseBuilder.getPose().getTransform().getPosition().setZ(13.0f);
  lastResponseBuilder.getPose().getTransform().getRotation().setW(0.7071067811865476f);
  lastResponseBuilder.getPose().getTransform().getRotation().setX(0);
  lastResponseBuilder.getPose().getTransform().getRotation().setY(-0.7071067811865476f);
  lastResponseBuilder.getPose().getTransform().getRotation().setZ(0);

  requestBuilder.getPose().getDeviceAcceleration().setX(-1.0f);
  requestBuilder.getPose().getDeviceAcceleration().setY(2.0f);
  requestBuilder.getPose().getDeviceAcceleration().setZ(5.0f);
  requestBuilder.getPose().getSensorRotationToPortrait().setW(1.0f);

  PoseRequestExecutor executor;
  executor.execute(
    lastEventTimeMicros,
    currentEventTimeMicros,
    DEVICE_MODEL,
    DEVICE_MANUFACTURER,
    INTRINSIC,
    flagsMessage.reader(),
    lastRequestBuilder.asReader(),
    lastResponseBuilder.asReader(),
    realityRequestMessage.reader().getAppContext(),
    requestBuilder.asReader(),
    realityRequestMessage.reader().getDebugData(),
    featuresMessage.reader(),
    &responseBuilder,
    &tracker,
    &targets,
    &random);

  EXPECT_FLOAT_EQ(-0.016f, responseBuilder.getVelocity().getPosition().getX());
  EXPECT_FLOAT_EQ(0.032f, responseBuilder.getVelocity().getPosition().getY());
  EXPECT_FLOAT_EQ(-0.08f, responseBuilder.getVelocity().getPosition().getZ());
  // TODO(nb): Make this work again.
  EXPECT_FLOAT_EQ(0.0f, responseBuilder.getTransform().getPosition().getX());  // 10.99936f
  EXPECT_FLOAT_EQ(0.0f, responseBuilder.getTransform().getPosition().getY());  // -11.99872f
  EXPECT_FLOAT_EQ(0.0, responseBuilder.getTransform().getPosition().getZ());   // 12.9968f
}

TEST_F(PoseRequestExecutorTest, TestARKitDisplacement) {
  // NOTE: The outputs of this test reflect the current code and are not known to be the correct
  // answers.  This helps prevent accidental regressions from refactorings, but at some points the
  // results should be carefully vetted against what should happen.
  ScopeTimer rt("test");
  Tracker tracker;
  int64_t lastEventTimeMicros = 1000000;
  int64_t currentEventTimeMicros = 2000000;
  auto lastRequestBuilder = lastRequestMessage.builder();
  auto lastResponseBuilder = lastResponseMessage.builder();
  auto requestBuilder = realityRequestMessage.builder().getSensors();
  auto responseBuilder = responseMessage.builder();

  lastResponseBuilder.getPose().getInitialTransform().getRotation().setW(1.0f);
  lastResponseBuilder.getPose().getInitialTransform().getRotation().setX(0.0f);
  lastResponseBuilder.getPose().getInitialTransform().getRotation().setY(0.0f);
  lastResponseBuilder.getPose().getInitialTransform().getRotation().setZ(0.0f);
  lastResponseBuilder.getPose().getInitialTransform().getPosition().setX(0.0);
  lastResponseBuilder.getPose().getInitialTransform().getPosition().setY(0.0);
  lastResponseBuilder.getPose().getInitialTransform().getPosition().setZ(0.0);

  auto arq = arkitRotationFromXRRotation(Q0);
  auto arp = arkitPositionFromXRPosition(HPoint3(1.0f, 2.0f, 3.0f));

  requestBuilder.getARKit().getPose().getRotation().setW(arq.w());
  requestBuilder.getARKit().getPose().getRotation().setX(arq.x());
  requestBuilder.getARKit().getPose().getRotation().setY(arq.y());
  requestBuilder.getARKit().getPose().getRotation().setZ(arq.z());
  requestBuilder.getARKit().getPose().getTranslation().setX(arp.x());
  requestBuilder.getARKit().getPose().getTranslation().setY(arp.y());
  requestBuilder.getARKit().getPose().getTranslation().setZ(arp.z());
  requestBuilder.getPose().getSensorRotationToPortrait().setW(1.0f);

  PoseRequestExecutor executor;
  executor.execute(
    lastEventTimeMicros,
    currentEventTimeMicros,
    DEVICE_MODEL,
    DEVICE_MANUFACTURER,
    INTRINSIC,
    flagsMessage.reader(),
    lastRequestBuilder.asReader(),
    lastResponseBuilder.asReader(),
    realityRequestMessage.reader().getAppContext(),
    requestBuilder.asReader(),
    realityRequestMessage.reader().getDebugData(),
    featuresMessage.reader(),
    &responseBuilder,
    &tracker,
    &targets,
    &random);

  EXPECT_NEAR(1.0f, responseBuilder.getTransform().getPosition().getX(), 2e-3);
  EXPECT_NEAR(2.0f, responseBuilder.getTransform().getPosition().getY(), 2e-3);
  EXPECT_NEAR(3.0f, responseBuilder.getTransform().getPosition().getZ(), 2e-3);
}

TEST_F(PoseRequestExecutorTest, TestARCorePositionWithoutTracking) {
  // NOTE: The outputs of this test reflect the current code and are not known to be the correct
  // answers.  This helps prevent accidental regressions from refactorings, but at some points the
  // results should be carefully vetted against what should happen.
  ScopeTimer rt("test");
  Tracker tracker;
  int64_t lastEventTimeMicros = 1000000;
  int64_t currentEventTimeMicros = 2000000;
  auto lastRequestBuilder = lastRequestMessage.builder();
  auto lastResponseBuilder = lastResponseMessage.builder();
  auto requestBuilder = realityRequestMessage.builder().getSensors();
  auto responseBuilder = responseMessage.builder();

  auto initialRot = lastResponseBuilder.getPose().getInitialTransform().getRotation();
  initialRot.setW(1.0);
  initialRot.setX(0.0);
  initialRot.setY(0.0);
  initialRot.setZ(0.0);

  auto lastRotation = lastResponseBuilder.getPose().getTransform().getRotation();
  lastRotation.setW(1.0);
  lastRotation.setX(1.0);
  lastRotation.setY(1.0);
  lastRotation.setZ(1.0);

  auto lastPosition = lastResponseBuilder.getPose().getTransform().getPosition();
  lastPosition.setX(50.0);
  lastPosition.setY(20.0);
  lastPosition.setZ(30.0);

  // Initialize ARCore, but don't give it pose data (i.e. ARCore without tracking).
  requestBuilder.initARCore();

  PoseRequestExecutor executor;
  executor.execute(
    lastEventTimeMicros,
    currentEventTimeMicros,
    DEVICE_MODEL,
    DEVICE_MANUFACTURER,
    INTRINSIC,
    flagsMessage.reader(),
    lastRequestBuilder.asReader(),
    lastResponseBuilder.asReader(),
    realityRequestMessage.reader().getAppContext(),
    requestBuilder.asReader(),
    realityRequestMessage.reader().getDebugData(),
    featuresMessage.reader(),
    &responseBuilder,
    &tracker,
    &targets,
    &random);

  EXPECT_EQ(50.0, responseBuilder.getTransform().getPosition().getX());
  EXPECT_EQ(20.0, responseBuilder.getTransform().getPosition().getY());
  EXPECT_EQ(30.0, responseBuilder.getTransform().getPosition().getZ());
}
}  // namespace c8
