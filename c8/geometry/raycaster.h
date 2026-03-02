// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Nathan Waters (nathanwaters@nianticlabs.com)
//
// Class for handling ray/triangle and ray/mesh intersections.  Used for intersecting the
// FrameWithPoints.hpoints() against a VPS mesh for creating depth points.

#pragma once

#include <optional>

#include "c8/geometry/line3.h"
#include "c8/geometry/mesh-types.h"
#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/vector.h"

namespace c8 {

// This could work in world or cam space, as long as it's consistent.
// Plane is defined as:
//   planeNormal.x * x + planeNormal.y * y + planeNormal.z * z + planeD = 0
// return false if there is no intersection;
// otherwise, store the intersection point into outIntersectionPoint
bool rayPlaneIntersection(
  HVector3 rayOrigin,
  HVector3 rayVector,
  HVector3 planeNormal,
  float planeD,
  HPoint3 *outIntersectionPoint);

// One triangle per 3 points.
bool intersectFrustumWithPlane(
  const Vector<float> &plane,
  const Vector<HVector3> &frustumRayDir,
  const Vector<HPoint3> &triangles,
  const Vector<Line3> &boundaryLines,
  Vector<HPoint3> *interPts);

}  // namespace c8
