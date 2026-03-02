// Copyright (c) 2024 Niantic, Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":feature-manager",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x92c69dc8);

#include <iostream>

#include "gtest/gtest.h"
#include "reality/engine/features/feature-manager.h"

namespace c8 {

OrbFeature descriptor1(std::array<uint8_t, 32>{
  0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111,
  0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111,
  0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111});

OrbFeature descriptor2(std::array<uint8_t, 32>{
  0b000, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111,
  0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111,
  0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111});

GorbFeature descriptor4(std::array<uint8_t, 32>{
  0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111,
  0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111,
  0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111});
GorbFeature descriptor5(std::array<uint8_t, 32>{
  0b000, 0b000, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111,
  0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111,
  0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111});

class FeatureManagerTest : public ::testing::Test {};

TEST_F(FeatureManagerTest, TestAddKeypoints) {
  FeatureStore store;
  EXPECT_EQ(store.numKeypoints(), 0);
  store.addKeypoints(3);
  EXPECT_EQ(store.numKeypoints(), 3);
  store.addKeypoints(2);
  EXPECT_EQ(store.numKeypoints(), 5);
}

TEST_F(FeatureManagerTest, TestAbsenceByFeature) {
  FeatureStore store;
  store.addKeypoints(3);

  // Verify absence of features for keypoint 0
  EXPECT_FALSE(store.has<OrbFeature>(0));
  EXPECT_FALSE(store.has<GorbFeature>(0));
  EXPECT_FALSE(store.has<LearnedFeature>(0));
}

TEST_F(FeatureManagerTest, TestAddAndPresence) {
  FeatureStore store;
  store.addKeypoints(3);

  // Add Orb descriptor to keypoint 0
  store.add<OrbFeature>(descriptor1.clone(), 0);

  // Verify presence of Orb feature for keypoint 0
  EXPECT_TRUE(store.has<OrbFeature>(0));
  EXPECT_FALSE(store.has<GorbFeature>(0));
  EXPECT_FALSE(store.has<LearnedFeature>(0));
}

TEST_F(FeatureManagerTest, TestRepeatedAddRuntimeException) {
  FeatureStore store;
  store.addKeypoints(3);

  // Add Orb descriptor to keypoint 0
  store.add<OrbFeature>(descriptor1.clone(), 0);

  // Adding a feature to a keypoint which already has a feature should throw
  EXPECT_THROW(store.add<OrbFeature>({}, 0), std::runtime_error);
}

TEST_F(FeatureManagerTest, TestDirectAccessAndHamming) {
  FeatureStore store;
  store.addKeypoints(3);

  // Add Orb descriptors to keypoints 0 and 2
  store.add<OrbFeature>(descriptor1.clone(), 0);
  store.add<OrbFeature>(descriptor2.clone(), 2);

  // Direct access for keypoint 0 feature by type
  const OrbFeature &descriptor3 = store.get<OrbFeature>(0);
  EXPECT_EQ(descriptor3.hammingDistance(descriptor1), 0);
}

TEST_F(FeatureManagerTest, TestHammingDistanceInStore) {
  FeatureStore store;
  store.addKeypoints(3);

  // Add Orb descriptors to keypoints 0 and 2
  store.add<OrbFeature>(descriptor1.clone(), 0);
  store.add<OrbFeature>(descriptor2.clone(), 2);

  // Adding Gorb descriptors to keypoints 0, 1 and 2
  store.add<GorbFeature>(descriptor4.clone(), 0);
  store.add<GorbFeature>(descriptor4.clone(), 1);
  store.add<GorbFeature>(descriptor5.clone(), 2);
}

TEST_F(FeatureManagerTest, TestIterators) {
  FeatureStore store;
  store.addKeypoints(3);

  // Add Orb descriptors to keypoints 0 and 2
  store.add<OrbFeature>(descriptor1.clone(), 0);
  store.add<OrbFeature>(descriptor2.clone(), 2);

  // Add Gorb descriptors to keypoints 0, 1 and 2
  store.add<GorbFeature>(descriptor4.clone(), 0);
  store.add<GorbFeature>(descriptor4.clone(), 1);
  store.add<GorbFeature>(descriptor5.clone(), 2);
}

}  // namespace c8
