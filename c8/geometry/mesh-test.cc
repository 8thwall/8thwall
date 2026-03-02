// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nathan Waters (nathan@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":mesh",
    "//c8:hmatrix",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x789db25f);

#include "c8/geometry/mesh.h"
#include "c8/hmatrix.h"
#include "c8/stats/scope-timer.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace c8 {

class MeshTest : public ::testing::Test {};

using testing::Eq;
using testing::Pointwise;

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsHPoint(const HPoint3 &vec) { return Pointwise(AreWithin(0.0001), vec.data()); }

/**
 * Test mesh computeVertexNormals with a cube
 */
TEST_F(MeshTest, TestComputeVertexNormalsWithCube) {
  ScopeTimer t("test-compute-vertex-normals-with-cube");

  // clang-format off
  Vector<HPoint3> boxVertices = {
    {0.5f, 0.5f, 0.5f},
    {0.5f, 0.5f, -0.5f},
    {0.5f, -0.5f, 0.5f},
    {0.5f, -0.5f, -0.5f},
    {-0.5f, 0.5f, -0.5f},
    {-0.5f, 0.5f, 0.5f},
    {-0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f, 0.5f},
  };

  const Vector<HVector3> expectedBoxVertexNormals = {
    {0.4082482f, 0.4082482f, 0.8164965f},
    {0.6666666f, 0.6666666f, -0.3333333f},
    {0.6666666f, -0.6666666f, 0.3333333f},
    {0.4082482f, -0.4082482f, -0.8164965f},
    {-0.4082482f, 0.4082482f, -0.8164965f},
    {-0.6666666f, 0.6666666f, 0.3333333f},
    {-0.6666666f, -0.6666666f, -0.3333333f},
    {-0.4082482f, -0.4082482f, 0.8164965f},
  };

  Vector<MeshIndices> boxIndices = {
    {0, 2, 1},
    {2, 3, 1},
    {4, 6, 5},
    {6, 7, 5},
    {4, 5, 1},
    {5, 0, 1},
    {7, 6, 2},
    {6, 3, 2},
    {5, 7, 0},
    {7, 2, 0},
    {1, 3, 4},
    {3, 6, 4},
  };
  // clang-format on

  Vector<HVector3> boxVertexNormals;
  computeVertexNormals(boxVertices, boxIndices, &boxVertexNormals);

  ASSERT_EQ(expectedBoxVertexNormals.size(), boxVertexNormals.size());

  for (int i = 0; i < expectedBoxVertexNormals.size(); ++i) {
    EXPECT_FLOAT_EQ(expectedBoxVertexNormals[i].x(), boxVertexNormals[i].x());
    EXPECT_FLOAT_EQ(expectedBoxVertexNormals[i].y(), boxVertexNormals[i].y());
    EXPECT_FLOAT_EQ(expectedBoxVertexNormals[i].z(), boxVertexNormals[i].z());
  }
}

/**
 * Test mesh computeVertexNormals with a cone
 */
