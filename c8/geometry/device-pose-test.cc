// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "//c8/geometry:device-pose",
    "//c8/geometry:egomotion",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x2a79a9a2);

#include "c8/geometry/device-pose.h"
#include "c8/geometry/egomotion.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace c8 {

using testing::Eq;
using testing::Pointwise;

// A quaternion sampled from the unit sphere.
constexpr Quaternion Q0(
  0.6413764532829037f, 0.5723576570056439f, -0.4252083875837609f, 0.283268044031922f);

class DevicePoseTest : public ::testing::Test {};

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsHVector(const HVector3 &vec) {
  return Pointwise(AreWithin(0.0001), vec.data());
}
testing::AssertionResult AreEqual(Quaternion q1, Quaternion q2, float epsilon = 1e-6) {
  // The straightforward case
  if (
    (fabs(q1.w() - q2.w()) < epsilon) && (fabs(q1.x() - q2.x()) < epsilon)
    && (fabs(q1.y() - q2.y()) < epsilon) && (fabs(q1.z() - q2.z()) < epsilon)) {
    return testing::AssertionSuccess();
  }
  // The negated case
  if (
    (fabs(q1.w() - -q2.w()) < epsilon) && (fabs(q1.x() - -q2.x()) < epsilon)
    && (fabs(q1.y() - -q2.y()) < epsilon) && (fabs(q1.z() - -q2.z()) < epsilon)) {
    return testing::AssertionSuccess();
  }

  return testing::AssertionFailure() << "q1 " << q1.toString().c_str() << " does not equal "
                                     << q2.toString().c_str() << " with epsilon " << epsilon;
}

TEST_F(DevicePoseTest, DeviceAndNar) {
  // The device coordinate system is opengl right-handed: +x: right, +y: up, +z: towards.
  // The NAR coordinate system is opencv right-handed: +x: right, +y: down, +z: away.
  // https://<REMOVED_BEFORE_OPEN_SOURCING>.atlassian.net/wiki/spaces/AR/pages/1044873887/Coordinate+Systems+in+NAR+ARKit+ARCore+Unity

  // If sy is the change of basis matrix, then the math for an extrinsic in one form in another is:
  // H_xr = sy.inv() * H_nar * sy
  // H_nar = sy.inv() * H_xr * sy
  // See: https://en.wikipedia.org/wiki/Matrix_similarity
  auto sy = HMatrix{{1, 0, 0, 0}, {0, -1, 0, 0}, {0, 0, -1, 0}, {0, 0, 0, 1}};

  // Position
  auto pInDevice = HMatrixGen::translation(1.f, 1.f, 1.f);
  auto pInNar = sy.inv() * pInDevice * sy;
  EXPECT_THAT(translation(pInNar).data(), equalsHVector(narFromDevice(translation(pInDevice))));
  EXPECT_THAT(translation(pInDevice).data(), equalsHVector(deviceFromNar(translation(pInNar))));

  // Rotation
  auto rInDevice = HMatrixGen::rotationD(30.f, 45.f, 60.f);
  auto rInNar = sy.inv() * rInDevice * sy;
  EXPECT_TRUE(AreEqual(rotation(rInNar), narFromDevice(rotation(rInDevice))));
  EXPECT_TRUE(AreEqual(rotation(rInDevice), deviceFromNar(rotation(rInNar))));

  // HMatrix
  auto mInDevice = deviceFromNar(translation(pInNar), rotation(rInNar));
  EXPECT_THAT(translation(pInDevice).data(), equalsHVector(translation(mInDevice)));
  EXPECT_TRUE(AreEqual(rotation(rInDevice), rotation(mInDevice)));
}

