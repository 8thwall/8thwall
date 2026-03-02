// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/geometry/mesh-types.h"
#include "c8/hpoint.h"
#include "c8/vector.h"

namespace c8 {

// Static utilities for projecting 3D points to 2D cameras, and for inferring
// 3D structure from 2D point correspondences.
class Worlds {
public:
  // This world consists of 15 corners of three axis-aligned regular polygons.
  static Vector<HPoint3> axisAlignedPolygonsWorld();

  // This world consists of several planes parallel to the ground plane, roughly resembling a floor,
  // table and chairs.
  static Vector<HPoint3> gravityNormalPlanesWorld();

  // This world consists of 25 points of a Z-axis aligned grid.
  static Vector<HPoint3> axisAlignedGridWorld();

  // This world consists of 99 points at distance 1 from the camera suitable for viewing from the
  // origin.
  static Vector<HPoint3> flatPlaneFromOriginWorld();

  // This world consists of 6 points at the eyes, ears, mouth and nose of a face.
  static Vector<HPoint3> sparseFaceMeshWorld();

  // Returns the indices for the points in sparseFaceMeshWorld
  static Vector<MeshIndices> sparseFaceMeshIndices();

  // This world consists of 6 points at the eyes, ears, mouth and nose of a face.
  static Vector<HPoint3> denseFaceMeshWorld();

  // Returns the indices for the points in denseFaceMeshWorld
  static Vector<MeshIndices> denseFaceMeshIndices();

  // This is a static utility class, it can't be constructred, copied or moved.
  Worlds() = delete;
  Worlds(Worlds &&) = delete;
  Worlds &operator=(Worlds &&) = delete;
  Worlds(const Worlds &) = delete;
  Worlds &operator=(const Worlds &) = delete;

private:
};

}  // namespace c8
