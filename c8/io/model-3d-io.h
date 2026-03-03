// Copyright (c) 2023 Niantic Labs
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)
//
// Read/write 3D models

#pragma once

#include "c8/geometry/mesh-types.h"
#include "c8/hpoint.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

void writeModelToPLY(
  const String &fileName,
  const int vertexCount,
  const Vector<HPoint3> &vertexData,
  const Vector<HVector3> &vertexNormal,
  const Vector<HPoint2> &vertexUv,
  const int faceCount,
  const Vector<MeshIndices> &faceIndices);

}  // namespace c8