TEST_F(MeshTest, TestComputeVertexNormalsWithCone) {
  ScopeTimer t("text-compute-vertex-normals-with-cone");

  // clang-format off
  Vector<HPoint3> coneVertices = {
    {0.0f, 5.0f, 0.0f},
    {0.0f, -5.0f, 5.0f},
    {3.5355339f, -5.0f, 3.5355339f},
    {5.0f, -5.0f, 0.0f},
    {3.5355339f, -5.0f, -3.5355339f},
    {0.0f, -5.0f, -5.0f},
    {-3.5355339f, -5.0f, -3.5355339f},
    {-5.0f, -5.0f, 0.0f},
    {-3.5355339f, -5.0f, 3.5355339f},
    {0.0f, -5.0f, 0.0f},
  };

  const Vector<HVector3> expectedConeVertexNormals = {
    {0.f, 1.0f, 0.0f},
    {0.f, 0.f, 1.0f},
    {0.7071067f, 0.0f, 0.7071067f},
    {1.0f, 0.0f, 0.0f},
    {0.7071067f, 0.0f, -0.7071067f},
    {0.f, 0.0f, -1.0f},
    {-0.7071067f, 0.0f, -0.70710677f},
    {-1.0f, 0.f, 0.0f},
    {-0.7071067f, 0.0f, 0.7071067f},
    {0.f, -1.0f, 0.0f}
  };

  Vector<MeshIndices> coneIndices = {
    {1, 2, 0},
    {2, 3, 0},
    {3, 4, 0},
    {4, 5, 0},
    {5, 6, 0},
    {6, 7, 0},
    {7, 8, 0},
    {8, 1, 0},
    {2, 1, 9},
    {3, 2, 9},
    {4, 3, 9},
    {5, 4, 9},
    {6, 5, 9},
    {7, 6, 9},
    {8, 7, 9},
    {1, 8, 9},
  };
  // clang-format on

  Vector<HVector3> coneVertexNormals;
  computeVertexNormals(coneVertices, coneIndices, &coneVertexNormals);

  ASSERT_EQ(expectedConeVertexNormals.size(), coneVertexNormals.size());

  for (int i = 0; i < expectedConeVertexNormals.size(); ++i) {
    EXPECT_FLOAT_EQ(expectedConeVertexNormals[i].x(), coneVertexNormals[i].x());
    EXPECT_FLOAT_EQ(expectedConeVertexNormals[i].y(), coneVertexNormals[i].y());
    EXPECT_FLOAT_EQ(expectedConeVertexNormals[i].z(), coneVertexNormals[i].z());
  }
}

/**
 * Test getFaceNormalUnit across z axis
 */
TEST_F(MeshTest, TestGetFaceNormalUnitZ) {

  const HPoint3 a(0.0f, 0.0f, 0.0f);
  const HPoint3 b(2.0f, 2.0f, 0.0f);
  const HPoint3 c(4.0f, 0.0f, 0.0f);

  HVector3 normal = getFaceNormalUnit(a, b, c);
  const HPoint3 clockwiseNormal(0.0f, 0.0f, -1.0f);

  EXPECT_FLOAT_EQ(clockwiseNormal.x(), normal.x());
  EXPECT_FLOAT_EQ(clockwiseNormal.y(), normal.y());
  EXPECT_FLOAT_EQ(clockwiseNormal.z(), normal.z());

  normal = getFaceNormalUnit(a, c, b);
  const HPoint3 counterClockwiseNormal(0.0f, 0.0f, 1.0f);

  EXPECT_FLOAT_EQ(counterClockwiseNormal.x(), normal.x());
  EXPECT_FLOAT_EQ(counterClockwiseNormal.y(), normal.y());
  EXPECT_FLOAT_EQ(counterClockwiseNormal.z(), normal.z());
}

/**
 * Test getFaceNormalUnit across y axis
 */
TEST_F(MeshTest, TestGetFaceNormalUnitY) {

  const HPoint3 a(0.0f, 0.0f, 0.0f);
  const HPoint3 b(2.0f, 0.0f, 2.0f);
  const HPoint3 c(4.0f, 0.0f, 0.0f);

  HVector3 normal = getFaceNormalUnit(a, b, c);
  const HPoint3 clockwiseNormal(0.0f, 1.0f, 0.0f);

  EXPECT_FLOAT_EQ(clockwiseNormal.x(), normal.x());
  EXPECT_FLOAT_EQ(clockwiseNormal.y(), normal.y());
  EXPECT_FLOAT_EQ(clockwiseNormal.z(), normal.z());

  normal = getFaceNormalUnit(a, c, b);
  const HPoint3 counterClockwiseNormal(0.0f, -1.0f, 0.0f);

  EXPECT_FLOAT_EQ(counterClockwiseNormal.x(), normal.x());
  EXPECT_FLOAT_EQ(counterClockwiseNormal.y(), normal.y());
  EXPECT_FLOAT_EQ(counterClockwiseNormal.z(), normal.z());
}

