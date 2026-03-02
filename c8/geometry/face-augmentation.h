// Copyright (c) 2023 Niantic Labs, Inc
// Original Author: Nathan Waters (nathan@nianticlabs.com)

#pragma once

#include "c8/geometry/face-types.h"
#include "c8/geometry/mesh-types.h"
#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/vector.h"

namespace c8 {

// Artificially raises the predicted vertices of the head mesh to cover more of the face.
void increaseFaceMeshHeight(
  const Vector<int> &topFaceIndices,
  const Vector<int> &nearlyTopFaceIndices,
  float increasedTopPointsY,
  Vector<HPoint3> *vertices);

// Raises the UVs to match how we artificially raised the mesh geometry in increaseFaceMeshHeight
Vector<UV> increaseFaceUVHeight(
  const Vector<UV> &originalUVs,
  const Vector<int> &topFaceIndices,
  const Vector<int> &nearlyTopFaceIndices,
  float increasedTopPointsV);

}  // namespace c8
