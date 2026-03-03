// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":pixel-transforms",
    "//c8/pixels:pixel-buffer",
    "//c8:color-maps",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xd6b30a14)

#include <cmath>

#include "c8/color-maps.h"
#include "c8/exceptions.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/stats/scope-timer.h"
#include "gtest/gtest.h"

namespace c8 {

class PixelTransformsTest : public ::testing::Test {};

TEST_F(PixelTransformsTest, TestCopyPixels) {
  ScopeTimer t("test");

  // rows = 3, cols = 2, row bytes = 5
  const uint8_t src[15] = {
    // clang-format off
    1, 2, 3, 4, 100,  // row 1
    5, 6, 7, 8, 101,  // row 2
    9, 10, 11, 12, 102,  // row 3
    // clang-format on
  };

  // rows = 2, cols = 3, row bytes = 10
  uint8_t dest[15];

  ConstUVPlanePixels srcPix(3, 2, 5, src);
  UVPlanePixels destPix(3, 2, 5, dest);

  copyPixels(srcPix, &destPix);

  EXPECT_EQ(1, dest[0]);
  EXPECT_EQ(2, dest[1]);
  EXPECT_EQ(3, dest[2]);
  EXPECT_EQ(4, dest[3]);

  EXPECT_EQ(5, dest[0 + 5]);
  EXPECT_EQ(6, dest[1 + 5]);
  EXPECT_EQ(7, dest[2 + 5]);
  EXPECT_EQ(8, dest[3 + 5]);

  EXPECT_EQ(9, dest[0 + 10]);
  EXPECT_EQ(10, dest[1 + 10]);
  EXPECT_EQ(11, dest[2 + 10]);
  EXPECT_EQ(12, dest[3 + 10]);
}

TEST_F(PixelTransformsTest, TestCopyFloatPixels) {
  ScopeTimer t("test");

  // rows = 3, cols = 2, row elements = 5
  const float src[15] = {
    // clang-format off
    0.1, 0.2, 3, 4, 10.0,  // row 1
    5, 6, 7, 8, 10.1,  // row 2
    9, 1.0, 1.1, 1.2, 102,  // row 3
    // clang-format on
  };

  // rows = 2, cols = 3, row elements = 10
  float dest[15];

  ConstFloatPixels srcPix(3, 2, 5, src);
  FloatPixels destPix(3, 2, 5, dest);

  copyFloatPixels(srcPix, &destPix);

  EXPECT_EQ(0.1f, dest[0]);
  EXPECT_EQ(0.2f, dest[1]);
  EXPECT_EQ(3, dest[2]);
  EXPECT_EQ(4, dest[3]);

  EXPECT_EQ(5, dest[0 + 5]);
  EXPECT_EQ(6, dest[1 + 5]);
  EXPECT_EQ(7, dest[2 + 5]);
  EXPECT_EQ(8, dest[3 + 5]);

  EXPECT_EQ(9, dest[0 + 10]);
  EXPECT_EQ(1.0f, dest[1 + 10]);
  EXPECT_EQ(1.1f, dest[2 + 10]);
  EXPECT_EQ(1.2f, dest[3 + 10]);
}

/*
 * Tests that if the four channel source and destination image are the same size then it will
 * simply copy over the data
 */
TEST_F(PixelTransformsTest, TestDownsize4ChannelSameSize) {
  ScopeTimer t("test");

  const uint8_t src[16] = {
    // clang-format off
    // row 1
    0, 1, 2, 3,
    4, 5, 6, 7,
    // row 2
    0, 1, 2, 3,
    4, 5, 6, 7,
    // clang-format on
  };

  uint8_t dest[16];

  ConstFourChannelPixels srcPix4(2, 2, 8, src);
  FourChannelPixels destPix4(2, 2, 8, dest);

  downsize(srcPix4, &destPix4);

  EXPECT_EQ(0, dest[0]);
  EXPECT_EQ(1, dest[1]);
  EXPECT_EQ(2, dest[2]);
  EXPECT_EQ(3, dest[3]);
  EXPECT_EQ(4, dest[4]);
  EXPECT_EQ(5, dest[5]);
  EXPECT_EQ(6, dest[6]);
  EXPECT_EQ(7, dest[7]);
  EXPECT_EQ(0, dest[8]);
  EXPECT_EQ(1, dest[9]);
  EXPECT_EQ(2, dest[10]);
  EXPECT_EQ(3, dest[11]);
  EXPECT_EQ(4, dest[12]);
  EXPECT_EQ(5, dest[13]);
  EXPECT_EQ(6, dest[14]);
  EXPECT_EQ(7, dest[15]);
}

/*
 * Tests that if the four channel destination image is smaller by an integer resize factor
 * that it will succesfully downsize the source image
 */
TEST_F(PixelTransformsTest, TestDownsize4ChannelSmallerSize) {
  ScopeTimer t("test");

  const uint8_t src[16] = {
    // clang-format off
    // row 1
    0, 1, 2, 3,
    4, 5, 6, 7,
    // row 2
    0, 1, 2, 3,
    4, 5, 6, 7,
    // clang-format on
  };

  uint8_t dest[4];

  ConstFourChannelPixels srcPix4(2, 2, 8, src);
  FourChannelPixels destPix4(1, 1, 4, dest);

  downsize(srcPix4, &destPix4);

  EXPECT_EQ(4, dest[0]);
  EXPECT_EQ(5, dest[1]);
  EXPECT_EQ(6, dest[2]);
  EXPECT_EQ(7, dest[3]);
}

/*
 * Asserts that if the destination image is not smaller by an integer resize factor
 * that it will throw a c8::RuntimeError
 */
TEST_F(PixelTransformsTest, TestDownsize4ChannelIncorrectResize) {
  ScopeTimer t("test");

  const uint8_t src[64] = {
    // clang-format off
    // row 1
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    // row 2
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    // row 3
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    // row 4
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    // clang-format on
  };

  uint8_t dest[48];

  ConstFourChannelPixels srcPix4(4, 4, 16, src);
  FourChannelPixels destPix4(3, 3, 12, dest);

  ASSERT_THROW(downsize(srcPix4, &destPix4), RuntimeError);
}

TEST_F(PixelTransformsTest, TestMergePixels) {
  ScopeTimer t("test");

  // rows = 2, cols = 3, row bytes = 4
  const uint8_t srcU[8] = {
    // clang-format off
    1, 3, 5, 1,
    7, 9, 11, 2,
    // clang-format on
  };

  // rows = 2, cols = 3, row bytes = 3
  const uint8_t srcV[6] = {
    // clang-format off
    2, 4, 6,
    8, 10, 12
    // clang-format on
  };

  // rows = 2, cols = 3, row bytes = 6
  uint8_t dest[12];

  ConstUPlanePixels srcUPix(2, 3, 4, srcU);
  ConstVPlanePixels srcVPix(2, 3, 3, srcV);
  UVPlanePixels destPix(2, 3, 6, dest);

  mergePixels(srcUPix, srcVPix, &destPix);

  EXPECT_EQ(1, dest[0]);
  EXPECT_EQ(2, dest[1]);
  EXPECT_EQ(3, dest[2]);
  EXPECT_EQ(4, dest[3]);
  EXPECT_EQ(5, dest[4]);
  EXPECT_EQ(6, dest[5]);

  EXPECT_EQ(7, dest[0 + 6]);
  EXPECT_EQ(8, dest[1 + 6]);
  EXPECT_EQ(9, dest[2 + 6]);
  EXPECT_EQ(10, dest[3 + 6]);
  EXPECT_EQ(11, dest[4 + 6]);
  EXPECT_EQ(12, dest[5 + 6]);
}

TEST_F(PixelTransformsTest, TestMergeUVSkipPlanePixels) {
  ScopeTimer t("test");

  // rows = 3, cols = 4, row bytes = 8
  // U data visual:
  // 1 100 3 101 5 2 7 4 -- Row 1
  // 9 6 11 8 13 10 15 12 -- Row 2
  // 17 14 19 16 21 18 23 20 -- Row 3
  //
  // V data visual:
  // 2 7 4 9 6 11 8 13
  // 10 15 12 17 14 19 16 21
  // 18 23 20 101 22 100 24 101

  const uint8_t src[29] = {
    // clang-format off
    1, 100, 3, 101, 5, 2, 7, 4,
    9, 6, 11, 8, 13, 10, 15, 12,
    17, 14, 19, 16, 21, 18, 23, 20,
    101, 22, 100, 24, 101
    // clang-format on
  };

  // rows = 3, cols = 4, row bytes = 8
  uint8_t dest[24];

  ConstUSkipPlanePixels srcUPix(3, 4, 8, src);
  ConstVSkipPlanePixels srcVPix(3, 4, 8, src + 5);
  UVPlanePixels destPix(4, 3, 6, dest);

  mergePixels(srcUPix, srcVPix, &destPix);

  // Row 1
  EXPECT_EQ(1, dest[0]);
  EXPECT_EQ(2, dest[1]);
  EXPECT_EQ(3, dest[2]);
  EXPECT_EQ(4, dest[3]);
  EXPECT_EQ(5, dest[4]);
  EXPECT_EQ(6, dest[5]);
  EXPECT_EQ(7, dest[6]);
  EXPECT_EQ(8, dest[7]);

  // Row 2
  EXPECT_EQ(9, dest[0 + 8]);
  EXPECT_EQ(10, dest[1 + 8]);
  EXPECT_EQ(11, dest[2 + 8]);
  EXPECT_EQ(12, dest[3 + 8]);
  EXPECT_EQ(13, dest[4 + 8]);
  EXPECT_EQ(14, dest[5 + 8]);
  EXPECT_EQ(15, dest[6 + 8]);
  EXPECT_EQ(16, dest[7 + 8]);

  // Row 3
  EXPECT_EQ(17, dest[0 + 16]);
  EXPECT_EQ(18, dest[1 + 16]);
  EXPECT_EQ(19, dest[2 + 16]);
  EXPECT_EQ(20, dest[3 + 16]);
  EXPECT_EQ(21, dest[4 + 16]);
  EXPECT_EQ(22, dest[5 + 16]);
  EXPECT_EQ(23, dest[6 + 16]);
  EXPECT_EQ(24, dest[7 + 16]);
}

TEST_F(PixelTransformsTest, TestRotate90OneChannel) {
  ScopeTimer t("test");

  // rows = 3, cols = 4, row bytes = 5
  const uint8_t src[15] = {
    // clang-format off
    1, 2, 3, 4, 100,  // row 1
    5, 6, 7, 8, 101,  // row 2
    9, 10, 11, 12, 102,  // row 3
    // clang-format on
  };

  // rows = 4, cols = 3, row bytes = 6
  uint8_t dest[24];

  ConstYPlanePixels srcPix(3, 4, 5, src);
  YPlanePixels destPix(4, 3, 6, dest);

  rotate90Clockwise(srcPix, &destPix);

  EXPECT_EQ(9, dest[0]);
  EXPECT_EQ(5, dest[1]);
  EXPECT_EQ(1, dest[2]);

  EXPECT_EQ(10, dest[0 + 6]);
  EXPECT_EQ(6, dest[1 + 6]);
  EXPECT_EQ(2, dest[2 + 6]);

  EXPECT_EQ(11, dest[0 + 12]);
  EXPECT_EQ(7, dest[1 + 12]);
  EXPECT_EQ(3, dest[2 + 12]);

  EXPECT_EQ(12, dest[0 + 18]);
  EXPECT_EQ(8, dest[1 + 18]);
  EXPECT_EQ(4, dest[2 + 18]);
}

TEST_F(PixelTransformsTest, TestRotateFloat90OneChannel) {
  ScopeTimer t("test");
  // rows = 3, cols = 4
  const float src[12] = {
    // clang-format off
    1, 2.5, 3, 4,  // row 1
    5, 6, 7.2, .8,  // row 2
    9, 10, 11.1, 12  // row 3
    // clang-format on
  };

  // rows = 4, cols = 3
  float dest[12];

  ConstDepthFloatPixels srcPix(3, 4, 4, src);
  DepthFloatPixels destPix(4, 3, 3, dest);

  rotateFloat90Clockwise(srcPix, &destPix);

  EXPECT_EQ(9, dest[0]);
  EXPECT_EQ(5, dest[1]);
  EXPECT_EQ(1, dest[2]);

  EXPECT_EQ(10, dest[0 + 3]);
  EXPECT_EQ(6, dest[1 + 3]);
  EXPECT_EQ(2.5f, dest[2 + 3]);

  EXPECT_EQ(11.1f, dest[0 + 6]);
  EXPECT_EQ(7.2f, dest[1 + 6]);
  EXPECT_EQ(3, dest[2 + 6]);

  EXPECT_EQ(12, dest[0 + 9]);
  EXPECT_EQ(0.8f, dest[1 + 9]);
  EXPECT_EQ(4, dest[2 + 9]);
}

TEST_F(PixelTransformsTest, TestRotate180OneChannel) {
  ScopeTimer t("test");

  // rows = 3, cols = 4, row bytes = 5
  const uint8_t src[15] = {
    // clang-format off
    1, 2, 3, 4, 100,  // row 1
    5, 6, 7, 8, 101,  // row 2
    9, 10, 11, 12, 102,  // row 3
    // clang-format on
  };

  // rows = 3, cols = 4, row bytes = 6
  uint8_t dest[18];

  ConstYPlanePixels srcPix(3, 4, 5, src);
  YPlanePixels destPix(4, 3, 6, dest);

  rotate180Clockwise(srcPix, &destPix);

  EXPECT_EQ(12, dest[0]);
  EXPECT_EQ(11, dest[1]);
  EXPECT_EQ(10, dest[2]);
  EXPECT_EQ(9, dest[3]);

  EXPECT_EQ(8, dest[0 + 6]);
  EXPECT_EQ(7, dest[1 + 6]);
  EXPECT_EQ(6, dest[2 + 6]);
  EXPECT_EQ(5, dest[3 + 6]);

  EXPECT_EQ(4, dest[0 + 12]);
  EXPECT_EQ(3, dest[1 + 12]);
  EXPECT_EQ(2, dest[2 + 12]);
  EXPECT_EQ(1, dest[3 + 12]);
}

TEST_F(PixelTransformsTest, TestRotate270OneChannel) {
  ScopeTimer t("test");

  // rows = 3, cols = 4, row bytes = 5
  const uint8_t src[15] = {
    // clang-format off
    1, 2, 3, 4, 100,  // row 1
    5, 6, 7, 8, 101,  // row 2
    9, 10, 11, 12, 102,  // row 3
    // clang-format on
  };

  // rows = 4, cols = 3, row bytes = 6
  uint8_t dest[24];

  ConstYPlanePixels srcPix(3, 4, 5, src);
  YPlanePixels destPix(4, 3, 6, dest);

  rotate270Clockwise(srcPix, &destPix);

  EXPECT_EQ(4, dest[0]);
  EXPECT_EQ(8, dest[1]);
  EXPECT_EQ(12, dest[2]);

  EXPECT_EQ(3, dest[0 + 6]);
  EXPECT_EQ(7, dest[1 + 6]);
  EXPECT_EQ(11, dest[2 + 6]);

  EXPECT_EQ(2, dest[0 + 12]);
  EXPECT_EQ(6, dest[1 + 12]);
  EXPECT_EQ(10, dest[2 + 12]);

  EXPECT_EQ(1, dest[0 + 18]);
  EXPECT_EQ(5, dest[1 + 18]);
  EXPECT_EQ(9, dest[2 + 18]);
}

TEST_F(PixelTransformsTest, TestRotate90TwoChannelOddRows) {
  ScopeTimer t("test");

  // rows = 3, cols = 2, row bytes = 5
  const uint8_t src[15] = {
    // clang-format off
    1, 2, 3, 4, 100,  // row 1
    5, 6, 7, 8, 101,  // row 2
    9, 10, 11, 12, 102,  // row 3
    // clang-format on
  };

  // rows = 2, cols = 3, row bytes = 10
  uint8_t dest[20];

  ConstUVPlanePixels srcPix(3, 2, 5, src);
  UVPlanePixels destPix(2, 3, 10, dest);

  rotate90Clockwise(srcPix, &destPix);

  EXPECT_EQ(9, dest[0]);
  EXPECT_EQ(10, dest[1]);
  EXPECT_EQ(5, dest[2]);
  EXPECT_EQ(6, dest[3]);
  EXPECT_EQ(1, dest[4]);
  EXPECT_EQ(2, dest[5]);

  EXPECT_EQ(11, dest[0 + 10]);
  EXPECT_EQ(12, dest[1 + 10]);
  EXPECT_EQ(7, dest[2 + 10]);
  EXPECT_EQ(8, dest[3 + 10]);
  EXPECT_EQ(3, dest[4 + 10]);
  EXPECT_EQ(4, dest[5 + 10]);
}

TEST_F(PixelTransformsTest, TestRotate90TwoChannelEvenRows) {
  ScopeTimer t("test");

  // rows = 3, cols = 2, row bytes = 5
  const uint8_t src[25] = {
    // clang-format off
    1, 2, 3, 4, 100,  // row 1
    5, 6, 7, 8, 101,  // row 2
    9, 10, 11, 12, 102,  // row 3
    13, 14, 15, 16, 102,  // row 4
    // clang-format on
  };

  // rows = 2, cols = 3, row bytes = 10
  uint8_t dest[20];

  ConstUVPlanePixels srcPix(4, 2, 5, src);
  UVPlanePixels destPix(2, 4, 10, dest);

  rotate90Clockwise(srcPix, &destPix);

  EXPECT_EQ(13, dest[0]);
  EXPECT_EQ(14, dest[1]);
  EXPECT_EQ(9, dest[2]);
  EXPECT_EQ(10, dest[3]);
  EXPECT_EQ(5, dest[4]);
  EXPECT_EQ(6, dest[5]);
  EXPECT_EQ(1, dest[6]);
  EXPECT_EQ(2, dest[7]);

  EXPECT_EQ(15, dest[0 + 10]);
  EXPECT_EQ(16, dest[1 + 10]);
  EXPECT_EQ(11, dest[2 + 10]);
  EXPECT_EQ(12, dest[3 + 10]);
  EXPECT_EQ(7, dest[4 + 10]);
  EXPECT_EQ(8, dest[5 + 10]);
  EXPECT_EQ(3, dest[6 + 10]);
  EXPECT_EQ(4, dest[7 + 10]);
}

TEST_F(PixelTransformsTest, TestRotate180TwoChannelEvenRows) {
  ScopeTimer t("test");

  // rows = 3, cols = 2, row bytes = 5
  const uint8_t src[25] = {
    // clang-format off
    1, 2, 3, 4, 100,  // row 1
    5, 6, 7, 8, 101,  // row 2
    9, 10, 11, 12, 102,  // row 3
    13, 14, 15, 16, 102,  // row 4
    // clang-format on
  };

  // rows = 2, cols = 3, row bytes = 10
  uint8_t dest[40];

  ConstUVPlanePixels srcPix(4, 2, 5, src);
  UVPlanePixels destPix(4, 2, 10, dest);

  rotate180Clockwise(srcPix, &destPix);

  EXPECT_EQ(15, dest[0]);
  EXPECT_EQ(16, dest[1]);
  EXPECT_EQ(13, dest[2]);
  EXPECT_EQ(14, dest[3]);

  EXPECT_EQ(11, dest[0 + 10]);
  EXPECT_EQ(12, dest[1 + 10]);
  EXPECT_EQ(9, dest[2 + 10]);
  EXPECT_EQ(10, dest[3 + 10]);

  EXPECT_EQ(7, dest[0 + 20]);
  EXPECT_EQ(8, dest[1 + 20]);
  EXPECT_EQ(5, dest[2 + 20]);
  EXPECT_EQ(6, dest[3 + 20]);

  EXPECT_EQ(3, dest[0 + 30]);
  EXPECT_EQ(4, dest[1 + 30]);
  EXPECT_EQ(1, dest[2 + 30]);
  EXPECT_EQ(2, dest[3 + 30]);
}

TEST_F(PixelTransformsTest, TestRotate270TwoChannelEvenRows) {
  ScopeTimer t("test");

  // rows = 3, cols = 2, row bytes = 5
  const uint8_t src[25] = {
    // clang-format off
    1, 2, 3, 4, 100,  // row 1
    5, 6, 7, 8, 101,  // row 2
    9, 10, 11, 12, 102,  // row 3
    13, 14, 15, 16, 102,  // row 4
    // clang-format on
  };

  // rows = 2, cols = 3, row bytes = 10
  uint8_t dest[20];

  ConstUVPlanePixels srcPix(4, 2, 5, src);
  UVPlanePixels destPix(2, 4, 10, dest);

  rotate270Clockwise(srcPix, &destPix);

  EXPECT_EQ(3, dest[0]);
  EXPECT_EQ(4, dest[1]);
  EXPECT_EQ(7, dest[2]);
  EXPECT_EQ(8, dest[3]);
  EXPECT_EQ(11, dest[4]);
  EXPECT_EQ(12, dest[5]);
  EXPECT_EQ(15, dest[6]);
  EXPECT_EQ(16, dest[7]);

  EXPECT_EQ(1, dest[0 + 10]);
  EXPECT_EQ(2, dest[1 + 10]);
  EXPECT_EQ(5, dest[2 + 10]);
  EXPECT_EQ(6, dest[3 + 10]);
  EXPECT_EQ(9, dest[4 + 10]);
  EXPECT_EQ(10, dest[5 + 10]);
  EXPECT_EQ(13, dest[6 + 10]);
  EXPECT_EQ(14, dest[7 + 10]);
}

TEST_F(PixelTransformsTest, TestRotate90IndividualUVChannels) {
  ScopeTimer t("test");

  // rows = 3, cols = 4, row bytes = 4
  const uint8_t srcU[12] = {
    // clang-format off
    1, 3, 5, 7,
    9, 11, 13, 15,
    17, 19, 21, 23
    // clang-format on
  };

  // rows = 3, cols = 4, row bytes = 4
  const uint8_t srcV[12] = {
    // clang-format off
    2, 4, 6, 8,
    10, 12, 14, 16,
    18, 20, 22, 24
    // clang-format on
  };

  // rows = 4, cols = 3, row bytes = 6
  uint8_t dest[24];

  ConstUPlanePixels srcUPix(3, 4, 4, srcU);
  ConstVPlanePixels srcVPix(3, 4, 4, srcV);
  UVPlanePixels destPix(4, 3, 6, dest);

  rotate90Clockwise(srcUPix, srcVPix, &destPix);

  // Row 1
  EXPECT_EQ(17, dest[0]);
  EXPECT_EQ(18, dest[1]);
  EXPECT_EQ(9, dest[2]);
  EXPECT_EQ(10, dest[3]);
  EXPECT_EQ(1, dest[4]);
  EXPECT_EQ(2, dest[5]);

  // Row 2
  EXPECT_EQ(19, dest[0 + 6]);
  EXPECT_EQ(20, dest[1 + 6]);
  EXPECT_EQ(11, dest[2 + 6]);
  EXPECT_EQ(12, dest[3 + 6]);
  EXPECT_EQ(3, dest[4 + 6]);
  EXPECT_EQ(4, dest[5 + 6]);

  // Row 3
  EXPECT_EQ(21, dest[0 + 12]);
  EXPECT_EQ(22, dest[1 + 12]);
  EXPECT_EQ(13, dest[2 + 12]);
  EXPECT_EQ(14, dest[3 + 12]);
  EXPECT_EQ(5, dest[4 + 12]);
  EXPECT_EQ(6, dest[5 + 12]);

  // Row 4
  EXPECT_EQ(23, dest[0 + 18]);
  EXPECT_EQ(24, dest[1 + 18]);
  EXPECT_EQ(15, dest[2 + 18]);
  EXPECT_EQ(16, dest[3 + 18]);
  EXPECT_EQ(7, dest[4 + 18]);
  EXPECT_EQ(8, dest[5 + 18]);
}

TEST_F(PixelTransformsTest, TestRotate90UVSkipChannel) {
  ScopeTimer t("test");

  // Some versions of Android are strange and don't provide us with perfectly interleaved
  // UV bytes. Let's simulate this and ensure we can still rotate + copy bytes correctly.

  // rows = 3, cols = 4, row bytes = 8
  // U data visual:
  // 1 100 3 101 5 2 7 4 -- Row 1
  // 9 6 11 8 13 10 15 12 -- Row 2
  // 17 14 19 16 21 18 23 20 -- Row 3
  //
  // V data visual:
  // 2 7 4 9 6 11 8 13
  // 10 15 12 17 14 19 16 21
  // 18 23 20 101 22 100 24 101

  const uint8_t src[29] = {
    // clang-format off
    1, 100, 3, 101, 5, 2, 7, 4,
    9, 6, 11, 8, 13, 10, 15, 12,
    17, 14, 19, 16, 21, 18, 23, 20,
    101, 22, 100, 24, 101
    // clang-format on
  };

  // rows = 4, cols = 3, row bytes = 6
  uint8_t dest[24];

  ConstUSkipPlanePixels srcUPix(3, 4, 8, src);
  ConstVSkipPlanePixels srcVPix(3, 4, 8, src + 5);
  UVPlanePixels destPix(4, 3, 6, dest);

  rotate90Clockwise(srcUPix, srcVPix, &destPix);

  // Row 1
  EXPECT_EQ(17, dest[0]);
  EXPECT_EQ(18, dest[1]);
  EXPECT_EQ(9, dest[2]);
  EXPECT_EQ(10, dest[3]);
  EXPECT_EQ(1, dest[4]);
  EXPECT_EQ(2, dest[5]);

  // Row 2
  EXPECT_EQ(19, dest[0 + 6]);
  EXPECT_EQ(20, dest[1 + 6]);
  EXPECT_EQ(11, dest[2 + 6]);
  EXPECT_EQ(12, dest[3 + 6]);
  EXPECT_EQ(3, dest[4 + 6]);
  EXPECT_EQ(4, dest[5 + 6]);

  // Row 3
  EXPECT_EQ(21, dest[0 + 12]);
  EXPECT_EQ(22, dest[1 + 12]);
  EXPECT_EQ(13, dest[2 + 12]);
  EXPECT_EQ(14, dest[3 + 12]);
  EXPECT_EQ(5, dest[4 + 12]);
  EXPECT_EQ(6, dest[5 + 12]);

  // Row 4
  EXPECT_EQ(23, dest[0 + 18]);
  EXPECT_EQ(24, dest[1 + 18]);
  EXPECT_EQ(15, dest[2 + 18]);
  EXPECT_EQ(16, dest[3 + 18]);
  EXPECT_EQ(7, dest[4 + 18]);
  EXPECT_EQ(8, dest[5 + 18]);
}

TEST_F(PixelTransformsTest, TestRotate90FourChannel) {
  ScopeTimer t("test");

  // rows = 3, cols = 2, row bytes = 9
  const uint8_t src[27] = {
    // clang-format off
    1,  2,  3,  4,  4,  3,  2, 1, 100,  // row 1
    5,  6,  7,  8,  8,  7,  6, 5, 101,  // row 2
    9, 10, 11, 12, 12, 11, 10, 9, 102,  // row 3
    // clang-format on
  };

  // rows = 2, cols = 3, row bytes = 12
  uint8_t dest[24];

  ConstRGBA8888PlanePixels srcPix(3, 2, 9, src);
  RGBA8888PlanePixels destPix(2, 3, 12, dest);

  rotate90Clockwise(srcPix, &destPix);

  EXPECT_EQ(9, dest[0]);
  EXPECT_EQ(10, dest[1]);
  EXPECT_EQ(11, dest[2]);
  EXPECT_EQ(12, dest[3]);
  EXPECT_EQ(5, dest[4]);
  EXPECT_EQ(6, dest[5]);
  EXPECT_EQ(7, dest[6]);
  EXPECT_EQ(8, dest[7]);
  EXPECT_EQ(1, dest[8]);
  EXPECT_EQ(2, dest[9]);
  EXPECT_EQ(3, dest[10]);
  EXPECT_EQ(4, dest[11]);

  EXPECT_EQ(12, dest[12]);
  EXPECT_EQ(11, dest[13]);
  EXPECT_EQ(10, dest[14]);
  EXPECT_EQ(9, dest[15]);
  EXPECT_EQ(8, dest[16]);
  EXPECT_EQ(7, dest[17]);
  EXPECT_EQ(6, dest[18]);
  EXPECT_EQ(5, dest[19]);
  EXPECT_EQ(4, dest[20]);
  EXPECT_EQ(3, dest[21]);
  EXPECT_EQ(2, dest[22]);
  EXPECT_EQ(1, dest[23]);
}

TEST_F(PixelTransformsTest, TestRotate270FourChannel) {
  ScopeTimer t("test");

  // rows = 3, cols = 2, row bytes = 9
  const uint8_t src[27] = {
    // clang-format off
    1,  2,  3,  4,  4,  3,  2, 1, 100,  // row 1
    5,  6,  7,  8,  8,  7,  6, 5, 101,  // row 2
    9, 10, 11, 12, 12, 11, 10, 9, 102,  // row 3
    // clang-format on
  };

  // rows = 2, cols = 3, row bytes = 12
  uint8_t dest[24];

  ConstRGBA8888PlanePixels srcPix(3, 2, 9, src);
  RGBA8888PlanePixels destPix(2, 3, 12, dest);

  rotate270Clockwise(srcPix, &destPix);

  EXPECT_EQ(4, dest[0]);
  EXPECT_EQ(3, dest[1]);
  EXPECT_EQ(2, dest[2]);
  EXPECT_EQ(1, dest[3]);

  EXPECT_EQ(8, dest[4]);
  EXPECT_EQ(7, dest[5]);
  EXPECT_EQ(6, dest[6]);
  EXPECT_EQ(5, dest[7]);

  EXPECT_EQ(12, dest[8]);
  EXPECT_EQ(11, dest[9]);
  EXPECT_EQ(10, dest[10]);
  EXPECT_EQ(9, dest[11]);

  EXPECT_EQ(1, dest[12]);
  EXPECT_EQ(2, dest[13]);
  EXPECT_EQ(3, dest[14]);
  EXPECT_EQ(4, dest[15]);

  EXPECT_EQ(5, dest[16]);
  EXPECT_EQ(6, dest[17]);
  EXPECT_EQ(7, dest[18]);
  EXPECT_EQ(8, dest[19]);

  EXPECT_EQ(9, dest[20]);
  EXPECT_EQ(10, dest[21]);
  EXPECT_EQ(11, dest[22]);
  EXPECT_EQ(12, dest[23]);
}

TEST_F(PixelTransformsTest, TestComputeHistogram) {
  ScopeTimer t("test");

  // rows = 4, cols = 4, row bytes = 5
  const uint8_t src[20] = {
    // clang-format off
    0, 62, 127, 255, 100,  // row 1
    255, 127, 63, 1, 101,  // row 2
    63, 2, 255, 127, 100,  // row 3
    127, 255, 3, 62, 101,  // row 4
    // clang-format on
  };

  ConstYPlanePixels srcPix(4, 4, 5, src);

  auto hist = computeHistogram(srcPix);

  for (int i = 0; i < 256; ++i) {
    switch (i) {
      case 0:
        EXPECT_EQ(1, hist[i]);
        break;
      case 1:
        EXPECT_EQ(1, hist[i]);
        break;
      case 2:
        EXPECT_EQ(1, hist[i]);
        break;
      case 3:
        EXPECT_EQ(1, hist[i]);
        break;
      case 62:
        EXPECT_EQ(2, hist[i]);
        break;
      case 63:
        EXPECT_EQ(2, hist[i]);
        break;
      case 127:
        EXPECT_EQ(4, hist[i]);
        break;
      case 255:
        EXPECT_EQ(4, hist[i]);
        break;
      default:
        EXPECT_EQ(0, hist[i]);
        break;
    }
  }
}

TEST_F(PixelTransformsTest, TestEstimateExposureScore) {
  TaskQueue taskQueue;
  ThreadPool threadPool(1);

  const uint8_t srcData[] = {
    // clang-format off
    0,   15,  15,  15,  15,   0,    // first row
    15,  15,  25,  25,  25,   250,  // second row
    25,  25,  26,  26,  26,   87,   // third row
    127, 127, 127, 229, 229,  150,  // fourth row
    230, 230, 230, 230, 230,  87,   // fifth row
    230, 230, 230, 230, 230,  41,   // sixth row
    230, 230, 230, 255, 255,  12,   // seventh row
    255, 255, 255, 255, 255,  31,   // eigth row
    // clang-format on
  };

  ConstYPlanePixels src(8, 5, 6, srcData);

  // Resulting histogram looks like:
  // hist[0] = 1;
  // hist[15] = 6;
  // hist[25] = 5;
  // hist[26] = 3;
  // hist[127] = 3;
  // hist[229] = 2;
  // hist[230] = 13;
  // hist[255] = 7;

  // Expected score is (20 - 12) / 40 = .2
  EXPECT_FLOAT_EQ(0.2f, estimateExposureScore(src, &taskQueue, &threadPool, 1));
  ConstYPlanePixels firstColumnSrc(8, 1, 6, srcData);
  EXPECT_FLOAT_EQ(0.125, estimateExposureScore(firstColumnSrc, &taskQueue, &threadPool, 4));
}

#if defined(__ARM_NEON__) || FORCE_NEON_CODEPATH
TEST_F(PixelTransformsTest, TestEstimateExposureScoreNEON) {
  uint8_t srcData[256 * 256];
  for (int i = 0; i < 256; ++i) {
    for (int j = 0; j < 256; ++j) {
      srcData[i * 256 + j] = rand();
    }
  }
  TaskQueue taskQueue;
  ThreadPool threadPool(1);

  ConstYPlanePixels src(256, 3 * 16 * 3, 256, srcData);
  float exposureScore = estimateExposureScore(src, &taskQueue, &threadPool, 1);
  EXPECT_TRUE(abs(exposureScore - estimateExposureScoreNEON(src)) < 1e-6);

  ConstYPlanePixels src2(256, 3 * 16 * 3 - 1, 256, srcData);
  exposureScore = estimateExposureScore(src2, &taskQueue, &threadPool, 1);
  EXPECT_TRUE(abs(exposureScore - estimateExposureScoreNEON(src2)) < 1e-6);
}
#endif

TEST_F(PixelTransformsTest, TestYuvToRgb) {
  ScopeTimer t("test");

  // rows = 4, cols = 4, row bytes = 5
  const uint8_t srcY[20] = {
    // clang-format off
    0,    63, 127, 255, 100,  // row 1
    255, 127,  63,   0, 101,  // row 2
    63,    0,  255, 127, 100,  // row 3
    127, 255,   0,  63, 101,  // row 4
    // clang-format on
  };

  // rows = 2, cols = 2, row bytes = 6
  const uint8_t srcUV[12] = {
    // clang-format off
    128, 128, 128, 128, 100, 101,  // row 1
    0, 255, 255, 0, 100, 101,  // row 2
    // clang-format on
  };

  // rows = 4, cols = 4, row bytes = 30
  uint8_t dest[120];

  ConstYPlanePixels srcYPix(4, 4, 5, srcY);
  ConstUVPlanePixels srcUVPix(2, 2, 6, srcUV);
  RGBA8888PlanePixels destPix(4, 4, 30, dest);

  yuvToRgb(srcYPix, srcUVPix, &destPix);

  // Row 0
  EXPECT_EQ(0, dest[0]);
  EXPECT_EQ(0, dest[1]);
  EXPECT_EQ(0, dest[2]);

  EXPECT_EQ(63, dest[0 + 4]);
  EXPECT_EQ(63, dest[1 + 4]);
  EXPECT_EQ(63, dest[2 + 4]);

  EXPECT_EQ(127, dest[0 + 8]);
  EXPECT_EQ(127, dest[1 + 8]);
  EXPECT_EQ(127, dest[2 + 8]);

  EXPECT_EQ(255, dest[0 + 12]);
  EXPECT_EQ(255, dest[1 + 12]);
  EXPECT_EQ(255, dest[2 + 12]);

  // Row 1
  EXPECT_EQ(255, dest[0 + 30]);
  EXPECT_EQ(255, dest[1 + 30]);
  EXPECT_EQ(255, dest[2 + 30]);

  EXPECT_EQ(127, dest[0 + 4 + 30]);
  EXPECT_EQ(127, dest[1 + 4 + 30]);
  EXPECT_EQ(127, dest[2 + 4 + 30]);

  EXPECT_EQ(63, dest[0 + 8 + 30]);
  EXPECT_EQ(63, dest[1 + 8 + 30]);
  EXPECT_EQ(63, dest[2 + 8 + 30]);

  EXPECT_EQ(0, dest[0 + 12 + 30]);
  EXPECT_EQ(0, dest[1 + 12 + 30]);
  EXPECT_EQ(0, dest[2 + 12 + 30]);

  // Row 2
  EXPECT_EQ(240, dest[0 + 60]);
  EXPECT_EQ(16, dest[1 + 60]);
  EXPECT_EQ(0, dest[2 + 60]);

  EXPECT_EQ(177, dest[0 + 4 + 60]);
  EXPECT_EQ(0, dest[1 + 4 + 60]);
  EXPECT_EQ(0, dest[2 + 4 + 60]);

  EXPECT_EQ(76, dest[0 + 8 + 60]);
  EXPECT_EQ(255, dest[1 + 8 + 60]);
  EXPECT_EQ(255, dest[2 + 8 + 60]);

  EXPECT_EQ(0, dest[0 + 12 + 60]);
  EXPECT_EQ(175, dest[1 + 12 + 60]);
  EXPECT_EQ(255, dest[2 + 12 + 60]);

  // Row 3
  EXPECT_EQ(255, dest[0 + 90]);
  EXPECT_EQ(80, dest[1 + 90]);
  EXPECT_EQ(0, dest[2 + 90]);

  EXPECT_EQ(255, dest[0 + 4 + 90]);
  EXPECT_EQ(208, dest[1 + 4 + 90]);
  EXPECT_EQ(30, dest[2 + 4 + 90]);

  EXPECT_EQ(0, dest[0 + 8 + 90]);
  EXPECT_EQ(48, dest[1 + 8 + 90]);
  EXPECT_EQ(224, dest[2 + 8 + 90]);

  EXPECT_EQ(0, dest[0 + 12 + 90]);
  EXPECT_EQ(111, dest[1 + 12 + 90]);
  EXPECT_EQ(255, dest[2 + 12 + 90]);
}

TEST_F(PixelTransformsTest, TestYuvPlaneToRgb) {
  ScopeTimer t("test");

  // rows = 4, cols = 4, row bytes = 5
  const uint8_t srcY[20] = {
    // clang-format off
    0,    63, 127, 255, 100,  // row 1
    255, 127,  63,   0, 101,  // row 2
    63,    0,  255, 127, 100,  // row 3
    127, 255,   0,  63, 101,  // row 4
    // clang-format on
  };

  // rows = 2, cols = 2, row bytes = 4
  const uint8_t srcU[8] = {
    // clang-format off
    128, 128, 100, 101,  // row 1
    0, 255, 100, 101,  // row 2
    // clang-format on
  };

  // rows = 2, cols = 2, row bytes = 3
  const uint8_t srcV[6] = {
    // clang-format off
    128, 128, 100,   // row 1
    255, 0, 100,   // row 2
    // clang-format on
  };

  // rows = 4, cols = 4, row bytes = 30
  uint8_t dest[120];

  ConstYPlanePixels srcYPix(4, 4, 5, srcY);
  ConstUPlanePixels srcUPix(2, 2, 4, srcU);
  ConstVPlanePixels srcVPix(2, 2, 3, srcV);
  RGBA8888PlanePixels destPix(4, 4, 30, dest);

  yuvToRgb(srcYPix, srcUPix, srcVPix, &destPix);

  // Row 0
  EXPECT_EQ(0, dest[0]);
  EXPECT_EQ(0, dest[1]);
  EXPECT_EQ(0, dest[2]);

  EXPECT_EQ(63, dest[0 + 4]);
  EXPECT_EQ(63, dest[1 + 4]);
  EXPECT_EQ(63, dest[2 + 4]);

  EXPECT_EQ(127, dest[0 + 8]);
  EXPECT_EQ(127, dest[1 + 8]);
  EXPECT_EQ(127, dest[2 + 8]);

  EXPECT_EQ(255, dest[0 + 12]);
  EXPECT_EQ(255, dest[1 + 12]);
  EXPECT_EQ(255, dest[2 + 12]);

  // Row 1
  EXPECT_EQ(255, dest[0 + 30]);
  EXPECT_EQ(255, dest[1 + 30]);
  EXPECT_EQ(255, dest[2 + 30]);

  EXPECT_EQ(127, dest[0 + 4 + 30]);
  EXPECT_EQ(127, dest[1 + 4 + 30]);
  EXPECT_EQ(127, dest[2 + 4 + 30]);

  EXPECT_EQ(63, dest[0 + 8 + 30]);
  EXPECT_EQ(63, dest[1 + 8 + 30]);
  EXPECT_EQ(63, dest[2 + 8 + 30]);

  EXPECT_EQ(0, dest[0 + 12 + 30]);
  EXPECT_EQ(0, dest[1 + 12 + 30]);
  EXPECT_EQ(0, dest[2 + 12 + 30]);

  // Row 2
  EXPECT_EQ(240, dest[0 + 60]);
  EXPECT_EQ(16, dest[1 + 60]);
  EXPECT_EQ(0, dest[2 + 60]);

  EXPECT_EQ(177, dest[0 + 4 + 60]);
  EXPECT_EQ(0, dest[1 + 4 + 60]);
  EXPECT_EQ(0, dest[2 + 4 + 60]);

  EXPECT_EQ(76, dest[0 + 8 + 60]);
  EXPECT_EQ(255, dest[1 + 8 + 60]);
  EXPECT_EQ(255, dest[2 + 8 + 60]);

  EXPECT_EQ(0, dest[0 + 12 + 60]);
  EXPECT_EQ(175, dest[1 + 12 + 60]);
  EXPECT_EQ(255, dest[2 + 12 + 60]);

  // Row 3
  EXPECT_EQ(255, dest[0 + 90]);
  EXPECT_EQ(80, dest[1 + 90]);
  EXPECT_EQ(0, dest[2 + 90]);

  EXPECT_EQ(255, dest[0 + 4 + 90]);
  EXPECT_EQ(208, dest[1 + 4 + 90]);
  EXPECT_EQ(30, dest[2 + 4 + 90]);

  EXPECT_EQ(0, dest[0 + 8 + 90]);
  EXPECT_EQ(48, dest[1 + 8 + 90]);
  EXPECT_EQ(224, dest[2 + 8 + 90]);

  EXPECT_EQ(0, dest[0 + 12 + 90]);
  EXPECT_EQ(111, dest[1 + 12 + 90]);
  EXPECT_EQ(255, dest[2 + 12 + 90]);
}

TEST_F(PixelTransformsTest, TestYuvToBgr) {
  ScopeTimer t("test");

  // rows = 4, cols = 4, row bytes = 5
  const uint8_t srcY[20] = {
    // clang-format off
    0, 63, 127, 255, 100,  // row 1
    255, 127, 63, 0, 101,  // row 2
    63, 0, 255, 127, 100,  // row 3
    127, 255, 0, 63, 101,  // row 4
    // clang-format on
  };

  // rows = 2, cols = 2, row bytes = 6
  const uint8_t srcUV[12] = {
    // clang-format off
    128, 128, 128, 128, 100, 101,  // row 1
    0, 255, 255, 0, 100, 101,  // row 2
    // clang-format on
  };

  // rows = 4, cols = 4, row bytes = 30
  uint8_t dest[120];

  ConstYPlanePixels srcYPix(4, 4, 5, srcY);
  ConstUVPlanePixels srcUVPix(2, 2, 6, srcUV);
  BGR888PlanePixels destPix(4, 4, 30, dest);

  yuvToBgr(srcYPix, srcUVPix, &destPix);

  // Row 0
  EXPECT_EQ(0, dest[0]);
  EXPECT_EQ(0, dest[1]);
  EXPECT_EQ(0, dest[2]);

  EXPECT_EQ(63, dest[0 + 3]);
  EXPECT_EQ(63, dest[1 + 3]);
  EXPECT_EQ(63, dest[2 + 3]);

  EXPECT_EQ(127, dest[0 + 6]);
  EXPECT_EQ(127, dest[1 + 6]);
  EXPECT_EQ(127, dest[2 + 6]);

  EXPECT_EQ(255, dest[0 + 9]);
  EXPECT_EQ(255, dest[1 + 9]);
  EXPECT_EQ(255, dest[2 + 9]);

  // Row 1
  EXPECT_EQ(255, dest[0 + 30]);
  EXPECT_EQ(255, dest[1 + 30]);
  EXPECT_EQ(255, dest[2 + 30]);

  EXPECT_EQ(127, dest[0 + 3 + 30]);
  EXPECT_EQ(127, dest[1 + 3 + 30]);
  EXPECT_EQ(127, dest[2 + 3 + 30]);

  EXPECT_EQ(63, dest[0 + 6 + 30]);
  EXPECT_EQ(63, dest[1 + 6 + 30]);
  EXPECT_EQ(63, dest[2 + 6 + 30]);

  EXPECT_EQ(0, dest[0 + 9 + 30]);
  EXPECT_EQ(0, dest[1 + 9 + 30]);
  EXPECT_EQ(0, dest[2 + 9 + 30]);

  // Row 2
  EXPECT_EQ(0, dest[0 + 60]);
  EXPECT_EQ(16, dest[1 + 60]);
  EXPECT_EQ(240, dest[2 + 60]);

  EXPECT_EQ(0, dest[0 + 3 + 60]);
  EXPECT_EQ(0, dest[1 + 3 + 60]);
  EXPECT_EQ(177, dest[2 + 3 + 60]);

  EXPECT_EQ(255, dest[0 + 6 + 60]);
  EXPECT_EQ(255, dest[1 + 6 + 60]);
  EXPECT_EQ(76, dest[2 + 6 + 60]);

  EXPECT_EQ(255, dest[0 + 9 + 60]);
  EXPECT_EQ(175, dest[1 + 9 + 60]);
  EXPECT_EQ(0, dest[2 + 9 + 60]);

  // Row 3
  EXPECT_EQ(0, dest[0 + 90]);
  EXPECT_EQ(80, dest[1 + 90]);
  EXPECT_EQ(255, dest[2 + 90]);

  EXPECT_EQ(30, dest[0 + 3 + 90]);
  EXPECT_EQ(208, dest[1 + 3 + 90]);
  EXPECT_EQ(255, dest[2 + 3 + 90]);

  EXPECT_EQ(224, dest[0 + 6 + 90]);
  EXPECT_EQ(48, dest[1 + 6 + 90]);
  EXPECT_EQ(0, dest[2 + 6 + 90]);

  EXPECT_EQ(255, dest[0 + 9 + 90]);
  EXPECT_EQ(111, dest[1 + 9 + 90]);
  EXPECT_EQ(0, dest[2 + 9 + 90]);
}

TEST_F(PixelTransformsTest, TestDownsizeAndRotate90TwoChannel) {
  ScopeTimer t("test");

  // rows = 8, cols = 4, row bytes = 9
  const uint8_t srcUV[72] = {
    // clang-format off
    1, 2, 3, 4, 5, 6, 7, 8, 100,  // row 1
    9, 10, 11, 12, 13, 14, 15, 16, 101, // row 2
    17, 18, 19, 20, 21, 22, 23, 24, 100, // row 3
    25, 26, 27, 28, 29, 30, 31, 32, 100, // row 4
    33, 34, 35, 36, 37, 38, 39, 40, 101, // row 5
    41, 42, 43, 44, 45, 46, 47, 48, 100, // row 6
    49, 50, 51, 52, 53, 54, 55, 56, 101, // row 7
    57, 58, 59, 60, 61, 62, 63, 64, 101, // row 8
    // clang-format on
  };

  uint8_t dest[16];

  ConstUVPlanePixels srcPix(8, 4, 9, srcUV);
  UVPlanePixels destPix(4, 2, 4, dest);

  downsizeAndRotate90Clockwise(srcPix, &destPix);

  // Row 1
  EXPECT_EQ(33, dest[0]);
  EXPECT_EQ(34, dest[1]);
  EXPECT_EQ(25, dest[2]);
  EXPECT_EQ(26, dest[3]);

  // Row 2
  EXPECT_EQ(35, dest[0 + 4]);
  EXPECT_EQ(36, dest[1 + 4]);
  EXPECT_EQ(27, dest[2 + 4]);
  EXPECT_EQ(28, dest[3 + 4]);

  // Row 3
  EXPECT_EQ(37, dest[0 + 8]);
  EXPECT_EQ(38, dest[1 + 8]);
  EXPECT_EQ(29, dest[2 + 8]);
  EXPECT_EQ(30, dest[3 + 8]);

  // Row 4
  EXPECT_EQ(39, dest[0 + 12]);
  EXPECT_EQ(40, dest[1 + 12]);
  EXPECT_EQ(31, dest[2 + 12]);
  EXPECT_EQ(32, dest[3 + 12]);
}

TEST_F(PixelTransformsTest, TestDownsizeAndRotate90TwoSkipChannel) {
  ScopeTimer t("test");

  // rows = 8, cols = 4, row bytes = 8
  // U representation:
  // 1, 101, 3, 100, 5, 2, 7, 4, -- row 1
  // 9, 6, 11, 8, 13, 10, 15, 12, -- row 2
  // 17, 14, 19, 16, 21, 18, 23, 20, -- row 3
  // 25, 22, 27, 24, 29, 26, 31, 28, -- row 4
  // 33, 30, 35, 32, 37, 34, 39, 36, -- row 5
  // 41, 38, 43, 40, 45, 42, 47, 44, -- row 6
  // 49, 46, 51, 48, 53, 50, 55, 52, -- row 7
  // 57, 54, 59, 56, 61, 58, 63, 60, -- row 8

  // V representation:
  // 2, 7, 4, 9, 6, 11, 8, 13 -- row 1
  // 10, 15, 12, 17, 14, 19, 16, 21 -- row 2
  // 18, 23, 20, 25, 22, 27, 24, 29 -- row 3
  // 26, 31, 28, 33, 30, 35, 32, 37 -- row 4
  // 34, 39, 36, 41, 38, 43, 40, 45 -- row 5
  // 42, 47, 44, 49, 46, 51, 48, 53 -- row 6
  // 50, 55, 52, 57, 54, 59, 56, 61 -- row 7
  // 58, 63, 60, 101, 62, 100, 64, 101 -- row 8

  const uint8_t srcUV[72] = {
    // clang-format off
    1, 101, 3, 100, 5, 2, 7, 4,
    9, 6, 11, 8, 13, 10, 15, 12,
    17, 14, 19, 16, 21, 18, 23, 20,
    25, 22, 27, 24, 29, 26, 31, 28,
    33, 30, 35, 32, 37, 34, 39, 36,
    41, 38, 43, 40, 45, 42, 47, 44,
    49, 46, 51, 48, 53, 50, 55, 52,
    57, 54, 59, 56, 61, 58, 63, 60,
    101, 62, 100, 64, 101
    // clang-format on
  };

  uint8_t dest[16];

  ConstUSkipPlanePixels srcUPix(8, 4, 8, srcUV);
  ConstVSkipPlanePixels srcVPix(8, 4, 8, srcUV + 5);
  UVPlanePixels destPix(4, 2, 4, dest);

  downsizeAndRotate90Clockwise(srcUPix, srcVPix, &destPix);

  // Row 1
  EXPECT_EQ(33, dest[0]);
  EXPECT_EQ(34, dest[1]);
  EXPECT_EQ(25, dest[2]);
  EXPECT_EQ(26, dest[3]);

  // Row 2
  EXPECT_EQ(35, dest[0 + 4]);
  EXPECT_EQ(36, dest[1 + 4]);
  EXPECT_EQ(27, dest[2 + 4]);
  EXPECT_EQ(28, dest[3 + 4]);

  // Row 3
  EXPECT_EQ(37, dest[0 + 8]);
  EXPECT_EQ(38, dest[1 + 8]);
  EXPECT_EQ(29, dest[2 + 8]);
  EXPECT_EQ(30, dest[3 + 8]);

  // Row 4
  EXPECT_EQ(39, dest[0 + 12]);
  EXPECT_EQ(40, dest[1 + 12]);
  EXPECT_EQ(31, dest[2 + 12]);
  EXPECT_EQ(32, dest[3 + 12]);
}

TEST_F(PixelTransformsTest, TestDownsizeAndRotate90TwoSingleChannel) {
  ScopeTimer t("test");

  // rows = 8, cols = 4, row bytes = 5
  const uint8_t srcU[40] = {
    // clang-format off
    1, 3, 5, 7, 101, // row 1
    9, 11, 13, 15, 101, // row 2
    17, 19, 21, 23, 100, // row 3
    25, 27, 29, 31, 100, // row 4
    33, 35, 37, 39, 101, // row 5
    41, 43, 45, 47, 100, // row 6
    49, 51, 53, 55, 101, // row 7
    57, 59, 61, 63, 101, // row 8
    // clang-format on
  };

  const uint8_t srcV[40] = {
    // clang-format off
    2, 4, 6, 8, 101, // row 1
    10, 12, 14, 16, 100, // row 2
    18, 20, 22, 24, 101, // row 3
    26, 28, 30, 32, 101, // row 4
    34, 36, 38, 40, 100, // row 5
    42, 44, 46, 48, 101, // row 6
    50, 52, 54, 56, 100, // row 7
    58, 60, 62, 64, 101, // row 8
    // clang-format on
  };

  uint8_t dest[16];

  ConstUPlanePixels srcUPix(8, 4, 5, srcU);
  ConstVPlanePixels srcVPix(8, 4, 5, srcV);
  UVPlanePixels destPix(4, 2, 4, dest);

  downsizeAndRotate90Clockwise(srcUPix, srcVPix, &destPix);

  // Row 1
  EXPECT_EQ(33, dest[0]);
  EXPECT_EQ(34, dest[1]);
  EXPECT_EQ(25, dest[2]);
  EXPECT_EQ(26, dest[3]);

  // Row 2
  EXPECT_EQ(35, dest[0 + 4]);
  EXPECT_EQ(36, dest[1 + 4]);
  EXPECT_EQ(27, dest[2 + 4]);
  EXPECT_EQ(28, dest[3 + 4]);

  // Row 3
  EXPECT_EQ(37, dest[0 + 8]);
  EXPECT_EQ(38, dest[1 + 8]);
  EXPECT_EQ(29, dest[2 + 8]);
  EXPECT_EQ(30, dest[3 + 8]);

  // Row 4
  EXPECT_EQ(39, dest[0 + 12]);
  EXPECT_EQ(40, dest[1 + 12]);
  EXPECT_EQ(31, dest[2 + 12]);
  EXPECT_EQ(32, dest[3 + 12]);
}

TEST_F(PixelTransformsTest, TestFlipVertical) {
  ScopeTimer t("flip-vertical");

  // rows = 4, cols = 4, row bytes = 5
  const uint8_t srcU[40] = {
    // clang-format off
    1,  3,  5,  7,  101,  // row 1
    9,  11, 13, 15, 101,  // row 2
    17, 19, 21, 23, 100,  // row 3
    25, 27, 29, 31, 100,  // row 4
    // clang-format on
  };

  // rows = 4, cols = 4, dual channels, row bytes = 9
  const uint8_t srcUV[40] = {
    // clang-format off
    2,  4,  6,  8,  2,  4,  6,  8,  101,  // row 1
    10, 12, 14, 16, 10, 12, 14, 16, 100,  // row 2
    18, 20, 22, 24, 18, 20, 22, 24, 101,  // row 3
    26, 28, 30, 32, 26, 28, 30, 32, 101,  // row 4
    // clang-format on
  };

  uint8_t destU[16];
  uint8_t destUV[16 * 2];

  ConstUPlanePixels srcUPix(4, 4, 5, srcU);
  ConstUVPlanePixels srcUVPix(4, 4, 9, srcUV);
  UPlanePixels destUPix(4, 4, 4, destU);
  UVPlanePixels destUVPix(4, 4, 8, destUV);

  flipVertical(srcUPix, &destUPix);

  // Row 1
  EXPECT_EQ(25, destU[0]);
  EXPECT_EQ(27, destU[1]);
  EXPECT_EQ(29, destU[2]);
  EXPECT_EQ(31, destU[3]);

  // Row 2
  EXPECT_EQ(17, destU[0 + 4]);
  EXPECT_EQ(19, destU[1 + 4]);
  EXPECT_EQ(21, destU[2 + 4]);
  EXPECT_EQ(23, destU[3 + 4]);

  // Row 3
  EXPECT_EQ(9, destU[0 + 8]);
  EXPECT_EQ(11, destU[1 + 8]);
  EXPECT_EQ(13, destU[2 + 8]);
  EXPECT_EQ(15, destU[3 + 8]);

  // Row 4
  EXPECT_EQ(1, destU[0 + 12]);
  EXPECT_EQ(3, destU[1 + 12]);
  EXPECT_EQ(5, destU[2 + 12]);
  EXPECT_EQ(7, destU[3 + 12]);

  // Working on the dual channel
  flipVertical(srcUVPix, &destUVPix);
  // Row 1
  EXPECT_EQ(26, destUV[0]);
  EXPECT_EQ(28, destUV[1]);
  EXPECT_EQ(30, destUV[2]);
  EXPECT_EQ(32, destUV[3]);
  EXPECT_EQ(26, destUV[4]);
  EXPECT_EQ(28, destUV[5]);
  EXPECT_EQ(30, destUV[6]);
  EXPECT_EQ(32, destUV[7]);

  // Row 2
  EXPECT_EQ(18, destUV[0 + 8]);
  EXPECT_EQ(20, destUV[1 + 8]);
  EXPECT_EQ(22, destUV[2 + 8]);
  EXPECT_EQ(24, destUV[3 + 8]);
  EXPECT_EQ(18, destUV[4 + 8]);
  EXPECT_EQ(20, destUV[5 + 8]);
  EXPECT_EQ(22, destUV[6 + 8]);
  EXPECT_EQ(24, destUV[7 + 8]);

  // Row 3
  EXPECT_EQ(10, destUV[0 + 16]);
  EXPECT_EQ(12, destUV[1 + 16]);
  EXPECT_EQ(14, destUV[2 + 16]);
  EXPECT_EQ(16, destUV[3 + 16]);
  EXPECT_EQ(10, destUV[4 + 16]);
  EXPECT_EQ(12, destUV[5 + 16]);
  EXPECT_EQ(14, destUV[6 + 16]);
  EXPECT_EQ(16, destUV[7 + 16]);

  // Row 4
  EXPECT_EQ(2, destUV[0 + 24]);
  EXPECT_EQ(4, destUV[1 + 24]);
  EXPECT_EQ(6, destUV[2 + 24]);
  EXPECT_EQ(8, destUV[3 + 24]);
  EXPECT_EQ(2, destUV[4 + 24]);
  EXPECT_EQ(4, destUV[5 + 24]);
  EXPECT_EQ(6, destUV[6 + 24]);
  EXPECT_EQ(8, destUV[7 + 24]);
}

TEST_F(PixelTransformsTest, TestBgrToGrayscale) {
  ScopeTimer t("test");

  // rows = 3, cols = 2, row bytes = 6
  const uint8_t src[18] = {
    // clang-format off
    0, 0, 0, 127, 127, 127,  // row 1
    0, 0, 255, 0, 255, 0,    // row 2
    255, 0, 0, 100, 150, 200 // row 3
    // clang-format on
  };

  // rows = 3, cols = 2, row bytes = 2
  uint8_t dest[6];

  ConstBGR888PlanePixels srcBgrPix(3, 2, 6, src);
  YPlanePixels destPix(3, 2, 2, dest);
  bgrToGray(srcBgrPix, &destPix);

  // Row 0
  EXPECT_EQ(0, dest[0]);
  EXPECT_EQ(127, dest[1]);

  // Row 1
  EXPECT_EQ(76, dest[0 + 2]);
  EXPECT_EQ(150, dest[1 + 2]);

  // Row 2
  EXPECT_EQ(29, dest[0 + 4]);
  EXPECT_EQ(159, dest[1 + 4]);
}

TEST_F(PixelTransformsTest, TestRgbToGrayscale) {
  ScopeTimer t("test");

  // rows = 3, cols = 2, row bytes = 6
  const uint8_t src[18] = {
    // clang-format off
    0, 0, 0, 127, 127, 127,  // row 1
    255, 0, 0, 0, 255, 0,    // row 2
    0, 0, 255, 200, 150, 100 // row 3
    // clang-format on
  };

  // rows = 3, cols = 2, row bytes = 2
  uint8_t dest[6];

  ConstRGB888PlanePixels srcPix(3, 2, 6, src);
  YPlanePixels destPix(3, 2, 2, dest);
  rgbToGray(srcPix, &destPix);

  // Row 0
  EXPECT_EQ(0, dest[0]);
  EXPECT_EQ(127, dest[1]);

  // Row 1
  EXPECT_EQ(76, dest[0 + 2]);
  EXPECT_EQ(150, dest[1 + 2]);

  // Row 2
  EXPECT_EQ(29, dest[0 + 4]);
  EXPECT_EQ(159, dest[1 + 4]);
}

TEST_F(PixelTransformsTest, TestRgbaToGrayscale) {
  ScopeTimer t("test");

  // rows = 3, cols = 2, row bytes = 6
  const uint8_t src[24] = {
    // clang-format off
    0, 0, 0, 0, 127, 127, 127, 127,  // row 1
    255, 0, 0, 123, 0, 255, 0, 231,    // row 2
    0, 0, 255, 50, 200, 150, 100, 50 // row 3
    // clang-format on
  };

  // rows = 3, cols = 2, row bytes = 2
  uint8_t dest[6];

  ConstRGBA8888PlanePixels srcPix(3, 2, 8, src);
  YPlanePixels destPix(3, 2, 2, dest);
  rgbToGray(srcPix, &destPix);

  // Row 0
  EXPECT_EQ(0, dest[0]);
  EXPECT_EQ(127, dest[1]);

  // Row 1
  EXPECT_EQ(76, dest[0 + 2]);
  EXPECT_EQ(150, dest[1 + 2]);

  // Row 2
  EXPECT_EQ(29, dest[0 + 4]);
  EXPECT_EQ(159, dest[1 + 4]);
}

TEST_F(PixelTransformsTest, TestFloatToRgbaGrayscale) {
  ScopeTimer t("test");

  // rows = 4, cols = 2
  const float src[8] = {
    // clang-format off
    0.755, 0.2,  // row 1
    0.5, 1.6,    // row 2
    10, 100,     // row 3
    5, 9         // row 4
    // clang-format on
  };

  // rows = 4, cols = 2, row bytes = 8
  uint8_t dest[32];

  ConstFloatPixels srcPix(4, 2, 2, src);
  RGBA8888PlanePixels destPix(4, 2, 8, dest);
  floatToRgbaGray(srcPix, &destPix, 0.5, 10);

  // Row 0
  EXPECT_EQ(6, dest[0]);
  EXPECT_EQ(6, dest[1]);
  EXPECT_EQ(6, dest[2]);

  EXPECT_EQ(0, dest[0 + 4]);
  EXPECT_EQ(0, dest[1 + 4]);
  EXPECT_EQ(0, dest[2 + 4]);

  // Row 1
  EXPECT_EQ(0, dest[0 + 8]);
  EXPECT_EQ(0, dest[1 + 8]);
  EXPECT_EQ(0, dest[2 + 8]);

  EXPECT_EQ(29, dest[0 + 4 + 8]);
  EXPECT_EQ(29, dest[1 + 4 + 8]);
  EXPECT_EQ(29, dest[2 + 4 + 8]);

  // Row 2
  EXPECT_EQ(255, dest[0 + 16]);
  EXPECT_EQ(255, dest[1 + 16]);
  EXPECT_EQ(255, dest[2 + 16]);

  EXPECT_EQ(255, dest[0 + 4 + 16]);
  EXPECT_EQ(255, dest[1 + 4 + 16]);
  EXPECT_EQ(255, dest[2 + 4 + 16]);

  // Row 3
  EXPECT_EQ(120, dest[0 + 24]);
  EXPECT_EQ(120, dest[1 + 24]);
  EXPECT_EQ(120, dest[2 + 24]);

  EXPECT_EQ(228, dest[0 + 4 + 24]);
  EXPECT_EQ(228, dest[1 + 4 + 24]);
  EXPECT_EQ(228, dest[2 + 4 + 24]);
}

TEST_F(PixelTransformsTest, TestFloatToRgba) {
  ScopeTimer t("test");

  // rows = 4, cols = 2
  const float src[8] = {
    // clang-format off
    0.75, 0.2,  // row 1
    0.5, 1.6,    // row 2
    10, 100,     // row 3
    5, 9         // row 4
    // clang-format on
  };

  // rows = 4, cols = 2, row bytes = 8
  uint8_t dest[32];

  ConstFloatPixels srcPix(4, 2, 2, src);
  RGBA8888PlanePixels destPix(4, 2, 8, dest);
  floatToRgba(srcPix, &destPix, VIRIDIS_RGB_256, 0.5, 10);

  // Row 0
  EXPECT_EQ(70, dest[0]);
  EXPECT_EQ(9, dest[1]);
  EXPECT_EQ(92, dest[2]);

  EXPECT_EQ(68, dest[0 + 4]);
  EXPECT_EQ(1, dest[1 + 4]);
  EXPECT_EQ(84, dest[2 + 4]);

  // Row 1
  EXPECT_EQ(68, dest[0 + 8]);
  EXPECT_EQ(1, dest[1 + 8]);
  EXPECT_EQ(84, dest[2 + 8]);

  EXPECT_EQ(71, dest[0 + 4 + 8]);
  EXPECT_EQ(40, dest[1 + 4 + 8]);
  EXPECT_EQ(120, dest[2 + 4 + 8]);

  // Row 2
  EXPECT_EQ(253, dest[0 + 16]);
  EXPECT_EQ(231, dest[1 + 16]);
  EXPECT_EQ(36, dest[2 + 16]);

  EXPECT_EQ(253, dest[0 + 4 + 16]);
  EXPECT_EQ(231, dest[1 + 4 + 16]);
  EXPECT_EQ(36, dest[2 + 4 + 16]);

  // Row 3
  EXPECT_EQ(35, dest[0 + 24]);
  EXPECT_EQ(137, dest[1 + 24]);
  EXPECT_EQ(141, dest[2 + 24]);

  EXPECT_EQ(183, dest[0 + 4 + 24]);
  EXPECT_EQ(221, dest[1 + 4 + 24]);
  EXPECT_EQ(41, dest[2 + 4 + 24]);
}

TEST_F(PixelTransformsTest, TestFloatToRgbaRemoveLetterbox) {
  ScopeTimer t("test");

  // rows = 2, cols = 4
  const float src[8] = {
    // clang-format off
    0.7, 0.2, 0.5, 0.6, // row 1
    0.8, 0.6, 0.9, 0.4, // row 2
    // clang-format on
  };

  // rows = 4, cols = 2, row bytes = 8
  uint8_t dest[16];

  ConstFloatPixels srcPix(2, 4, 4, src);
  RGBA8888PlanePixels destPix(2, 2, 8, dest);
  floatToRgbaRemoveLetterbox(srcPix, &destPix, GRAYSCALE_RGB_256, 0.0f, 1.0f);

  // Row 0
  EXPECT_EQ(51, dest[0]);
  EXPECT_EQ(51, dest[1]);
  EXPECT_EQ(51, dest[2]);
  EXPECT_EQ(255, dest[3]);

  EXPECT_EQ(127, dest[0 + 4]);
  EXPECT_EQ(127, dest[1 + 4]);
  EXPECT_EQ(127, dest[2 + 4]);
  EXPECT_EQ(255, dest[3 + 4]);

  // Row 1
  EXPECT_EQ(153, dest[0 + 8]);
  EXPECT_EQ(153, dest[1 + 8]);
  EXPECT_EQ(153, dest[2 + 8]);
  EXPECT_EQ(255, dest[3 + 8]);

  EXPECT_EQ(229, dest[0 + 12]);
  EXPECT_EQ(229, dest[1 + 12]);
  EXPECT_EQ(229, dest[2 + 12]);
  EXPECT_EQ(255, dest[3 + 12]);
}

TEST_F(PixelTransformsTest, TestFloatToRgbaGrayRemoveLetterbox) {
  ScopeTimer t("test");

  // rows = 2, cols = 4
  const float src[8] = {
    // clang-format off
    0.7, 0.2, 0.5, 0.6, // row 1
    0.8, 0.6, 0.9, 0.4, // row 2
    // clang-format on
  };

  // rows = 4, cols = 2, row bytes = 8
  uint8_t dest[16];

  ConstFloatPixels srcPix(2, 4, 4, src);
  RGBA8888PlanePixels destPix(2, 2, 8, dest);
  floatToRgbaGrayRemoveLetterbox(srcPix, &destPix, 0.0f, 1.0f);

  // Row 0
  EXPECT_EQ(51, dest[0]);
  EXPECT_EQ(51, dest[1]);
  EXPECT_EQ(51, dest[2]);
  EXPECT_EQ(255, dest[3]);

  EXPECT_EQ(127, dest[0 + 4]);
  EXPECT_EQ(127, dest[1 + 4]);
  EXPECT_EQ(127, dest[2 + 4]);
  EXPECT_EQ(255, dest[3 + 4]);

  // Row 1
  EXPECT_EQ(153, dest[0 + 8]);
  EXPECT_EQ(153, dest[1 + 8]);
  EXPECT_EQ(153, dest[2 + 8]);
  EXPECT_EQ(255, dest[3 + 8]);

  EXPECT_EQ(229, dest[0 + 12]);
  EXPECT_EQ(229, dest[1 + 12]);
  EXPECT_EQ(229, dest[2 + 12]);
  EXPECT_EQ(255, dest[3 + 12]);
}

TEST_F(PixelTransformsTest, TestMean) {
  ScopeTimer t("test");

  // rows = 3, cols = 2, row bytes = 6
  const uint8_t src[26] = {
    // clang-format off
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,  // row 1
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13    // row 2
    // clang-format on
  };

  ConstOneChannelPixels srcPix1(2, 12, 13, src);
  ConstTwoChannelPixels srcPix2(2, 6, 13, src);
  ConstThreeChannelPixels srcPix3(2, 4, 13, src);
  ConstFourChannelPixels srcPix4(2, 3, 13, src);

  auto one = meanPixelValue(srcPix1);
  auto two = meanPixelValue(srcPix2);
  auto three = meanPixelValue(srcPix3);
  auto four = meanPixelValue(srcPix4);

  EXPECT_FLOAT_EQ(6.0f, one[0]);

  EXPECT_FLOAT_EQ(5.5f, two[0]);
  EXPECT_FLOAT_EQ(6.5f, two[1]);

  EXPECT_FLOAT_EQ(5.0f, three[0]);
  EXPECT_FLOAT_EQ(6.0f, three[1]);
  EXPECT_FLOAT_EQ(7.0f, three[2]);

  EXPECT_FLOAT_EQ(4.5f, four[0]);
  EXPECT_FLOAT_EQ(5.5f, four[1]);
  EXPECT_FLOAT_EQ(6.5f, four[2]);
  EXPECT_FLOAT_EQ(7.5f, four[3]);
}

TEST_F(PixelTransformsTest, TestLetterboxDimension) {
  ScopeTimer t("test");

  int letterboxWidth = 0;
  int letterboxHeight = 0;
  letterboxDimensionSameAspectRatio(640, 480, 256, 144, &letterboxWidth, &letterboxHeight);
  EXPECT_EQ(letterboxWidth, 192);
  EXPECT_EQ(letterboxHeight, 144);
}

TEST_F(PixelTransformsTest, TestCropDimension) {
  ScopeTimer t("test");

  int cropWidth = 0;
  int cropHeight = 0;
  cropDimensionByAspectRatio(480, 640, 9.0 / 16.0, &cropWidth, &cropHeight);
  EXPECT_EQ(cropWidth, 360);
  EXPECT_EQ(cropHeight, 640);

  cropDimensionByAspectRatio(480, 640, 1.0, &cropWidth, &cropHeight);
  EXPECT_EQ(cropWidth, 480);
  EXPECT_EQ(cropHeight, 480);
}

TEST_F(PixelTransformsTest, TestToLetterboxRGBFloat0to1) {
  ScopeTimer t("test");

  // rows = 4, cols = 2, row bytes = 8
  const int rows = 4;
  const int cols = 2;
  const uint8_t src[32] = {
    // clang-format off
    0, 15, 33, 255, 0, 15, 33, 255,            // row 1
    64, 72, 103, 255, 64, 72, 103, 255,        // row 2
    127, 160, 200, 255, 127, 160, 200, 255,    // row 3
    180, 230, 255, 255, 180, 230, 255, 255,    // row 4
    // clang-format on
  };

  float gt[rows * cols * 3];
  float result[rows * cols * 3];
  int srcInd = 0;
  int dstInd = 0;
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      gt[dstInd] = static_cast<float>(src[srcInd]) / 255.0f;
      gt[dstInd + 1] = static_cast<float>(src[srcInd + 1]) / 255.0f;
      gt[dstInd + 2] = static_cast<float>(src[srcInd + 2]) / 255.0f;
      dstInd += 3;
      srcInd += 4;
    }
  }

  ConstRGBA8888PlanePixels srcPix(rows, cols, cols * 4, src);

  toLetterboxRGBFloat0To1(srcPix, cols, rows, result);

  for (int i = 0; i < rows * cols * 3; ++i) {
    EXPECT_FLOAT_EQ(result[i], gt[i]);
  }
}

