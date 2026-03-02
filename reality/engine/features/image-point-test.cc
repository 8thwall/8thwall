// Copyright (c) 2025 Niantic, Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "//reality/engine/features:image-point",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xe385770d);

#include "gtest/gtest.h"
#include "reality/engine/features/image-point.h"

namespace c8 {

class ImagePointTest : public ::testing::Test {};

TEST_F(ImagePointTest, TestCompareImagePointXY) {
  ImagePointXY a(1.0f, 2.0f);
  ImagePointXY b(1.0f, 2.0f);
  ImagePointXY c(2.0f, 1.0f);

  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST_F(ImagePointTest, TestCompareImagePointLocation) {
  ImagePointLocation a(1.0f, 2.0f, 3.0f, 4, 5.0f, 6.0f, 7.0f, 8);
  ImagePointLocation b(1.0f, 2.0f, 3.0f, 4, 5.0f, 6.0f, 7.0f, 8);
  ImagePointLocation c(2.0f, 1.0f, 3.0f, 4, 5.0f, 6.0f, 7.0f, 8);

  EXPECT_EQ(a, b);
  EXPECT_NE(a, c);
}

TEST_F(ImagePointTest, TestImagePoint) {
  ImagePoint point;
  ImagePointLocation location(1.0f, 2.0f, 3.0f, 4, 5.0f, 6.0f, 7.0f, 8);
  point.setLocation(location);
  EXPECT_EQ(point.location(), location);

  FeatureSet features;
  features.orbFeature =
    OrbFeature({0b000, 0b000, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111,
                0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111,
                0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111});
  point.mutableFeatures() = features.clone();
  EXPECT_TRUE(point.features().orbFeature.has_value());
  EXPECT_EQ(features.orbFeature->hammingDistance(point.features().orbFeature.value()), 0);
  EXPECT_FALSE(point.features().gorbFeature.has_value());
  EXPECT_FALSE(point.features().learnedFeature.has_value());
}

TEST_F(ImagePointTest, TestImagePoints) {
  // Reserve space for 2 points, but add 3.
  ImagePoints points(2);

  ImagePointLocation pts[3] = {
    ImagePointLocation(1.0f, 1.0f, 1.0f, 4, 5.0f, 6.0f, 7.0f, 8),
    ImagePointLocation(2.0f, 2.0f, 2.0f, 4, 5.0f, 6.0f, 7.0f, 8),
    ImagePointLocation(3.0f, 3.0f, 3.0f, 4, 5.0f, 6.0f, 7.0f, 8)};

  EXPECT_EQ(points.size(), 0);
  points.push_back(pts[0]);
  EXPECT_EQ(points.size(), 1);
  points.push_back(pts[1]);
  EXPECT_EQ(points.size(), 2);
  points.push_back(pts[2]);
  EXPECT_EQ(points.size(), 3);

  for (size_t i = 0; i < 3; i++) {
    EXPECT_EQ(pts[i], points.at(i).location());
  }
}

TEST_F(ImagePointTest, TestAngleWithUpToPortion) {
  EXPECT_EQ(angleWithUpToPortion(0.f), 0b00000001);
  EXPECT_EQ(angleWithUpToPortion(10.f), 0b00000001);
  EXPECT_EQ(angleWithUpToPortion(20.f), 0b00000001);
  // 22.5 is a threshold
  EXPECT_EQ(angleWithUpToPortion(30.f), 0b00000010);
  EXPECT_EQ(angleWithUpToPortion(40.f), 0b00000010);
  // 45.0 is a threshold
  EXPECT_EQ(angleWithUpToPortion(50.f), 0b00000100);
  EXPECT_EQ(angleWithUpToPortion(60.f), 0b00000100);
  // 67.5 is a threshold
  EXPECT_EQ(angleWithUpToPortion(70.f), 0b00001000);
  EXPECT_EQ(angleWithUpToPortion(80.f), 0b00001000);
  // 90.0 is a threshold
  EXPECT_EQ(angleWithUpToPortion(90.f), 0b00010000);
  EXPECT_EQ(angleWithUpToPortion(100.f), 0b00010000);
  EXPECT_EQ(angleWithUpToPortion(110.f), 0b00010000);
  // 112.5 is a threshold
  EXPECT_EQ(angleWithUpToPortion(120.f), 0b00100000);
  EXPECT_EQ(angleWithUpToPortion(130.f), 0b00100000);
  // 135.0 is a threshold
  EXPECT_EQ(angleWithUpToPortion(140.f), 0b01000000);
  EXPECT_EQ(angleWithUpToPortion(150.f), 0b01000000);
  // 157.5 is a threshold
  EXPECT_EQ(angleWithUpToPortion(160.f), 0b10000000);
  EXPECT_EQ(angleWithUpToPortion(170.f), 0b10000000);
  EXPECT_EQ(angleWithUpToPortion(180.f), 0b10000000);
}

}  // namespace c8
