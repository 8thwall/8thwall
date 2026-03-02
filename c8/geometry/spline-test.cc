// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":spline",
    "//c8:c8-log",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x8ad400f2);

#include "c8/c8-log.h"
#include "c8/geometry/spline.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace c8 {

namespace {
Vector<float> getXs(float min, float max, float step) {
  Vector<float> r;
  for (float i = min; i < max; i += step) {
    r.push_back(i);
  }
  return r;
}
}  // namespace

class SplineTest : public ::testing::Test {};

TEST_F(SplineTest, Bin) {
  Vector<float> x{0, 1, 2, 3};

  EXPECT_EQ(0, splineBin(x, -1.f));
  EXPECT_EQ(0, splineBin(x, 0.f));
  EXPECT_EQ(0, splineBin(x, .5f));
  EXPECT_EQ(0, splineBin(x, .9f));
  EXPECT_EQ(1, splineBin(x, 1.f));
  EXPECT_EQ(1, splineBin(x, 1.001f));
  EXPECT_EQ(1, splineBin(x, 1.f));
  EXPECT_EQ(1, splineBin(x, 1.999f));
  EXPECT_EQ(2, splineBin(x, 2.f));
  EXPECT_EQ(3, splineBin(x, 3.f));
  EXPECT_EQ(3, splineBin(x, 4.f));
  EXPECT_EQ(3, splineBin(x, 5.f));
}

TEST_F(SplineTest, EdgeCases) {
  // Non-matching sizes.
  auto s = spline({}, {1.f, 2.f, 3.f});
  EXPECT_TRUE(s.empty());
  s = spline({1.f, 2.f, 3.f}, {});
  EXPECT_TRUE(s.empty());

  // One element.
  s = spline({1.f}, {1.f});
  EXPECT_TRUE(s.empty());

  // Two elements (a line).
  Vector<float> xs{1, 2};
  Vector<float> ys{1, 2};
  s = spline(xs, ys);
  EXPECT_EQ(s.size(), 1);
  for (int i = 0; i < s.size(); ++i) {
    auto c = s[i];
    EXPECT_FLOAT_EQ(c.pos(), ys[i]);
    EXPECT_FLOAT_EQ(c.vel(), 1.f);
    EXPECT_FLOAT_EQ(c.accel(), 0.f);
  }

  // Three elements (a line).
  xs = {1, 2, 3};
  ys = {1, 2, 3};
  s = spline(xs, ys);
  EXPECT_EQ(s.size(), 2);
  for (int i = 0; i < s.size(); ++i) {
    auto c = s[i];
    EXPECT_FLOAT_EQ(c.pos(), ys[i]);
    EXPECT_FLOAT_EQ(c.vel(), 1.f);
    EXPECT_FLOAT_EQ(c.accel(), 0.f);
  }
}

TEST_F(SplineTest, Line) {
  // Line with equations:
  // y = x
  // y' = 1
  // y" = 0
  Vector<float> xs{1, 2, 3, 4, 5};
  Vector<float> ys{1, 2, 3, 4, 5};

  auto s = spline(xs, ys);
  for (int i = 0; i < s.size(); ++i) {
    auto c = s[i];
    EXPECT_FLOAT_EQ(c.pos(), ys[i]);
    EXPECT_FLOAT_EQ(c.vel(), 1.f);
    EXPECT_FLOAT_EQ(c.accel(), 0.f);
  }
}

TEST_F(SplineTest, Parabola) {
  // Parabola with equations:
  // y = x^2
  // y' = 2x
  // y" = 2
  Vector<float> xs;
  Vector<float> ys;
  for (int i = -20; i < 20; ++i) {
    xs.push_back(i);
    ys.push_back(i * i);
  }

  auto s = spline(xs, ys);
  for (int i = 0; i < s.size(); ++i) {
    auto c = s[i];
    EXPECT_FLOAT_EQ(c.pos(), ys[i]);
    // The begininng and end cubics have more error than the rest.
    if (i > 2 && i < s.size() - 2) {
      EXPECT_NEAR(c.vel(), 2.f * xs[i], 2e-2f);
      EXPECT_NEAR(c.accel(), 2.f, 4e-2f);
    }
  }
}