TEST_F(PixelTransformsTest, TestToLetterboxRGBFloat0to1FlipX) {
  ScopeTimer t("test");

  // rows = 4, cols = 2, row bytes = 8
  const int rows = 4;
  const int cols = 2;
  const int rowBytes = 8;
  const uint8_t src[32] = {
    // clang-format off
    0, 15, 33, 255, 0, 65, 83, 255,         // row 1
    64, 72, 103, 255, 124, 142, 153, 255,   // row 2
    127, 160, 200, 255, 77, 80, 100, 255,   // row 3
    180, 230, 255, 255, 80, 240, 215, 255,  // row 4
    // clang-format on
  };

  float gt[rows * cols * 3];
  float result[rows * cols * 3];
  int srcInd = 0;
  int dstInd = 0;
  for (int r = 0; r < rows; ++r) {
    srcInd = (r + 1) * rowBytes - 4;
    for (int c = 0; c < cols; ++c) {
      gt[dstInd] = static_cast<float>(src[srcInd]) / 255.0f;
      gt[dstInd + 1] = static_cast<float>(src[srcInd + 1]) / 255.0f;
      gt[dstInd + 2] = static_cast<float>(src[srcInd + 2]) / 255.0f;
      dstInd += 3;
      srcInd -= 4;
    }
  }

  ConstRGBA8888PlanePixels srcPix(rows, cols, cols * 4, src);

  toLetterboxRGBFloat0To1FlipX(srcPix, cols, rows, result);

  for (int i = 0; i < rows * cols * 3; ++i) {
    EXPECT_FLOAT_EQ(result[i], gt[i]);
  }
}