TEST_F(DevicePoseTest, XrAndNar) {
  // The Xr coordinate system is unity left-handed: +x: right, +y: up, +z: away.
  // The NAR coordinate system is opencv right-handed: +x: right, +y: down, +z: away.
  // https://<REMOVED_BEFORE_OPEN_SOURCING>.atlassian.net/wiki/spaces/AR/pages/1044873887/Coordinate+Systems+in+NAR+ARKit+ARCore+Unity

  // If sy is the change of basis matrix, then the math for an extrinsic in one form in another is:
  // H_xr = sy.inv() * H_nar * sy
  // H_nar = sy.inv() * H_xr * sy
  // See: https://en.wikipedia.org/wiki/Matrix_similarity
  auto sy = HMatrix{{1, 0, 0, 0}, {0, -1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

  // Position
  auto pInXr = HMatrixGen::translation(1.f, 1.f, 1.f);
  auto pInNar = sy.inv() * pInXr * sy;
  EXPECT_THAT(translation(pInNar).data(), equalsHVector(narFromXr(translation(pInXr))));
  EXPECT_THAT(translation(pInXr).data(), equalsHVector(xrFromNar(translation(pInNar))));

  // Rotation
  auto rInXr = HMatrixGen::rotationD(30.f, 45.f, 60.f);
  auto rInNar = sy.inv() * rInXr * sy;
  EXPECT_TRUE(AreEqual(rotation(rInNar), narFromXr(rotation(rInXr))));
  EXPECT_TRUE(AreEqual(rotation(rInXr), xrFromNar(rotation(rInNar))));

  // HMatrix
  auto mInXr = xrFromNar(translation(pInNar), rotation(rInNar));
  EXPECT_THAT(translation(mInXr).data(), equalsHVector(translation(mInXr)));
  EXPECT_TRUE(AreEqual(rotation(rInXr), rotation(mInXr)));

  auto mInXrFromMat = xrFromNar(pInNar);
  EXPECT_THAT(translation(mInXrFromMat).data(), equalsHVector(translation(mInXrFromMat)));
  EXPECT_TRUE(AreEqual(rotation(mInXrFromMat), rotation(mInXrFromMat)));
}

TEST_F(DevicePoseTest, XRRotationFromDeviceRotation) {
  Quaternion q = xrRotationFromDeviceRotation(Q0);
  EXPECT_FLOAT_EQ(q.w(), 0.85823959f);
  EXPECT_FLOAT_EQ(q.x(), 0.04880365f);
  EXPECT_FLOAT_EQ(q.y(), 0.10036699f);
  EXPECT_FLOAT_EQ(q.z(), 0.50096846f);
}

TEST_F(DevicePoseTest, DeviceRotationFromXRRotation) {
  {
    Quaternion q(1.0f, 0.0f, 0.0f, 0.0f);
    auto qDevice = deviceRotationFromXrRotation(q);
    Quaternion qr = xrRotationFromDeviceRotation(qDevice);
    EXPECT_FLOAT_EQ(q.w(), qr.w());
    EXPECT_NEAR(q.x(), qr.x(), 1e-6f);
    EXPECT_FLOAT_EQ(q.y(), qr.y());
    EXPECT_FLOAT_EQ(q.z(), qr.z());
  }
  {
    auto qDevice = deviceRotationFromXrRotation(Q0);
    Quaternion qr = xrRotationFromDeviceRotation(qDevice);
    EXPECT_FLOAT_EQ(Q0.w(), qr.w());
    EXPECT_FLOAT_EQ(Q0.x(), qr.x());
    EXPECT_FLOAT_EQ(Q0.y(), qr.y());
    EXPECT_FLOAT_EQ(Q0.z(), qr.z());
  }
}

TEST_F(DevicePoseTest, XRRotationFromARKitRotation) {
  Quaternion q = xrRotationFromARKitRotation(Q0);
  EXPECT_FLOAT_EQ(q.w(), 0.70538574f);
  EXPECT_FLOAT_EQ(q.x(), -0.65382236f);
  EXPECT_FLOAT_EQ(q.y(), -0.25322092f);
  EXPECT_NEAR(q.z(), -0.10405032f, 1e-6);

  Quaternion qr = arkitRotationFromXRRotation(q);
  EXPECT_FLOAT_EQ(Q0.w(), qr.w());
  EXPECT_FLOAT_EQ(Q0.x(), qr.x());
  EXPECT_FLOAT_EQ(Q0.y(), qr.y());
  EXPECT_FLOAT_EQ(Q0.z(), qr.z());
}

TEST_F(DevicePoseTest, XRRotationFromARCoreRotationWhileTracking) {
  Quaternion q = xrRotationFromARCoreRotationWhileTracking(Q0);
  EXPECT_FLOAT_EQ(q.w(), 0.64137644f);
  EXPECT_FLOAT_EQ(q.x(), -0.57235765f);
  EXPECT_FLOAT_EQ(q.y(), 0.42520839f);
  EXPECT_FLOAT_EQ(q.z(), 0.28326803f);
}

TEST_F(DevicePoseTest, XRRotationFromTangoRotation) {
  Quaternion q = xrRotationFromTangoRotation(Q0);
  EXPECT_FLOAT_EQ(q.w(), 0.85823959f);
  EXPECT_FLOAT_EQ(q.x(), 0.04880365f);
  EXPECT_FLOAT_EQ(q.y(), 0.10036699f);
  EXPECT_FLOAT_EQ(q.z(), 0.50096846f);
}

TEST_F(DevicePoseTest, PortraitDeviceFromLandscapeMassf) {
  // Rotation
  auto q = portraitDeviceFromLandscapeMassf(Q0);
  EXPECT_FLOAT_EQ(q.w(), 0.10547957f);
  EXPECT_NEAR(q.x(), 0.252629f, 1e-6f);
  EXPECT_FLOAT_EQ(q.y(), -0.96110535f);
  EXPECT_FLOAT_EQ(q.z(), -0.0364608f);

  // HMatrix
  auto pInLandscapeMassf = HVector3{1.f, 2.f, 3.f};
  auto rInLandscapeMassf = Quaternion::fromPitchYawRollDegrees(180.0f, 0.0f, 45.0f);

  auto matInPortraitDevice = portraitDeviceFromLandscapeMassf(pInLandscapeMassf, rInLandscapeMassf);

  EXPECT_TRUE(
    AreEqual(rotation(matInPortraitDevice), portraitDeviceFromLandscapeMassf(rInLandscapeMassf)));
  EXPECT_THAT(translation(matInPortraitDevice).data(), equalsHVector(HVector3{1.0f, -3.0f, 2.0f}));
}

TEST_F(DevicePoseTest, UpdateWorldPosition) {
  auto p = updateWorldPosition(
    HPoint3(1.0f, 2.0f, 3.0f), HPoint3(0.5f, -0.5f, 0.25f), HPoint3(-0.5f, 1.5f, 2.25f), 1.0f);
  EXPECT_FLOAT_EQ(p.x(), 0.5f);
  EXPECT_FLOAT_EQ(p.y(), 2.0f);
  EXPECT_FLOAT_EQ(p.z(), 4.0f);
}

TEST_F(DevicePoseTest, UpdateWorldVelocity) {
  auto v = updateWorldVelocity(HPoint3(1.0f, 2.0f, 3.0f), HPoint3(0.5f, -0.5f, 0.25f), 1.0f, 1.0f);
  EXPECT_FLOAT_EQ(v.x(), 1.0f);
  EXPECT_FLOAT_EQ(v.y(), 2.0f);
  EXPECT_FLOAT_EQ(v.z(), 3.0f);
}

TEST_F(DevicePoseTest, AccelSimple) {
  auto prev = HVector3(2.f, 0.f, 0.f);
  auto curr = HVector3(2.f, 2.f, 0.f);
  auto next = HVector3(2.f, 6.f, 0.f);

  // Start with: p_k+1 = p_k + v_k * dt + 0.5 * a_k * dt^2
  // Solving for a_k: 6 = 2 + 2 + 1.0 * 0.5 * a_k -> a_k = 4
  auto v = curr - prev;
  auto a = accelMidpoint(v, curr, next, 1.f);
  EXPECT_THAT(a.data(), equalsHVector({0.f, 4.f, 0.f}));

  // Start with: p_k+1 = 2 * p_k - p_k-1 + a_k * dt^2
  // Solving for a_k: 6 = 2 * 2 - 0 + a_k * 1.0 -> a_k = 2
  a = accelBasicVerlet(prev, curr, next, 1.f);
  EXPECT_THAT(a.data(), equalsHVector({0.f, 2.f, 0.f}));

  // Both verlet methods should give the same result for constant timestamps.
  a = accelVerlet(prev, curr, next, 1.f, 1.f);
  EXPECT_THAT(a.data(), equalsHVector({0.f, 2.f, 0.f}));
}

TEST_F(DevicePoseTest, AccelDifferentDts) {
  auto dt = 7.f;
  auto prev = HVector3(2.f, 0.f, 0.f);
  auto curr = HVector3(2.f, 2.f, 0.f);
  auto next = HVector3(2.f, 6.f, 0.f);

  // Acceleration is porportional to 1 / (dt^2).
  auto a = accelVerlet(prev, curr, next, dt, dt);
  auto a2 = accelVerlet(prev, curr, next, 1.f, 1.f);
  EXPECT_THAT(a.data(), equalsHVector(a2 * (1.f / std::pow(dt, 2.f))));

  a = accelBasicVerlet(prev, curr, next, dt);
  a2 = accelBasicVerlet(prev, curr, next, 1.f);
  EXPECT_THAT(a.data(), equalsHVector(a2 * (1.f / std::pow(dt, 2.f))));
}

TEST_F(DevicePoseTest, AccelChangingDt) {
  auto prevDt = 1.f;                    // From time k - 1 -> k.
  auto dt = 2.f;                        // From time k -> k + 1.
  auto prev = HVector3(2.f, 0.f, 0.f);  // Time k - 1.
  auto curr = HVector3(2.f, 2.f, 0.f);  // Time k.
  auto next = HVector3(2.f, 6.f, 0.f);  // Time k + 1.

  // If our velocity was already 2, then to go 4 units without any acceleration we just need to wait
  // for twice as long.
  // p_k+1 = p_k + (p_k - p_k-1) * (dt_k / dt_k-1) + 0.5 * a_k * dt_k * (dt_k +_dt_k-1)
  // 6 = 2 + (2 - 0) * (2 / 1) + 0.5 * a_k * 2 * (2 + 1) -> 6 = 2 + 4 + 2 * a_k -> a_k = 0
  auto a = accelVerlet(prev, curr, next, prevDt, dt);
  EXPECT_THAT(a.data(), equalsHVector(HVector3()));
}
}  // namespace c8