/**
 * Test getFaceNormalUnit across x axis
 */
TEST_F(MeshTest, TestGetFaceNormalUnitX) {

  const HPoint3 a(0.0f, 0.0f, 0.0f);
  const HPoint3 b(0.0f, 2.0f, 2.0f);
  const HPoint3 c(0.0f, 0.0f, 4.0f);

  HVector3 normal = getFaceNormalUnit(a, b, c);
  const HPoint3 clockwiseNormal(1.0f, 0.0f, 0.0f);

  EXPECT_FLOAT_EQ(clockwiseNormal.x(), normal.x());
  EXPECT_FLOAT_EQ(clockwiseNormal.y(), normal.y());
  EXPECT_FLOAT_EQ(clockwiseNormal.z(), normal.z());

  normal = getFaceNormalUnit(a, c, b);
  const HPoint3 counterClockwiseNormal(-1.0f, 0.0f, 0.0f);

  EXPECT_FLOAT_EQ(counterClockwiseNormal.x(), normal.x());
  EXPECT_FLOAT_EQ(counterClockwiseNormal.y(), normal.y());
  EXPECT_FLOAT_EQ(counterClockwiseNormal.z(), normal.z());
}

/**
 * Test getFaceNormalUnit off-axis
 */
TEST_F(MeshTest, TestGetFaceNormalUnitOffAxis) {

  const HPoint3 a(-1.0f, 0.5f, 2.0f);
  const HPoint3 b(1.0f, 3.0f, 2.0f);
  const HPoint3 c(4.0f, -1.0f, 3.0f);

  HVector3 normal = getFaceNormalUnit(a, b, c);
  const HPoint3 clockwiseNormal(0.157956f, -0.12636481f, -0.97932726f);

  EXPECT_FLOAT_EQ(clockwiseNormal.x(), normal.x());
  EXPECT_FLOAT_EQ(clockwiseNormal.y(), normal.y());
  EXPECT_FLOAT_EQ(clockwiseNormal.z(), normal.z());

  normal = getFaceNormalUnit(a, c, b);
  const HPoint3 counterClockwiseNormal(-0.157956f, 0.12636481f, 0.97932726f);

  EXPECT_FLOAT_EQ(counterClockwiseNormal.x(), normal.x());
  EXPECT_FLOAT_EQ(counterClockwiseNormal.y(), normal.y());
  EXPECT_FLOAT_EQ(counterClockwiseNormal.z(), normal.z());
}

/**
 * Test getFaceNormal across z axis
 */
TEST_F(MeshTest, TestGetFaceNormalZ) {

  const HPoint3 a(0.0f, 0.0f, 0.0f);
  const HPoint3 b(2.0f, 2.0f, 0.0f);
  const HPoint3 c(4.0f, 0.0f, 0.0f);

  HVector3 normal = getFaceNormal(a, b, c);
  const HPoint3 clockwiseNormal(0.0f, 0.0f, -8.0f);

  EXPECT_FLOAT_EQ(clockwiseNormal.x(), normal.x());
  EXPECT_FLOAT_EQ(clockwiseNormal.y(), normal.y());
  EXPECT_FLOAT_EQ(clockwiseNormal.z(), normal.z());

  normal = getFaceNormal(a, c, b);
  const HPoint3 counterClockwiseNormal(0.0f, 0.0f, 8.0f);

  EXPECT_FLOAT_EQ(counterClockwiseNormal.x(), normal.x());
  EXPECT_FLOAT_EQ(counterClockwiseNormal.y(), normal.y());
  EXPECT_FLOAT_EQ(counterClockwiseNormal.z(), normal.z());
}

/**
 * Test getFaceNormal across y axis
 */
