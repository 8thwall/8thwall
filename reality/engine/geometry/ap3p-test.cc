// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":ap3p",
    "//c8/geometry:worlds",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x544ed6e8);

#include "reality/engine/geometry/ap3p.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "c8/geometry/worlds.h"

using testing::Eq;
using testing::FloatNear;
using testing::Pointwise;

namespace c8 {

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsMatrix(const HMatrix &matrix) {
  return Pointwise(AreWithin(0.0001), matrix.data());
}

decltype(auto) equalsPoint(const HPoint3 &pt) { return Pointwise(AreWithin(0.0001), pt.data()); }

class AP3PTest : public ::testing::Test {};

TEST_F(AP3PTest, TestSolve) {
  Vector<HPoint3> p3 = Worlds::axisAlignedPolygonsWorld();
  auto cam = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  for (int i = 0; i < p3.size() - 4; ++i) {
    Vector<HPoint3> pts = {p3[i], p3[i + 1], p3[i + 2], p3[i + 3]};
    auto p2 = flatten<2>(cam.inv() * pts);

    auto reconstructionCam =
      ap3p::solve({pts[0], pts[1], pts[2], pts[3]}, {p2[0], p2[1], p2[2], p2[3]});

    EXPECT_THAT(cam.data(), equalsMatrix(reconstructionCam));
  }
}

}  // namespace c8
