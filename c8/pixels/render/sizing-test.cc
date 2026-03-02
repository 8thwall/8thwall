// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":sizing",
    "//c8:c8-log",
    "@com_google_googletest//:gtest_main",
    "@json//:json",
  };
}
cc_end(0x9e0ed69c);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/c8-log.h"
#include "c8/pixels/render/sizing.h"

namespace c8 {

class SizingTest : public ::testing::Test {};

TEST_F(SizingTest, ConstructSingleRow) {
  SceneSize singleRowScene{3, 1};
  auto fullRect = singleRowScene();
  EXPECT_EQ(0, fullRect.x);
  EXPECT_EQ(0, fullRect.y);
  EXPECT_EQ(480 * 3, fullRect.width);
  EXPECT_EQ(640, fullRect.height);

  auto left1 = singleRowScene.row0(0, 1);
  EXPECT_EQ(0, left1.x);
  EXPECT_EQ(0, left1.y);
  EXPECT_EQ(480, left1.width);
  EXPECT_EQ(640, left1.height);
  auto mid1 = singleRowScene.row0(1, 1);
  EXPECT_EQ(480, mid1.x);
  EXPECT_EQ(0, mid1.y);
  EXPECT_EQ(480, mid1.width);
  EXPECT_EQ(640, mid1.height);
  auto right1 = singleRowScene.row0(2, 1);
  EXPECT_EQ(480 * 2, right1.x);
  EXPECT_EQ(0, right1.y);
  EXPECT_EQ(480, right1.width);
  EXPECT_EQ(640, right1.height);

  auto left2 = singleRowScene.row0(0, 2);
  EXPECT_EQ(0, left2.x);
  EXPECT_EQ(0, left2.y);
  EXPECT_EQ(480 * 2, left2.width);
  EXPECT_EQ(640, left2.height);
  auto right2 = singleRowScene.row0(1, 2);
  EXPECT_EQ(480, right2.x);
  EXPECT_EQ(0, right2.y);
  EXPECT_EQ(480 * 2, right2.width);
  EXPECT_EQ(640, right2.height);
}

TEST_F(SizingTest, ConstructDoubleRow) {
  SceneSize doubleRowScene{3, 2};
  auto fullRect = doubleRowScene();
  EXPECT_EQ(0, fullRect.x);
  EXPECT_EQ(0, fullRect.y);
  EXPECT_EQ(480 * 3, fullRect.width);
  EXPECT_EQ(640 * 2, fullRect.height);

  auto row2Left = doubleRowScene.row1(0, 1);
  EXPECT_EQ(0, row2Left.x);
  EXPECT_EQ(640, row2Left.y);
  EXPECT_EQ(480, row2Left.width);
  EXPECT_EQ(640, row2Left.height);
  auto row2Mid = doubleRowScene.row1(1, 1);
  EXPECT_EQ(480, row2Mid.x);
  EXPECT_EQ(640, row2Mid.y);
  EXPECT_EQ(480, row2Mid.width);
  EXPECT_EQ(640, row2Mid.height);
  auto row2Right = doubleRowScene.row1(2, 1);
  EXPECT_EQ(480 * 2, row2Right.x);
  EXPECT_EQ(640, row2Right.y);
  EXPECT_EQ(480, row2Right.width);
  EXPECT_EQ(640, row2Right.height);
}

TEST_F(SizingTest, ArbitrarySubregion) {
  SceneSize scene{3, 3};
  auto top2x2 = scene(0, 0, 2, 2);
  EXPECT_EQ(0, top2x2.x);
  EXPECT_EQ(0, top2x2.y);
  EXPECT_EQ(480 * 2, top2x2.width);
  EXPECT_EQ(640 * 2, top2x2.height);

  auto topRight2x3 = scene(1, 0, 2, 3);
  EXPECT_EQ(480, topRight2x3.x);
  EXPECT_EQ(0, topRight2x3.y);
  EXPECT_EQ(480 * 2, topRight2x3.width);
  EXPECT_EQ(640 * 3, topRight2x3.height);

  auto bottom1x1 = scene(1, 2, 1, 1);
  EXPECT_EQ(480, bottom1x1.x);
  EXPECT_EQ(640 * 2, bottom1x1.y);
  EXPECT_EQ(480, bottom1x1.width);
  EXPECT_EQ(640, bottom1x1.height);
}

}  // namespace c8