TEST_F(MeshTest, TestGetFaceNormalY) {

  const HPoint3 a(0.0f, 0.0f, 0.0f);
  const HPoint3 b(2.0f, 0.0f, 2.0f);
  const HPoint3 c(4.0f, 0.0f, 0.0f);

  HVector3 normal = getFaceNormal(a, b, c);
  const HPoint3 clockwiseNormal(0.0f, 8.0f, 0.0f);

  EXPECT_FLOAT_EQ(clockwiseNormal.x(), normal.x());
  EXPECT_FLOAT_EQ(clockwiseNormal.y(), normal.y());
  EXPECT_FLOAT_EQ(clockwiseNormal.z(), normal.z());

  normal = getFaceNormal(a, c, b);
  const HPoint3 counterClockwiseNormal(0.0f, -8.0f, 0.0f);

  EXPECT_FLOAT_EQ(counterClockwiseNormal.x(), normal.x());
  EXPECT_FLOAT_EQ(counterClockwiseNormal.y(), normal.y());
  EXPECT_FLOAT_EQ(counterClockwiseNormal.z(), normal.z());
}

/**
 * Test getFaceNormal across x axis
 */
TEST_F(MeshTest, TestGetFaceNormalX) {

  const HPoint3 a(0.0f, 0.0f, 0.0f);
  const HPoint3 b(0.0f, 2.0f, 2.0f);
  const HPoint3 c(0.0f, 0.0f, 4.0f);

  HVector3 normal = getFaceNormal(a, b, c);
  const HPoint3 clockwiseNormal(8.0f, 0.0f, 0.0f);

  EXPECT_FLOAT_EQ(clockwiseNormal.x(), normal.x());
  EXPECT_FLOAT_EQ(clockwiseNormal.y(), normal.y());
  EXPECT_FLOAT_EQ(clockwiseNormal.z(), normal.z());

  normal = getFaceNormal(a, c, b);
  const HPoint3 counterClockwiseNormal(-8.0f, 0.0f, 0.0f);

  EXPECT_FLOAT_EQ(counterClockwiseNormal.x(), normal.x());
  EXPECT_FLOAT_EQ(counterClockwiseNormal.y(), normal.y());
  EXPECT_FLOAT_EQ(counterClockwiseNormal.z(), normal.z());
}

/**
 * Test getFaceNormal off-axis
 */
TEST_F(MeshTest, TestGetFaceNormalOffAxis) {

  const HPoint3 a(-1.0f, 0.5f, 2.0f);
  const HPoint3 b(1.0f, 3.0f, 2.0f);
  const HPoint3 c(4.0f, -1.0f, 3.0f);

  HVector3 normal = getFaceNormal(a, b, c);
  const HPoint3 clockwiseNormal(2.5f, -2.0f, -15.5f);

  EXPECT_FLOAT_EQ(clockwiseNormal.x(), normal.x());
  EXPECT_FLOAT_EQ(clockwiseNormal.y(), normal.y());
  EXPECT_FLOAT_EQ(clockwiseNormal.z(), normal.z());

  normal = getFaceNormal(a, c, b);
  const HPoint3 counterClockwiseNormal(-2.5f, 2.0f, 15.5f);

  EXPECT_FLOAT_EQ(counterClockwiseNormal.x(), normal.x());
  EXPECT_FLOAT_EQ(counterClockwiseNormal.y(), normal.y());
  EXPECT_FLOAT_EQ(counterClockwiseNormal.z(), normal.z());
}

/**
 * Test computeCentroid on a triangle
 */
TEST_F(MeshTest, TestComputeCentroidTriangle) {

  // clang-format off
  Vector<HPoint3> vertices = {
    {0.0f, 0.0f, 0.0f},
    {1.0f, 3.0f, 0.0f},
    {2.0f, 0.0f, 0.0f},
  };
  // clang-format on

  HPoint3 expected(1.0f, 1.0f, 0.0f);
  HPoint3 actual = computeCentroid(vertices);
  EXPECT_FLOAT_EQ(expected.x(), actual.x());
  EXPECT_FLOAT_EQ(expected.y(), actual.y());
  EXPECT_FLOAT_EQ(expected.z(), actual.z());
}

