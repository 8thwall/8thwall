// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":linear-bin",
    "//c8:hpoint",
    "//reality/engine/features:frame-point",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xb0a56e1c);

#include "c8/hpoint.h"
#include "gtest/gtest.h"
#include "reality/engine/binning/linear-bin.h"
#include "reality/engine/features/frame-point.h"

namespace c8 {

class LinearBinTest : public ::testing::Test {};

TEST_F(LinearBinTest, LinearBinNumberTest) {
  LinearBin bin(32, 10, 330);
  EXPECT_EQ(0, bin.binNum(-1000));
  EXPECT_EQ(0, bin.binNum(9));
  EXPECT_EQ(0, bin.binNum(10));
  EXPECT_EQ(0, bin.binNum(19));
  EXPECT_EQ(1, bin.binNum(20));
  EXPECT_EQ(2, bin.binNum(39));
  EXPECT_EQ(3, bin.binNum(40));
  EXPECT_EQ(31, bin.binNum(329));
  EXPECT_EQ(31, bin.binNum(330));
  EXPECT_EQ(31, bin.binNum(1000));
}

TEST_F(LinearBinTest, BinNumberTest) {
  c8_PixelPinholeCameraModel k{320, 240, 0.0f, 0.0f, 1.0f, -1.0f};
  HPoint2 mn;
  HPoint2 mx;
  frameBounds(k, &mn, &mx);
  FrameBin bin(mn.x(), mn.y(), mx.x(), mx.y(), 32, 24);
  EXPECT_EQ(131, bin.binNumPt(30.0f, 40.0f));
}

TEST_F(LinearBinTest, BinNumberTest2) {
  c8_PixelPinholeCameraModel k{480, 640, 240.0f, 320.0f, 625.0f, 625.0f};
  HPoint2 mn;
  HPoint2 mx;
  frameBounds(k, &mn, &mx);
  FrameBin bin(mn.x(), mn.y(), mx.x(), mx.y(), 20, 30);
  EXPECT_EQ(0, bin.binNumPt(-.384f, -0.512f));
  EXPECT_EQ(19, bin.binNumPt(.384f, -0.512f));
  EXPECT_EQ(580, bin.binNumPt(-.384f, 0.512f));
  EXPECT_EQ(599, bin.binNumPt(.384f, 0.512f));
}

TEST_F(LinearBinTest, BinNumberZeroWidthTest) {
  c8_PixelPinholeCameraModel k;
  HPoint2 mn;
  HPoint2 mx;
  frameBounds(k, &mn, &mx);
  FrameBin bin(mn.x(), mn.y(), mx.x(), mx.y(), 32, 24);
  EXPECT_EQ(0, bin.binNumPt(30.0f, 40.0f));
}

TEST_F(LinearBinTest, BinsForPoint) {
  BinMap map(5, 5);
  map.reset<HPoint2>(-1.0f, -1.0f, 1.0f, 1.0f, {});
  Vector<size_t> bins;

  // Center point, radius doesn't exceed bin size.
  map.binsForPoint({0.0f, 0.0f}, 0.05f, &bins);
  EXPECT_EQ(1, bins.size());
  EXPECT_EQ(12, bins[0]);

  // Off center point, radius overlaps two bins.
  map.binsForPoint({0.0f, 0.18f}, 0.05f, &bins);
  EXPECT_EQ(2, bins.size());
  EXPECT_EQ(12, bins[0]);
  EXPECT_EQ(17, bins[1]);

  // Center point, radius exceeds bin size.
  map.binsForPoint({0.0f, 0.0f}, 0.25f, &bins);
  EXPECT_EQ(9, bins.size());
  EXPECT_EQ(6, bins[0]);
  EXPECT_EQ(7, bins[1]);
  EXPECT_EQ(8, bins[2]);
  EXPECT_EQ(11, bins[3]);
  EXPECT_EQ(12, bins[4]);
  EXPECT_EQ(13, bins[5]);
  EXPECT_EQ(16, bins[6]);
  EXPECT_EQ(17, bins[7]);
  EXPECT_EQ(18, bins[8]);
}

TEST_F(LinearBinTest, BinsOnLineHorizontal) {
  BinMap map(5, 5);
  map.reset<HPoint2>(-1.0f, -1.0f, 1.0f, 1.0f, {});
  Vector<size_t> bins;

  // Center horizontal line, radius doesn't exceed bin size.
  map.binsOnLine({{-1.0f, 0.0f}, {1.0f, 0.0f}}, 0.05f, &bins);
  EXPECT_EQ(5, bins.size());
  EXPECT_EQ(10, bins[0]);
  EXPECT_EQ(11, bins[1]);
  EXPECT_EQ(12, bins[2]);
  EXPECT_EQ(13, bins[3]);
  EXPECT_EQ(14, bins[4]);

  // Off-center horizontal line, radius doesn't exceed bin size.
  map.binsOnLine({{-1.0f, 0.18f}, {1.0f, 0.18f}}, 0.05f, &bins);
  EXPECT_EQ(10, bins.size());
  EXPECT_EQ(10, bins[0]);
  EXPECT_EQ(15, bins[1]);
  EXPECT_EQ(11, bins[2]);
  EXPECT_EQ(16, bins[3]);
  EXPECT_EQ(12, bins[4]);
  EXPECT_EQ(17, bins[5]);
  EXPECT_EQ(13, bins[6]);
  EXPECT_EQ(18, bins[7]);
  EXPECT_EQ(14, bins[8]);
  EXPECT_EQ(19, bins[9]);
}

TEST_F(LinearBinTest, BinsOnLinePoint) {
  BinMap map(5, 5);
  map.reset<HPoint2>(-1.0f, -1.0f, 1.0f, 1.0f, {});
  Vector<size_t> bins;

  // Center point, radius exceeds bin size.
  map.binsOnLine({{0.0f, 0.0f}, {0.0f, 0.0f}}, 0.25f, &bins);
  EXPECT_EQ(9, bins.size());
  EXPECT_EQ(6, bins[0]);
  EXPECT_EQ(7, bins[1]);
  EXPECT_EQ(8, bins[2]);
  EXPECT_EQ(11, bins[3]);
  EXPECT_EQ(12, bins[4]);
  EXPECT_EQ(13, bins[5]);
  EXPECT_EQ(16, bins[6]);
  EXPECT_EQ(17, bins[7]);
  EXPECT_EQ(18, bins[8]);
}

TEST_F(LinearBinTest, BinsOnLineVertical) {
  BinMap map(5, 5);
  map.reset<HPoint2>(-1.0f, -1.0f, 1.0f, 1.0f, {});
  Vector<size_t> bins;

  // Center vertical line, radius doesn't exceed bin size.
  map.binsOnLine({{0.0f, -1.0f}, {0.0f, 1.0f}}, 0.05f, &bins);
  EXPECT_EQ(5, bins.size());
  EXPECT_EQ(2, bins[0]);
  EXPECT_EQ(7, bins[1]);
  EXPECT_EQ(12, bins[2]);
  EXPECT_EQ(17, bins[3]);
  EXPECT_EQ(22, bins[4]);

  // Center vertical line, radius exceeds bin size, line doesn't take the whole grid.
  map.binsOnLine({{0.0f, -0.25f}, {0.0f, 0.25f}}, 0.25f, &bins);
  EXPECT_EQ(9, bins.size());
  EXPECT_EQ(6, bins[0]);
  EXPECT_EQ(7, bins[1]);
  EXPECT_EQ(8, bins[2]);
  EXPECT_EQ(11, bins[3]);
  EXPECT_EQ(12, bins[4]);
  EXPECT_EQ(13, bins[5]);
  EXPECT_EQ(16, bins[6]);
  EXPECT_EQ(17, bins[7]);
  EXPECT_EQ(18, bins[8]);
}

TEST_F(LinearBinTest, BinsForPoint3d) {
  // Make a 3x3x3 grid of points with a 1.0 unit spacing (like a Rubik's cube)
  Vector<HPoint3> points;
  points.reserve(27);
  for (int x = 0; x < 3; ++x) {
    for (int y = 0; y < 3; ++y) {
      for (int z = 0; z < 3; ++z) {
        points.push_back({x + 0.5f, y + 0.5f, z + 0.5f});
      }
    }
  }
  // Make a 3x3x3 grid so each point is at the center of a bin
  BinMap3d map(3, 3, 3);
  map.reset<HPoint3>(0.0f, 0.0f, 0.0f, 3.0f, 3.0f, 3.0f, points);
  Vector<size_t> bins;

  // Center point, radius doesn't exceed bin size.
  map.binsForPoint<HPoint3>({1.5f, 1.5f, 1.5f}, 0.4f, &bins);
  EXPECT_EQ(1, bins.size());
  EXPECT_EQ(13, bins[0]);

  // Center, radius overlaps all 27 bins
  map.binsForPoint<HPoint3>({1.5f, 1.5f, 1.5f}, 1.5f, &bins);
  EXPECT_EQ(27, bins.size());

  // Corner point, radius overlaps 8 bins
  map.binsForPoint<HPoint3>({0.5f, 0.5f, 0.5f}, 0.6f, &bins);
  EXPECT_EQ(8, bins.size());

  // Edge point, radius overlaps 18 bins
  map.binsForPoint<HPoint3>({1.5f, 1.5f, 0.5f}, 0.6f, &bins);
  EXPECT_EQ(18, bins.size());
}

}  // namespace c8