TEST_F(PixelTransformsTest, TestToLetterboxRGBFloatN1to1) {
  ScopeTimer t("test");

  // rows = 4, cols = 2, row bytes = 8
  const int rows = 4;
  const int cols = 2;
  const uint8_t src[32] = {
    // clang-format off
    0, 15, 33, 255, 0, 15, 33, 255,            // row 1
    64, 72, 103, 255, 64, 72, 103, 255,        // row 2
    127, 160, 200, 255, 127, 160, 200, 255,    // row 3
    180, 230, 255, 255, 180, 230, 255, 255,    // row 4
    // clang-format on
  };

  float gt[rows * cols * 3];
  float result[rows * cols * 3];
  int srcInd = 0;
  int dstInd = 0;
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      gt[dstInd] = static_cast<float>(src[srcInd]) / 127.5f - 1.0f;
      gt[dstInd + 1] = static_cast<float>(src[srcInd + 1]) / 127.5f - 1.0f;
      gt[dstInd + 2] = static_cast<float>(src[srcInd + 2]) / 127.5f - 1.0f;
      dstInd += 3;
      srcInd += 4;
    }
  }

  ConstRGBA8888PlanePixels srcPix(rows, cols, cols * 4, src);

  toLetterboxRGBFloatN1to1(srcPix, cols, rows, result);

  for (int i = 0; i < rows * cols * 3; ++i) {
    EXPECT_FLOAT_EQ(result[i], gt[i]);
  }
}