/**
 * Test computeCentroid on a list of vertices
 */
TEST_F(MeshTest, TestComputeCentroidVertList) {

  // clang-format off
  Vector<HPoint3> vertices = {
    {0.0f, 0.0f, 0.0f},
    {1.0f, 3.0f, 0.0f},
    {2.0f, 0.0f, 0.0f},
  };
  // clang-format on
  Vector<int> indices = {0, 1, 2};

  HPoint3 expected(1.0f, 1.0f, 0.0f);
  HPoint3 actual = computeCentroid(vertices, indices);
  EXPECT_FLOAT_EQ(expected.x(), actual.x());
  EXPECT_FLOAT_EQ(expected.y(), actual.y());
  EXPECT_FLOAT_EQ(expected.z(), actual.z());
}

/**
 * Test computeCentroid on a box
 */
TEST_F(MeshTest, TestComputeCentroidBox) {

  // clang-format off
  Vector<HPoint3> vertices = {
    {0.5f, 0.5f, 0.5f},    // 0 - back top right
    {0.5f, 0.5f, -0.5f},   // 1 - front top right
    {0.5f, -0.5f, 0.5f},   // 2 - back bottom right
    {0.5f, -0.5f, -0.5f},  // 3 - front bottom right
    {-0.5f, 0.5f, -0.5f},  // 4 - front top left
    {-0.5f, 0.5f, 0.5f},   // 5 - back top left
    {-0.5f, -0.5f, -0.5f}, // 6 - front bottom left
    {-0.5f, -0.5f, 0.5f},  // 7 - back bottom left
  };
  // clang-format on

  HPoint3 expected(0.0f, 0.0f, 0.0f);
  HPoint3 actual = computeCentroid(vertices);
  EXPECT_FLOAT_EQ(expected.x(), actual.x());
  EXPECT_FLOAT_EQ(expected.y(), actual.y());
  EXPECT_FLOAT_EQ(expected.z(), actual.z());

  // translate the box
  HMatrix translationMatrix = HMatrixGen::translation(2.0f, 3.0f, -2.0f);

  for (int i = 0; i < vertices.size(); i++) {
    vertices[i] = translationMatrix * vertices[i];
  }

  HPoint3 expectedTranslated(2.0f, 3.0f, -2.0f);
  HPoint3 actualTranslated = computeCentroid(vertices);
  EXPECT_FLOAT_EQ(expectedTranslated.x(), actualTranslated.x());
  EXPECT_FLOAT_EQ(expectedTranslated.y(), actualTranslated.y());
  EXPECT_FLOAT_EQ(expectedTranslated.z(), actualTranslated.z());
}

TEST_F(MeshTest, TestNormalMatrix) {

  Vector<HPoint3> pts = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};

  HVector3 normal = getFaceNormalUnit(pts[0], pts[1], pts[2]);
  HVector3 clockwiseNormal(0.57735026919f, 0.57735026919f, 0.57735026919f);

  EXPECT_FLOAT_EQ(clockwiseNormal.x(), normal.x());
  EXPECT_FLOAT_EQ(clockwiseNormal.y(), normal.y());
  EXPECT_FLOAT_EQ(clockwiseNormal.z(), normal.z());

  auto trs = HMatrixGen::translation(3, -4, 5) * HMatrixGen::rotationD(-30, 20, 15)
    * HMatrixGen::scale(0.5f, 3.0f, -1.0f);

  auto rpt = trs * pts;

  // Analytical solution for rotated normal. We need to change the winding order because
  // the of the negative z scale.
  auto expectedNormal = getFaceNormalUnit(rpt[1], rpt[0], rpt[2]);
  auto computedNormal = (normalMatrix(trs) * normal).unit();  // make sure to normalize.

  EXPECT_NEAR(expectedNormal.x(), computedNormal.x(), 2e-7f);
  EXPECT_NEAR(expectedNormal.y(), computedNormal.y(), 2e-7f);
  EXPECT_NEAR(expectedNormal.z(), computedNormal.z(), 2e-7f);
}

