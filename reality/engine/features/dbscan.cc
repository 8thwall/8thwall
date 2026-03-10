// Copyright (c) 2024 Niantic, Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "dbscan.h",
  };
  deps = {
    "//c8:vector",
    "//reality/engine/features:image-descriptor",
  };
}
cc_end(0xf995e10d);

#include "dbscan.h"

namespace c8 {

namespace {

constexpr int UNVISITED = -2;
constexpr int NOISE = -1;

// Utility to find all points within the epsilon distance of a point
// @param descriptors: input descriptors
// @param index: index of the descriptor to find neighbors for
// @param epsilon: maximum distance between two descriptors to be considered neighbors
// @return vector of indices of descriptors within epsilon distance of the input descriptor
template <typename Descriptor>
Vector<int> regionQuery(const Vector<Descriptor> &descriptors, const int index, const int epsilon) {
  Vector<int> neighbors;
  // TODO(Riyaan): Consider using a more efficient data structure or ANN for this
  // e.g. a k-d tree or an upper triangular distance lookup table
  for (int i = 0; i < descriptors.size(); ++i) {
    if (descriptors[index].hammingDistance(descriptors[i]) <= epsilon) {
      neighbors.push_back(i);
    }
  }
  return neighbors;
}

// Utility to expand a cluster
// @param descriptors: input descriptors
// @param index: index of the descriptor to expand the cluster from
// @param epsilon: maximum distance between two descriptors to be considered neighbors
// @param minPts: minimum number of points required to form a cluster (including the point itself)
// @param clusterId: id of the cluster to expand
// @param cluster: output vector of indices of descriptors in the cluster
// @param neighbors: input vector of indices of neighbors of the input descriptor
// @param clusterIds: output vector of cluster assignments for each descriptor
template <typename Descriptor>
void expandCluster(
  const Vector<Descriptor> &descriptors,
  const int index,
  const int epsilon,
  const int minPts,
  const int clusterId,
  Vector<int> *cluster,
  Vector<int> *neighbors,
  Vector<int> *clusterIds) {
  cluster->push_back(index);
  (*clusterIds)[index] = clusterId;

  while (!neighbors->empty()) {
    int current = neighbors->back();
    neighbors->pop_back();
    if ((*clusterIds)[current] == UNVISITED) {
      Vector<int> currentNeighbors = regionQuery<Descriptor>(descriptors, current, epsilon);
      if (currentNeighbors.size() >= minPts) {
        neighbors->insert(
          neighbors->end(),
          currentNeighbors.begin(),
          currentNeighbors.end());  // Appending new neighbors
      }
    }
    // TODO(Riyaan): Consider doing DBSCAN* which leaves non-core (i.e. points which border multiple
    // clusters) as noise. For now, add to first cluster the point encounters.
    auto &currentClusterId = (*clusterIds)[current];
    if (currentClusterId == UNVISITED || currentClusterId == NOISE) {
      currentClusterId = clusterId;
      cluster->push_back(current);
    }
  }
}

}  // namespace

// DBSCAN clustering algorithm
template <typename Descriptor>
Vector<Vector<int>> dbscan(
  const Vector<Descriptor> &descriptors,
  const int epsilon,
  const int minPts,
  Vector<int> *assignments) {
  *assignments = Vector<int>(descriptors.size(), UNVISITED);
  int clusterId = 0;
  Vector<Vector<int>> clusters;

  for (int i = 0; i < descriptors.size(); ++i) {
    if ((*assignments)[i] == UNVISITED) {
      Vector<int> neighbors = regionQuery<Descriptor>(descriptors, i, epsilon);
      if (neighbors.size() < minPts) {
        (*assignments)[i] = NOISE;
      } else {
        Vector<int> cluster;
        expandCluster<Descriptor>(
          descriptors, i, epsilon, minPts, clusterId, &cluster, &neighbors, assignments);
        clusters.emplace_back(std::move(cluster));
        clusterId++;
      }
    }
  }
  return clusters;
}

template Vector<Vector<int>> dbscan<ImageDescriptor<8>>(
  const Vector<ImageDescriptor<8>> &descriptors,
  const int epsilon,
  const int minPts,
  Vector<int> *assignments);

template Vector<Vector<int>> dbscan<ImageDescriptor<16>>(
  const Vector<ImageDescriptor<16>> &descriptors,
  const int epsilon,
  const int minPts,
  Vector<int> *assignments);

template Vector<Vector<int>> dbscan<ImageDescriptor<32>>(
  const Vector<ImageDescriptor<32>> &descriptors,
  const int epsilon,
  const int minPts,
  Vector<int> *assignments);

template Vector<Vector<int>> dbscan<ImageDescriptor<64>>(
  const Vector<ImageDescriptor<64>> &descriptors,
  const int epsilon,
  const int minPts,
  Vector<int> *assignments);

template Vector<Vector<int>> dbscan<OrbFeature>(
  const Vector<OrbFeature> &descriptors,
  const int epsilon,
  const int minPts,
  Vector<int> *assignments);

template Vector<Vector<int>> dbscan<GorbFeature>(
  const Vector<GorbFeature> &descriptors,
  const int epsilon,
  const int minPts,
  Vector<int> *assignments);

}  // namespace c8
