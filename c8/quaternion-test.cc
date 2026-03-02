// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":quaternion",
    "//c8/geometry:egomotion",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xa78e2c57);

#include <gtest/gtest.h>

#include <limits>

#include "c8/geometry/egomotion.h"
#include "c8/quaternion.h"

namespace c8 {

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

float toRadians(float degrees) { return M_PI * degrees / 180.0f; }

class QuaternionTest : public ::testing::Test {};

TEST_F(QuaternionTest, ToEulerAngleDegrees) {
  float xDeg, yDeg, zDeg;
  Quaternion q(0.7071067811865476f, 0.7071067811865476f, 0.0f, 0.0f);

  q.toEulerAngleDegrees(&xDeg, &yDeg, &zDeg);

  EXPECT_FLOAT_EQ(90.0f, xDeg);
  EXPECT_FLOAT_EQ(0.0f, yDeg);
  EXPECT_FLOAT_EQ(0.0f, zDeg);
}

TEST_F(QuaternionTest, ToPitchYawRollDegreesPitch) {
  float xDeg, yDeg, zDeg;
  Quaternion q(0.7071067811865476f, 0.7071067811865476f, 0.0f, 0.0f);

  q.toPitchYawRollDegrees(&xDeg, &yDeg, &zDeg);

  EXPECT_NEAR(90.0f, xDeg, 2e-2f);
  EXPECT_FLOAT_EQ(0.0f, yDeg);
  EXPECT_FLOAT_EQ(0.0f, zDeg);

  EXPECT_NEAR(90.0f, q.pitchDegrees(), 2e-2f);
  EXPECT_FLOAT_EQ(0.0f, q.yawDegrees());
  EXPECT_FLOAT_EQ(0.0f, q.rollDegrees());
}

TEST_F(QuaternionTest, ToPitchYawRollDegreesYaw) {
  float xDeg, yDeg, zDeg;
  Quaternion q(0.7071067811865476f, 0.0f, 0.7071067811865476f, 0.0f);

  q.toPitchYawRollDegrees(&xDeg, &yDeg, &zDeg);

  EXPECT_FLOAT_EQ(0.0f, xDeg);
  EXPECT_FLOAT_EQ(90.0f, yDeg);
  EXPECT_FLOAT_EQ(0.0f, zDeg);

  EXPECT_FLOAT_EQ(0.0f, q.pitchDegrees());
  EXPECT_FLOAT_EQ(90.0f, q.yawDegrees());
  EXPECT_FLOAT_EQ(0.0f, q.rollDegrees());
}

TEST_F(QuaternionTest, ToPitchYawRollDegreesRoll) {
  float xDeg, yDeg, zDeg;
  Quaternion q(0.7071067811865476f, 0.0f, 0.0f, 0.7071067811865476f);

  q.toPitchYawRollDegrees(&xDeg, &yDeg, &zDeg);

  EXPECT_FLOAT_EQ(0.0f, xDeg);
  EXPECT_FLOAT_EQ(0.0f, yDeg);
  EXPECT_FLOAT_EQ(90.0f, zDeg);

  EXPECT_FLOAT_EQ(0.0f, q.pitchDegrees());
  EXPECT_FLOAT_EQ(0.0f, q.yawDegrees());
  EXPECT_FLOAT_EQ(90.0f, q.rollDegrees());
}

TEST_F(QuaternionTest, ToPitchYawRollDegrees) {
  float yawDeg = 30.0f;
  float pitchDeg = -20.0f;
  float rollDeg = -10.0f;

  auto yawQ = Quaternion::yDegrees(yawDeg);
  auto pitchQ = Quaternion::xDegrees(pitchDeg);
  auto rollQ = Quaternion::zDegrees(rollDeg);

  auto fullQ = updateWorldPosition(updateWorldPosition(yawQ, pitchQ), rollQ);

  float xDeg, yDeg, zDeg;

  fullQ.toPitchYawRollDegrees(&xDeg, &yDeg, &zDeg);

  EXPECT_FLOAT_EQ(pitchDeg, xDeg);
  EXPECT_FLOAT_EQ(yawDeg, yDeg);
  EXPECT_FLOAT_EQ(rollDeg, zDeg);

  EXPECT_FLOAT_EQ(pitchDeg, fullQ.pitchDegrees());
  EXPECT_FLOAT_EQ(yawDeg, fullQ.yawDegrees());
  EXPECT_FLOAT_EQ(rollDeg, fullQ.rollDegrees());

  auto fromQ = Quaternion::fromPitchYawRollDegrees(pitchDeg, yawDeg, rollDeg);
  EXPECT_FLOAT_EQ(fromQ.w(), fullQ.w());
  EXPECT_FLOAT_EQ(fromQ.x(), fullQ.x());
  EXPECT_FLOAT_EQ(fromQ.y(), fullQ.y());
  EXPECT_FLOAT_EQ(fromQ.z(), fullQ.z());
}

TEST_F(QuaternionTest, ToPitchYawRollDegreesExtremeAngles) {
  float yawDeg = -179.9f;
  float pitchDeg = -89.9f;
  float rollDeg = 179.9f;

  auto yawQ = Quaternion::yDegrees(yawDeg);
  auto pitchQ = Quaternion::xDegrees(pitchDeg);
  auto rollQ = Quaternion::zDegrees(rollDeg);

  auto fullQ = updateWorldPosition(updateWorldPosition(yawQ, pitchQ), rollQ);

  float xDeg, yDeg, zDeg;

  fullQ.toPitchYawRollDegrees(&xDeg, &yDeg, &zDeg);

  EXPECT_NEAR(pitchDeg, xDeg, 6e-3f);
  EXPECT_FLOAT_EQ(yawDeg, yDeg);
  EXPECT_FLOAT_EQ(rollDeg, zDeg);

  EXPECT_NEAR(pitchDeg, fullQ.pitchDegrees(), 6e-3f);
  EXPECT_FLOAT_EQ(yawDeg, fullQ.yawDegrees());
  EXPECT_FLOAT_EQ(rollDeg, fullQ.rollDegrees());

  auto fromQ = Quaternion::fromPitchYawRollDegrees(pitchDeg, yawDeg, rollDeg);
  EXPECT_FLOAT_EQ(fromQ.w(), fullQ.w());
  EXPECT_FLOAT_EQ(fromQ.x(), fullQ.x());
  EXPECT_FLOAT_EQ(fromQ.y(), fullQ.y());
  EXPECT_FLOAT_EQ(fromQ.z(), fullQ.z());
}

TEST_F(QuaternionTest, ToPitchYawRollDegreesIdentity) {
  float yawDeg = 0.0f;
  float pitchDeg = 0.0f;
  float rollDeg = 0.0f;

  auto yawQ = Quaternion::yDegrees(yawDeg);
  auto pitchQ = Quaternion::xDegrees(pitchDeg);
  auto rollQ = Quaternion::zDegrees(rollDeg);

  auto fullQ = updateWorldPosition(updateWorldPosition(yawQ, pitchQ), rollQ);

  float xDeg, yDeg, zDeg;

  fullQ.toPitchYawRollDegrees(&xDeg, &yDeg, &zDeg);

  EXPECT_FLOAT_EQ(pitchDeg, xDeg);
  EXPECT_FLOAT_EQ(yawDeg, yDeg);
  EXPECT_FLOAT_EQ(rollDeg, zDeg);

  EXPECT_FLOAT_EQ(pitchDeg, fullQ.pitchDegrees());
  EXPECT_FLOAT_EQ(yawDeg, fullQ.yawDegrees());
  EXPECT_FLOAT_EQ(rollDeg, fullQ.rollDegrees());

  auto fromQ = Quaternion::fromPitchYawRollDegrees(pitchDeg, yawDeg, rollDeg);
  EXPECT_FLOAT_EQ(fromQ.w(), fullQ.w());
  EXPECT_FLOAT_EQ(fromQ.x(), fullQ.x());
  EXPECT_FLOAT_EQ(fromQ.y(), fullQ.y());
  EXPECT_FLOAT_EQ(fromQ.z(), fullQ.z());
}

TEST_F(QuaternionTest, ToPitchYawRollDegreesAmbiguousRoll) {
  // When the camera is pitched forward 90 degrees, yaw and roll are in the same plane.
  // In this case, we rotate 15 degrees, pitch forward 90 degrees, and then roll 30 degrees in the
  // opposite direction from the original yaw. The expectations here is that there should be no
  // roll and -15 degrees yaw.
  float yawDeg = 15.0f;
  float pitchDeg = 90.0f;
  float rollDeg = 30.0f;

  auto yawQ = Quaternion::yDegrees(yawDeg);
  auto pitchQ = Quaternion::xDegrees(pitchDeg);
  auto rollQ = Quaternion::zDegrees(rollDeg);

  auto fullQ = updateWorldPosition(updateWorldPosition(yawQ, pitchQ), rollQ);

  float xDeg, yDeg, zDeg;

  fullQ.toPitchYawRollDegrees(&xDeg, &yDeg, &zDeg);

  EXPECT_NEAR(xDeg, pitchDeg, 2e-2f);
  EXPECT_FLOAT_EQ(yDeg, -yawDeg);
  EXPECT_FLOAT_EQ(zDeg, 0.0f);
}

TEST_F(QuaternionTest, XYZRadians) {
  Vector<float> rads = {0.0f, 0.1f, M_PI / 4.0f, M_PI / 3.0f, M_PI / 2.0f, 3.0f, M_PI};
  for (auto r : rads) {
    {
      auto a = Quaternion::xRadians(r);
      auto b = Quaternion::fromHMatrix(HMatrixGen::xRadians(r));
      EXPECT_FLOAT_EQ(a.w(), b.w());
      EXPECT_FLOAT_EQ(a.x(), b.x());
      EXPECT_FLOAT_EQ(a.y(), b.y());
      EXPECT_FLOAT_EQ(a.z(), b.z());
    }
    {
      auto a = Quaternion::yRadians(r);
      auto b = Quaternion::fromHMatrix(HMatrixGen::yRadians(r));
      EXPECT_FLOAT_EQ(a.w(), b.w());
      EXPECT_FLOAT_EQ(a.x(), b.x());
      EXPECT_FLOAT_EQ(a.y(), b.y());
      EXPECT_FLOAT_EQ(a.z(), b.z());
    }
    {
      auto a = Quaternion::zRadians(r);
      auto b = Quaternion::fromHMatrix(HMatrixGen::zRadians(r));
      EXPECT_FLOAT_EQ(a.w(), b.w());
      EXPECT_FLOAT_EQ(a.x(), b.x());
      EXPECT_FLOAT_EQ(a.y(), b.y());
      EXPECT_FLOAT_EQ(a.z(), b.z());
    }
  }
}

TEST_F(QuaternionTest, TestAxisAngleToQuaternion) {
  const float kPi = 3.14159265358979323846f;
  const float kHalfSqrt2 = 0.707106781186547524401f;

  HVector3 axisAngle = {kPi / 2.0f, 0.0f, 0.0f};
  Quaternion expected = {kHalfSqrt2, kHalfSqrt2, 0.0f, 0.0f};
  Quaternion q = Quaternion::fromAxisAngle(axisAngle);
  EXPECT_FLOAT_EQ(expected.w(), q.w());
  EXPECT_FLOAT_EQ(expected.x(), q.x());
  EXPECT_FLOAT_EQ(expected.y(), q.y());
  EXPECT_FLOAT_EQ(expected.z(), q.z());

  // Very small value that could potentially cause underflow.
  float theta = pow(std::numeric_limits<float>::min(), 0.75f);
  axisAngle = {theta, 0.0f, 0.0f};
  expected = {cos(theta / 2.0f), sin(theta / 2.0f), 0.0f, 0.0f};
  q = Quaternion::fromAxisAngle(axisAngle);
  EXPECT_FLOAT_EQ(expected.w(), q.w());
  EXPECT_FLOAT_EQ(expected.x(), q.x());
  EXPECT_FLOAT_EQ(expected.y(), q.y());
  EXPECT_FLOAT_EQ(expected.z(), q.z());

  // Test that if theta is 0, it will return identity.
  axisAngle = {0.0f, 0.0f, 0.0f};
  expected = {1.0f, 0.0f, 0.0f, 0.0f};
  q = Quaternion::fromAxisAngle(axisAngle);
  EXPECT_FLOAT_EQ(expected.w(), q.w());
  EXPECT_FLOAT_EQ(expected.x(), q.x());
  EXPECT_FLOAT_EQ(expected.y(), q.y());
  EXPECT_FLOAT_EQ(expected.z(), q.z());
}

TEST_F(QuaternionTest, TestQuaternionToAxisAngle) {
  Quaternion q = {1, 0, 0, 0};
  HVector3 expected = {0.0f, 0.0f, 0.0f};
  HVector3 output = q.toAxisAngle();
  EXPECT_FLOAT_EQ(expected.x(), output.x());
  EXPECT_FLOAT_EQ(expected.y(), output.y());
  EXPECT_FLOAT_EQ(expected.z(), output.z());
}

TEST_F(QuaternionTest, TestQuaternionToAxisAngleAndBack) {
  Vector<Quaternion> quaternions = {
    {0.2f, 0.5f, 0.3f, 0.5f},
    {0.4f, 0.3f, 0.9f, 0.6f},
    {0.8f, 0.8f, 0.0f, 0.2f},
    {0.1f, 0.7f, 0.1f, 0.9f},
    {0.9f, 0.4f, 0.2f, 0.0f},
  };
  for (auto &q : quaternions) {
    // Normalize the quaternion
    q = q.normalize();

    HVector axisAngle = q.toAxisAngle();
    Quaternion output = Quaternion::fromAxisAngle(axisAngle);
    EXPECT_FLOAT_EQ(output.norm(), 1.0f);
    EXPECT_FLOAT_EQ(q.x(), output.x());
    EXPECT_FLOAT_EQ(q.y(), output.y());
    EXPECT_FLOAT_EQ(q.z(), output.z());
    EXPECT_FLOAT_EQ(q.w(), output.w());
  }
}

TEST_F(QuaternionTest, RotationMatToQuaternion) {
  // A quaternion sampled from the unit sphere.
  Quaternion q(0.5143245711561157f, 0.43316012270641296f, 0.1292451970381261f, -0.728792304188662f);

  // quaternion -> rotation mat -> quaternion
  auto r = q.toRotationMat();
  auto q2 = Quaternion::fromHMatrix(r);
  EXPECT_FLOAT_EQ(q2.w(), q.w());
  EXPECT_FLOAT_EQ(q2.x(), q.x());
  EXPECT_FLOAT_EQ(q2.y(), q.y());
  EXPECT_FLOAT_EQ(q2.z(), q.z());
}

TEST_F(QuaternionTest, SMatToQuaternion) {
  auto mat = HMatrixGen::scale(2.0f, 3.0f, 4.0f);
  auto res = Quaternion::fromTrsMatrix(mat);
  EXPECT_FLOAT_EQ(res.w(), 1.f);
  EXPECT_FLOAT_EQ(res.x(), 0.f);
  EXPECT_FLOAT_EQ(res.y(), 0.f);
  EXPECT_FLOAT_EQ(res.z(), 0.f);
}

TEST_F(QuaternionTest, RsMatToQuaternion) {
  Quaternion q(0.8775826f, 0.4794255f, 0.0f, 0.0f);
  auto mat = q.toRotationMat() * HMatrixGen::scale(2.0f, 3.0f, 4.0f);
  auto res = Quaternion::fromTrsMatrix(mat);
  EXPECT_FLOAT_EQ(res.w(), q.w());
  EXPECT_FLOAT_EQ(res.x(), q.x());
  EXPECT_FLOAT_EQ(res.y(), q.y());
  EXPECT_FLOAT_EQ(res.z(), q.z());
}

TEST_F(QuaternionTest, NegativeXRsMatToQuaternion) {
  Quaternion q(0, 0, 1, 0);
  auto mat = q.toRotationMat() * HMatrixGen::scale(-2.0f, 3.0f, 4.0f);
  auto res = Quaternion::fromTrsMatrix(mat);
  EXPECT_FLOAT_EQ(res.w(), q.w());
  EXPECT_FLOAT_EQ(res.x(), q.x());
  EXPECT_FLOAT_EQ(res.y(), q.y());
  EXPECT_FLOAT_EQ(res.z(), q.z());
}

TEST_F(QuaternionTest, NegativeYRsMatToQuaternion) {
  Quaternion q(0.9522119508747534, 0.2971683846688858, 0.03293589830081514, 0.06243859677875135);
  auto mat = q.toRotationMat() * HMatrixGen::scale(2.0f, -3.0f, 4.0f);
  auto res = Quaternion::fromTrsMatrix(mat);

  // NOTE(christoph): Our matrix logic always normalizes to negative x if there is mirroring,
  // (const auto invXScale = detSign / std::sqrt(xScale2))
  // The final matrix will be as if x is mirrored instead, but has this angle adjustment.
  auto adjustment = Quaternion::fromHMatrix(HMatrixGen::scale(-1.0f, -1.0f, 1.0f));
  auto expected = q.times(adjustment);

  EXPECT_FLOAT_EQ(res.w(), expected.w());
  EXPECT_FLOAT_EQ(res.x(), expected.x());
  EXPECT_FLOAT_EQ(res.y(), expected.y());
  EXPECT_FLOAT_EQ(res.z(), expected.z());
}

TEST_F(QuaternionTest, UniformNegativeSrMatToQuaternion) {
  Quaternion q(0.9900333, 0.0993347, -0.0099667, 0.0993347);
  auto justRot = q.toRotationMat();
  auto justScale = HMatrixGen::scale(-1.0f, 1.0f, 1.0f);
  auto mat = justScale * justRot;
  auto res = Quaternion::fromTrsMatrix(mat);
  EXPECT_FLOAT_EQ(res.w(), 0.9900332817437772f);
  EXPECT_FLOAT_EQ(res.x(), 0.09933470172744087f);
  EXPECT_FLOAT_EQ(res.y(), 0.009966700176739998f);
  EXPECT_FLOAT_EQ(res.z(), -0.099334701726748f);
}

TEST_F(QuaternionTest, EulerAnglesToQuaternion) {
  // A quaternion sampled from the unit sphere.
  Quaternion q(0.5143245711561157f, 0.43316012270641296f, 0.1292451970381261f, -0.728792304188662f);

  // quaternion -> angle -> rotation mat -> quaternion
  float xDeg, yDeg, zDeg;
  q.toEulerAngleDegrees(&xDeg, &yDeg, &zDeg);
  auto q2 = Quaternion::fromEulerAngleDegrees(xDeg, yDeg, zDeg);
  EXPECT_FLOAT_EQ(q2.w(), q.w());
  EXPECT_FLOAT_EQ(q2.x(), q.x());
  EXPECT_FLOAT_EQ(q2.y(), q.y());
  EXPECT_FLOAT_EQ(q2.z(), q.z());
}

TEST_F(QuaternionTest, PitchYawRollToQuaternion) {
  // A quaternion sampled from the unit sphere.
  Quaternion q(0.5143245711561157f, 0.43316012270641296f, 0.1292451970381261f, -0.728792304188662f);

  // quaternion -> angle -> rotation mat -> quaternion
  float xDeg, yDeg, zDeg;
  q.toPitchYawRollDegrees(&xDeg, &yDeg, &zDeg);
  auto q2 = Quaternion::fromPitchYawRollDegrees(xDeg, yDeg, zDeg);
  EXPECT_FLOAT_EQ(q2.w(), q.w());
  EXPECT_FLOAT_EQ(q2.x(), q.x());
  EXPECT_FLOAT_EQ(q2.y(), q.y());
  EXPECT_FLOAT_EQ(q2.z(), q.z());
}

TEST_F(QuaternionTest, QuaternionDelta) {
  // Quaternions sampled from the unit sphere.
  Quaternion q1(
    0.5143245711561157f, 0.43316012270641296f, 0.1292451970381261f, -0.728792304188662f);
  Quaternion q2(
    0.7744079932607568f, 0.60921278119571f, -0.12202887220438137f, -0.1194194353954246f);

  auto qd = q1.delta(q2);
  auto r1 = q1.toRotationMat();
  auto rd = qd.toRotationMat();

  auto q2r = Quaternion::fromHMatrix(rd * r1);

  EXPECT_NEAR(q2.w(), q2r.w(), 1e-6);
  EXPECT_NEAR(q2.x(), q2r.x(), 1e-6);
  EXPECT_NEAR(q2.y(), q2r.y(), 1e-6);
  EXPECT_NEAR(q2.z(), q2r.z(), 1e-6);
}

void TestInterpolation(Quaternion q1, Quaternion q2) {
  float distance = q1.radians(q2);
  float halfDistance = distance / 2.0f;
  auto q1Negated = q1.negate();
  auto q2Negated = q2.negate();

  // Test blending with and without negated quaternions using 0.0f.
  auto blend0 = q1.interpolate(q2, 0.0f);
  auto blend0N1 = q1Negated.interpolate(q2, 0.0f);
  auto blend0N2 = q1.interpolate(q2Negated, 0.0f);
  auto blend0NBoth = q1Negated.interpolate(q2Negated, 0.0f);
  EXPECT_TRUE(AreEqual(q1, blend0));
  EXPECT_TRUE(AreEqual(q1, blend0N1));
  EXPECT_TRUE(AreEqual(q1, blend0N2));
  EXPECT_TRUE(AreEqual(q1, blend0NBoth));

  // Test blending with and without negated quaternions using 1.0f.
  auto blend1 = q1.interpolate(q2, 1.0f);
  auto blend1N1 = q1Negated.interpolate(q2, 1.0f);
  auto blend1N2 = q1.interpolate(q2Negated, 1.0f);
  auto blend1NBoth = q1Negated.interpolate(q2Negated, 1.0f);
  EXPECT_TRUE(AreEqual(q2, blend1));
  EXPECT_TRUE(AreEqual(q2, blend1N1));
  EXPECT_TRUE(AreEqual(q2, blend1N2));
  EXPECT_TRUE(AreEqual(q2, blend1NBoth));

  // Test distance of interpolated values.
  auto halfBlend = q1.interpolate(q2, 0.5f);
  auto halfBlendN1 = q1Negated.interpolate(q2, 0.5f);
  auto halfBlendN2 = q1.interpolate(q2Negated, 0.5f);
  auto halfBlendNBoth = q1Negated.interpolate(q2Negated, 0.5f);

  // I'm seeing differences like 0.016615964 vs 0.016544065
  EXPECT_NEAR(distance, blend0.radians(blend1), 3e-4);
  EXPECT_NEAR(halfDistance, blend0.radians(halfBlend), 3e-4);
  EXPECT_NEAR(halfDistance, halfBlend.radians(blend1), 3e-4);
  EXPECT_NEAR(halfDistance, blend0.radians(halfBlendN1), 3e-4);
  EXPECT_NEAR(halfDistance, halfBlendN1.radians(blend1), 3e-4);
  EXPECT_NEAR(halfDistance, blend0.radians(halfBlendN2), 3e-4);
  EXPECT_NEAR(halfDistance, halfBlendN2.radians(blend1), 3e-4);
  EXPECT_NEAR(halfDistance, blend0.radians(halfBlendNBoth), 3e-4);
  EXPECT_NEAR(halfDistance, halfBlendNBoth.radians(blend1), 3e-4);
}

void TestExpectedBlends(Quaternion q1, Quaternion q2, Quaternion expectedMidway) {
  TestInterpolation(q1, q2);
  auto q1Negated = q1.negate();
  auto q2Negated = q2.negate();

  auto blendMidway = q1.interpolate(q2, 0.5f);
  auto blendMidwayNegated1 = q1Negated.interpolate(q2, 0.5f);
  auto blendMidwayNegated2 = q1.interpolate(q2Negated, 0.5f);
  auto blendMidwayNegated3 = q1Negated.interpolate(q2Negated, 0.5f);

  EXPECT_TRUE(AreEqual(expectedMidway, blendMidway));
  EXPECT_TRUE(AreEqual(expectedMidway, blendMidwayNegated1));
  EXPECT_TRUE(AreEqual(expectedMidway, blendMidwayNegated2));
  EXPECT_TRUE(AreEqual(expectedMidway, blendMidwayNegated3));
}

TEST_F(QuaternionTest, TestInterpolation) {
  Quaternion q1 = Quaternion::fromHMatrix(HMatrixGen::rotationD(0.0f, 0.0f, 20.0f));
  Quaternion q2 = Quaternion::fromHMatrix(HMatrixGen::rotationD(0.0f, 0.0f, 40.0f));
  Quaternion expectedMidway = Quaternion::fromHMatrix(HMatrixGen::rotationD(0.0f, 0.0f, 30.0f));
  TestExpectedBlends(q1, q2, expectedMidway);

  q1 = Quaternion::fromHMatrix(HMatrixGen::rotationD(0.0f, 0.0f, 70.0f));
  q2 = Quaternion::fromHMatrix(HMatrixGen::rotationD(0.0f, 0.0f, -70.0f));
  expectedMidway = Quaternion::fromHMatrix(HMatrixGen::rotationD(0.0f, 0.0f, 0.0f));
  TestExpectedBlends(q1, q2, expectedMidway);

  TestInterpolation(
    {0.367712f, 0.123778f, -0.835141f, 0.38988f}, {-0.368608f, -0.130994f, 0.832328f, -0.392681f});

  TestInterpolation(
    {0.135882f, 0.0412288f, -0.917256f, 0.372125f},
    {-0.136954f, -0.0420794f, 0.919192f, -0.366822f});

  TestInterpolation(
    {0.194859f, 0.0546276f, -0.884757f, 0.419823f},
    {-0.191989f, -0.0466577f, 0.886522f, -0.418381f});

  TestInterpolation(
    {0.200892f, 0.0606602f, -0.882936f, 0.419985f},
    {-0.200211f, -0.0587565f, 0.883392f, -0.419621f});
}

TEST_F(QuaternionTest, InterpolateAcross180) {
  Quaternion a = Quaternion::zDegrees(210);
  Quaternion b = Quaternion::zDegrees(150);
  Quaternion expected = Quaternion::zDegrees(180);
  Quaternion actual = a.interpolate(b, 0.5f);
  EXPECT_TRUE(AreEqual(actual, expected));
}

TEST_F(QuaternionTest, InterpolateAcross180Negative) {
  Quaternion a = Quaternion::zDegrees(-150);
  Quaternion b = Quaternion::zDegrees(150);
  Quaternion expected = Quaternion::zDegrees(180);
  Quaternion actual = a.interpolate(b, 0.5f);
  EXPECT_TRUE(AreEqual(actual, expected));
}

TEST_F(QuaternionTest, InterpolateAcross180Negative2) {
  Quaternion a = Quaternion::zDegrees(-130);
  Quaternion b = Quaternion::zDegrees(130);
  Quaternion expected = Quaternion::zDegrees(-160);
  Quaternion actual = a.interpolate(b, 0.3f);
  EXPECT_TRUE(AreEqual(actual, expected));
}

TEST_F(QuaternionTest, InterpolateAcross180Negative3) {
  Quaternion a = Quaternion::zDegrees(-130);
  Quaternion b = Quaternion::zDegrees(130);
  Quaternion expected = Quaternion::zDegrees(160);
  Quaternion actual = a.interpolate(b, 0.7f);
  EXPECT_TRUE(AreEqual(actual, expected));
}

TEST_F(QuaternionTest, InterpolateAcross180Negative4) {
  Quaternion a = Quaternion::zDegrees(-130);
  Quaternion b = Quaternion::zDegrees(130);
  Quaternion expected = Quaternion::zDegrees(179);
  Quaternion actual = a.interpolate(b, 0.51f);
  EXPECT_TRUE(AreEqual(actual, expected));
}

TEST_F(QuaternionTest, QuaternionRadians) {
  Quaternion q1 = Quaternion::fromHMatrix(HMatrixGen::rotationD(0.0f, 0.0f, 0.0f));
  Quaternion qSmallDiff = Quaternion::fromHMatrix(HMatrixGen::rotationD(0.0f, 0.0f, 0.1f));
  Quaternion q2 = Quaternion::fromHMatrix(HMatrixGen::rotationD(0.0f, 0.0f, 90.0f));
  Quaternion q3 = Quaternion::fromHMatrix(HMatrixGen::rotationD(0.0f, 0.0f, 180.0f));
  Quaternion q4 = Quaternion::fromHMatrix(HMatrixGen::rotationD(0.0f, 0.0f, 270.0f));
  auto q2Inv = q2.inverse();
  auto q2Negated = q2.negate();

  EXPECT_NEAR(q1.radians(qSmallDiff), toRadians(0.1f), 2e-4);
  EXPECT_FLOAT_EQ(q1.radians(q2), toRadians(90.0f));
  EXPECT_FLOAT_EQ(q1.radians(q3), toRadians(180.0f));
  EXPECT_FLOAT_EQ(q1.radians(q4), toRadians(90.0f));
  EXPECT_FLOAT_EQ(q2.radians(q2Inv), toRadians(180.0f));
  EXPECT_FLOAT_EQ(q2.radians(q2Negated), toRadians(0.0f));

  for (int i = 0; i < 180; ++i) {
    Quaternion q = Quaternion::fromHMatrix(HMatrixGen::rotationD(0.0f, 0.0f, i));
    EXPECT_NEAR(q1.radians(q), toRadians(i), 3e-6f);
  }

  for (int i = 0; i < 180; ++i) {
    Quaternion q = Quaternion::fromHMatrix(HMatrixGen::rotationD(0.0f, i, 0.0f));
    EXPECT_NEAR(q1.radians(q), toRadians(i), 3e-6f);
  }

  for (int i = 0; i < 180; ++i) {
    Quaternion q = Quaternion::fromHMatrix(HMatrixGen::rotationD(i, 0.0f, 0.0f));
    EXPECT_NEAR(q1.radians(q), toRadians(i), 3e-6f);
  }

  Quaternion q5 = {0.969569f, 0.238495f, 0.0100453f, 0.0543614f};
  Quaternion q6 = {0.969569f, 0.238495f, 0.0100453f, 0.0543614f};

  EXPECT_FLOAT_EQ(q5.radians(q6), 0.0f);

  for (int i = 0; i < 180; ++i) {
    Quaternion q = Quaternion::fromHMatrix(HMatrixGen::rotationD(0.0f, i, 0.0f));
    q = q.times(q5);
    EXPECT_NEAR(q5.radians(q), toRadians(i), 3e-5f);
  }
}

TEST_F(QuaternionTest, QuaternionDist) {
  {
    Quaternion q = Quaternion::fromHMatrix(HMatrixGen::rotationD(0.0f, 0.0f, 0.0f));
    EXPECT_FLOAT_EQ(q.dist(Quaternion::fromHMatrix(HMatrixGen::rotationD(180.f, 0.f, 0.f))), 1.f);
    EXPECT_FLOAT_EQ(q.dist(Quaternion::fromHMatrix(HMatrixGen::rotationD(0.f, 180.f, 0.f))), 1.f);
    EXPECT_FLOAT_EQ(q.dist(Quaternion::fromHMatrix(HMatrixGen::rotationD(0.f, 0.f, 180.f))), 1.f);
  }

  {
    Quaternion q = Quaternion::fromHMatrix(HMatrixGen::rotationD(180.0f, 0.0f, 0.0f));
    EXPECT_FLOAT_EQ(q.dist(Quaternion::fromHMatrix(HMatrixGen::rotationD(0.f, 0.f, 0.f))), 1.f);
    EXPECT_FLOAT_EQ(q.dist(Quaternion::fromHMatrix(HMatrixGen::rotationD(0.f, 180.f, 0.f))), 1.f);
    EXPECT_FLOAT_EQ(q.dist(Quaternion::fromHMatrix(HMatrixGen::rotationD(0.f, 0.f, 180.f))), 1.f);
  }

  for (int x = 0; x <= 360; x += 20) {
    for (int y = 0; y <= 360; y += 20) {
      for (int z = 0; z <= 360; z += 20) {
        Quaternion q = Quaternion::fromHMatrix(HMatrixGen::rotationD(x, y, z));
        EXPECT_NEAR(q.dist(q), 0.f, 9e-7);
      }
    }
  }
}

TEST_F(QuaternionTest, MemoryLayout) {
  // Tests that a quaternion is layed out in memory as {x, y, z, w} so it can be directly uploaded
  // to GPU.
  Quaternion q = {1.0f, 2.0f, 3.0f, 4.0f};
  float *qPtr = reinterpret_cast<float *>(&q);
  EXPECT_EQ(qPtr[0], 2.0f);
  EXPECT_EQ(qPtr[1], 3.0f);
  EXPECT_EQ(qPtr[2], 4.0f);
  EXPECT_EQ(qPtr[3], 1.0f);
}

}  // namespace c8
