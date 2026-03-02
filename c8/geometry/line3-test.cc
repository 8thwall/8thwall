// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Yuyan Song (yuyansong@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":line3",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xa3f3bd37);

#include "c8/geometry/line3.h"
#include "gtest/gtest.h"

namespace c8 {

class Line3Test : public ::testing::Test {};

TEST_F(Line3Test, TestLine3Magnitude) {
  Line3 line(HPoint3(0.0f, 0.0f, 0.0f), HPoint3(1.0f, 1.0f, 1.0f));
  float length = line.magnitude();
  EXPECT_FLOAT_EQ(length, 1.732051);
}

TEST_F(Line3Test, TestLine3NoIntersection) {
  float muThis = 0.0f;
  HPoint3 ptThis;
  float muOther = 0.0f;
  HPoint3 ptOther;

  Line3 lineZ(HPoint3(0.0f, 0.0f, 0.0f), HPoint3(0.0f, 0.0f, 1.0f));
  Line3 lineZParallel(HPoint3(0.0f, 1.0f, 0.0f), HPoint3(0.0f, 1.0f, 1.0f));

  bool doIntersect = lineZ.intersects(lineZParallel, &muThis, &muOther, &ptThis, &ptOther);
  EXPECT_FALSE(doIntersect);
}

TEST_F(Line3Test, TestLine3Intersection) {
  float muThis = 0.0f;
  HPoint3 ptThis;
  float muOther = 0.0f;
  HPoint3 ptOther;

  Line3 lineCross0(HPoint3(-1.0f, -1.0f, 0.9f), HPoint3(1.0f, 1.0f, 0.9f));
  Line3 lineCross1(HPoint3(-1.0f, 1.0f, 1.0f), HPoint3(1.0f, -1.0f, 1.0f));
  bool doIntersect = lineCross0.intersects(lineCross1, &muThis, &muOther, &ptThis, &ptOther);
  EXPECT_TRUE(doIntersect);
  EXPECT_FLOAT_EQ(muThis, 0.5f);
  EXPECT_FLOAT_EQ(muOther, 0.5f);
  EXPECT_FLOAT_EQ(ptThis.x(), ptOther.x());
  EXPECT_FLOAT_EQ(ptThis.y(), ptOther.y());
  EXPECT_FLOAT_EQ(ptThis.z(), 0.9f);
  EXPECT_FLOAT_EQ(ptOther.z(), 1.0f);
}

}  // namespace c8