TEST_F(SplineTest, Cubic) {
  // Cubic with equations:
  // y = x^3
  // y' = 3x^2
  // y" = 6x
  Vector<float> xs;
  Vector<float> ys;
  for (int i = -20; i < 20; ++i) {
    xs.push_back(i);
    ys.push_back(i * i * i);
  }

  auto s = spline(xs, ys);
  for (int i = 0; i < s.size(); ++i) {
    auto c = s[i];
    EXPECT_FLOAT_EQ(c.pos(), ys[i]);
    // The begininng and end cubics have more error than the rest.
    if (i > 5 && i < s.size() - 5) {
      EXPECT_NEAR(c.vel(), 3.f * xs[i] * xs[i], 2e-2f);
      EXPECT_NEAR(c.accel(), 6.f * xs[i], 5e-2f);
    }
  }
}

TEST_F(SplineTest, InterpolateLine) {
  // Line with equations:
  // y = x
  // y' = 1
  // y" = 0
  Vector<float> xs;
  Vector<HPoint3> ys;
  for (float i = -10; i < 10; ++i) {
    xs.push_back(i);
    ys.push_back({i, i, i});
  }

  auto s = spline3<HPoint3>(xs, ys);
  auto newXs = getXs(-15.f, 15.f, .1f);
  auto eval = evalSpline3<HPoint3>(s, newXs);
  for (int i = 0; i < newXs.size(); ++i) {
    auto x = newXs[i];
    EXPECT_FLOAT_EQ(eval.pos[i].x(), x);
    EXPECT_FLOAT_EQ(eval.vel[i].x(), 1.f);
    EXPECT_FLOAT_EQ(eval.accel[i].x(), 0.f);
  }
}

TEST_F(SplineTest, InterpolateParabola) {
  // Parabola with equations:
  // y = x^2
  // y' = 2x
  // y" = 2
  Vector<float> xs;
  Vector<HPoint3> ys;
  for (float i = -10; i < 10; ++i) {
    xs.push_back(i);
    auto i2 = static_cast<float>(i * i);
    ys.push_back({i2, i2, i2});
  }

  auto s = spline3<HPoint3>(xs, ys);
  auto newXs = getXs(-15.f, 15.f, .5f);
  auto eval = evalSpline3<HPoint3>(s, newXs);
  for (int i = 0; i < newXs.size(); ++i) {
    auto x = newXs[i];
    if (x > xs[2] && x < xs[xs.size() - 2]) {
      EXPECT_NEAR(eval.pos[i].x(), x * x, 3e-2);
      EXPECT_NEAR(eval.vel[i].x(), 2.f * x, 2e-1f);
      EXPECT_NEAR(eval.accel[i].x(), 2.f, 2e-1f);
    }
  }
}

TEST_F(SplineTest, InterpolateCubic) {
  // Cubic with equations:
  // y = x^3
  // y' = 3x^2
  // y" = 6x
  Vector<float> xs;
  Vector<HPoint3> ys;
  for (int i = -20; i < 20; ++i) {
    xs.push_back(i);
    auto i3 = static_cast<float>(i * i * i);
    ys.push_back({i3, i3, i3});
  }

  auto s = spline3<HPoint3>(xs, ys);
  auto newXs = getXs(-15.f, 15.f, .5f);
  auto eval = evalSpline3<HPoint3>(s, newXs);
  for (int i = 0; i < newXs.size(); ++i) {
    auto x = newXs[i];
    if (x > xs[2] && x < xs[xs.size() - 3]) {
      EXPECT_NEAR(eval.pos[i].x(), x * x * x, 3e-2);
      EXPECT_NEAR(eval.vel[i].x(), 3.f * x * x, 5e-2f);
      EXPECT_NEAR(eval.accel[i].x(), 6.f * x, 3e-1f);
    }
  }
}

}  // namespace c8
