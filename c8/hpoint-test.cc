// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Unit tests for HPoint classes.

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":hpoint",
    ":vector",
    "//bzl/inliner:rules",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x270434ba);

#include <cmath>

#include "c8/hpoint.h"
#include "c8/vector.h"
#include "gtest/gtest.h"

namespace c8 {

class HPointTest : public ::testing::Test {};

TEST_F(HPointTest, Point2InitTest) {
  HPoint2 p0 = {1.0f, 2.0f};
  EXPECT_FLOAT_EQ(p0.x(), 1.0f);
  EXPECT_FLOAT_EQ(p0.y(), 2.0f);

  HPoint2 p1 = std::array<float, 2>{{1.0f, 2.0f}};
  EXPECT_FLOAT_EQ(p1.x(), 1.0f);
  EXPECT_FLOAT_EQ(p1.y(), 2.0f);

  HPoint2 p2 = std::array<float, 3>{{1.0f, 2.0f, 2.0f}};
  EXPECT_FLOAT_EQ(p2.x(), 0.5f);
  EXPECT_FLOAT_EQ(p2.y(), 1.0f);

  float foo[2] = {1.0f, 2.0f};
  HPoint2 p3(foo);
  EXPECT_FLOAT_EQ(p3.x(), 1.0f);
  EXPECT_FLOAT_EQ(p3.y(), 2.0f);

  float bar[3] = {1.0f, 2.0f, 4.0f};
  HPoint2 p4(bar);
  EXPECT_FLOAT_EQ(p4.x(), 0.25f);
  EXPECT_FLOAT_EQ(p4.y(), 0.5f);
}

TEST_F(HPointTest, Point3InitTest) {
  HPoint3 p0 = {1.0f, 2.0f, 3.0f};
  EXPECT_FLOAT_EQ(p0.x(), 1.0f);
  EXPECT_FLOAT_EQ(p0.y(), 2.0f);
  EXPECT_FLOAT_EQ(p0.z(), 3.0f);

  HPoint3 p1 = std::array<float, 3>{{1.0f, 2.0f, 3.0f}};
  EXPECT_FLOAT_EQ(p1.x(), 1.0f);
  EXPECT_FLOAT_EQ(p1.y(), 2.0f);
  EXPECT_FLOAT_EQ(p1.z(), 3.0f);

  HPoint3 p2 = std::array<float, 4>{{1.0f, 2.0f, 3.0f, 2.0f}};
  EXPECT_FLOAT_EQ(p2.x(), 0.5f);
  EXPECT_FLOAT_EQ(p2.y(), 1.0f);
  EXPECT_FLOAT_EQ(p2.z(), 1.5f);
}

TEST_F(HPointTest, FlattenTest) {
  HPoint3 p3d0 = {1.0f, 2.0f, 4.0f};
  HPoint2 p2d0 = p3d0.flatten();

  EXPECT_FLOAT_EQ(p2d0.x(), 0.25f);
  EXPECT_FLOAT_EQ(p2d0.y(), 0.5f);

  HPoint3 p3d1 = std::array<float, 4>{{1.0f, 2.0f, 2.0f, 4.0f}};
  HPoint2 p2d1 = p3d1.flatten();

  EXPECT_FLOAT_EQ(p2d1.x(), 0.5f);
  EXPECT_FLOAT_EQ(p2d1.y(), 1.0f);

  Vector<HPoint2> flat = flatten<2>(Vector<HPoint3>{p3d0, p3d1});
  EXPECT_FLOAT_EQ(flat[0].x(), 0.25f);
  EXPECT_FLOAT_EQ(flat[0].y(), 0.5f);
  EXPECT_FLOAT_EQ(flat[1].x(), 0.5f);
  EXPECT_FLOAT_EQ(flat[1].y(), 1.0f);
}

TEST_F(HPointTest, ExtrudeTest) {
  HPoint2 p2d0 = {1.0f, 2.0f};
  HPoint3 p3d0 = p2d0.extrude();

  EXPECT_FLOAT_EQ(p3d0.x(), 1.0f);
  EXPECT_FLOAT_EQ(p3d0.y(), 2.0f);
  EXPECT_FLOAT_EQ(p3d0.z(), 1.0f);

  HPoint2 p2d1 = std::array<float, 3>{{1.0f, 2.0f, 4.0f}};
  HPoint3 p3d1 = p2d1.extrude();

  EXPECT_FLOAT_EQ(p3d1.x(), 1.0f);
  EXPECT_FLOAT_EQ(p3d1.y(), 2.0f);
  EXPECT_FLOAT_EQ(p3d1.z(), 4.0f);

  Vector<HPoint3> extruded = extrude<3>(Vector<HPoint2>{p2d0, p2d1});
  EXPECT_FLOAT_EQ(extruded[0].x(), 1.0f);
  EXPECT_FLOAT_EQ(extruded[0].y(), 2.0f);
  EXPECT_FLOAT_EQ(extruded[0].z(), 1.0f);
  EXPECT_FLOAT_EQ(extruded[1].x(), 1.0f);
  EXPECT_FLOAT_EQ(extruded[1].y(), 2.0f);
  EXPECT_FLOAT_EQ(extruded[1].z(), 4.0f);
}

TEST_F(HPointTest, DistTest) {
  HPoint2 p20 = {0.0f, 0.0f};
  HPoint2 p2 = {3.0f, 4.0f};  // length=5
  HPoint3 p30 = {0.0f, 0.0f, 0.0f};
  HPoint3 p3 = {2.0f, 1.0f, 3.0f};  // length = sqrt(14)

  EXPECT_FLOAT_EQ(p2.dist(p20), 5.0f);
  EXPECT_FLOAT_EQ(p20.dist(p2), 5.0f);
  EXPECT_FLOAT_EQ(p2.dist(p2), 0.0f);

  EXPECT_FLOAT_EQ(p3.dist(p30), std::sqrt(14.0f));
  EXPECT_FLOAT_EQ(p30.dist(p3), std::sqrt(14.0f));
  EXPECT_FLOAT_EQ(p3.dist(p3), 0.0f);
}

TEST_F(HPointTest, HPoint2SqDistTest) {
  HPoint2 p0 = {0.0f, 0.0f};
  HPoint2 p1 = {3.0f, 4.0f};  // length=5
  EXPECT_FLOAT_EQ(p0.sqdist(p1), 25.0f);
}

TEST_F(HPointTest, HPoint2L1DistTest) {
  HPoint2 p0 = {0.0f, 0.0f};
  HPoint2 p1 = {3.0f, 4.0f};  // length=5
  EXPECT_FLOAT_EQ(p0.l1dist(p1), 7.0f);
}

}  // namespace c8
