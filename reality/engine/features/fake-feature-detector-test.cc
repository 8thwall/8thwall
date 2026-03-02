// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":fake-feature-detector",
    ":local-matcher",
    "//c8/geometry:worlds",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xb7f837d2);

#include "reality/engine/features/fake-feature-detector.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "c8/geometry/worlds.h"
#include "c8/vector.h"
#include "reality/engine/features/local-matcher.h"

namespace c8 {

class FakeFeatureDetectorTest : public ::testing::Test {};

TEST_F(FakeFeatureDetectorTest, TestFakeFeaturesWorld) {
  ScopeTimer rt("test");

  Vector<HPoint3> worldPts = Worlds::axisAlignedPolygonsWorld();
  auto cam1 = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto cam2 =
    HMatrixGen::translation(5.02, 5.01, 18.99) * HMatrixGen::rotationD(11.9, 190.1, -15.2);

  c8_PixelPinholeCameraModel m{480, 640, 240.0f, 320.0f, 538.0105f, 538.0105f};

  FakeFeatureDetector d(FakeFeatureDetectorNoise::none());
  FrameWithPoints pts1(m);
  d.detectFeatures(cam1, worldPts, &pts1);

  FrameWithPoints pts2(m);
  d.detectFeatures(cam2, worldPts, &pts2);

  const auto &f1 = pts1.store().getFeatures<OrbFeature>();
  const auto &f2 = pts2.store().getFeatures<OrbFeature>();
  for (int i = 0; i < f1.size(); ++i) {
    EXPECT_EQ(0.0f, f1[i].hammingDistance(f2[i]));
    for (int j = i + 1; j < f1.size(); ++j) {
      EXPECT_GT(f1[i].hammingDistance(f1[j]), 0.0f);
    }
  }

  LocalMatcher<OrbFeature> localMatcher(30, 20, .1f);
  Vector<PointMatch> matches;

  localMatcher.match(pts1, pts2, &matches);
  EXPECT_EQ(15, matches.size());
  for (int i = 0; i < matches.size(); ++i) {
    EXPECT_EQ(i, matches[i].wordsIdx);
    EXPECT_EQ(i, matches[i].dictionaryIdx);
    EXPECT_EQ(0.0f, matches[i].descriptorDist);
  }
}

TEST_F(FakeFeatureDetectorTest, TestFakeFeaturesWorldWithDifferentViews) {
  ScopeTimer rt("test");

  Vector<HPoint3> worldPts = Worlds::axisAlignedPolygonsWorld();
  auto cam1 = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto cam2 = HMatrixGen::translation(5.1, 5.6, 18.5) * HMatrixGen::rotationD(11.0, 192.1, -16);

  c8_PixelPinholeCameraModel m{480, 640, 240.0f, 320.0f, 538.0105f, 538.0105f};

  FakeFeatureDetector d(FakeFeatureDetectorNoise::realistic());
  FrameWithPoints pts1(m);
  d.detectFeatures(cam1, worldPts, &pts1);

  FrameWithPoints pts2(m);
  d.detectFeatures(cam2, worldPts, &pts2);

  LocalMatcher<OrbFeature> localMatcher(30, 20, .1f);
  Vector<PointMatch> matches;

  localMatcher.match(pts1, pts2, &matches);

  // Expect between 10-13 matches out of 15 possible.
  EXPECT_GT(13, matches.size());
  EXPECT_LT(10, matches.size());
}

TEST_F(FakeFeatureDetectorTest, TestFakeFeaturesPix) {
  ScopeTimer rt("test");

  Vector<HPoint3> worldPts = Worlds::axisAlignedPolygonsWorld();
  auto cam1 = HMatrixGen::translation(5.0, 5.0, 19.0) * HMatrixGen::rotationD(12.0, 190.0, -15.0);
  auto cam2 =
    HMatrixGen::translation(5.02, 5.01, 18.99) * HMatrixGen::rotationD(13.0, 191.0, -16.0);

  c8_PixelPinholeCameraModel m{480, 640, 240.0f, 320.0f, 538.0105f, 538.0105f};

  HMatrix K = HMatrixGen::intrinsic(m);

  auto pix1 = flatten<2>(K * cam1.inv() * worldPts);
  auto pix2 = flatten<2>(K * cam2.inv() * worldPts);

  Vector<uint8_t> foundMask(pix1.size(), 1);

  FakeFeatureDetector d(FakeFeatureDetectorNoise::realistic());
  FrameWithPoints pts1(m);
  d.detectFeatures(pix1, foundMask, &pts1);

  FrameWithPoints pts2(m);
  d.detectFeatures(pix2, foundMask, &pts2);

  const auto &f1 = pts1.store().getFeatures<OrbFeature>();
  const auto &f2 = pts2.store().getFeatures<OrbFeature>();
  for (int i = 0; i < f1.size(); ++i) {
    EXPECT_EQ(0.0f, f1[i].hammingDistance(f2[i]));
    for (int j = i + 1; j < f1.size(); ++j) {
      EXPECT_GT(f1[i].hammingDistance(f1[j]), 0.0f);
    }
  }

  LocalMatcher<OrbFeature> localMatcher(30, 20, .1f);
  Vector<PointMatch> matches;

  localMatcher.match(pts1, pts2, &matches);
  EXPECT_EQ(15, matches.size());
  for (int i = 0; i < matches.size(); ++i) {
    EXPECT_EQ(i, matches[i].wordsIdx);
    EXPECT_EQ(i, matches[i].dictionaryIdx);
    EXPECT_EQ(0.0f, matches[i].descriptorDist);
  }
}

}  // namespace c8