TEST_F(MeshTest, TestProjectPointToPlane) {
  // This triangle is flat on the x,z plane, facing upwards. Points are clockwise in left-handed
  // coordinate systems.
  Vector<HPoint3> triangle = {
    {-1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 0.0f},
  };

  // This point floats right above the bottom line of the triangle, distance 1.
  HPoint3 pt = {0.0f, 3.0f, 0.0f};
  HPoint3 expectedProjection = {0.0f, 0.0f, 0.0f};
  EXPECT_THAT(projectPointToPlane(pt, triangle).data(), equalsHPoint(expectedProjection));

  pt = {0.0f, 9.0f, 1.0f};
  expectedProjection = {0.0f, 0.0f, 1.0f};
  EXPECT_THAT(projectPointToPlane(pt, triangle).data(), equalsHPoint(expectedProjection));

  pt = {0.0f, -100.0f, 1.0f};
  expectedProjection = {0.0f, 0.0f, 1.0f};
  EXPECT_THAT(projectPointToPlane(pt, triangle).data(), equalsHPoint(expectedProjection));

  // This triangle is flat on the y,z plane, facing the positive x axis.
  triangle = {
    {3.0f, 1.0f, 0.0f},
    {3.0f, 0.0f, 1.0f},
    {3.0f, -1.0f, 0.0f},
  };

  pt = {5.0f, 1.0f, 1.0f};
  expectedProjection = {3.0f, 1.0f, 1.0f};
  EXPECT_THAT(projectPointToPlane(pt, triangle).data(), equalsHPoint(expectedProjection));

  pt = {-25.0f, 25.0f, 25.0f};
  expectedProjection = {3.0f, 25.0f, 25.0f};
  EXPECT_THAT(projectPointToPlane(pt, triangle).data(), equalsHPoint(expectedProjection));
}

// Helper function to test cartisian to barycentric point conversions for various triangles
void testToBarycentricWeights(const Vector<HPoint3> &triangle) {
  // The centroid of a triangle should be {1/3, 1/3, 1/3}
  auto centroid = computeCentroid(triangle);
  HPoint3 baryCoord = {0.333333f, 0.333333f, 0.333333f};
  EXPECT_THAT(toBarycentricWeights(centroid, triangle), equalsHPoint(baryCoord));

  // Using one of the vertices of the triangle should give that corner full weight.
  EXPECT_THAT(toBarycentricWeights(triangle[0], triangle), equalsHPoint({1.0f, 0.0f, 0.0f}));
  EXPECT_THAT(toBarycentricWeights(triangle[1], triangle), equalsHPoint({0.0f, 1.0f, 0.0f}));
  EXPECT_THAT(toBarycentricWeights(triangle[2], triangle), equalsHPoint({0.0f, 0.0f, 1.0f}));

  // Should be midway on the line between two points
  HPoint3 midway = {
    (triangle[0].x() + triangle[1].x()) / 2.0f,
    (triangle[0].y() + triangle[1].y()) / 2.0f,
    (triangle[0].z() + triangle[1].z()) / 2.0f};
  EXPECT_THAT(toBarycentricWeights(midway, triangle), equalsHPoint({0.5f, 0.5f, 0.0f}));
}

