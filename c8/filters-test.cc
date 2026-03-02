// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":filters",
    "//c8:vector",
    "//c8:hpoint",
    "//c8/string:format",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xbfd81cc5);

#include <string>

#include "c8/filters.h"
#include "c8/hpoint.h"
#include "c8/vector.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::Eq;
using testing::Pointwise;

namespace c8 {

namespace {
Vector<HPoint3> getPtsRange(float min, float max, float step) {
  Vector<HPoint3> res;
  for (float i = min; i < max; i += step) {
    res.push_back({i, i, i});
  }
  return res;
}
}  // namespace

class FiltersTest : public ::testing::Test {};

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }
MATCHER_P(ArePointsWithin, eps, "") {
  return testing::get<0>(arg).x() - testing::get<1>(arg).x() < eps
    && testing::get<0>(arg).y() - testing::get<1>(arg).y() < eps
    && testing::get<0>(arg).z() - testing::get<1>(arg).z() < eps;
}
decltype(auto) equalsVector(const Vector<HPoint3> &v, float threshold = 1e-5f) {
  return Pointwise(ArePointsWithin(threshold), v);
}
decltype(auto) equalsVector(const Vector<float> &v, float threshold = 1e-5f) {
  return Pointwise(AreWithin(threshold), v);
}
decltype(auto) equalsPoint(const HPoint3 &pt, float threshold = 1e-6) {
  return Pointwise(AreWithin(threshold), pt.data());
}

TEST_F(FiltersTest, RecursiveFilterLowPass) {
  RecursiveFilterLowPass<float> lowpassFilter(0.5f);
  float input = 1.0f;
  float output = lowpassFilter.filter(input);
  EXPECT_FLOAT_EQ(output, input);
  output = lowpassFilter.filter(2.0f);
  EXPECT_FLOAT_EQ(output, 1.5f);
  output = lowpassFilter.filter(2.0f);
  EXPECT_FLOAT_EQ(output, 1.75f);
}

TEST_F(FiltersTest, TrailingLowPass) {
  auto pts = getPtsRange(-10.f, 10.f, 1.f);

  // alpha = 1 means we use only the new value.
  TrailingLowPass<HPoint3> tlp(1.f);
  EXPECT_THAT(tlp.push(pts), equalsVector(pts));

  // alpha = 0 means we use only the old value, so it will be the first element of pts.
  tlp = {0.f};
  auto res = tlp.push(pts);
  for (auto pt : res) {
    EXPECT_THAT(pt.data(), equalsPoint(pts[0]));
  }
}

TEST_F(FiltersTest, TestRollingMeanNormalUsage) {
  RollingMean rollingMean(1.f);
  EXPECT_FLOAT_EQ(1.f, rollingMean.push(0.f, 1.f));
  EXPECT_FLOAT_EQ(1.5f, rollingMean.push(0.2f, 2.f));
  EXPECT_FLOAT_EQ(2.f, rollingMean.push(0.3f, 3.f));

  // the next second, first element is popped
  EXPECT_FLOAT_EQ(7.f / 3, rollingMean.push(1.1f, 2.f));

  // 1.2f to 0.2f is exactly the window size, POP
  EXPECT_FLOAT_EQ(8.f / 3, rollingMean.push(1.2f, 3.f));
  EXPECT_FLOAT_EQ(3.f, rollingMean.push(1.3f, 4.f));

  // this data is again outside the window
  EXPECT_FLOAT_EQ(4.f, rollingMean.push(3.2f, 4.f));
}

TEST_F(FiltersTest, TestRollingMeanSparseData) {
  RollingMean rollingMean(1.f);
  EXPECT_FLOAT_EQ(1.f, rollingMean.push(0.f, 1.f));

  // this data is outside the window, the first element is popped
  EXPECT_FLOAT_EQ(2.f, rollingMean.push(1.1f, 2.f));

  // this data is in the window (at the edge)
  EXPECT_FLOAT_EQ(2.5, rollingMean.push(2.1f, 3.f));

  // this data is again outside the window
  EXPECT_FLOAT_EQ(4.f, rollingMean.push(3.2f, 4.f));
}

TEST_F(FiltersTest, GaussianKernel) {
  // Based on:
  // import numpy as np
  // def gauss(sigma, x):
  //     return 1 / (sigma * np.sqrt(2*np.pi)) * np.exp(-x**2/(2*sigma**2))
  // sigma = 2
  // vals = [gauss(sigma, i) for i in range(-1, 2)]
  // s = sum(vals)
  // vals = [x / s for x in vals]
  EXPECT_THAT(gaussKernel(1.f, 3), equalsVector({0.274068f, 0.451862f, 0.274068f}));
  EXPECT_THAT(gaussKernel(2.f, 3), equalsVector({0.319167f, 0.361664f, 0.319167f}));

  EXPECT_THAT(
    gaussKernel(1.f, 4), equalsVector({0.054488f, 0.244201f, 0.402619f, 0.244201f, 0.054488f}));
  EXPECT_THAT(
    gaussKernel(2.f, 4), equalsVector({0.152469f, 0.221841f, 0.251379f, 0.221841f, 0.152469f}));
}

TEST_F(FiltersTest, GaussianFilterEdgeCases) {
  // Empty input
  EXPECT_TRUE(gaussianFilter({}, 1.f, 1).empty());

  // Window of zero returns original input
  Vector<float> vals = {1.f, 2.f, 3.f};
  EXPECT_THAT(gaussianFilter(vals, 3.f, 0), equalsVector(vals));

  // Window of one returns original input
  vals = {1.f, 2.f, 3.f};
  EXPECT_THAT(gaussianFilter(vals, 3.f, 1), equalsVector(vals));

  // Sigma of one returns original input
  vals = {-100.f};
  EXPECT_THAT(gaussianFilter(vals, 1.f, 100), equalsVector(vals));

  vals = {2.f, 2.f};
  EXPECT_THAT(gaussianFilter(vals, 1.f, 100), equalsVector(vals));

  vals = {0.f, 0.f, 0.f};
  EXPECT_THAT(gaussianFilter(vals, 1.f, 100), equalsVector(vals));
}

TEST_F(FiltersTest, GaussianFilterSamples) {
  Vector<float> vals{1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f};

  // With a sample size of 2 the first and last elements should be different but we recover the
  // original values for all other indices.
  auto sigma = 1.f;
  auto window = 2;
  auto kernel = gaussKernel(sigma, window);
  auto width = window / 2;
  auto res = gaussianFilter(vals, sigma, window);
  EXPECT_TRUE(res.size() == vals.size());
  for (int i = 0; i < res.size(); ++i) {
    if (i == 0) {
      // With a sample window of 2, which is rounded up to 3, the first element will just have data
      // from index 0 and 1. Calculate by hand, including normalization.
      EXPECT_FLOAT_EQ(
        res[0],
        (vals[0] * kernel[width] + vals[1] * kernel[width + 1])
          / (kernel[width] + kernel[width + 1]));
    } else if (i == res.size() - 1) {
      // With a sample window of 2, which is rounded up to 3, the first element will just have data
      // from index 0 and 1. Calculate by hand, including normalization.
      EXPECT_FLOAT_EQ(
        res.back(),
        (vals.back() * kernel[width] + vals[vals.size() - 2] * kernel[width - 1])
          / (kernel[width] + kernel[width - 1]));
    } else {
      EXPECT_FLOAT_EQ(res[i], vals[i]);
    }
  }
}
}  // namespace c8
