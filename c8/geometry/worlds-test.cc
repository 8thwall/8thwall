// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "//c8:vector",
    "//c8:hpoint",
    "//c8/geometry:facemesh-data",
    "//c8/geometry:mesh",
    "//c8/geometry:worlds",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x3a288160);

#include "c8/geometry/facemesh-data.h"
#include "c8/geometry/mesh.h"
#include "c8/geometry/worlds.h"
#include "c8/hpoint.h"
#include "c8/stats/scope-timer.h"
#include "c8/vector.h"
#include "gtest/gtest.h"

namespace c8 {

class WorldsTest : public ::testing::Test {};

TEST_F(WorldsTest, TestAxisAlignedPolygonsWorld) {
  Vector<HPoint3> world = c8::Worlds::axisAlignedPolygonsWorld();
  EXPECT_EQ(15, world.size());
}

TEST_F(WorldsTest, TestAxisAlignedGridWorld) {
  Vector<HPoint3> world = c8::Worlds::axisAlignedGridWorld();
  EXPECT_EQ(25, world.size());
}

/*
 * Blazeface and Facemesh Tests
 */

// Assert the interpupilliary distance is what we expect
TEST_F(WorldsTest, TestSparseInterpupilliaryBreadth) {
  auto sparsePoints = c8::Worlds::sparseFaceMeshWorld();
  float interPupilliaryBreadth =
    sparsePoints[BLAZEFACE_R_EYE].x() - sparsePoints[BLAZEFACE_L_EYE].x();
  EXPECT_FLOAT_EQ(interPupilliaryBreadth, INTERPUPILLIARY_WIDTH);
}

TEST_F(WorldsTest, TestDenseInterpupilliaryBreadth) {
  auto densePoints = c8::Worlds::denseFaceMeshWorld();
  float rEyeX = densePoints[FACEMESH_UPPER_R_EYE].x();
  float lEyeX = densePoints[FACEMESH_UPPER_L_EYE].x();

  float interPupilliaryBreadth = lEyeX - rEyeX;
  EXPECT_NEAR(interPupilliaryBreadth, INTERPUPILLIARY_WIDTH, 0.003f);
}

// Assert the width of the face is what we expect
TEST_F(WorldsTest, TestSparseFaceBreadth) {
  auto sparsePoints = c8::Worlds::sparseFaceMeshWorld();
  float faceBreadth = sparsePoints[BLAZEFACE_R_EAR].x() - sparsePoints[BLAZEFACE_L_EAR].x();
  EXPECT_FLOAT_EQ(faceBreadth, FACE_WIDTH);
}

TEST_F(WorldsTest, TestDenseFaceBreadth) {
  auto densePoints = c8::Worlds::denseFaceMeshWorld();
  float faceBreadth = densePoints[FACEMESH_L_EAR].x() - densePoints[FACEMESH_R_EAR].x();
  EXPECT_NEAR(faceBreadth, FACE_WIDTH, 0.016f);
}

// Assert the face is facing forward
TEST_F(WorldsTest, TestSparseZOrdering) {
  auto sparsePoints = c8::Worlds::sparseFaceMeshWorld();
  EXPECT_LT(sparsePoints[BLAZEFACE_L_EAR].z(), sparsePoints[BLAZEFACE_L_EYE].z());
  EXPECT_LT(sparsePoints[BLAZEFACE_L_EYE].z(), sparsePoints[BLAZEFACE_NOSE].z());
}

TEST_F(WorldsTest, TestDenseZOrdering) {
  auto densePoints = c8::Worlds::denseFaceMeshWorld();
  EXPECT_LT(densePoints[FACEMESH_L_EAR].z(), densePoints[FACEMESH_UPPER_L_EYE].z());
  EXPECT_LT(densePoints[FACEMESH_UPPER_L_EYE].z(), densePoints[FACEMESH_NOSE].z());
}

// Assert the face is not tilted on the z axis by comparing two points that should be on the same
// vertical line
TEST_F(WorldsTest, TestSparseZOrientation) {
  auto sparsePoints = c8::Worlds::sparseFaceMeshWorld();
  EXPECT_FLOAT_EQ(sparsePoints[BLAZEFACE_NOSE].x() - sparsePoints[BLAZEFACE_MOUTH].x(), 0.0f);
}

TEST_F(WorldsTest, TestDenseZOrientation) {
  auto densePoints = c8::Worlds::denseFaceMeshWorld();
  float xDifference =
    densePoints[FACEMESH_NOSE].x() - densePoints[FACEMESH_MOUTH_BOTTOM_MIDDLE].x();
  EXPECT_NEAR(xDifference, 0.0f, 0.002f);
}

// Assert the face is facing directly forward
TEST_F(WorldsTest, TestSparseYOrientation) {
  auto sparsePoints = c8::Worlds::sparseFaceMeshWorld();
  EXPECT_FLOAT_EQ(sparsePoints[BLAZEFACE_NOSE].x(), 0.0f);
  EXPECT_FLOAT_EQ(sparsePoints[BLAZEFACE_MOUTH].x(), 0.0f);
}

TEST_F(WorldsTest, TestDenseYOrientation) {
  auto densePoints = c8::Worlds::denseFaceMeshWorld();
  EXPECT_NEAR(densePoints[FACEMESH_NOSE].x(), 0.0f, 0.002f);
  EXPECT_NEAR(densePoints[FACEMESH_MOUTH_BOTTOM_MIDDLE].x(), 0.0f, 0.002f);
}

// Assert the eyes are level
TEST_F(WorldsTest, TestSparseEyesLevel) {
  auto sparsePoints = c8::Worlds::sparseFaceMeshWorld();
  EXPECT_FLOAT_EQ(sparsePoints[BLAZEFACE_L_EYE].y() - sparsePoints[BLAZEFACE_R_EYE].y(), 0.0f);
}

TEST_F(WorldsTest, TestDenseEyesLevel) {
  auto densePoints = c8::Worlds::denseFaceMeshWorld();
  float yDifference = densePoints[FACEMESH_UPPER_L_EYE].y() - densePoints[FACEMESH_UPPER_R_EYE].y();
  EXPECT_NEAR(yDifference, 0.0f, 0.002f);
}

// Assert the normals are facing forward in z distance
TEST_F(WorldsTest, TestSparseNormals) {
  ScopeTimer rt("test-compute-vertex-normals-with-cone");
  auto sparsePoints = c8::Worlds::sparseFaceMeshWorld();
  auto sparseIndices = c8::Worlds::sparseFaceMeshIndices();

  Vector<HVector3> vertexNormals;
  computeVertexNormals(sparsePoints, sparseIndices, &vertexNormals);
  for (int i = 0; i < vertexNormals.size(); i++) {
    auto normal = vertexNormals[i];
    EXPECT_GE(normal.z(), 0.0f);
  }
}

}  // namespace c8