TEST_F(MeshTest, TestToBarycentricWeights) {
  // This triangle is flat on the x,z plane, facing upwards. Points are clockwise in left-handed
  // coordinate systems.
  Vector<HPoint3> triangle = {
    {-1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 0.0f},
  };
  testToBarycentricWeights(triangle);

  // This triangle is flat on the y,z plane, facing the positive x axis.
  triangle = {
    {3.0f, 1.0f, 0.0f},
    {3.0f, 0.0f, 1.0f},
    {3.0f, -1.0f, 0.0f},
  };
  testToBarycentricWeights(triangle);

  // This triangle is flat on the x,y plane, facing the positive z axis.
  triangle = {
    {3.0f, 0.0f, -2.0f},
    {0.0f, 2.0f, -2.0f},
    {-1.0f, 0.0f, -2.0f},
  };
  testToBarycentricWeights(triangle);

  // Test outside of the boundary of the triangle.  These are still valid barycentric coordinates.
  // This triangle is flat on the x,z plane, facing upwards.
  triangle = {
    {-1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 0.0f},
  };
  HPoint3 pt = {-2.0f, 0.0f, 0.0f};
  EXPECT_THAT(toBarycentricWeights(pt, triangle), equalsHPoint({1.5f, 0.0f, -0.5f}));
  pt = {-2.0f, 0.0f, 2.0f};
  EXPECT_THAT(toBarycentricWeights(pt, triangle), equalsHPoint({0.5f, 2.0f, -1.5f}));
}

TEST_F(MeshTest, TestClosestTrianglePoint) {
  // This triangle is flat on the x,z plane, facing upwards. Points are clockwise in left-handed
  // coordinate systems.
  Vector<HPoint3> triangle = {
    {-1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 0.0f},
  };

  // Test cases where the projected point is inside the triangle.
  EXPECT_THAT(
    closestTrianglePoint({0.0f, 2.0f, 0.0f}, triangle).data(), equalsHPoint({0.0f, 0.0f, 0.0f}));
  EXPECT_THAT(
    closestTrianglePoint({0.0f, 4.0f, 1.0f}, triangle).data(), equalsHPoint({0.0f, 0.0f, 1.0f}));
  EXPECT_THAT(
    closestTrianglePoint({0.0f, -6.0f, 1.0f}, triangle).data(), equalsHPoint({0.0f, 0.0f, 1.0f}));

  // Test cases where the closest point on the triangle should be one of the triangle's vertices.
  EXPECT_THAT(
    closestTrianglePoint({-2.0f, 0.0f, 0.0f}, triangle).data(), equalsHPoint({-1.0f, 0.0f, 0.0f}));
  EXPECT_THAT(
    closestTrianglePoint({0.0f, 0.0f, 5.0f}, triangle).data(), equalsHPoint({0.0f, 0.0f, 1.0f}));
  EXPECT_THAT(
    closestTrianglePoint({4.0f, 4.0f, 0.0f}, triangle).data(), equalsHPoint({1.0f, 0.0f, 0.0f}));

  // Test cases where the closest point on the triangle should be one of the triangle's edges.
  EXPECT_THAT(
    closestTrianglePoint({1.0f, 0.0f, 1.0f}, triangle).data(), equalsHPoint({0.5f, 0.0f, 0.5f}));
  EXPECT_THAT(
    closestTrianglePoint({-1.0f, 0.0f, 1.0f}, triangle).data(), equalsHPoint({-0.5f, 0.0f, 0.5f}));
  EXPECT_THAT(
    closestTrianglePoint({1.0f, 1.0f, 1.0f}, triangle).data(), equalsHPoint({0.5f, 0.0f, 0.5f}));
  EXPECT_THAT(
    closestTrianglePoint({-1.0f, 1.0f, 1.0f}, triangle).data(), equalsHPoint({-0.5f, 0.0f, 0.5f}));
  EXPECT_THAT(
    closestTrianglePoint({1.0f, -1.0f, 1.0f}, triangle).data(), equalsHPoint({0.5f, 0.0f, 0.5f}));
  EXPECT_THAT(
    closestTrianglePoint({-1.0f, -1.0f, 1.0f}, triangle).data(), equalsHPoint({-0.5f, 0.0f, 0.5f}));
}

