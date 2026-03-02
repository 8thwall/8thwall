// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":interpolation",
    "//c8:c8-log",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xe36eedc9);

#include "c8/c8-log.h"
#include "c8/geometry/interpolation.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::Eq;
using testing::Pointwise;

namespace c8 {

class InterpolationTest : public ::testing::Test {};

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }
decltype(auto) equalsVector(const HVector3 &v, float threshold = 1e-6) {
  return Pointwise(AreWithin(threshold), v.data());
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

TEST_F(InterpolationTest, lessThanIndexSorted) {
  Vector<float> xs{1, 2, 3, 4};
  EXPECT_EQ(lessThanIndexSorted(xs, 0.f), 0);
  EXPECT_EQ(lessThanIndexSorted(xs, 1.f), 0);
  EXPECT_EQ(lessThanIndexSorted(xs, 1.1f), 0);
  EXPECT_EQ(lessThanIndexSorted(xs, 1.5f), 0);
  EXPECT_EQ(lessThanIndexSorted(xs, 1.6f), 0);
  EXPECT_EQ(lessThanIndexSorted(xs, 1.99999f), 0);
  EXPECT_EQ(lessThanIndexSorted(xs, 2.f), 1);
  EXPECT_EQ(lessThanIndexSorted(xs, 3.f), 2);
  EXPECT_EQ(lessThanIndexSorted(xs, 4.f), 3);
  EXPECT_EQ(lessThanIndexSorted(xs, 4.1f), 3);
}

TEST_F(InterpolationTest, nearestNeighbourIndexSorted) {
  Vector<float> xs{1, 2, 3, 4};
  EXPECT_EQ(nearestNeighbourIndexSorted(xs, 0.f), 0);
  EXPECT_EQ(nearestNeighbourIndexSorted(xs, 1.f), 0);
  EXPECT_EQ(nearestNeighbourIndexSorted(xs, 1.1f), 0);
  EXPECT_EQ(nearestNeighbourIndexSorted(xs, 1.5f), 1);
  EXPECT_EQ(nearestNeighbourIndexSorted(xs, 1.6f), 1);
  EXPECT_EQ(nearestNeighbourIndexSorted(xs, 1.9f), 1);
  EXPECT_EQ(nearestNeighbourIndexSorted(xs, 2.f), 1);
  EXPECT_EQ(nearestNeighbourIndexSorted(xs, 3.f), 2);
  EXPECT_EQ(nearestNeighbourIndexSorted(xs, 4.f), 3);
  EXPECT_EQ(nearestNeighbourIndexSorted(xs, 4.1f), 3);
}

TEST_F(InterpolationTest, InterpEmpty) {
  // Everything empty.
  Vector<float> x;
  Vector<float> y;
  Vector<float> newX;
  auto res = interp(x, y, newX);
  EXPECT_TRUE(res.empty());

  // Just x and y empty.
  newX = {1.f, 2.f, 3.f};
  res = interp(x, y, newX);
  EXPECT_TRUE(res.empty());

  // Just newX empty.
  x = {1, 2, 3, 4, 5};
  y = {1, 1, 1, 1, 1};
  newX = {};
  res = interp(x, y, newX);
  EXPECT_TRUE(res.empty());
}

TEST_F(InterpolationTest, InterpOneElement) {
  Vector<float> x = {1.f};
  Vector<float> y = {.5f};
  Vector<float> newX = {1.f};
  auto res = interp(x, y, newX);
  EXPECT_EQ(res.size(), 1);
  EXPECT_EQ(res[0], y[0]);

  newX = {0.f, 1.f, 2.f};
  res = interp(x, y, newX);
  EXPECT_EQ(res.size(), newX.size());
  for (int i = 0; i < newX.size(); ++i) {
    EXPECT_EQ(res[i], y[0]);
  }
}

TEST_F(InterpolationTest, InterpStationary) {
  Vector<float> x{1, 2, 3, 4, 5};
  Vector<float> y{1, 1, 1, 1, 1};
  Vector<float> newX{-1, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto res = interp(x, y, newX);
  for (auto i : res) {
    EXPECT_FLOAT_EQ(i, 1);
  }
}

TEST_F(InterpolationTest, InterpConstantVel) {
  Vector<float> x{1, 2, 3, 4, 5};
  Vector<float> y{1, 2, 3, 4, 5};
  Vector<float> newX{-1, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto res = interp(x, y, newX);
  for (auto i : res) {
    EXPECT_FLOAT_EQ(i, i);
  }
}

TEST_F(InterpolationTest, InterpConstantVelBetween) {
  Vector<float> x{1, 2, 3, 4, 5};
  Vector<float> y{1, 2, 3, 4, 5};
  Vector<float> newX{0, .5f, 1.5f, 2.5f, 3.5f, 4.5f};
  auto res = interp(x, y, newX);
  for (int i = 0; i < res.size(); ++i) {
    EXPECT_FLOAT_EQ(res[i], newX[i]);
  }
}

TEST_F(InterpolationTest, InterpConstantVelDuplicates) {
  Vector<float> x{1, 2, 3, 3, 3, 4, 5};
  Vector<float> y{1, 2, 3, 3, 3, 4, 5};
  Vector<float> newX{-1, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto res = interp(x, y, newX);
  for (auto i : res) {
    EXPECT_FLOAT_EQ(i, i);
  }

  newX = {-1, 1, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  res = interp(x, y, newX);
  for (auto i : res) {
    EXPECT_FLOAT_EQ(i, i);
  }
}

TEST_F(InterpolationTest, InterpConstantAccel) {
  const auto a = 1.f;
  auto v = 1.f;
  Vector<float> x = {-1};
  Vector<float> y = {0};
  for (int i = 0; i < 15; i++) {
    x.push_back(i);
    y.push_back(y.back() + v + a * .5f);
    v += a;
  }
  Vector<float> newX{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  auto res = interp(x, y, newX);
  for (int i = 0; i < newX.size(); ++i) {
    EXPECT_FLOAT_EQ(res[i], y[i + 2]);
  }
}

Vector<Quaternion> getQs(int startD, int endD, int stepD) {
  Vector<Quaternion> res;
  for (int i = startD; i < endD; i += stepD) {
    res.push_back(Quaternion::fromPitchYawRollDegrees(i, i, i));
  }
  return res;
}

TEST_F(InterpolationTest, InterpQ) {
  auto y = getQs(0, 180, 45);
  Vector<float> x;
  for (int i = 0; i < y.size(); ++i) {
    x.push_back(i);
  }
  // Interpolate with newX values at the same positions as x, or greater.
  Vector<float> newX{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto res = interp(x, y, newX);
  for (int i = 0; i < newX.size(); ++i) {
    if (newX[i] > x[y.size() - 1]) {
      // If newX is above x[n], should return y[n]
      EXPECT_TRUE(AreEqual(res[i], y[y.size() - 1]));
    } else {
      EXPECT_TRUE(AreEqual(res[i], y[i]));
    }
  }

  // Interpolate with newX values which are all before x.
  newX = {-10, -5, -1};
  res = interp(x, y, newX);
  for (int i = 0; i < newX.size(); ++i) {
    EXPECT_TRUE(AreEqual(res[i], y[0]));
  }
}

TEST_F(InterpolationTest, InterpQInBetween) {
  auto y = getQs(0, 180, 45);
  Vector<float> x;
  for (int i = 0; i < y.size(); ++i) {
    x.push_back(i);
  }
  // Interpolate with newX in between x.
  Vector<float> newX{0.f, .5f, 1.5f, 2.5f};
  auto res = interp(x, y, newX);
  EXPECT_TRUE(AreEqual(res[0], Quaternion()));
  for (int i = 1; i < newX.size(); ++i) {
    EXPECT_TRUE(AreEqual(res[i], y[i - 1].interpolate(y[i], 0.5f)));
  }
}

TEST_F(InterpolationTest, InterpQSmall) {
  auto y = getQs(0, 180, 45);
  Vector<float> x;
  for (int i = 0; i < y.size(); ++i) {
    x.push_back(i);
  }

  // Interpolate with newX on either side of the nearest neighbors search.
  Vector<float> newX{.1, .5f, .9};
  auto res = interp(x, y, newX);
  EXPECT_TRUE(AreEqual(res[0], y[0].interpolate(y[1], 0.1f)));
  EXPECT_TRUE(AreEqual(res[1], y[0].interpolate(y[1], 0.5f)));
  EXPECT_TRUE(AreEqual(res[2], y[0].interpolate(y[1], 0.9f)));
}

TEST_F(InterpolationTest, InterpQEmpty) {
  // Everything empty.
  Vector<float> x;
  Vector<Quaternion> y;
  Vector<float> newX;
  auto res = interp(x, y, newX);
  EXPECT_TRUE(res.empty());

  // Just x and y empty.
  newX = {1.f, 2.f, 3.f};
  res = interp(x, y, newX);
  EXPECT_TRUE(res.empty());

  // Just newX empty.
  x = {1, 2, 3, 4, 5};
  y = getQs(0, 180, 45);
  newX = {};
  res = interp(x, y, newX);
  EXPECT_TRUE(res.empty());
}

}  // namespace c8