TEST_F(PixelTransformsTest, TestFloatToSingleFloatRescale0To1) {
  ScopeTimer t("test");

  // rows = 4, cols = 2
  const float src[8] = {
    // clang-format off
    0.75,  0.2,  // row 1
     0.5,  1.0,  // row 2
     0.6, 10.0,  // row 3
     0.7,  0.9,  // row 4
    // clang-format on
  };

  float dst[8];

  float result[8] = {
    // clang-format off
    1.0, 0.0,  // row 1
    0.0, 1.0,  // row 2
    0.4, 1.0,  // row 3
    0.8, 1.0,  // row 4
    // clang-format on
  };

  ConstOneChannelFloatPixels srcPix(4, 2, 2, src);
  OneChannelFloatPixels dstPix(4, 2, 2, dst);

  floatToOneChannelFloatRescale0To1(srcPix, &dstPix, 0.5f, 0.75f);
  for (int i = 0; i < 8; ++i) {
    EXPECT_FLOAT_EQ(result[i], dst[i]);
  }
}

TEST_F(PixelTransformsTest, SplitPixels) {
  // rows = 4, cols = 4, dual channels, row bytes = 9
  const uint8_t srcUV[40] = {
    // clang-format off
    1,  4,  3, 8,  5,  12,  7,  16,  101,  // row 1
    9, 20, 11, 24, 13, 28, 15, 32, 100,    // row 2
    17, 36, 19, 40, 21, 44, 23, 48, 101,   // row 3
    25, 52, 27, 56, 29, 60, 31, 64, 101,   // row 4
    // clang-format on
  };
  ConstUVPlanePixels srcUvPix(4, 4, 9, srcUV);
  UPlanePixelBuffer uBuf{4, 4};
  VPlanePixelBuffer vBuf{4, 4};
  splitPixels(srcUvPix, uBuf.pixels(), vBuf.pixels());

  uint8_t *uData = uBuf.pixels().pixels();
  uint8_t *vData = vBuf.pixels().pixels();
  for (int i = 0; i < 16; i++) {
    EXPECT_EQ(i * 2 + 1, uData[i]);
    EXPECT_EQ((i + 1) * 4, vData[i]);
  }
}

