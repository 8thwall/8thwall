// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":global-matcher",
    "//reality/engine/features:point-descriptor",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x3aa0ec6a);

#include <array>

#include "c8/parameter-data.h"
#include "gtest/gtest.h"
#include "reality/engine/features/global-matcher.h"
#include "reality/engine/features/point-descriptor.h"

namespace c8 {

std::array<uint8_t, 32> descriptor0{{0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                                     11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                                     22, 23, 24, 25, 26, 27, 28, 29, 30, 31}};

std::array<uint8_t, 32> descriptor1{{255, 2,  1,  3,  4,  5,  6,  7,  8,  9,  10,
                                     11,  12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                                     22,  23, 24, 25, 26, 27, 28, 29, 30, 31}};

std::array<uint8_t, 32> descriptor2{
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

std::array<uint8_t, 32> descriptor3{{255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                     255, 255, 255, 255, 255, 255, 255, 255, 255, 255}};

std::array<uint8_t, 32> descriptor4{{255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                     255, 255, 255, 255, 255, 255, 255, 255, 255, 0}};
std::array<uint8_t, 32> descriptor5{{255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                     255, 255, 255, 255, 255, 255, 255, 255, 254, 0}};

HPoint2 lastPoint1{10.0f, 9.0f};
HPoint2 lastPoint2{15.0f, 9.0f};
HPoint2 lastPoint3{15.0f, 10.0f};

class GlobalMatcherTest : public ::testing::Test {};

TEST_F(GlobalMatcherTest, TestFindBestMatch) {
  GlobalMatcher<OrbFeature> matcher;
  FrameWithPoints f(c8_PixelPinholeCameraModel{50, 50, 0.0f, 0.0f, 1.0f, -1.0f});
  f.addImagePixelPoint(lastPoint1, 0, 0, 0, {OrbFeature(descriptor1)});
  f.addImagePixelPoint(lastPoint2, 0, 0, 0, {OrbFeature(descriptor2)});
  f.addImagePixelPoint(lastPoint3, 0, 0, 0, {OrbFeature(descriptor3)});

  FrameWithPoints f2(c8_PixelPinholeCameraModel{50, 50, 0.0f, 0.0f, 1.0f, -1.0f});
  f2.addImagePixelPoint(lastPoint1, 0, 0, 0, {OrbFeature(descriptor3)});
  f2.addImagePixelPoint(lastPoint2, 0, 0, 0, {OrbFeature(descriptor1)});
  f2.addImagePixelPoint(lastPoint3, 0, 0, 0, {OrbFeature(descriptor0)});

  Vector<PointMatch> m;

  matcher.match(f, f2, &m);

  EXPECT_EQ(2, m.size());
  EXPECT_EQ(0, m[0].wordsIdx);
  EXPECT_EQ(1, m[0].dictionaryIdx);
  EXPECT_EQ(2, m[1].wordsIdx);
  EXPECT_EQ(0, m[1].dictionaryIdx);
}

TEST_F(GlobalMatcherTest, TestEmptyMatch) {
  GlobalMatcher<OrbFeature> matcher;
  FrameWithPoints f1(c8_PixelPinholeCameraModel{});
  FrameWithPoints f2(c8_PixelPinholeCameraModel{50, 50, 0.0f, 0.0f, 1.0f, -1.0f});
  f2.addImagePixelPoint(lastPoint1, 0, 0, 0, {OrbFeature(descriptor1)});
  f2.addImagePixelPoint(lastPoint2, 0, 0, 0, {OrbFeature(descriptor2)});
  f2.addImagePixelPoint(lastPoint3, 0, 0, 0, {OrbFeature(descriptor3)});

  Vector<PointMatch> m;
  matcher.match(f1, f2, &m);

  EXPECT_EQ(0, m.size());
}

TEST_F(GlobalMatcherTest, TestDirectMatchMap) {
  GlobalMatcher<OrbFeature> matcher;
  FeatureStore mapPointDescriptors;
  mapPointDescriptors.append<OrbFeature>(descriptor1);
  mapPointDescriptors.append<OrbFeature>(descriptor2);
  RandomNumbers random;
  Vector<PointMatch> m;

  {
    ScopedParameterUpdates update1 = {
      {"GlobalMatcher.maxNumPointsToConsiderMapMatch", std::numeric_limits<int>::max()}};

    FrameWithPoints f1(c8_PixelPinholeCameraModel{});
    f1.addImagePixelPoint(lastPoint1, 0, 0, 0, {OrbFeature(descriptor1)});

    matcher.matchMapBruteForce(f1, mapPointDescriptors, 1, &random, &m);
    EXPECT_EQ(1, m.size());
    GlobalMatcher<OrbFeature>::matchMapBruteForce(f1, mapPointDescriptors, 1, &random, &m);
    EXPECT_EQ(1, m.size());

    f1.addImagePixelPoint(lastPoint2, 0, 0, 0, {OrbFeature(descriptor2)});
    matcher.matchMapBruteForce(f1, mapPointDescriptors, 1, &random, &m);
    EXPECT_EQ(2, m.size());

    FrameWithPoints f2(c8_PixelPinholeCameraModel{});
    std::array<uint8_t, 32> tempDesc{{32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
                                      32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
                                      32, 32, 32, 32, 32, 32, 32, 32, 32, 32}};
    f2.addImagePixelPoint(lastPoint3, 0, 0, 0, {OrbFeature(tempDesc)});
    matcher.matchMapBruteForce(f2, mapPointDescriptors, 1, &random, &m);
    EXPECT_EQ(0, m.size());
  }

  {
    ScopedParameterUpdates update2 = {{"GlobalMatcher.maxNumPointsToConsiderMapMatch", 1}};

    FrameWithPoints f(c8_PixelPinholeCameraModel{});
    f.addImagePixelPoint(lastPoint1, 0, 0, 0, {OrbFeature(descriptor1)});
    f.addImagePixelPoint(lastPoint2, 0, 0, 0, {OrbFeature(descriptor2)});
    f.addImagePixelPoint(lastPoint3, 0, 0, 0, {OrbFeature(descriptor3)});
    matcher.matchMapBruteForce(f, mapPointDescriptors, 1, &random, &m);

    // Regardless of which map point is sampled, we should only get 1 match
    EXPECT_EQ(1, m.size());
  }
}

TEST_F(GlobalMatcherTest, TestTopKMatchMap) {
  GlobalMatcher<OrbFeature> matcher;

  FeatureStore mapPointDescriptors;
  mapPointDescriptors.append<OrbFeature>(descriptor1);
  mapPointDescriptors.append<OrbFeature>(descriptor2);
  mapPointDescriptors.append<OrbFeature>(descriptor4);
  mapPointDescriptors.append<OrbFeature>(descriptor5);

  FrameWithPoints f1(c8_PixelPinholeCameraModel{});
  f1.addImagePixelPoint(lastPoint1, 0, 0, 0, {OrbFeature(descriptor1)});

  Vector<Vector<PointMatch>> m;
  matcher.matchMapBruteForceTopK(f1, mapPointDescriptors, 2, 32, -1.f, &m);
  EXPECT_EQ(1, m.size());
  EXPECT_EQ(1, m[0].size());
  EXPECT_EQ(0, m[0][0].dictionaryIdx);

  f1.addImagePixelPoint(lastPoint3, 0, 0, 0, {OrbFeature(descriptor3)});
  // No relative threshold, so we should get all matches
  matcher.matchMapBruteForceTopK(f1, mapPointDescriptors, 2, 32, -1.f, &m);
  EXPECT_EQ(2, m.size());
  EXPECT_EQ(1, m[0].size());
  EXPECT_EQ(2, m[1].size());

  // Relative threshold, so we should get only 1 match for the 2nd word
  matcher.matchMapBruteForceTopK(f1, mapPointDescriptors, 2, 32, 1.01f, &m);
  EXPECT_EQ(2, m.size());
  EXPECT_EQ(1, m[0].size());
  EXPECT_EQ(1, m[1].size());  // 2nd best match discounted due to relative threshold

  // Relative threshold, but it's high so we should get all matches
  matcher.matchMapBruteForceTopK(f1, mapPointDescriptors, 2, 32, 1.5f, &m);
  EXPECT_EQ(2, m.size());
  EXPECT_EQ(1, m[0].size());
  EXPECT_EQ(2, m[1].size());  // 2nd best match is close enough to the best match
}

TEST_F(GlobalMatcherTest, TestMatchPopCount) {
  FeatureStore mapPointDescriptors;
  mapPointDescriptors.append<OrbFeature>(descriptor0);

  // descriptor0 and descriptor1 have a hamming distance of 12 and a pop count difference of 8
  EXPECT_EQ(12, OrbFeature(descriptor0).hammingDistance(OrbFeature(descriptor1)));
  int d0PopCount = OrbFeature(descriptor0).totalPopCount();
  int d1PopCount = OrbFeature(descriptor1).totalPopCount();
  EXPECT_EQ(8, std::abs(d0PopCount - d1PopCount));

  GlobalMatcher<OrbFeature> matcher;
  matcher.preparePopCountLut(mapPointDescriptors);

  FrameWithPoints f1(c8_PixelPinholeCameraModel{});
  f1.addImagePixelPoint(lastPoint1, 0, 0, 0, {OrbFeature(descriptor1)});

  // Should behave the same as brute force matching
  Vector<PointMatch> m;
  matcher.matchMapPopCount(f1, &m, 32, 32);
  EXPECT_EQ(1, m.size());

  // Descriptors have a pop count difference of 8, so this should match
  m.clear();
  matcher.matchMapPopCount(f1, &m, 32, 8);
  EXPECT_EQ(1, m.size());

  // Descriptors have a pop count difference of 8, so this should not match
  m.clear();
  matcher.matchMapPopCount(f1, &m, 32, 7);
  EXPECT_EQ(0, m.size());
}

}  // namespace c8
