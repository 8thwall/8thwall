// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nathan Waters (nathan@8thwall.com)

#pragma once

#include "c8/geometry/mesh-types.h"
#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/vector.h"

namespace c8 {

// Computes a matrix that can be used to transform normals from a model into view space, accounting
// for shifts in scale.
HMatrix normalMatrix(const HMatrix &modelViewMatrix);

// Get the normal vector to a clockwise triangle defined by vertices a, b, and c. The magnitude of
// the vector will be twice the triangle's area. Use getFaceNormalUnit() if a unit vector is
// required.
HVector3 getFaceNormal(const HPoint3 &a, const HPoint3 &b, const HPoint3 &c);

// Get the unit normal vector to a clockwise triangle defined by vertices a, b, and c.
HVector3 getFaceNormalUnit(const HPoint3 &a, const HPoint3 &b, const HPoint3 &c);

// Given three points of a triangle, return the un-normalized normal.  This accepts HVector3
// because of the 20% speed improvement over HPoint3 due the division by w component when accessing
// the elements of the point
HVector3 getFaceNormalVector(const HVector3 &a, const HVector3 &b, const HVector3 &c);

// Converts indices A,B,C to A,C,B.
Vector<MeshIndices> rotateIndices(const Vector<MeshIndices> &indices);

// Mirrors {u, v} to -
//   if mirrorY is false, mirrors to {1.0f - u, v}
//   if mirrorY is true, mirrors to {u, 1.0f - v}
Vector<HPoint2> mirrorUvs(const Vector<HPoint2> &uvs, bool mirrorY = false);

// Compute the face normals.  Rough shading.  Smoother shading is provided by computeVertexNormals
void computeFaceNormals(
  const Vector<HPoint3> &vertices,
  const Vector<MeshIndices> &indices,
  Vector<HVector3> *faceNormals);

// Mesh(Vector<HPoint3> vertices, Vector<int> indices, Vector<UV> uvs);
void computeVertexNormals(
  const Vector<HPoint3> &vertices,
  const Vector<MeshIndices> &indices,
  Vector<HVector3> *vertexNormals);

// Given a list of vertices it will compute the centroid of the vertices
HPoint3 computeCentroid(const Vector<HPoint3> &vertices);

// Given a list of vertices and subset vertex indices,
// it will compute the centroid of the subset vertices
HPoint3 computeCentroid(const Vector<HPoint3> &vertices, const Vector<int> &indices);

// Takes in points and centers them by placing the centroid at the origin
void centerCentroid(Vector<HPoint3> *verticesPtr);

// Returns the barycentric coordinates of a given point that lies on the plane of a triangle.
// Barycentric points are weighted averages of the triangle's three vertices.  They are useful for
// specifying points along the surface of a triangle.  The conversion from barycentric weights (b)
// to world space, where v1 is the triangle's first vertex and b1 is the barycentric weight's first
// component, is defined by b1 * v1 + b2 * v2 + b3 * v3
// For example, the barycentric weight of:
// - (1/3, 1/3, 1/3) is the centroid of the triangle.
// - (1, 0, 0) is at the triangle's first vertex.
// - (0, 0, 1) is at the triangle's third vertex.
// - (0, 0.5, 0.5) is midway along the edge between the triangle's second and third vertex.
// The provided point must lie on the plane of the triangle, otherwise the behavior is undefined.
// You can project a point onto the triangle's plane using projectPointToPlane().
std::array<float, 3> toBarycentricWeights(const HPoint3 &pt, const Vector<HPoint3> &triangle);

// Returns the location of a point projected on a plane.  This is also the closest point on the
// plane to the given point.
HPoint3 projectPointToPlane(const HPoint3 &pt, const Vector<HPoint3> &pointsOnPlane);

// Returns the closest point on a line segment to a given point.
HPoint3 closestSegmentPoint(const HPoint3 &pt, const HPoint3 &start, const HPoint3 &end);

// Returns the closest point on the triangle to a given point.
HPoint3 closestTrianglePoint(const HPoint3 &pt, const Vector<HPoint3> &triangle);

// Return true if the point pt is inside the triangle.
// This algorithm first tests if pt is on the triangle plane.
// If on triangle plane, we will compute the barycentric coordinates for pt.
// pt = alpha * A + beta * B + gamma * C
// where alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && alpha+beta < 1.
bool isPointInTriangle(const HPoint3 &pt, const Vector<HPoint3> &triangle);

// compute the min/max distance between refPt and a subset of vertices
void computeMinMaxDistanceFromPoint(
  const HPoint3 &refPt,
  const Vector<HPoint3> &vertices,
  const Vector<int> &indices,
  float *minRadius,
  float *maxRadius);

// average the normals of the subset of vertices, assume all the normals are units.
// If the average normal is 0, return a (0, 1, 0) normal.
HVector3 computeAverageVertexNormals(const Vector<HVector3> &normals, const Vector<int> &indices);

}  // namespace c8