TEST_F(PixelTransformsTest, SplitThenMerge) {
  // rows = 4, cols = 4, dual channels, row bytes = 9
  const uint8_t srcUV[40] = {
    // clang-format off
    1,  4,  3, 8,  5,  12,  7,  16,  101,  // row 1
    9, 20, 11, 24, 13, 28, 15, 32, 100,    // row 2
    17, 36, 19, 40, 21, 44, 23, 48, 101,   // row 3
    25, 52, 27, 56, 29, 60, 31, 64, 101,   // row 4
    // clang-format on
  };
  ConstUVPlanePixels srcUvPix(4, 4, 9, srcUV);
  UPlanePixelBuffer uBuf{4, 4};
  VPlanePixelBuffer vBuf{4, 4};
  splitPixels(srcUvPix, uBuf.pixels(), vBuf.pixels());

  UVPlanePixelBuffer mergedUv(4, 4);
  auto mergedUvPixels = mergedUv.pixels();
  mergePixels(uBuf.pixels(), vBuf.pixels(), &mergedUvPixels);

  auto src = srcUvPix.pixels();
  auto merged = mergedUvPixels.pixels();
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      EXPECT_EQ(src[i * srcUvPix.rowBytes() + j], merged[i * mergedUvPixels.rowBytes() + j]);
    }
  }
}

