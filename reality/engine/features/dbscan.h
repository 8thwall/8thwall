// Copyright (c) 2024 Niantic, Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)

#pragma once

#include "c8/vector.h"
#include "reality/engine/features/image-descriptor.h"

namespace c8 {

// DBSCAN clustering algorithm
// @param descriptors: input descriptors
// @param epsilon: maximum distance between two descriptors to be considered neighbors
// @param minPts: minimum number of points required to form a cluster (including the point itself)
// @param assignments: optional output vector of cluster assignments for each descriptor
// @return vector of clusters. Each cluster is a vector of indices of descriptors in the cluster
template <typename Descriptor>
Vector<Vector<int>> dbscan(
  const Vector<Descriptor> &descriptors,
  const int epsilon,
  const int minPts,
  Vector<int> *assignments);

}  // namespace c8