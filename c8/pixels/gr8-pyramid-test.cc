// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "//c8/pixels:gr8-pyramid",
    "//c8:c8-log",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xe289bebf);

#include "c8/pixels/gr8-pyramid.h"

#include "c8/c8-log.h"
#include <queue>
#include <vector>
#include "gtest/gtest.h"

namespace c8 {


class Gr8PyramidTest : public ::testing::Test {};

TEST_F(Gr8PyramidTest, TestMapToBase) {
  Gr8Pyramid p;
  p.levels.push_back({0, 0, 12, 24, false});
  p.levels.push_back({0, 0, 12, 6, true});  // rotated
  p.levels.push_back({0, 0, 4, 8, false});
  p.levels.push_back({0, 0, 3, 6, false});

  auto map = p.mapLevelsToBase();

  // Level 0 x:
  EXPECT_FLOAT_EQ(0, 0 * map[0][0] + map[0][1]);
  EXPECT_FLOAT_EQ(5, 5 * map[0][0] + map[0][1]);
  EXPECT_FLOAT_EQ(6, 6 * map[0][0] + map[0][1]);
  EXPECT_FLOAT_EQ(11, 11 * map[0][0] + map[0][1]);
  // Level 0 y:
  EXPECT_FLOAT_EQ(0, 0 * map[0][2] + map[0][3]);
  EXPECT_FLOAT_EQ(11, 11 * map[0][2] + map[0][3]);
  EXPECT_FLOAT_EQ(12, 12 * map[0][2] + map[0][3]);
  EXPECT_FLOAT_EQ(23, 23 * map[0][2] + map[0][3]);
  // Level 0 scale:
  EXPECT_FLOAT_EQ(1, map[0][4]);

  // Level 1 x:
  EXPECT_FLOAT_EQ(0.5, 0 * map[1][0] + map[1][1]);
  EXPECT_FLOAT_EQ(4.5, 2 * map[1][0] + map[1][1]);
  EXPECT_FLOAT_EQ(6.5, 3 * map[1][0] + map[1][1]);
  EXPECT_FLOAT_EQ(10.5, 5 * map[1][0] + map[1][1]);
  // Level 1 y:
  EXPECT_FLOAT_EQ(0.5, 0 * map[1][2] + map[1][3]);
  EXPECT_FLOAT_EQ(10.5, 5 * map[1][2] + map[1][3]);
  EXPECT_FLOAT_EQ(12.5, 6 * map[1][2] + map[1][3]);
  EXPECT_FLOAT_EQ(22.5, 11 * map[1][2] + map[1][3]);
  // Level 1 scale:
  EXPECT_FLOAT_EQ(2, map[1][4]);

  // Level 2 x:
  EXPECT_FLOAT_EQ(1, 0 * map[2][0] + map[2][1]);
  EXPECT_FLOAT_EQ(4, 1 * map[2][0] + map[2][1]);
  EXPECT_FLOAT_EQ(7, 2 * map[2][0] + map[2][1]);
  EXPECT_FLOAT_EQ(10, 3 * map[2][0] + map[2][1]);
  // Level 2 y:
  EXPECT_FLOAT_EQ(1, 0 * map[2][2] + map[2][3]);
  EXPECT_FLOAT_EQ(10, 3 * map[2][2] + map[2][3]);
  EXPECT_FLOAT_EQ(13, 4 * map[2][2] + map[2][3]);
  EXPECT_FLOAT_EQ(22, 7 * map[2][2] + map[2][3]);
  // Level 2 scale:
  EXPECT_FLOAT_EQ(3, map[2][4]);

  // Level 3 x:
  EXPECT_FLOAT_EQ(1.5, 0 * map[3][0] + map[3][1]);
  EXPECT_FLOAT_EQ(5.5, 1 * map[3][0] + map[3][1]);
  EXPECT_FLOAT_EQ(9.5, 2 * map[3][0] + map[3][1]);
  // Level 3 y:
  EXPECT_FLOAT_EQ(1.5, 0 * map[3][2] + map[3][3]);
  EXPECT_FLOAT_EQ(9.5, 2 * map[3][2] + map[3][3]);
  EXPECT_FLOAT_EQ(13.5, 3 * map[3][2] + map[3][3]);
  EXPECT_FLOAT_EQ(21.5, 5 * map[3][2] + map[3][3]);
  // Level 3 scale:
  EXPECT_FLOAT_EQ(4, map[3][4]);

}

}  // namespace c8
