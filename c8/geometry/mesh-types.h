// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Nathan Waters (nathan@8thwall.com)

#pragma once

#include "c8/color.h"
#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/vector.h"

namespace c8 {

// Stores uv coordinates (position in a texture map) for a vertex
struct UV {
  float u;
  float v;
};

// Represents one triangle in a mesh. Each element is an index in a list of vertices.
struct MeshIndices {
  uint32_t a = -1;
  uint32_t b = -1;
  uint32_t c = -1;
};

struct PointIndices {
  uint32_t a = -1;
};

// Represents one mesh with geometry (position, color, normals and uvs per vertex) and its triangle
// connections.  Not all fields may be set.
struct MeshGeometry {
  // Aggregate type. No user-defined constructors.

  // Positions of vertices in local xr space
  Vector<HPoint3> points;
  // Vertex colors
  Vector<Color> colors;
  // Indices
  Vector<MeshIndices> triangles;

  // Normals
  Vector<HVector3> normals;
  // UVs
  Vector<HVector2> uvs;

  MeshGeometry clone() const {
    MeshGeometry ret;
    ret.points = points;
    ret.colors = colors;
    ret.triangles = triangles;
    ret.normals = normals;
    ret.uvs = uvs;
    return ret;
  }
};

// Represents one point cloud with geometry (position, color and normals per point). Not all fields
// may be set.
struct PointCloudGeometry {
  // Aggregate type. No user-defined constructors.

  // Positions of vertices in local xr space
  Vector<HPoint3> points;

  // Vertex colors
  Vector<Color> colors;

  // Normals
  Vector<HVector3> normals;

  PointCloudGeometry clone() const {
    PointCloudGeometry ret;
    ret.points = points;
    ret.colors = colors;
    ret.normals = normals;
    return ret;
  }
};

}  // namespace c8