TEST_F(PixelTransformsTest, ColorMatConstruction) {
  // Check the basic formula that goes from coefficients to matrices.
  auto bt601 = ColorMat::rgbToYCbCrBt601Digital();
  auto bt601i = bt601.inv();

  // Expected values from Wikipedia: https://en.wikipedia.org/wiki/YCbCr
  // RGB -> Y'
  EXPECT_NEAR(bt601(0, 0), 65.481f / 255.0f, 5e-5f);
  EXPECT_NEAR(bt601(0, 1), 128.553f / 255.0f, 5e-5f);
  EXPECT_NEAR(bt601(0, 2), 24.966f / 255.0f, 5e-5f);
  EXPECT_NEAR(bt601(0, 3), 16.0f, 5e-5f);

  // RGB -> Cb
  EXPECT_NEAR(bt601(1, 0), -37.797f / 255.0f, 5e-5f);
  EXPECT_NEAR(bt601(1, 1), -74.203f / 255.0f, 5e-5f);
  EXPECT_NEAR(bt601(1, 2), 112.0f / 255.0f, 5e-5f);
  EXPECT_NEAR(bt601(1, 3), 128.0f, 5e-5f);

  // RGB -> Cr
  EXPECT_NEAR(bt601(2, 0), 112.0f / 255.0f, 5e-5f);
  EXPECT_NEAR(bt601(2, 1), -93.786f / 255.0f, 5e-5f);
  EXPECT_NEAR(bt601(2, 2), -18.214f / 255.0f, 5e-5f);
  EXPECT_NEAR(bt601(2, 3), 128.0f, 5e-5f);

  // Bottom Row
  EXPECT_NEAR(bt601(3, 0), 0.0f, 5e-5f);
  EXPECT_NEAR(bt601(3, 1), 0.0f, 5e-5f);
  EXPECT_NEAR(bt601(3, 2), 0.0f, 5e-5f);
  EXPECT_NEAR(bt601(3, 3), 1.0f, 5e-5f);

  // Y'CbCr -> R'
  EXPECT_NEAR(bt601i(0, 0), 255.0f / 219.0f, 5e-5f);
  EXPECT_NEAR(bt601i(0, 1), 0.0f, 5e-5f);
  EXPECT_NEAR(bt601i(0, 2), 1.402f * 255.0f / 224.0f, 5e-5f);
  EXPECT_NEAR(bt601i(0, 3), -16.0f * 255.0f / 219.0f - 128.0f * 1.402f * 255.0f / 224.0f, 5e-5f);

  // Y'CbCr -> G'
  EXPECT_NEAR(bt601i(1, 0), 255.0f / 219.0f, 5e-5f);
  EXPECT_NEAR(bt601i(1, 1), -1.772f * (0.114f / 0.587f) * 255.0f / 224.0f, 5e-5f);
  EXPECT_NEAR(bt601i(1, 2), -1.402f * (0.299f / 0.587f) * 255.0f / 224.0f, 5e-5f);
  EXPECT_NEAR(
    bt601i(1, 3),
    -16.0f * 255.0f / 219.0f + 128.0f * 1.772f * (0.114f / 0.587f) * 255.0f / 224.0f
      + 128.0f * 1.402f * (0.299f / 0.587f) * 255.0f / 224.0f,
    5e-5f);

  // Y'CbCr -> B'
  EXPECT_NEAR(bt601i(2, 0), 255.0f / 219.0f, 5e-5f);
  EXPECT_NEAR(bt601i(2, 1), 1.772f * 255.0f / 224.0f, 5e-5f);
  EXPECT_NEAR(bt601i(2, 2), 0.0f, 5e-5f);
  EXPECT_NEAR(bt601i(2, 3), -16.0f * 255.0f / 219.0f - 128.0f * 1.772f * 255.0f / 224.0f, 5e-5f);

  // Bottom Row
  EXPECT_NEAR(bt601i(3, 0), 0.0f, 5e-5f);
  EXPECT_NEAR(bt601i(3, 1), 0.0f, 5e-5f);
  EXPECT_NEAR(bt601i(3, 2), 0.0f, 5e-5f);
  EXPECT_NEAR(bt601i(3, 3), 1.0f, 5e-5f);
}

