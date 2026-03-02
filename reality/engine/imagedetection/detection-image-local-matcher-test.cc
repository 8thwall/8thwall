// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":detection-image-local-matcher",
    "//reality/engine/binning:linear-bin",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x82d276b7);

#include "gtest/gtest.h"
#include "reality/engine/binning/linear-bin.h"
#include "reality/engine/imagedetection/detection-image-local-matcher.h"

namespace c8 {

std::array<uint8_t, 32> descriptorKey{{0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                                       11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                                       22, 23, 24, 25, 26, 27, 28, 29, 30, 31}};

std::array<uint8_t, 32> descriptor1{{255, 1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                                     11,  12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                                     22,  23, 24, 25, 26, 27, 28, 29, 30, 31}};

std::array<uint8_t, 32> descriptor2{
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

std::array<uint8_t, 32> descriptor3{{255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                     255, 255, 255, 255, 255, 255, 255, 255, 255, 255}};

HPoint2 keyPt{12.0f, 12.0f};
HPoint2 lastPoint1{10.0f, 9.0f};
HPoint2 lastPoint2{15.0f, 9.0f};
HPoint2 lastPoint3{15.0f, 10.0f};

class DetectionImageLocalMatcherTest : public ::testing::Test {};

TEST_F(DetectionImageLocalMatcherTest, LinearBinNumberTest) {
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

TEST_F(DetectionImageLocalMatcherTest, BinNumberTest) {
  c8_PixelPinholeCameraModel k{320, 240, 0.0f, 0.0f, 1.0f, -1.0f};
  HPoint2 mn;
  HPoint2 mx;
  frameBounds(k, &mn, &mx);
  TargetBin bin(mn, mx, 32, 24);
  EXPECT_EQ(131, bin.binNum(HPoint2(30.0f, 40.0f)));
}

TEST_F(DetectionImageLocalMatcherTest, BinNumberTest2) {
  c8_PixelPinholeCameraModel k{480, 640, 240.0f, 320.0f, 625.0f, 625.0f};
  HPoint2 mn;
  HPoint2 mx;
  frameBounds(k, &mn, &mx);
  TargetBin bin(mn, mx, 20, 30);
  EXPECT_EQ(0, bin.binNum(HPoint2(-1.0f, -1.0f)));
  EXPECT_EQ(0, bin.binNum(HPoint2(-.384f, -0.512f)));
  EXPECT_EQ(19, bin.binNum(HPoint2(.384f, -0.512f)));
  EXPECT_EQ(580, bin.binNum(HPoint2(-.384f, 0.512f)));
  EXPECT_EQ(599, bin.binNum(HPoint2(.384f, 0.512f)));
  EXPECT_EQ(599, bin.binNum(HPoint2(1.0f, 1.0f)));
}

TEST_F(DetectionImageLocalMatcherTest, BinNumberZeroWidthTest) {
  c8_PixelPinholeCameraModel k;
  HPoint2 mn;
  HPoint2 mx;
  frameBounds(k, &mn, &mx);
  TargetBin bin(mn, mx, 32, 24);
  EXPECT_EQ(0, bin.binNum(HPoint2(30.0f, 40.0f)));
}

TEST_F(DetectionImageLocalMatcherTest, TestFindBestMatch) {
  DetectionImageLocalMatcher matcher(5, 5, 10.0f);
  TargetWithPoints f(c8_PixelPinholeCameraModel{50, 50, 0.0f, 0.0f, 1.0f, -1.0f});
  f.addImagePixelPoint(lastPoint1, 0, 0, 0, {OrbFeature(descriptor1)});
  f.addImagePixelPoint(lastPoint2, 0, 0, 0, {OrbFeature(descriptor2)});
  f.addImagePixelPoint(lastPoint3, 0, 0, 0, {OrbFeature(descriptor3)});

  PointMatch m;
  matcher.setQueryPointsPointer(f);
  bool isBestMatch =
    matcher.findBestMatch<OrbFeature>(FramePoint(keyPt, 0, 0, 0), {OrbFeature(descriptorKey)}, &m);
  EXPECT_EQ(0, m.dictionaryIdx);
  EXPECT_EQ(8.0f, m.descriptorDist);
  EXPECT_TRUE(isBestMatch);

  // If there are two feature points having the same hamming distance from the keypoint, then it
  // fails the ratio test and the point should not be considered.
  // Add one more point having same descriptor as another point.
  f.addImagePixelPoint(HPoint2(8.0f, 8.0f), 0, 0, 0, {OrbFeature(descriptor1)});
  matcher.setQueryPointsPointer(f);

  isBestMatch =
    matcher.findBestMatch<OrbFeature>(FramePoint(keyPt, 0, 0, 0), {OrbFeature(descriptorKey)}, &m);
  EXPECT_FALSE(isBestMatch);

  // If a feature point has the same descriptor as keypoint.
  f.addImagePixelPoint(HPoint2(8.0f, 8.0f), 0, 0, 0, {OrbFeature(descriptorKey)});
  matcher.setQueryPointsPointer(f);
  isBestMatch =
    matcher.findBestMatch<OrbFeature>(FramePoint(keyPt, 0, 0, 0), {OrbFeature(descriptorKey)}, &m);

  EXPECT_TRUE(isBestMatch);
  EXPECT_EQ(4, m.dictionaryIdx);
  EXPECT_EQ(0.0f, m.descriptorDist);

  // If 2 feature points has the same descriptor as keypoint, the test should fail
  f.addImagePixelPoint(lastPoint1, 0, 0, 0, {OrbFeature(descriptorKey)});
  matcher.setQueryPointsPointer(f);

  isBestMatch =
    matcher.findBestMatch<OrbFeature>(FramePoint(keyPt, 0, 0, 0), {OrbFeature(descriptorKey)}, &m);
  EXPECT_FALSE(isBestMatch);
}

}  // namespace c8
