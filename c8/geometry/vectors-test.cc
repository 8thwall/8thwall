// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":vectors",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x5c5de149);

#include "c8/geometry/vectors.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace c8 {

class VectorsTest : public ::testing::Test {};

using testing::Pointwise;

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

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsHVector(const HVector3 &vec) {
  return Pointwise(AreWithin(0.0001), vec.data());
}

TEST_F(VectorsTest, RotationToVector) {
  auto dest = HVector3{0.75f, 1.5f, -3.0f};

  // Case: Arbitrary vectors.
  auto src = HVector3{1.0f, -1.0f, 1.0f};

  // Rotated vector has the same size as the src, and the same direction as the dest.
  auto rotated = rotationToVector(src, dest).toRotationMat() * src;
  EXPECT_FLOAT_EQ(src.l2Norm(), rotated.l2Norm());
  EXPECT_FLOAT_EQ(1.0f, dest.unit().dot(rotated.unit()));

  // Case: 0-angle vectors.
  src = HVector3{1.5f, 3.0f, -6.0f};
  rotated = rotationToVector(src, dest).toRotationMat() * src;
  EXPECT_FLOAT_EQ(src.l2Norm(), rotated.l2Norm());
  EXPECT_FLOAT_EQ(1.0f, dest.unit().dot(rotated.unit()));

  // Case: Near 180-angle vectors 1.
  src = HVector3{-.74f, -1.6f, 2.9f};
  rotated = rotationToVector(src, dest).toRotationMat() * src;
  EXPECT_FLOAT_EQ(src.l2Norm(), rotated.l2Norm());
  EXPECT_FLOAT_EQ(1.0f, dest.unit().dot(rotated.unit()));

  // Case: Near 180-angle vectors 2.
  src = HVector3{-.76f, -1.4f, 3.1f};
  rotated = rotationToVector(src, dest).toRotationMat() * src;
  EXPECT_FLOAT_EQ(src.l2Norm(), rotated.l2Norm());
  EXPECT_FLOAT_EQ(1.0f, dest.unit().dot(rotated.unit()));

  // Case: Near 180-angle vectors 3.
  src = HVector3{-.76f, -1.6f, 3.1f};
  rotated = rotationToVector(src, dest).toRotationMat() * src;
  EXPECT_NEAR(src.l2Norm(), rotated.l2Norm(), 2e-6f);
  EXPECT_FLOAT_EQ(1.0f, dest.unit().dot(rotated.unit()));

  // Case: Near 180-angle vectors 4.
  src = HVector3{-.74f, -1.6f, 3.1f};
  rotated = rotationToVector(src, dest).toRotationMat() * src;
  EXPECT_FLOAT_EQ(src.l2Norm(), rotated.l2Norm());
  EXPECT_FLOAT_EQ(1.0f, dest.unit().dot(rotated.unit()));

  // Case: 180-angle vectors.
  src = HVector3{-.75f, -1.5f, 3.0f};
  rotated = rotationToVector(src, dest).toRotationMat() * src;
  EXPECT_NEAR(src.l2Norm(), rotated.l2Norm(), 2e-6f);
  EXPECT_FLOAT_EQ(1.0f, dest.unit().dot(rotated.unit()));

  // Case: 0-length
  src = HVector3{0.0f, 0.0f, 0.0f};
  rotated = rotationToVector(src, dest).toRotationMat() * src;
  EXPECT_FLOAT_EQ(src.l2Norm(), rotated.l2Norm());

  // Case: x-axis-aligned 180 degrees (test no infinite recursion) 1
  src = HVector3{1.0f, 0.0f, 0.0f};
  dest = HVector3{-1.0f, 0.0f, 0.0f};
  rotated = rotationToVector(src, dest).toRotationMat() * src;
  EXPECT_FLOAT_EQ(src.l2Norm(), rotated.l2Norm());
  EXPECT_FLOAT_EQ(1.0f, dest.unit().dot(rotated.unit()));

  // Case: x-axis-aligned 180 degrees (test no infinite recursion) 2
  src = HVector3{-1.0f, 0.0f, 0.0f};
  dest = HVector3{1.0f, 0.0f, 0.0f};
  rotated = rotationToVector(src, dest).toRotationMat() * src;
  EXPECT_FLOAT_EQ(src.l2Norm(), rotated.l2Norm());
  EXPECT_FLOAT_EQ(1.0f, dest.unit().dot(rotated.unit()));

  // Case: y-axis-aligned 180 degrees (test no infinite recursion) 1
  src = HVector3{0.0f, 1.0f, 0.0f};
  dest = HVector3{0.0f, -1.0f, 0.0f};
  rotated = rotationToVector(src, dest).toRotationMat() * src;
  EXPECT_FLOAT_EQ(src.l2Norm(), rotated.l2Norm());
  EXPECT_FLOAT_EQ(1.0f, dest.unit().dot(rotated.unit()));

  // Case: y-axis-aligned 180 degrees (test no infinite recursion) 2
  src = HVector3{0.0f, -1.0f, 0.0f};
  dest = HVector3{0.0f, 1.0f, 0.0f};
  rotated = rotationToVector(src, dest).toRotationMat() * src;
  EXPECT_FLOAT_EQ(src.l2Norm(), rotated.l2Norm());
  EXPECT_FLOAT_EQ(1.0f, dest.unit().dot(rotated.unit()));

  // Case: no src, src should be forward along the z axis
  src = HVector3{0.0f, 0.0f, 1.0f};
  rotated = rotationToVector(dest).toRotationMat() * src;
  EXPECT_FLOAT_EQ(src.l2Norm(), rotated.l2Norm());
  EXPECT_FLOAT_EQ(1.0f, dest.unit().dot(rotated.unit()));
}

