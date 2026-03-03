// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Nathan Waters (nathanwaters@nianticlabs.com)
//
// Class for handling ray/mesh intersections

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "raycaster.h",
  };
  deps = {
    ":mesh-types",
    "//c8:hmatrix",
    "//c8:hvector",
    "//c8:vector",
    "//c8/stats:scope-timer",
    "//c8/geometry:egomotion",
    "//c8/geometry:line3",
    "//c8/geometry:mesh",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xb5683b4e);

#include <optional>

#include "c8/geometry/egomotion.h"
#include "c8/geometry/mesh-types.h"
#include "c8/geometry/mesh.h"
#include "c8/geometry/raycaster.h"
#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/stats/scope-timer.h"
#include "c8/vector.h"

namespace c8 {

static constexpr float EPSILON = 0.0000001f;

// Ray-Plane intersection
bool rayPlaneIntersection(
  HVector3 rayOrigin,
  HVector3 rayVector,
  HVector3 planeNormal,
  float planeD,
  HPoint3 *outIntersectionPoint) {
  if (outIntersectionPoint == nullptr) {
    return false;
  }

  auto denom = rayVector.dot(planeNormal);
  if (std::fabs(denom) < EPSILON) {
    return false;
  }

  float t = -(rayOrigin.dot(planeNormal) + planeD) / denom;
  // ray intersection
  if (t > EPSILON) {
    (*outIntersectionPoint) = HPoint3{
      rayOrigin.x() + rayVector.x() * t,
      rayOrigin.y() + rayVector.y() * t,
      rayOrigin.z() + rayVector.z() * t};
    return true;
  }

  return false;
}

bool intersectFrustumWithPlane(
  const Vector<float> &plane,
  const Vector<HVector3> &frustumRayDir,
  const Vector<HPoint3> &triangles,
  const Vector<Line3> &boundaryLines,
  Vector<HPoint3> *interPts) {
  if (!interPts) {
    return false;
  }
  interPts->clear();

  int intersectionCount = 0;
  HPoint3 interPt;
  HVector3 originPosV = {0.0f, 0.0f, 0.0f};
  for (size_t i = 0; i < 4; ++i) {
    auto &rayDir = frustumRayDir[i];
    HVector3 planeNormal = {plane[0], plane[1], plane[2]};
    auto doesIntersect =
      rayPlaneIntersection(originPosV, rayDir.unit(), planeNormal, plane[3], &interPt);
    if (doesIntersect) {
      intersectionCount++;
      interPts->push_back(interPt);
    }
  }

  // full intersection
  if (intersectionCount == 4) {
    return true;
  }

  // no intersection
  if (intersectionCount == 0) {
    return false;
  }

  // test if any point is inside the polygon
  // if inside the polygon, return true
  for (size_t tri = 0; tri < triangles.size(); tri += 3) {
    Vector<c8::HPoint3> triangle = {triangles[tri], triangles[tri + 1], triangles[tri + 2]};
    for (size_t pi = 0; pi < interPts->size(); ++pi) {
      // if intersectionPoints[pi] is inside triangle, return true
      if (isPointInTriangle((*interPts)[pi], triangle)) {
        return true;
      }
    }
  }

  // if there is one intersection but not inside the polygon, no need to test further
  if (intersectionCount == 1) {
    return false;
  }

  // if more than one intersections, test if the lines are intersecting with polygon boundary lines.
  // if the lines intersect, there is overlap.
  float mua, mub;
  HPoint3 pa, pb;
  for (size_t i = 0; i < interPts->size() - 1; ++i) {
    for (size_t j = i + 1; j < interPts->size(); ++j) {
      // line i-j
      Line3 lineIj((*interPts)[i], (*interPts)[j]);
      for (size_t li = 0; li < boundaryLines.size(); ++li) {
        // if line i-j intersect with line boundaryLines[li], return true
        bool intersect = boundaryLines[li].intersects(lineIj, &mua, &mub, &pa, &pb);
        if (intersect && mua >= 0.0f && mua <= 1.0f && mub >= 0.0f && mub <= 1.0f) {
          return true;
        }
      }
    }
  }

  return false;
}

}  // namespace c8