TEST_F(PixelTransformsTest, ColorMatCoefficients) {
  // Spot check the expected coefficients for variants.
  {
    auto m = ColorMat::rgbToYCbCrBt601Digital();
    EXPECT_NEAR(m(0, 0), 0.299f * 219.0f / 255.0f, 1e-5f);
    EXPECT_NEAR(m(0, 1), 0.587f * 219.0f / 255.0f, 1e-5f);
    EXPECT_NEAR(m(0, 2), 0.114f * 219.0f / 255.0f, 1e-5f);
  }
  {
    auto m = ColorMat::rgbToYCbCrBt709Digital();
    EXPECT_NEAR(m(0, 0), 0.2126f * 219.0f / 255.0f, 1e-5f);
    EXPECT_NEAR(m(0, 1), 0.7152f * 219.0f / 255.0f, 1e-5f);
    EXPECT_NEAR(m(0, 2), 0.0722f * 219.0f / 255.0f, 1e-5f);
  }
  {
    auto m = ColorMat::rgbToYCbCrBt2020NcDigital();
    EXPECT_NEAR(m(0, 0), 0.2627f * 219.0f / 255.0f, 1e-5f);
    EXPECT_NEAR(m(0, 1), 0.6780f * 219.0f / 255.0f, 1e-5f);
    EXPECT_NEAR(m(0, 2), 0.0593f * 219.0f / 255.0f, 1e-5f);
  }
  {
    auto m = ColorMat::rgbToYCbCrJpg();
    EXPECT_NEAR(m(0, 0), 0.299f, 1e-5f);
    EXPECT_NEAR(m(0, 1), 0.587f, 1e-5f);
    EXPECT_NEAR(m(0, 2), 0.114f, 1e-5f);
  }
}

