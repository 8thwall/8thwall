// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Unit tests for HVector classes.

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":hvector",
    ":vector",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x0d26ba75);

#include "c8/hvector.h"
#include "c8/vector.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace c8 {

class HVectorTest : public ::testing::Test {};

using testing::Pointwise;

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsHVector(const HVector2 &vec) {
  return Pointwise(AreWithin(0.0001), vec.data());
}

decltype(auto) equalsHVector(const HVector3 &vec) {
  return Pointwise(AreWithin(0.0001), vec.data());
}

decltype(auto) equalsHVector(float x, float y) {
  return Pointwise(AreWithin(0.0001), HVector2(x, y).data());
}

decltype(auto) equalsHVector(float x, float y, float z) {
  return Pointwise(AreWithin(0.0001), HVector3(x, y, z).data());
}

TEST_F(HVectorTest, Vector2InitTest) {
  HVector2 p0 = {1.0f, 2.0f};
  EXPECT_FLOAT_EQ(p0.x(), 1.0f);
  EXPECT_FLOAT_EQ(p0.y(), 2.0f);
  EXPECT_FLOAT_EQ(p0.data()[0], 1.0f);
  EXPECT_FLOAT_EQ(p0.data()[1], 2.0f);
  EXPECT_FLOAT_EQ(p0.raw()[0], 1.0f);
  EXPECT_FLOAT_EQ(p0.raw()[1], 2.0f);
  EXPECT_FLOAT_EQ(p0.raw()[2], 0.0f);

  HVector2 p1 = std::array<float, 2>{{1.0f, 2.0f}};
  EXPECT_FLOAT_EQ(p1.x(), 1.0f);
  EXPECT_FLOAT_EQ(p1.y(), 2.0f);
  EXPECT_FLOAT_EQ(p1.data()[0], 1.0f);
  EXPECT_FLOAT_EQ(p1.data()[1], 2.0f);
  EXPECT_FLOAT_EQ(p1.raw()[0], 1.0f);
  EXPECT_FLOAT_EQ(p1.raw()[1], 2.0f);
  EXPECT_FLOAT_EQ(p1.raw()[2], 0.0f);

  float foo[2] = {1.0f, 2.0f};
  HVector2 p2(foo);
  EXPECT_FLOAT_EQ(p2.x(), 1.0f);
  EXPECT_FLOAT_EQ(p2.y(), 2.0f);
  EXPECT_FLOAT_EQ(p2.data()[0], 1.0f);
  EXPECT_FLOAT_EQ(p2.data()[1], 2.0f);
  EXPECT_FLOAT_EQ(p2.raw()[0], 1.0f);
  EXPECT_FLOAT_EQ(p2.raw()[1], 2.0f);
  EXPECT_FLOAT_EQ(p2.raw()[2], 0.0f);
}

TEST_F(HVectorTest, Vector3InitTest) {
  HVector3 p0 = {1.0f, 2.0f, 3.0f};
  EXPECT_FLOAT_EQ(p0.x(), 1.0f);
  EXPECT_FLOAT_EQ(p0.y(), 2.0f);
  EXPECT_FLOAT_EQ(p0.z(), 3.0f);
  EXPECT_FLOAT_EQ(p0.data()[0], 1.0f);
  EXPECT_FLOAT_EQ(p0.data()[1], 2.0f);
  EXPECT_FLOAT_EQ(p0.data()[2], 3.0f);
  EXPECT_FLOAT_EQ(p0.raw()[0], 1.0f);
  EXPECT_FLOAT_EQ(p0.raw()[1], 2.0f);
  EXPECT_FLOAT_EQ(p0.raw()[2], 3.0f);
  EXPECT_FLOAT_EQ(p0.raw()[3], 0.0f);

  HVector3 p1 = std::array<float, 3>{{1.0f, 2.0f, 3.0f}};
  EXPECT_FLOAT_EQ(p1.x(), 1.0f);
  EXPECT_FLOAT_EQ(p1.y(), 2.0f);
  EXPECT_FLOAT_EQ(p1.z(), 3.0f);
  EXPECT_FLOAT_EQ(p1.data()[0], 1.0f);
  EXPECT_FLOAT_EQ(p1.data()[1], 2.0f);
  EXPECT_FLOAT_EQ(p1.data()[2], 3.0f);
  EXPECT_FLOAT_EQ(p1.raw()[0], 1.0f);
  EXPECT_FLOAT_EQ(p1.raw()[1], 2.0f);
  EXPECT_FLOAT_EQ(p1.raw()[2], 3.0f);
  EXPECT_FLOAT_EQ(p1.raw()[3], 0.0f);
}

