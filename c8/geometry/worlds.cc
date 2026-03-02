// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "worlds.h",
  };
  deps = {
    "//c8:hmatrix",
    "//c8:hpoint",
    "//c8/geometry:mesh",
    "//c8/geometry:mesh-types",
    "//c8/geometry:two-d",
    "//c8/geometry:facemesh-data",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xb3de112d);

#include <cmath>

#include "c8/hmatrix.h"
#include "c8/geometry/facemesh-data.h"
#include "c8/geometry/mesh.h"
#include "c8/geometry/mesh-types.h"
#include "c8/geometry/two-d.h"
#include "c8/geometry/worlds.h"

using namespace c8;

namespace {
float sinD(float x) { return std::sin(x * M_PI / 180.0f); }
float cosD(float x) { return std::cos(x * M_PI / 180.0f); }
}  // namespace

Vector<HPoint3> Worlds::axisAlignedGridWorld() {
  Vector<HPoint3> world;
  for (float i = -2; i <= 2; i += 1.0f) {
    for (float j = -2; j <= 2; j += 1.0f) {
      world.push_back(HPoint3(i, j, 0.0f));
    }
  }
  return world;
}

Vector<HPoint3> Worlds::flatPlaneFromOriginWorld() {
  // a set of points on a plane
  Vector<HPoint3> pointsOnPlane;
  pointsOnPlane.reserve(99);
  for (int i = 0; i < 11; i++) {
    for (int j = 0; j < 9; j++) {
      pointsOnPlane.emplace_back((i - 6.0f) * .05f, (j - 5.0f) * .05f, 1.0f);
    }
  }
  return pointsOnPlane;
}

Vector<HPoint3> Worlds::axisAlignedPolygonsWorld() {
  // The world consists of 15 corners of three axis-aligned regular polygons.

  // y-z plane square:
  Vector<HPoint3> yzSquare = {
    {0.0f, cosD(45.0f), -sinD(45.0f)},
    {0.0f, cosD(45.0f), sinD(45.0f)},
    {0.0f, -cosD(45.0f), sinD(45.0f)},
    {0.0f, -cosD(45.0f), -sinD(45.0f)},
  };

  // x-z plane pentagon:
  Vector<HPoint3> xzPentagon = {
    {cosD(54.0f), 0.0f, -sinD(54.0f)},
    {cosD(54.0f) + cosD(126.0f) + cosD(18.0f), 0.0f, sinD(126.0f) + sinD(162.0f) - sinD(54.0f)},
    {0.0f, 0.0f, 1.0f},
    {-cosD(54.0f) - cosD(126.0f) - cosD(18.0f), 0.0f, sinD(126.0f) + sinD(162.0f) - sinD(54.0f)},
    {-cosD(54.0f), 0.0f, -sinD(54.0f)},
  };

  // x-y plane hexagon:
  Vector<HPoint3> xyHexagon = {
    {cosD(60.0f), -sinD(60.0f), 0.0f},
    {1.0f, 0.0f, 0.0f},
    {cosD(60.0f), sinD(60.0f), 0.0f},
    {-cosD(60.0f), sinD(60.0f), 0.0f},
    {-1.0f, 0.0f, 0.0f},
    {-cosD(60.0f), -sinD(60.0f), 0.0f},
  };

  // The polygons are all offset from the origin in a positive direction in their respective
  yzSquare = HMatrixGen::translation(0.0f, 2.0f, 2.0f) * yzSquare;
  xzPentagon = HMatrixGen::translation(2.0f, 0.0f, 2.0f) * xzPentagon;
  xyHexagon = HMatrixGen::translation(2.0f, 2.0f, 0.0f) * xyHexagon;

  c8::Vector<HPoint3> result;
  result.insert(result.end(), yzSquare.begin(), yzSquare.end());
  result.insert(result.end(), xzPentagon.begin(), xzPentagon.end());
  result.insert(result.end(), xyHexagon.begin(), xyHexagon.end());

  return result;
}

