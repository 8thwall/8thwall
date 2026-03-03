// Copyright (c) 2024 Niantic, Inc.
// Original Author: Haomin Zhu (hzhu@nianticlabs.com)

#pragma once

#include "c8/vector.h"
#include "reality/engine/features/image-descriptor.h"

namespace c8 {

template <typename Descriptor>
bool clusterDescriptors(
  const Vector<Descriptor> &descriptors,
  int numClusters,
  Vector<Descriptor> *clusterCenters,
  Vector<int> *assignments = nullptr);

extern template bool clusterDescriptors<ImageDescriptor16>(
  const Vector<ImageDescriptor16> &descriptors,
  int numClusters,
  Vector<ImageDescriptor16> *clusterCenters,
  Vector<int> *assignments = nullptr);

extern template bool clusterDescriptors<ImageDescriptor32>(
  const Vector<ImageDescriptor32> &descriptors,
  int numClusters,
  Vector<ImageDescriptor32> *clusterCenters,
  Vector<int> *assignments = nullptr);

extern template bool clusterDescriptors<ImageDescriptor64>(
  const Vector<ImageDescriptor64> &descriptors,
  int numClusters,
  Vector<ImageDescriptor64> *clusterCenters,
  Vector<int> *assignments = nullptr);

}  // namespace c8
