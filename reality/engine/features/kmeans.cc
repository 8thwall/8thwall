// Copyright (c) 2024 Niantic, Inc.
// Original Author: Haomin Zhu (hzhu@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "kmeans.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:random-numbers",
    "//c8:vector",
    "//reality/engine/features:descriptor-bit-counter",
    "//reality/engine/features:image-descriptor"};
}
cc_end(0x1ae72a55);

#include <array>
#include <numeric>

#include "c8/c8-log.h"
#include "c8/random-numbers.h"
#include "kmeans.h"
#include "reality/engine/features/descriptor-bit-counter.h"

namespace c8 {

namespace {

constexpr int MAX_KMEANS_ITERATION_NUMS = 100;

// Kmeans++ initialization for cluster centers.
template <typename Descriptor>
bool initialization(
  const Vector<Descriptor> &descriptors, int numClusters, Vector<Descriptor> *clusterCenters) {
  // Initialize cluster centers using kmeans++
  std::vector<Descriptor> centers;

  // Uniformly pick the first cluster center
  c8::RandomNumbers random(214);
  int firstCenter = random.nextUnsignedInt() % descriptors.size();
  clusterCenters->emplace_back(descriptors[firstCenter].clone());

  // Get the rest of the cluster centers based on squared distances to the nearest cluster center
  Vector<int> minSqDistances(descriptors.size(), std::numeric_limits<int>::max());
  Vector<int> accumulatedSqDist(descriptors.size(), 0);
  while (clusterCenters->size() < numClusters) {
    for (size_t i = 0; i < descriptors.size(); ++i) {
      int distance = descriptors[i].hammingDistance(clusterCenters->back());
      minSqDistances[i] = std::min(minSqDistances[i], distance * distance);
    }
    accumulatedSqDist[0] = minSqDistances[0];
    for (size_t i = 1; i < descriptors.size(); ++i) {
      accumulatedSqDist[i] = accumulatedSqDist[i - 1] + minSqDistances[i];
    }
    // If all descriptors are the same, same as centers or selected as centers, no need to add more
    // clusters
    if (accumulatedSqDist.back() == 0) {
      return true;
    }
    int threshold = random.nextUnsignedInt() % accumulatedSqDist.back();
    for (size_t i = 0; i < descriptors.size(); ++i) {
      if (accumulatedSqDist[i] >= threshold) {
        clusterCenters->emplace_back(descriptors[i].clone());
        break;
      }
    }
  }
  return clusterCenters->size() == numClusters;
}

// Find the nearest centroid to a descriptor.
template <typename Descriptor>
int findNearestCentroid(const Descriptor &descriptor, const Vector<Descriptor> &centroids) {
  int minDistance = std::numeric_limits<int>::max();
  int nearestCentroid = -1;
  for (size_t i = 0; i < centroids.size(); i++) {
    int distance = descriptor.hammingDistance(centroids[i]);
    if (distance < minDistance) {
      minDistance = distance;
      nearestCentroid = i;
    }
  }
  return nearestCentroid;
}

// Assign each descriptor to its centroid.
template <typename Descriptor>
void assignCentroids(
  const Vector<Descriptor> &descriptors,
  const Vector<Descriptor> &centroids,
  Vector<int> *assignments) {
  if (!assignments) {
    C8Log("[kmeans] assignments is nullptr");
    return;
  }
  // Assign cluster label/index to all descriptors.
  for (size_t i = 0; i < descriptors.size(); i++) {
    (*assignments)[i] = findNearestCentroid(descriptors[i], centroids);
  }
}

// Compare if two assignments are same.
bool isSameAssignments(const Vector<int> &a, const Vector<int> &b) {
  if (a.size() != b.size()) {
    return false;
  }
  for (size_t i = 0; i < a.size(); i++) {
    if (a[i] != b[i]) {
      return false;
    }
  }
  return true;
}

// Calculate new centroids based on assignments.
template <typename Descriptor>
void calculateCentroids(
  const Vector<Descriptor> &descriptors, Vector<int> &assignments, Vector<Descriptor> *centroids) {
  // Group descriptors by their cluster assignments.
  std::vector<std::vector<size_t>> clusters(centroids->size());
  for (size_t i = 0; i < descriptors.size(); i++) {
    clusters[assignments[i]].emplace_back(i);
  }

  auto clusterIter = clusters.begin();
  auto centroidIter = centroids->begin();
  while (clusterIter != clusters.end()) {
    if (clusterIter->empty()) {
      clusterIter = clusters.erase(clusterIter);
      centroidIter = centroids->erase(centroidIter);
    } else {
      ++clusterIter;
      ++centroidIter;
    }
  }

  std::array<size_t, Descriptor::size() * 8> oneCounts;
  // Calculate new centroids.
  for (size_t i = 0; i < centroids->size(); ++i) {
    oneCounts.fill(0);
    for (const auto &descId : clusters[i]) {
      addOneCounts<Descriptor::size()>(descriptors[descId].data(), &oneCounts);
    }
    std::array<uint8_t, Descriptor::size()> majorityDesc;
    majorityDesc.fill(0);
    getMajorityFromOneCounts<Descriptor::size()>(
      oneCounts, clusters[i].size() / 2, majorityDesc.data());
    memcpy(centroids->at(i).mutableData(), majorityDesc.data(), Descriptor::size());
  }
}
}  // namespace

// kmeans clustering for desciptors
template <typename Descriptor>
bool clusterDescriptors(
  const Vector<Descriptor> &descriptors,
  int numClusters,
  Vector<Descriptor> *clusterCenters,
  Vector<int> *assignments) {
  if (!clusterCenters) {
    C8Log("[kmeans] clusterCenters is nullptr");
    return false;
  }
  if (descriptors.size() <= numClusters) {
    clusterCenters->reserve(descriptors.size());
    for (const auto &desc : descriptors) {
      clusterCenters->emplace_back(desc.clone());
    }
    // assignment is the same as the index of the descriptor lambda function
    if (assignments) {
      assignments->resize(descriptors.size());
      std::iota(assignments->begin(), assignments->end(), 0);
    }
    return true;
  }
  if (!initialization<Descriptor>(descriptors, numClusters, clusterCenters)) {
    C8Log("[kmeans] Failed to initialize cluster centers");
    return false;
  }
  Vector<int> clusterAssignments(descriptors.size(), -1);
  Vector<int> prevClusterAssignments(descriptors.size(), -1);

  for (int iters = 0; iters < MAX_KMEANS_ITERATION_NUMS; ++iters) {
    assignCentroids<Descriptor>(descriptors, *clusterCenters, &clusterAssignments);

    if (isSameAssignments(clusterAssignments, prevClusterAssignments)) {
      break;
    }
    calculateCentroids<Descriptor>(descriptors, clusterAssignments, clusterCenters);

    prevClusterAssignments.swap(clusterAssignments);
  }

  if (assignments) {
    assignments->swap(clusterAssignments);
  }
  return true;
}

template bool clusterDescriptors<ImageDescriptor16>(
  const Vector<ImageDescriptor16> &descriptors,
  int numClusters,
  Vector<ImageDescriptor16> *clusterCenters,
  Vector<int> *assignments = nullptr);

template bool clusterDescriptors<ImageDescriptor32>(
  const Vector<ImageDescriptor32> &descriptors,
  int numClusters,
  Vector<ImageDescriptor32> *clusterCenters,
  Vector<int> *assignments = nullptr);

template bool clusterDescriptors<ImageDescriptor64>(
  const Vector<ImageDescriptor64> &descriptors,
  int numClusters,
  Vector<ImageDescriptor64> *clusterCenters,
  Vector<int> *assignments = nullptr);

template bool clusterDescriptors<OrbFeature>(
  const Vector<OrbFeature> &descriptors,
  int numClusters,
  Vector<OrbFeature> *clusterCenters,
  Vector<int> *assignments = nullptr);

template bool clusterDescriptors<GorbFeature>(
  const Vector<GorbFeature> &descriptors,
  int numClusters,
  Vector<GorbFeature> *clusterCenters,
  Vector<int> *assignments = nullptr);

}  // namespace c8