TEST_F(VectorsTest, AngularVelocity) {
  for (int x = 0; x <= 360; x += 10) {
    for (int y = 0; y <= 360; y += 10) {
      for (int z = 0; z <= 360; z += 10) {
        auto start = Quaternion();
        auto end = Quaternion::fromHMatrix(HMatrixGen::rotationD(x, y, z));

        // For a timestep of 1, we recover the rotational diffence just with angularVelocity().
        auto omega = angularVelocity(start, end, 1.f);
        EXPECT_TRUE(AreEqual(Quaternion::fromAxisAngle(omega), end));
        auto delta = rotationOverTime(omega, 1.f);
        EXPECT_TRUE(AreEqual(delta, end));

        // For variable timesteps, we recover the rotational difference with angularVelocity() +
        // rotationOverTime().
        for (float dt = 0.1f; dt < 1.f; dt += .1f) {
          omega = angularVelocity(start, end, dt);
          auto rotation = rotationOverTime(omega, dt);
          EXPECT_TRUE(AreEqual(rotation, end));
        }
      }
    }
  }
}

TEST_F(VectorsTest, AngularVelocityNegatedQuaternions) {
  auto q1 = Quaternion::fromHMatrix(HMatrixGen::rotationD(0.0f, 0.0f, 0.0f));
  auto q2 = Quaternion::fromHMatrix(HMatrixGen::rotationD(10.0f, -20.0f, 90.0f));
  auto q2n = q2.negate();
  auto omega1 = angularVelocity(q1, q2, 1.f);
  auto omega2 = angularVelocity(q1, q2n, 1.f);
  EXPECT_THAT(omega1.data(), equalsHVector(omega2));
}

TEST_F(VectorsTest, Interpolate) {
  auto src = HVector3{1.5f, 3.0f, -6.0f};
  auto dest = HVector3{0.75f, 1.5f, -3.0f};
  auto halfDistance = (dest + src) * 0.5f;
  EXPECT_THAT(src.data(), equalsHVector(interpolate(src, dest, 0.f)));
  EXPECT_THAT(dest.data(), equalsHVector(interpolate(src, dest, 1.f)));
  EXPECT_THAT(halfDistance.data(), equalsHVector(interpolate(src, dest, .5f)));

  src = HVector3{0.0f, 0.0f, 0.0f};
  dest = HVector3{0.5f, 1.5f, -3.0f};
  halfDistance = (dest + src) * 0.5f;
  EXPECT_THAT(src.data(), equalsHVector(interpolate(src, dest, 0.f)));
  EXPECT_THAT(dest.data(), equalsHVector(interpolate(src, dest, 1.f)));
  EXPECT_THAT(halfDistance.data(), equalsHVector(interpolate(src, dest, .5f)));

  src = HVector3{0.0f, 0.0f, 0.0f};
  dest = HVector3{0.0f, 0.0f, 0.0f};
  halfDistance = (dest + src) * 0.5f;
  EXPECT_THAT(src.data(), equalsHVector(interpolate(src, dest, 0.f)));
  EXPECT_THAT(dest.data(), equalsHVector(interpolate(src, dest, 1.f)));
  EXPECT_THAT(halfDistance.data(), equalsHVector(interpolate(src, dest, .5f)));
}

}  // namespace c8
