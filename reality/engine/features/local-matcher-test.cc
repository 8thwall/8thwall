// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":local-matcher",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x08341869);

#include "gtest/gtest.h"
#include "reality/engine/features/local-matcher.h"

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

class LocalMatcherTest : public ::testing::Test {

protected:
  void SetUp() override {
    dictionaryFrame_.addImagePixelPoint(lastPoint1, 0, 0, 0, {OrbFeature(descriptor1)});
    dictionaryFrame_.addImagePixelPoint(lastPoint2, 0, 0, 0, {OrbFeature(descriptor2)});
    dictionaryFrame_.addImagePixelPoint(lastPoint3, 0, 0, 0, {OrbFeature(descriptor3)});

    matcher_.setQueryPointsPointer(dictionaryFrame_);
  }

  void TearDown() override { dictionaryFrame_.clear(); }

  LocalMatcher<OrbFeature> matcher_ = {5, 5, 10.0f};
  FrameWithPoints dictionaryFrame_ = {c8_PixelPinholeCameraModel{50, 50, 0.0f, 0.0f, 1.0f, -1.0f}};
  MatchResultFrequency matchFrequencies_;
  PointMatch pointMatch_;
};

TEST_F(LocalMatcherTest, TestFindBestMatch) {
  auto isBestMatch = matcher_.findBestMatch(
    FramePoint(keyPt, 0, 0, 0), OrbFeature(descriptorKey), &pointMatch_, &matchFrequencies_);
  EXPECT_EQ(0, pointMatch_.dictionaryIdx);
  EXPECT_EQ(8.0f, pointMatch_.descriptorDist);
  EXPECT_TRUE(isBestMatch);
  MatchResultFrequency expected;
  expected.match = 1;
  EXPECT_EQ(matchFrequencies_, expected);
}

TEST_F(LocalMatcherTest, TestFailedRatioTest) {
  // If there are two feature points having the same hamming distance from the keypoint, then it
  // fails the ratio test and the point should not be considered. Add one more point having same
  // descriptor as another point.
  dictionaryFrame_.addImagePixelPoint(lastPoint1, 0, 0, 0, {OrbFeature(descriptor1)});
  matcher_.setQueryPointsPointer(dictionaryFrame_);
  bool isBestMatch = matcher_.findBestMatch(
    FramePoint(keyPt, 0, 0, 0), OrbFeature(descriptorKey), &pointMatch_, &matchFrequencies_);
  EXPECT_FALSE(isBestMatch);
  MatchResultFrequency expected;
  expected.failedRatioTest = 1;
  EXPECT_EQ(matchFrequencies_, expected);
}

TEST_F(LocalMatcherTest, TestSameDescriptor) {
  // If a feature point has the same descriptor as keypoint.
  dictionaryFrame_.addImagePixelPoint(HPoint2(8.0f, 8.0f), 0, 0, 0, {OrbFeature(descriptorKey)});
  matcher_.setQueryPointsPointer(dictionaryFrame_);
  bool isBestMatch = matcher_.findBestMatch(
    FramePoint(keyPt, 0, 0, 0), OrbFeature(descriptorKey), &pointMatch_, &matchFrequencies_);
  EXPECT_TRUE(isBestMatch);
  EXPECT_EQ(3, pointMatch_.dictionaryIdx);
  EXPECT_EQ(0.0f, pointMatch_.descriptorDist);
  MatchResultFrequency expected;
  expected.match = 1;
  EXPECT_EQ(matchFrequencies_, expected);

  // If 2 feature points has the same descriptor as keypoint, the test should fail
  dictionaryFrame_.addImagePixelPoint(lastPoint1, 0, 0, 0, {OrbFeature(descriptorKey)});
  matcher_.setQueryPointsPointer(dictionaryFrame_);
  matchFrequencies_ = {};
  isBestMatch = matcher_.findBestMatch(
    FramePoint(keyPt, 0, 0, 0), OrbFeature(descriptorKey), &pointMatch_, &matchFrequencies_);
  EXPECT_FALSE(isBestMatch);
  expected = {};
  expected.failedRatioTest = 1;
  EXPECT_EQ(matchFrequencies_, expected);
}

TEST_F(LocalMatcherTest, TestBadScale) {
  // If the scale is off, the test should fail
  bool isBestMatch = matcher_.findBestMatch(
    FramePoint(lastPoint3, 4, 0, 0), OrbFeature(descriptor3), &pointMatch_, &matchFrequencies_);
  EXPECT_FALSE(isBestMatch);
  MatchResultFrequency expected;
  expected.failedScaleTest = 1;
  EXPECT_EQ(matchFrequencies_, expected);
}