TEST_F(PixelTransformsTest, ApplyColorMat) {
  // rows = 4, cols = 2, row bytes = 10
  const int rows = 4;
  const int cols = 2;
  // Source has two padding bytes per row.
  const uint8_t src[40] = {
    // clang-format off
      0,  15,  33, 255,  15,  33,   0, 254, 127, 127,  // row 0
     64,  72, 103, 253,  72, 103,  64, 252, 127, 127,  // row 1
    127, 160, 200, 251, 160, 200, 127, 250, 127, 127,  // row 2
    180, 230, 255, 249, 230, 255, 180, 248, 127, 127,  // row 3
    // clang-format on
  };

  // Dest has one padding byte per row.
  uint8_t dest[36] = {
    // clang-format off
      0,   0,   0,   0,   0,   0,   0,   0,  63,  // row 0
      0,   0,   0,   0,   0,   0,   0,   0, 127,  // row 0
      0,   0,   0,   0,   0,   0,   0,   0, 190,  // row 0
      0,   0,   0,   0,   0,   0,   0,   0, 255,  // row 0
    // clang-format on
  };

  // Expectation is that:
  // - red and blue are swapped
  // - green has one added (clipped to 255)
  // - alpha is unchanged
  // - dest padding is unchanged
  const uint8_t expected[36] = {
    // clang-format off
     33,  16,   0, 255,   0,  34,  15, 254,  63,  // row 0
    103,  73,  64, 253,  64, 104,  72, 252, 127,  // row 1
    200, 161, 127, 251, 127, 201, 160, 250, 190,  // row 2
    255, 231, 180, 249, 180, 255, 230, 248, 255,  // row 3
    // clang-format on
  };

  ConstRGBA8888PlanePixels srcPix(rows, cols, cols * 4 + 2, src);
  RGBA8888PlanePixels destPix(rows, cols, cols * 4 + 1, dest);

  HMatrix m {
    {0.0f, 0.0f, 1.0f, 0.0f},  // Swap blue to red
    {0.0f, 1.0f, 0.0f, 1.0f},  // Add one to green
    {1.0f, 0.0f, 0.0f, 0.0f},  // Swap red to blue
    {0.0f, 0.0f, 0.0f, 1.0f},  // Bottom Row
  };

  applyColorMatrixKeepAlpha(m, srcPix, &destPix);

  for (int r = 0; r < 4; ++r) {
    for (int c = 0; c < 9; ++c) {
      EXPECT_EQ(dest[r * 9 + c], expected[r * 9 + c]) << "r=" << r << " c=" << c;
    }
  }
}

}  // namespace c8
