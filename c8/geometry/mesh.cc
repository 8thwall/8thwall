// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nathan Waters (nathan@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "mesh.h",
  };
  deps = {
    "//c8:hpoint",
    "//c8:vector",
    "//c8:hmatrix",
    "//c8:hvector",
    "//c8/geometry:mesh-types",
    "//c8/stats:scope-timer",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x9c69cdcf);

#include <math.h>

#include <iostream>

#include "c8/geometry/mesh-types.h"
#include "c8/geometry/mesh.h"
#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/stats/scope-timer.h"
#include "c8/vector.h"

namespace c8 {

constexpr float EPSILON = 1.0e-6;

// Computes a matrix that can be used to transform normals from a model into view space, accounting
// for shifts in scale.
HMatrix normalMatrix(const HMatrix &m) {
  auto i = m.inv();
  // Normal matrix: Invert, Transpose, Truncate model view matrix.
  return HMatrix{
    {i(0, 0), i(1, 0), i(2, 0), 0.0f},
    {i(0, 1), i(1, 1), i(2, 1), 0.0f},
    {i(0, 2), i(1, 2), i(2, 2), 0.0f},
    {0.0000f, 0.0000f, 0.0000f, 1.0f},
    {m(0, 0), m(1, 0), m(2, 0), 0.0f},
    {m(0, 1), m(1, 1), m(2, 1), 0.0f},
    {m(0, 2), m(1, 2), m(2, 2), 0.0f},
    {0.0000f, 0.0000f, 0.0000f, 1.0f}};
}

// Return the normalized normal
HVector3 getFaceNormalUnit(const HPoint3 &a, const HPoint3 &b, const HPoint3 &c) {
  return getFaceNormal(a, b, c).unit();
}

// Given three points of a triangle, return the un-normalized normal
HVector3 getFaceNormal(const HPoint3 &a, const HPoint3 &b, const HPoint3 &c) {
  // Source: https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal
  // get the vectors of two lines in the triangle
  const HVector3 ba(b.x() - a.x(), b.y() - a.y(), b.z() - a.z());
  const HVector3 ca(c.x() - a.x(), c.y() - a.y(), c.z() - a.z());

  // return the non-normalized normal
  return ba.cross(ca);
}

// Return the normalized normal
HVector3 getFaceNormalVectorUnit(const HVector3 &a, const HVector3 &b, const HVector3 &c) {
  return getFaceNormalVector(a, b, c).unit();
}

// Given three points of a triangle, return the un-normalized normal.  This accepts HVector3
// because of the 20% speed improvement over HPoint3 due the division by w component when accessing
// the elements of the point
HVector3 getFaceNormalVector(const HVector3 &a, const HVector3 &b, const HVector3 &c) {
  // Source: https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal
  // get the vectors of two lines in the triangle
  const HVector3 ba(b.x() - a.x(), b.y() - a.y(), b.z() - a.z());
  const HVector3 ca(c.x() - a.x(), c.y() - a.y(), c.z() - a.z());

  // return the non-normalized normal
  return ba.cross(ca);
}

Vector<MeshIndices> rotateIndices(const Vector<MeshIndices> &indices) {
  Vector<MeshIndices> rotatedIndices;
  rotatedIndices.reserve(indices.size());

  for (const auto &triangle : indices) {
    rotatedIndices.push_back({triangle.a, triangle.c, triangle.b});
  }

  return rotatedIndices;
}

Vector<HPoint2> mirrorUvs(const Vector<HPoint2> &uvs, bool mirrorY) {
  Vector<HPoint2> mirroredUvs;
  mirroredUvs.reserve(uvs.size());

  if (mirrorY) {
    for (const auto &uv : uvs) {
      mirroredUvs.push_back({uv.x(), 1.0f - uv.y()});
    }
  } else {
    for (const auto &uv : uvs) {
      mirroredUvs.push_back({1.0f - uv.x(), uv.y()});
    }
  }

  return mirroredUvs;
}

// Compute the face normals.  Rough shading.  Smoother shading is provided by computeVertexNormals
void computeFaceNormals(
  const Vector<HPoint3> &verticesPoints,
  const Vector<MeshIndices> &indices,
  Vector<HVector3> *faceNormalsPtr) {
  ScopeTimer t("compute-face-normals");

  Vector<HVector3> &faceNormals = *faceNormalsPtr;
  faceNormals.clear();
  faceNormals.reserve(indices.size());
  for (auto face : indices) {
    const auto a = verticesPoints[face.a];
    const auto b = verticesPoints[face.b];
    const auto c = verticesPoints[face.c];
    // The larger the area, the larger the length of the normal.  By adding the non-normalized
    // face normal, the normals with a larger length have more of an effect on the vertex's final
    // summed normal.
    faceNormals.push_back(getFaceNormalUnit(a, b, c));
  }
}

// Compute the normal vector for each vertex which is weighted by the triangle areas.
// Inspired by THREE.js computeVertexNormals which is based on:
//   http://www.iquilezles.org/www/articles/normals/normals.htm
void computeVertexNormals(
  const Vector<HPoint3> &verticesPoints,
  const Vector<MeshIndices> &indices,
  Vector<HVector3> *vertexNormalsPtr) {
  ScopeTimer t("compute-vertex-normals");

  // The conversion from HPoint3 to HVector3 gave a 20% improvement on benchmarking tests since
  // it removes the division by w for HPoint3 when accessing x,y,z values.
  Vector<HVector3> vertices;
  vertices.resize(verticesPoints.size());
  std::transform(
    verticesPoints.begin(), verticesPoints.end(), vertices.begin(), [](HPoint3 p) -> HVector3 {
      return {p.x(), p.y(), p.z()};
    });

  Vector<HVector3> &vertexNormals = *vertexNormalsPtr;
  vertexNormals.resize(vertices.size());

  // go through each face index.  Add the face normal to vector's normal sum.
  for (auto face : indices) {
    const auto a = vertices[face.a];
    const auto b = vertices[face.b];
    const auto c = vertices[face.c];

    // The larger the area, the larger the length of the normal.  By adding the non-normalized
    // face normal, the normals with a larger length have more of an effect on the vertex's final
    // summed normal.
    HVector3 crossProduct = getFaceNormalVector(a, b, c);

    vertexNormals[face.a] += crossProduct;
    vertexNormals[face.b] += crossProduct;
    vertexNormals[face.c] += crossProduct;
  }

  // Now that we've added the weighted face normals, we normalize the sum for each vertex
  for (int i = 0; i < vertexNormals.size(); i += 1) {
    vertexNormals[i] = vertexNormals[i].unit();
  }
}

// The centroid of all of the points is simply the average
HPoint3 computeCentroid(const Vector<HPoint3> &vertices) {
  float centroidX = 0.0f;
  float centroidY = 0.0f;
  float centroidZ = 0.0f;

  for (auto vertex : vertices) {
    centroidX += vertex.x();
    centroidY += vertex.y();
    centroidZ += vertex.z();
  }

  centroidX /= (float)vertices.size();
  centroidY /= (float)vertices.size();
  centroidZ /= (float)vertices.size();
  return {centroidX, centroidY, centroidZ};
}

// Given a list of vertices and subset vertex indices,
// it will compute the centroid of the subset vertices
HPoint3 computeCentroid(const Vector<HPoint3> &vertices, const Vector<int> &indices) {
  float centroidX = 0.0f;
  float centroidY = 0.0f;
  float centroidZ = 0.0f;
  if (indices.empty()) {
    return {centroidX, centroidY, centroidZ};
  }

  for (auto index : indices) {
    centroidX += vertices[index].x();
    centroidY += vertices[index].y();
    centroidZ += vertices[index].z();
  }
  const float denorm = static_cast<float>(indices.size());
  centroidX /= denorm;
  centroidY /= denorm;
  centroidZ /= denorm;
  return {centroidX, centroidY, centroidZ};
}

// Takes in points and centers them by placing the centroid at the origin
void centerCentroid(Vector<HPoint3> *verticesPtr) {
  Vector<HPoint3> &vertices = *verticesPtr;
  const auto centroid = computeCentroid(vertices);
  for (int i = 0; i < vertices.size(); i++) {
    vertices[i] = {
      vertices[i].x() - centroid.x(),
      vertices[i].y() - centroid.y(),
      vertices[i].z() - centroid.z(),
    };
  }
}

// Given a point on a triangle's plane, return its location in Barycentric space.
std::array<float, 3> toBarycentricWeights(const HPoint3 &pt, const Vector<HPoint3> &triangle) {
  // One trick that works is to turn the 3D problem into a 2D problem simply by discarding one of
  // x, y or z. This simplifies the problem by projecting the triangle into one of the three world
  // cardinal planes. This works because the projected areas are proportional to the original areas.
  // We choose the plane of projection so as to maximize the area of the projected triangle.  This
  // is done by discarding the triangle's normal's component that has the largest absolute value.
  const auto normal = getFaceNormal(triangle[0], triangle[1], triangle[2]);
  float u1, u2, u3, u4;
  float v1, v2, v3, v4;

  if (
    (std::fabs(normal.x()) >= std::fabs(normal.y()))
    && (std::fabs(normal.x()) >= std::fabs(normal.z()))) {
    // Discard x, project onto yz plane.
    u1 = triangle[0].y() - triangle[2].y();
    u2 = triangle[1].y() - triangle[2].y();
    u3 = pt.y() - triangle[0].y();
    u4 = pt.y() - triangle[2].y();
    v1 = triangle[0].z() - triangle[2].z();
    v2 = triangle[1].z() - triangle[2].z();
    v3 = pt.z() - triangle[0].z();
    v4 = pt.z() - triangle[2].z();
  } else if (std::fabs(normal.y()) >= std::fabs(normal.z())) {
    // Discard y, project onto xz plane.
    u1 = triangle[0].z() - triangle[2].z();
    u2 = triangle[1].z() - triangle[2].z();
    u3 = pt.z() - triangle[0].z();
    u4 = pt.z() - triangle[2].z();
    v1 = triangle[0].x() - triangle[2].x();
    v2 = triangle[1].x() - triangle[2].x();
    v3 = pt.x() - triangle[0].x();
    v4 = pt.x() - triangle[2].x();
  } else {
    // Discard z, project onto xy plane
    u1 = triangle[0].x() - triangle[2].x();
    u2 = triangle[1].x() - triangle[2].x();
    u3 = pt.x() - triangle[0].x();
    u4 = pt.x() - triangle[2].x();
    v1 = triangle[0].y() - triangle[2].y();
    v2 = triangle[1].y() - triangle[2].y();
    v3 = pt.y() - triangle[0].y();
    v4 = pt.y() - triangle[2].y();
  }

  float denominator = v1 * u2 - v2 * u1;
  float invDenominator = 1.0f / denominator;
  float x = (v4 * u2 - v2 * u4) * invDenominator;
  float y = (v1 * u3 - v3 * u1) * invDenominator;
  // Return the barycentric coordinates.
  return {x, y, 1.0f - x - y};
}

// Given a point, project it onto the plane that is described by at least three points.
HPoint3 projectPointToPlane(const HPoint3 &pt, const Vector<HPoint3> &pointsOnPlane) {
  const auto planeNormal = getFaceNormalUnit(pointsOnPlane[0], pointsOnPlane[1], pointsOnPlane[2]);
  const HVector3 pointToPlane = {
    pt.x() - pointsOnPlane[0].x(), pt.y() - pointsOnPlane[0].y(), pt.z() - pointsOnPlane[0].z()};

  // scalar distance from point to plane along the normal
  float dist = pointToPlane.dot(planeNormal);
  const HPoint3 projectedPoint = {
    pt.x() - dist * planeNormal.x(),
    pt.y() - dist * planeNormal.y(),
    pt.z() - dist * planeNormal.z()};
  return projectedPoint;
}

HPoint3 closestSegmentPoint(const HPoint3 &pt, const HPoint3 &start, const HPoint3 &end) {
  const HVector3 ab = {end.x() - start.x(), end.y() - start.y(), end.z() - start.z()};
  const HVector3 av = {pt.x() - start.x(), pt.y() - start.y(), pt.z() - start.z()};

  // Percent between start and end of the line segment that the projection falls on.  We clamp
  // between 0 and 1 since it is a line segment.
  float t0 = std::clamp(ab.dot(av) / ab.dot(ab), 0.0f, 1.0f);

  const HPoint3 projection = {
    start.x() + t0 * ab.x(), start.y() + t0 * ab.y(), start.z() + t0 * ab.z()};

  return projection;
}

HPoint3 closestTrianglePoint(const HPoint3 &pt, const Vector<HPoint3> &triangle) {
  // It simplifies the problem to think of the triangle point that is closest to the given point
  // could be in three locations:
  // 1) Inside the triangle
  // 2) One of the three vertices of the triangle
  // 3) On one of the three edges of the triangle
  const auto projectedPoint = projectPointToPlane(pt, triangle);

  // Get projected point in barycentric space in order to determine if the projected point is within
  // the triangle.
  const auto baryWeights = toBarycentricWeights(projectedPoint, triangle);

  // 1) If the point is within the triangle, return the distance between the projected point and the
  // original point.
  if (baryWeights[0] >= 0.0f && baryWeights[1] >= 0.0f && baryWeights[2] >= 0.0f) {
    return projectedPoint;
  }

  // 2) If only one of the barycentric coordinates is positive, the closest point on the triangle is
  // that vertex.
  if (baryWeights[0] >= 1.0f && baryWeights[1] <= 0.0f && baryWeights[2] <= 0.0f) {
    return triangle[0];
  }
  if (baryWeights[0] <= 0.0f && baryWeights[1] >= 1.0f && baryWeights[2] <= 0.0f) {
    return triangle[1];
  }
  if (baryWeights[0] <= 0.0f && baryWeights[1] <= 0.0f && baryWeights[2] >= 1.0f) {
    return triangle[2];
  }

  // 3) The third option is that the closest point is along the edge of the triangle.
  if (baryWeights[0] <= 0.0f) {
    // x is negative, so the closest point is along triangle's BC edge.
    return closestSegmentPoint(pt, triangle[1], triangle[2]);
  }
  if (baryWeights[1] <= 0.0f) {
    // y is negative, so the closest point is along triangle's AC edge.
    return closestSegmentPoint(pt, triangle[0], triangle[2]);
  }
  if (baryWeights[2] <= 0.0f) {
    // z is negative, so the closest point is along triangle's AB edge.
    return closestSegmentPoint(pt, triangle[0], triangle[1]);
  }

  // We should not be able to hit this point.
  return {};
}

bool isPointInTriangle(const HPoint3 &pt, const Vector<HPoint3> &triangle) {
  if (triangle.size() < 3) {
    return false;
  }

  const HPoint3 &a = triangle[0];
  const HPoint3 &b = triangle[1];
  const HPoint3 &c = triangle[2];

  HVector3 n = getFaceNormal(a, b, c);
  float d = -(a.x() * n.x() + a.y() * n.y() + a.z() * n.z());

  // test if the point is on the triangle plane
  float plane = pt.x() * n.x() + pt.y() * n.y() + pt.z() * n.z() + d;
  if (std::fabs(plane) > EPSILON) {
    return false;
  }

  // test if the point is inside the triangle
  auto weights = toBarycentricWeights(pt, triangle);

  return weights[0] >= 0.0f && weights[0] <= 1.0f && weights[1] >= 0.0f && weights[1] <= 1.0f
    && (weights[0] + weights[1]) <= 1.0f;
}

void computeMinMaxDistanceFromPoint(
  const HPoint3 &refPt,
  const Vector<HPoint3> &vertices,
  const Vector<int> &indices,
  float *minRadius,
  float *maxRadius) {
  *minRadius = std::numeric_limits<float>::max();
  *maxRadius = 0.0f;
  for (auto index : indices) {
    const HPoint3 &pt = vertices[index];
    float dx = pt.x() - refPt.x();
    float dy = pt.y() - refPt.y();
    float dz = pt.z() - refPt.z();
    float dist = sqrt(dx * dx + dy * dy + dz * dz);
    if (dist > *maxRadius) {
      *maxRadius = dist;
    }
    if (dist < *minRadius) {
      *minRadius = dist;
    }
  }
}

HVector3 computeAverageVertexNormals(const Vector<HVector3> &normals, const Vector<int> &indices) {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
  if (indices.empty()) {
    return {0.0f, 1.0f, 0.0f};
  }

  for (auto index : indices) {
    x += normals[index].x();
    y += normals[index].y();
    z += normals[index].z();
  }
  HVector3 averageN(x, y, z);
  if (averageN.l2Norm() <= 1.0e-5) {
    return {0.0f, 1.0f, 0.0f};
  }
  return averageN.unit();
}

}  // namespace c8
