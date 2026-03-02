// Copyright (c) 2023 Niantic Labs, Inc
// Original Author: Nathan Waters (nathan@nianticlabs.com)

#include "bzl/inliner/rules2.h"
cc_library {
  hdrs = {
    "face-augmentation.h",
  };
  deps = {
    "//c8:hpoint",
    "//c8:hmatrix",
    "//c8:hvector",
    "//c8:vector",
    "//c8/geometry:face-types",
    "//c8/geometry:mesh-types",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x7a5a83ca);

#include "c8/geometry/face-types.h"
#include "c8/geometry/mesh-types.h"

namespace c8 {

void increaseFaceMeshHeight(
  const Vector<int> &topFaceIndices,
  const Vector<int> &nearlyTopFaceIndices,
  float increasedTopPointsY,
  Vector<HPoint3> *vertices) {
  for (auto index : topFaceIndices) {
    auto &vertex = vertices->at(index);
    (*vertices)[index] = {vertex.x(), vertex.y() + increasedTopPointsY, vertex.z()};
  }

  // Raise the points right below the top most points a bit as well.
  float increasedNearlyTopPointsY = increasedTopPointsY / 2.f;
  for (auto index : nearlyTopFaceIndices) {
    auto &vertex = vertices->at(index);
    (*vertices)[index] = {vertex.x(), vertex.y() + increasedNearlyTopPointsY, vertex.z()};
  }
}

Vector<UV> increaseFaceUVHeight(
  const Vector<UV> &originalUVs,
  const Vector<int> &topFaceIndices,
  const Vector<int> &nearlyTopFaceIndices,
  float increasedTopPointsV) {
  auto raisedUVs = originalUVs;
  for (auto index : topFaceIndices) {
    raisedUVs[index] = {raisedUVs[index].u, raisedUVs[index].v + increasedTopPointsV};
  }

  // Raise the points right below the top most points a bit as well.
  float increasedNearlyTopPointsY = increasedTopPointsV / 2.f;
  for (auto index : nearlyTopFaceIndices) {
    raisedUVs[index] = {raisedUVs[index].u, raisedUVs[index].v + increasedNearlyTopPointsY};
  }
  return raisedUVs;
}

}  // namespace c8
