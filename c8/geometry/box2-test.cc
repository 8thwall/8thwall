// Copyright (c) 2022 8th Wall, LLC
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":box2",
    "@com_google_googletest//:gtest_main",
  };
  linkstatic=1;
}
cc_end(0x6c9b4834);

#include <gtest/gtest.h>

#include "c8/geometry/box2.h"

namespace c8 {

class Box2Test : public ::testing::Test {};

TEST_F(Box2Test, TestBox2) {
  Box2 box1 = {0.0f, 0.0f, 1.0f, 1.0f};
  Box2 box2 = {0.5f, 0.5f, 1.0f, 1.0f};
  Box2 box3 = {1.0f, 1.0f, 1.0f, 1.0f};
  bool doIntersect = doBoxesOverlap(box1, box2);
  EXPECT_TRUE(doIntersect);
  doIntersect = doBoxesOverlap(box1, box3);
  EXPECT_FALSE(doIntersect);
  float iou = intersectionOverUnion(box1, box2);
  EXPECT_FLOAT_EQ(iou, 0.142857149);
}

}  // namespace c8
