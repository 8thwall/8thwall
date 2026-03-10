// Copyright (c) 2024 Niantic, Inc.
// Original Author: Haomin Zhu (hzhu@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "//c8:vector",
    "//c8:c8-log",
    "//reality/engine/features:descriptor-bit-counter",
    "//reality/engine/features:image-descriptor",
    "//reality/engine/features:kmeans",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xf57b8dc7);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <numeric>

#include "c8/c8-log.h"
#include "c8/vector.h"
#include "reality/engine/features/descriptor-bit-counter.h"
#include "reality/engine/features/image-descriptor.h"
#include "reality/engine/features/kmeans.h"

namespace c8 {

class KMeansTest : public ::testing::Test {
public:
  KMeansTest() {
    descriptors_.reserve(20);
    generateRandomDescriptors(descriptors_, 20);
  }

  const Vector<ImageDescriptor32> &descriptors() const { return descriptors_; }

  void generateRandomDescriptors(Vector<ImageDescriptor32> &descriptors, int numDescriptors) {
    descriptors.clear();
    descriptors.reserve(numDescriptors);
    for (int i = 0; i < numDescriptors; ++i) {
      auto &descriptor = descriptors.emplace_back();
      uint8_t *data = descriptor.mutableData();
      for (int j = 0; j < ImageDescriptor32::size(); ++j) {
        data[j] = rand() % 255;
      }
    }
  }

  // Check descriptor is closest to the cluster center it is assigned to
  bool checkClusterResults(
    const Vector<ImageDescriptor32> &descriptors,
    const Vector<ImageDescriptor32> &clusterCenters,
    const Vector<int> &assignments) {
    for (size_t i = 0; i < descriptors.size(); ++i) {
      int minDistance = descriptors[i].hammingDistance(clusterCenters[assignments[i]]);
      for (size_t j = 0; j < clusterCenters.size(); ++j) {
        int distance = descriptors[i].hammingDistance(clusterCenters[j]);
        if (distance < minDistance) {
          return false;
        }
      }
    }
    return true;
  }

private:
  Vector<ImageDescriptor32> descriptors_;
};

TEST_F(KMeansTest, numClustersMoreThanNumDescriptors) {
  // Test number of clusters is larger than the number of descriptors
  // If target cluster numbers is more than the number of descriptors, the cluster centers should be
  // the same as the descriptors
  Vector<ImageDescriptor32> clusterCenters;
  Vector<int> assignments = {};
  EXPECT_TRUE(
    clusterDescriptors<ImageDescriptor32>(descriptors(), 30, &clusterCenters, &assignments));
  EXPECT_EQ(clusterCenters.size(), 20);
  EXPECT_TRUE(checkClusterResults(descriptors(), clusterCenters, assignments));
}

TEST_F(KMeansTest, sinlgeCluster) {
  // Test single cluster
  // If target cluster number is 1, should be majority descriptor as all descriptors in the same
  // cluster
  Vector<ImageDescriptor32> clusterCenters;
  Vector<int> assignments = {};
  EXPECT_TRUE(
    clusterDescriptors<ImageDescriptor32>(descriptors(), 1, &clusterCenters, &assignments));
  EXPECT_EQ(clusterCenters.size(), 1);
  EXPECT_TRUE(checkClusterResults(descriptors(), clusterCenters, assignments));

  std::array<size_t, 256> oneCounts;
  oneCounts.fill(0);
  for (const auto &desc : descriptors()) {
    addOneCounts<32>(desc.data(), &oneCounts);
  }
  std::array<uint8_t, 32> majorityDesc;
  majorityDesc.fill(0);
  getMajorityFromOneCounts<32>(oneCounts, descriptors().size() / 2, majorityDesc.data());
  ImageDescriptor32 expectedClusterCenter(majorityDesc);
  EXPECT_EQ(expectedClusterCenter.hammingDistance(clusterCenters[0]), 0);
  EXPECT_EQ(*expectedClusterCenter.blockData(), *clusterCenters[0].blockData());
}

TEST_F(KMeansTest, randomNumberClusters) {
  // Test random number cluster
  Vector<ImageDescriptor32> clusterCenters;
  int testNum = 10;
  for (int i = 0; i < testNum; ++i) {
    clusterCenters.clear();
    Vector<int> assignments = {};
    // random number of clusters
    int numClusters = rand() % 20 + 1;
    EXPECT_TRUE(clusterDescriptors<ImageDescriptor32>(
      descriptors(), numClusters, &clusterCenters, &assignments));
    EXPECT_LE(clusterCenters.size(), numClusters);
    EXPECT_TRUE(checkClusterResults(descriptors(), clusterCenters, assignments));
  }
}

TEST_F(KMeansTest, centerCollapse) {
  // Test center collapse a.k.a cluster/center will be removed if empty
  Vector<ImageDescriptor32> clusterCenters;
  Vector<int> assignments = {};
  std::array<uint8_t, 32> data;
  std::iota(data.begin(), data.end(), 0);
  ImageDescriptor32 desc(data);
  Vector<ImageDescriptor32> descriptors = {};
  for (int i = 0; i < 10; ++i) {
    descriptors.emplace_back(desc.clone());
  }

  EXPECT_TRUE(clusterDescriptors<ImageDescriptor32>(descriptors, 2, &clusterCenters, &assignments));
  // As all descriptors are the same, they should only have 1 cluster even targeting 2
  EXPECT_EQ(clusterCenters.size(), 1);
  EXPECT_TRUE(checkClusterResults(descriptors, clusterCenters, assignments));
}

}  // namespace c8