Vector<HPoint3> Worlds::gravityNormalPlanesWorld() {
  float tableHeight = 1.0f;
  float chairHeight = 0.5f;
  float floorHeight = 0.0f;

  float tablePointDensity = 8.0f;
  float chairPointDensity = 4.0f;
  float floorPointDensity = 3.0f;

  Line2 table(HPoint2(-0.5f, -1.0f), HPoint2(0.5f, 1.0f));

  Vector<Line2> chairs{Line2(HPoint2(-0.5f, -2.25f), HPoint2(0.5f, -1.25f)),
                       Line2(HPoint2(-0.5f, 1.25f), HPoint2(0.5f, 2.25f)),
                       Line2(HPoint2(-1.75f, -1.25f), HPoint2(-.75f, -.25f)),
                       Line2(HPoint2(.75f, -1.25f), HPoint2(1.75f, -.25f)),
                       Line2(HPoint2(-1.75f, .25f), HPoint2(-.75f, 1.25f)),
                       Line2(HPoint2(.75f, .25f), HPoint2(1.75f, 1.25f))};

  Line2 floor(HPoint2(-3.0f, -3.0f), HPoint2(3.0f, 3.0f));

  Vector<HPoint3> result;

  // Table.
  for (float x = table.start().x(); x <= table.end().x(); x += 1.0f / tablePointDensity) {
    for (float z = table.start().y(); z <= table.end().y(); z += 1.0f / tablePointDensity) {
      result.push_back(HPoint3(x, tableHeight, z));
    }
  }

  // Chairs.
  for (auto chair : chairs) {
    for (float x = chair.start().x(); x <= chair.end().x(); x += 1.0f / chairPointDensity) {
      for (float z = chair.start().y(); z <= chair.end().y(); z += 1.0f / chairPointDensity) {
        result.push_back(HPoint3(x, chairHeight, z));
      }
    }
  }

  // Floor.
  for (float x = floor.start().x(); x <= floor.end().x(); x += 1.0f / floorPointDensity) {
    for (float z = floor.start().y(); z <= floor.end().y(); z += 1.0f / floorPointDensity) {
      // Table overlap.
      if (
        x >= table.start().x() && x <= table.end().x() && z >= table.start().y()
        && z <= table.end().y()) {
        continue;
      }

      // Chairs overlap.
      bool overlapChair = false;
      for (auto chair : chairs) {
        if (
          x >= chair.start().x() && x <= chair.end().x() && z >= chair.start().y()
          && z <= chair.end().y()) {
          overlapChair = true;
          break;
        }
      }
      if (overlapChair) {
        continue;
      }

      result.push_back(HPoint3(x, floorHeight, z));
    }
  }

  return result;
}

// This is in relation to a viewer's face, where they are at the origin, facing
// positive z.
Vector<HPoint3> Worlds::sparseFaceMeshWorld() { return BLAZEFACE_SAMPLE_VERTICES; }

Vector<MeshIndices> Worlds::sparseFaceMeshIndices() { return BLAZEFACE_INDICES; }

// We've centered the facemesh points around the origin and flipped the y upside down so
// that the head points are right-side up.
Vector<HPoint3> Worlds::denseFaceMeshWorld() {

  Vector<HPoint3> centeredFacemesh;
  centeredFacemesh.resize(FACEMESH_SAMPLE_OUTPUT.size());

  const auto scalingFactor = HMatrixGen::scale(0.00104f, 0.00104f, 0.00104f);
  const auto rotationFactor = HMatrixGen::rotationD(0.0f, -3.0f, 1.0f);
  HPoint3 centroid = computeCentroid(FACEMESH_SAMPLE_OUTPUT);
  centroid = rotationFactor * scalingFactor * centroid;
  const auto translation =
    HMatrixGen::translation(-1.0f * centroid.x(), -1.0f * centroid.y(), -1.0f * centroid.z());

  for (int i = 0; i < FACEMESH_SAMPLE_OUTPUT.size(); i++) {
    // You want to scale, then rotate, then translate
    auto centeredPoint = FACEMESH_SAMPLE_OUTPUT[i];
    centeredPoint = translation * rotationFactor * scalingFactor * centeredPoint;

    // flip the y and z upside down so that it is facing forward and the forehead is above the mouth
    centeredPoint =
      HPoint3(centeredPoint.x(), -1.0f * centeredPoint.y(), -1.0f * centeredPoint.z());

    centeredFacemesh[i] = centeredPoint;
  }

  return centeredFacemesh;
}

Vector<MeshIndices> Worlds::denseFaceMeshIndices() { return FACEMESH_INDICES; }