TEST_F(LocalMatcherTest, TestBadScaleWithScaleFilter) {
  // If we set the scale filter, then it should fail in the pair wise test.  Since all the
  // dictionary items are at scale 0, it will return bad scale will all of the words in its bin.
  matcher_.useScaleFilter(true);
  bool isBestMatch = matcher_.findBestMatch(
    FramePoint(lastPoint3, 4, 0, 0), OrbFeature(descriptor3), &pointMatch_, &matchFrequencies_);
  EXPECT_FALSE(isBestMatch);
  MatchResultFrequency expected;
  expected.unmatched = 1;
  expected.unmatchedPairWiseBadScale = 3;
  EXPECT_EQ(matchFrequencies_, expected);
}

TEST_F(LocalMatcherTest, TestPairWiseBadDescriptor) {
  std::array<uint8_t, 32> badDesc{{42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
                                   42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42}};
  matcher_.setDescriptorThreshold(64.0f);
  bool isBestMatch = matcher_.findBestMatch(
    FramePoint(lastPoint3, 0, 0, 0), OrbFeature(badDesc), &pointMatch_, &matchFrequencies_);
  EXPECT_FALSE(isBestMatch);
  MatchResultFrequency expected;
  expected.unmatched = 1;
  expected.unmatchedPairWiseBadDist = 3;
  EXPECT_EQ(matchFrequencies_, expected);
}

TEST_F(LocalMatcherTest, TestPairWiseBadRadius) {
  matcher_.useRatioTest(true);
  matcher_.setSearchRadius(0.0001);

  bool isBestMatch = matcher_.findBestMatch(
    FramePoint({lastPoint1.x() + 0.2f, lastPoint1.y() + 0.2f}, 0, 0, 0),
    OrbFeature(descriptor1),
    &pointMatch_,
    &matchFrequencies_);
  EXPECT_FALSE(isBestMatch);
  MatchResultFrequency expected;
  expected.unmatched = 1;
  expected.unmatchedPairWiseBadRadius = 2;
  EXPECT_EQ(matchFrequencies_, expected);
}

TEST_F(LocalMatcherTest, TestFoundBetterMatch) {
  matcher_.useRatioTest(true);
  matcher_.setSearchRadius(0.0001);

  std::array<uint8_t, 32> almostDescriptor1;
  std::copy(std::begin(descriptor1), std::end(descriptor1), std::begin(almostDescriptor1));
  almostDescriptor1[2] = 5;

  // Both seem good but the second one is even better of a match.
  Vector<FramePoint> points = {
    FramePoint(lastPoint1, 0, 0, 0),
    FramePoint(lastPoint1, 0, 0, 0),
  };

  FeatureStore store;
  store.append<OrbFeature>(OrbFeature(almostDescriptor1));
  store.append<OrbFeature>(OrbFeature(descriptor1));

  Vector<PointMatch> pointMatches;
  matcher_.findMatches(points, store, &pointMatches, &matchFrequencies_);
  MatchResultFrequency expected;
  expected.match = 1;
  expected.foundBetterMatch = 1;
  EXPECT_EQ(matchFrequencies_, expected);
}

TEST_F(LocalMatcherTest, TestSuppressMatch) {
  matcher_.useRatioTest(true);
  matcher_.setSearchRadius(0.0001);

  std::array<uint8_t, 32> almostDescriptor1;
  std::copy(std::begin(descriptor1), std::end(descriptor1), std::begin(almostDescriptor1));
  almostDescriptor1[2] = 5;

  // Both are the same so the second will get suppressed.
  Vector<FramePoint> points = {
    FramePoint(lastPoint1, 0, 0, 0),
    FramePoint(lastPoint1, 0, 0, 0),
  };

  FeatureStore store;
  store.append<OrbFeature>(OrbFeature(descriptor1));
  store.append<OrbFeature>(OrbFeature(descriptor1));

  Vector<PointMatch> pointMatches;
  matcher_.findMatches(points, store, &pointMatches, &matchFrequencies_);
  MatchResultFrequency expected;
  expected.match = 1;
  expected.suppressed = 1;
  EXPECT_EQ(matchFrequencies_, expected);
}

TEST_F(LocalMatcherTest, TestEmptyMatch) {
  LocalMatcher<OrbFeature> matcher(5, 5, 10.0f);
  ScopeTimer rt("test");
  FrameWithPoints f1(c8_PixelPinholeCameraModel{});
  FrameWithPoints f2(c8_PixelPinholeCameraModel{50, 50, 0.0f, 0.0f, 1.0f, -1.0f});
  f2.addImagePixelPoint(lastPoint1, 0, 0, 0, {OrbFeature(descriptor1)});
  f2.addImagePixelPoint(lastPoint2, 0, 0, 0, {OrbFeature(descriptor2)});
  f2.addImagePixelPoint(lastPoint3, 0, 0, 0, {OrbFeature(descriptor3)});

  Vector<PointMatch> m;
  matcher_.match(f1, f2, &m);

  EXPECT_EQ(0, m.size());
}

}  // namespace c8