TEST_F(HVectorTest, Vector2MathTest) {
  HVector2 v0 = {1.0f, 3.0f};
  HVector2 v1 = {3.0f, 4.0f};

  EXPECT_FLOAT_EQ(v0.dot(v1), 15.0f);
  EXPECT_FLOAT_EQ(v1.l2Norm(), 5.0f);

  HVector2 unit0 = v0.unit();
  EXPECT_THAT(unit0.data(), equalsHVector(0.31622776f, 0.94868326f));
  EXPECT_FLOAT_EQ(unit0.l2Norm(), 1.0f);

  HVector2 v2 = {1.0f, 3.0f};
  HVector2 v3 = {3.0f, 4.0f};

  v2 += v3;

  EXPECT_THAT(v2.data(), equalsHVector(4.0f, 7.0f));

  HVector2 v4 = {2.0f, 0.5f};
  v2 -= v4;

  EXPECT_THAT(v2.data(), equalsHVector(2.0f, 6.5f));
}

TEST_F(HVectorTest, Vector3MathTest) {
  HVector3 v0 = {1.0f, 2.0f, 3.0f};
  HVector3 v1 = {3.0f, 4.0f, 5.0f};

  EXPECT_FLOAT_EQ(v0.dot(v1), 26.0f);
  EXPECT_FLOAT_EQ(v1.l2Norm(), 7.0710678f);

  HVector3 v2 = {2.0f, -1.0f, 4.0f};
  HVector3 unit2 = v2.unit();
  EXPECT_THAT(unit2.data(), equalsHVector(0.43643576f, -0.21821788f, 0.87287152f));
  EXPECT_FLOAT_EQ(unit2.l2Norm(), 1.0f);

  HVector3 xp = v0.cross(v1);
  EXPECT_THAT(xp.data(), equalsHVector(-2.0f, 4.0f, -2.0f));

  EXPECT_THAT((v0 + v1).data(), equalsHVector(4.0f, 6.0f, 8.0f));
  EXPECT_THAT((v1 + v0).data(), equalsHVector(4.0f, 6.0f, 8.0f));
  EXPECT_THAT((v0 - v1).data(), equalsHVector(-2.0f, -2.0f, -2.0f));
  EXPECT_THAT((v1 - v0).data(), equalsHVector(2.0f, 2.0f, 2.0f));

  HVector3 v3 = {1.0f, 3.0f, 1.0f};
  HVector3 v4 = {3.0f, 4.0f, 2.0f};
  v3 += v4;

  EXPECT_THAT(v3.data(), equalsHVector(4.0f, 7.0f, 3.0f));

  HVector3 v5 = {2.0f, 0.5f, 0.0f};
  v3 -= v5;

  EXPECT_THAT(v3.data(), equalsHVector(2.0f, 6.5f, 3.0f));
}

TEST_F(HVectorTest, TestVector3Angles) {
  // Compute the degrees between two vectors
  HVector3 v0 = {1.0f, 0.0f, 0.0f};
  HVector3 v1 = {0.0f, 1.0f, 0.0f};

  EXPECT_FLOAT_EQ(v0.angleD(v1), 90.0f);
  EXPECT_FLOAT_EQ(v1.angleD(v0), 90.0f);

  v0 = {1.0f, 1.0f, 0.0f};
  v1 = {1.0f, 0.0f, 0.0f};

  EXPECT_FLOAT_EQ(v0.angleD(v1), 45.0f);
  EXPECT_FLOAT_EQ(v1.angleD(v0), 45.0f);
}

}  // namespace c8
