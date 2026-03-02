// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":orientation",
    "//c8:c8-log-proto",
    "//c8/io:capnp-messages",
    "//c8/protolog:xr-requests",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x073cea5c);

#include "reality/engine/geometry/orientation.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "c8/c8-log-proto.h"
#include "c8/io/capnp-messages.h"
#include "c8/protolog/xr-requests.h"

using testing::Eq;
using testing::Pointwise;

namespace c8 {

class OrientationTest : public ::testing::Test {};

TEST_F(OrientationTest, TestPreprocessLandscapeLeft) {
  AppContext::DeviceOrientation orientation = AppContext::DeviceOrientation::LANDSCAPE_LEFT;
  MutableRootMessage<RealityRequest> r;
  auto g = r.builder().getXRConfiguration().getGraphicsIntrinsics();
  g.setTextureWidth(640);
  g.setTextureHeight(480);

  deviceToPortraitOrientationPreprocess(orientation, r.builder());

  EXPECT_EQ(480, g.getTextureWidth());
  EXPECT_EQ(640, g.getTextureHeight());
}

TEST_F(OrientationTest, PostprocessLandscapeLeft) {
  AppContext::DeviceOrientation orientation = AppContext::DeviceOrientation::LANDSCAPE_LEFT;
  MutableRootMessage<RealityResponse> r;

  r.builder().getXRResponse().getCamera().getIntrinsic().initMatrix44f(16);
  auto m = r.builder().getXRResponse().getCamera().getIntrinsic().getMatrix44f();
  m.set(0 + 0 * 4, 1);
  m.set(0 + 1 * 4, 0);
  m.set(0 + 2 * 4, 2);
  m.set(0 + 3 * 4, 0);
  m.set(1 + 0 * 4, 0);
  m.set(1 + 1 * 4, 3);
  m.set(1 + 2 * 4, 4);
  m.set(1 + 3 * 4, 0);
  m.set(2 + 0 * 4, 0);
  m.set(2 + 1 * 4, 5);
  m.set(2 + 2 * 4, 6);
  m.set(2 + 3 * 4, 7);
  m.set(3 + 0 * 4, 0);
  m.set(3 + 1 * 4, 0);
  m.set(3 + 2 * 4, 8);
  m.set(3 + 3 * 4, 0);

  auto e = r.builder().getXRResponse().getCamera().getExtrinsic();
  auto et = e.getPosition();
  auto er = e.getRotation();
  setPosition32f(1.0f, 2.0f, 3.f, &et);
  setQuaternion32f(1.0f, 0.0f, 0.0f, 0.0f, &er);

  portraitToDeviceOrientationPostprocess(orientation, r.builder());

  EXPECT_EQ(3, m[0 + 0 * 4]);
  EXPECT_EQ(0, m[0 + 1 * 4]);
  EXPECT_EQ(-4, m[0 + 2 * 4]);
  EXPECT_EQ(0, m[0 + 3 * 4]);
  EXPECT_EQ(0, m[1 + 0 * 4]);
  EXPECT_EQ(1, m[1 + 1 * 4]);
  EXPECT_EQ(2, m[1 + 2 * 4]);
  EXPECT_EQ(0, m[1 + 3 * 4]);
  EXPECT_EQ(0, m[2 + 0 * 4]);
  EXPECT_EQ(5, m[2 + 1 * 4]);
  EXPECT_EQ(6, m[2 + 2 * 4]);
  EXPECT_EQ(7, m[2 + 3 * 4]);
  EXPECT_EQ(0, m[3 + 0 * 4]);
  EXPECT_EQ(0, m[3 + 1 * 4]);
  EXPECT_EQ(8, m[3 + 2 * 4]);
  EXPECT_EQ(0, m[3 + 3 * 4]);

  EXPECT_FLOAT_EQ(0.707107f, er.getW());
  EXPECT_EQ(0.0f, er.getX());
  EXPECT_EQ(0.0f, er.getY());
  EXPECT_FLOAT_EQ(-0.707107f, er.getZ());
  EXPECT_EQ(1.0f, et.getX());
  EXPECT_EQ(2.0f, et.getY());
  EXPECT_EQ(3.0f, et.getZ());
}

TEST_F(OrientationTest, RewriteCoordinateSystemPostprocessRightHandedSemanticsResponse) {
  MutableRootMessage<CameraCoordinates> system;
  {
    system.builder().setAxes(CameraCoordinates::Axes::RIGHT_HANDED);
    system.builder().setMirroredDisplay(false);
  }
  MutableRootMessage<SemanticsResponse> r;
  HPoint3 pos = {1.f, 2.f, 3.f};
  auto rot = Quaternion::fromPitchYawRollDegrees(30, 60, 90);
  {
    auto posBuilder = r.builder().getCamera().getExtrinsic().getPosition();
    auto rotBuilder = r.builder().getCamera().getExtrinsic().getRotation();
    setPosition(pos, &posBuilder);
    setQuaternion(rot, &rotBuilder);
  }

  rewriteCoordinateSystemPostprocess(system.reader(), r.builder());
  auto newPos = toHPoint(r.reader().getCamera().getExtrinsic().getPosition());
  EXPECT_FLOAT_EQ(pos.x(), newPos.x());
  EXPECT_FLOAT_EQ(pos.y(), newPos.y());
  EXPECT_FLOAT_EQ(pos.z(), -newPos.z());
  auto newRot = toQuaternion(r.reader().getCamera().getExtrinsic().getRotation());
  EXPECT_FLOAT_EQ(rot.x(), newRot.x());
  EXPECT_FLOAT_EQ(rot.y(), newRot.y());
  EXPECT_FLOAT_EQ(rot.z(), -newRot.z());
  EXPECT_FLOAT_EQ(rot.w(), -newRot.w());
}

TEST_F(OrientationTest, RewriteCoordinateSystemPostprocessMirroredSemanticsResponse) {
  MutableRootMessage<CameraCoordinates> system;
  {
    system.builder().setAxes(CameraCoordinates::Axes::LEFT_HANDED);
    system.builder().setMirroredDisplay(true);
  }
  MutableRootMessage<SemanticsResponse> r;
  HPoint3 pos = {1.f, 2.f, 3.f};
  auto rot = Quaternion::fromPitchYawRollDegrees(30, 60, 90);
  {
    auto posBuilder = r.builder().getCamera().getExtrinsic().getPosition();
    auto rotBuilder = r.builder().getCamera().getExtrinsic().getRotation();
    setPosition(pos, &posBuilder);
    setQuaternion(rot, &rotBuilder);
  }

  rewriteCoordinateSystemPostprocess(system.reader(), r.builder());
  auto newPos = toHPoint(r.reader().getCamera().getExtrinsic().getPosition());
  EXPECT_FLOAT_EQ(pos.x(), -newPos.x());
  EXPECT_FLOAT_EQ(pos.y(), newPos.y());
  EXPECT_FLOAT_EQ(pos.z(), newPos.z());
  auto newRot = toQuaternion(r.reader().getCamera().getExtrinsic().getRotation());
  EXPECT_FLOAT_EQ(rot.x(), newRot.x());
  EXPECT_FLOAT_EQ(rot.y(), -newRot.y());
  EXPECT_FLOAT_EQ(rot.z(), -newRot.z());
  EXPECT_FLOAT_EQ(rot.w(), newRot.w());
}

}  // namespace c8
