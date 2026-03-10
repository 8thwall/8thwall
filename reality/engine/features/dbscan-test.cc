// Copyright (c) 2024 Niantic, Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "//reality/engine/features:dbscan",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xe2f3521d);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "reality/engine/features/dbscan.h"

namespace c8 {

class DbscanTest : public ::testing::Test {};

TEST_F(DbscanTest, TestDbscan) {
  Vector<ImageDescriptor<8>> descriptors;
  // First cluster
  descriptors.push_back({{0b000, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000}});
  descriptors.push_back({{0b010, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000}});
  descriptors.push_back({{0b011, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000}});
  // Second cluster
  descriptors.push_back({{0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111}});
  descriptors.push_back({{0b101, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111}});
  descriptors.push_back({{0b100, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111}});

  Vector<int> assignments;
  auto clusters = dbscan<ImageDescriptor<8>>(descriptors, 1, 3, &assignments);

  EXPECT_EQ(clusters.size(), 2);
  EXPECT_EQ(clusters[0].size(), 3);
  EXPECT_EQ(clusters[1].size(), 3);
  // Check that the first three descriptors are in the same cluster
  EXPECT_EQ(assignments[0], assignments[1]);
  EXPECT_EQ(assignments[0], assignments[2]);
  // Check that the last three descriptors are in the same cluster
  EXPECT_EQ(assignments[3], assignments[4]);
  EXPECT_EQ(assignments[3], assignments[5]);
}

// Empty input
TEST_F(DbscanTest, TestDbscanEmpty) {
  Vector<ImageDescriptor<8>> descriptors;
  Vector<int> assignments;
  auto clusters = dbscan<ImageDescriptor<8>>(descriptors, 1, 3, &assignments);

  EXPECT_EQ(clusters.size(), 0);
  EXPECT_EQ(assignments.size(), 0);
}

// Single point
TEST_F(DbscanTest, TestDbscanSinglePoint) {
  Vector<ImageDescriptor<8>> descriptors;
  descriptors.push_back({{0b000, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000}});
  Vector<int> assignments;

  // Single point should be noise (since minPts = 3)
  auto clusters = dbscan<ImageDescriptor<8>>(descriptors, 1, 3, &assignments);

  EXPECT_EQ(clusters.size(), 0);
}

TEST_F(DbscanTest, TestDbscanOneClusterOneOutlier) {
  Vector<ImageDescriptor<8>> descriptors;
  // Cluster of 4 points with hamming counting (each point differs by 1 bit)
  descriptors.push_back({{0b000, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000}});
  descriptors.push_back({{0b001, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000}});
  descriptors.push_back({{0b011, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000}});
  descriptors.push_back({{0b010, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000}});

  // First outlier is far away from the cluster
  descriptors.push_back({{0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111, 0b111}});
  // Second outlier is close to the cluster, but not close enough to be considered part of it
  descriptors.push_back({{0b100, 0b100, 0b000, 0b000, 0b000, 0b000, 0b000, 0b000}});

  Vector<int> assignments;
  auto clusters = dbscan<ImageDescriptor<8>>(descriptors, 1, 3, &assignments);

  EXPECT_EQ(clusters.size(), 1);

  // Check that the first four descriptors are in the same cluster
  EXPECT_EQ(assignments[0], assignments[1]);
  EXPECT_EQ(assignments[0], assignments[2]);
  EXPECT_EQ(assignments[0], assignments[3]);

  // Check that the last two descriptors are noise
  EXPECT_EQ(assignments[4], -1);
  EXPECT_EQ(assignments[5], -1);
}

}  // namespace c8