TEST_F(MeshTest, TestClosestSegmentPoint) {
  // Test when point projects onto line segment
  HPoint3 start = {-1.0f, 0.0f, 0.0f};
  HPoint3 end = {1.0f, 0.0f, 0.0f};
  EXPECT_THAT(
    closestSegmentPoint({0.0f, 1.0f, 0.0f}, start, end).data(), equalsHPoint({0.0f, 0.0f, 0.0f}));
  EXPECT_THAT(
    closestSegmentPoint({0.5f, -10.0f, 10.0f}, start, end).data(),
    equalsHPoint({0.5f, 0.0f, 0.0f}));

  // Test when point projection is beyond line segment
  EXPECT_THAT(closestSegmentPoint({-2.0f, 0.0f, 0.0f}, start, end).data(), equalsHPoint(start));
  EXPECT_THAT(closestSegmentPoint({-5.0f, 3.0f, 0.0f}, start, end).data(), equalsHPoint(start));
  EXPECT_THAT(closestSegmentPoint({5.0f, -3.0f, 0.0f}, start, end).data(), equalsHPoint(end));
  EXPECT_THAT(closestSegmentPoint({9.0f, 8.0f, -10.0f}, start, end).data(), equalsHPoint(end));

  // Test on another line
  start = {-2.0f, -1.0f, 0.0f};
  end = {-2.0f, 1.0f, 0.0f};
  EXPECT_THAT(
    closestSegmentPoint({3.0f, 0.0f, 0.0f}, start, end).data(), equalsHPoint({-2.0f, 0.0f, 0.0f}));
  EXPECT_THAT(
    closestSegmentPoint({-3.0f, 1.0f, 0.0f}, start, end).data(), equalsHPoint({-2.0f, 1.0f, 0.0f}));
}

TEST_F(MeshTest, TestIsPointInTriangle) {
  Vector<HPoint3> triangle = {
    HPoint3{0.0f, 0.0f, 1.0f}, HPoint3{1.0f, 1.0f, 0.0f}, HPoint3{0.0f, 0.0f, -1.0f}};
  HPoint3 p1 = {0.5f, 0.5f, 0.0f};
  bool isP1 = isPointInTriangle(p1, triangle);
  EXPECT_TRUE(isP1);
  HPoint3 p2 = {0.5f, 0.0f, 0.0f};
  bool isP2 = isPointInTriangle(p2, triangle);
  EXPECT_FALSE(isP2);
  HPoint3 p3 = {0.0f, 0.0f, 2.0f};
  bool isP3 = isPointInTriangle(p3, triangle);
  EXPECT_FALSE(isP3);
}

TEST_F(MeshTest, TestMinMaxDistanceFromPoint) {
  HPoint3 origin(0.0f, 0.0f, 0.0f);
  Vector<HPoint3> verts = {
    HPoint3{0.0f, 0.0f, 1.0f},
    HPoint3{1.0f, 1.0f, 0.0f},
    HPoint3{0.0f, 0.0f, -1.0f},
    HPoint3{0.0f, -1.0f, 0.0f}};
  Vector<int> indices = {0, 1, 2, 3};
  float minDist, maxDist;
  computeMinMaxDistanceFromPoint(origin, verts, indices, &minDist, &maxDist);
  EXPECT_FLOAT_EQ(minDist, 1.0f);
  EXPECT_FLOAT_EQ(maxDist, 1.4142135f);
}

TEST_F(MeshTest, TestAverageVetexNormal) {
  Vector<HVector3> normals = {
    {-1.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
  };
  Vector<int> indices = {0, 1, 2};
  HVector3 normal = computeAverageVertexNormals(normals, indices);
  EXPECT_FLOAT_EQ(normal.x(), 0.0f);
  EXPECT_FLOAT_EQ(normal.y(), 1.0f);
  EXPECT_FLOAT_EQ(normal.z(), 0.0f);
}

}  // namespace c8
