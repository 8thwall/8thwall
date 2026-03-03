// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":point-regression",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xf9f85291);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "reality/engine/geometry/point-regression.h"

using testing::Eq;
using testing::Pointwise;

namespace c8 {

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsMatrix(const HMatrix &matrix, float tolerance = 1e-6f) {
  return Pointwise(AreWithin(tolerance), matrix.data());
}

class PointRegressionTest : public ::testing::Test {};

TEST_F(PointRegressionTest, fitLinear33HVec3) {
  auto transform = HMatrix{
    {1.0f, -2.0f, 3.0f, 0.0f},
    {-4.0f, -5.0f, 6.0f, 0.0f},
    {-7.0f, 8.0f, -9.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
  };

  Vector<HVector3> x = {
    {0.0f, 1.0f, 2.0f},
    {2.0f, 3.0f, 1.0f},
    {5.0f, 3.0f, 4.0f},
    {6.0f, 4.0f, 5.0f},
    {6.0f, 5.0f, 7.0f},
    {6.0f, 7.0f, 8.0f},
  };

  auto y = transform * x;

  auto estTransform = fitLinear33(x, y);

  EXPECT_THAT(estTransform.data(), equalsMatrix(transform, 3e-4f));

  EXPECT_NEAR(residual(estTransform, x, y), 0.0f, 9e-7f);
}

}  // namespace c8
