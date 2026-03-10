// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":geometry",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x3ce66176);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/pixels/render/geometry.h"

using testing::Pointwise;

namespace c8 {

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

MATCHER_P(AreEqualData3, eps, "") {
  const std::array<float, 3> &d1 = testing::get<0>(arg).data();
  const std::array<float, 3> &d2 = testing::get<1>(arg).data();
  return std::abs(d1[0] - d2[0]) < eps && std::abs(d1[1] - d2[1]) < eps
    && std::abs(d1[2] - d2[2]) < eps;
}

MATCHER_P(AreEqualData2, eps, "") {
  const std::array<float, 2> &d1 = testing::get<0>(arg).data();
  const std::array<float, 2> &d2 = testing::get<1>(arg).data();
  return std::abs(d1[0] - d2[0]) < eps && std::abs(d1[1] - d2[1]) < eps;
}

MATCHER(AreEqualPointIndices, "") { return testing::get<0>(arg).a == testing::get<1>(arg).a; }

MATCHER(AreEqualMeshIndices, "") {
  return testing::get<0>(arg).a == testing::get<1>(arg).a
    && testing::get<0>(arg).b == testing::get<1>(arg).b
    && testing::get<0>(arg).c == testing::get<1>(arg).c;
}

decltype(auto) equalsVec(const Vector<HPoint3> &points) {
  return Pointwise(AreEqualData3(1e-6f), points);
}

decltype(auto) equalsVec(const Vector<HVector2> &vec) {
  return Pointwise(AreEqualData2(1e-6f), vec);
}

decltype(auto) equalsVec(const Vector<HVector3> &vec) {
  return Pointwise(AreEqualData3(1e-6f), vec);
}

decltype(auto) equalsVec(const Vector<MeshIndices> &vec) {
  return Pointwise(AreEqualMeshIndices(), vec);
}

decltype(auto) equalsVec(const Vector<PointIndices> &vec) {
  return Pointwise(AreEqualPointIndices(), vec);
}

// Checks whether a triangle is pointed away from the origin, as a way of checking its winding
// direction.
bool facingOutFromOrigin(const std::array<HPoint3, 3> &verts) {
  auto a = HVector3(verts[0].x(), verts[0].y(), verts[0].z());
  auto b = HVector3(verts[1].x(), verts[1].y(), verts[1].z());
  auto c = HVector3(verts[2].x(), verts[2].y(), verts[2].z());
  auto norm = (b - a).cross(c - a);
  auto centerFromOrigin = (1.0f / 3.0f) * (a + b + c);
  return norm.dot(centerFromOrigin) >= 0.0f;
}

std::array<HPoint3, 3> triVerts(const Geometry &g, int index) {
  auto tri = g.triangles()[index];
  return {g.vertices()[tri.a], g.vertices()[tri.b], g.vertices()[tri.c]};
}

std::array<HVector2, 3> triUvs(const Geometry &g, int index) {
  auto tri = g.triangles()[index];
  return {g.uvs()[tri.a], g.uvs()[tri.b], g.uvs()[tri.c]};
}

std::array<HVector3, 3> triNormals(const Geometry &g, int index) {
  auto tri = g.triangles()[index];
  return {g.normals()[tri.a], g.normals()[tri.b], g.normals()[tri.c]};
}

class GeometryTest : public ::testing::Test {};

TEST_F(GeometryTest, TestQuad) {
  auto quad = GeoGen::quad(false);
  // Check basic stats.
  EXPECT_EQ(4, quad->vertices().size());
  EXPECT_EQ(2, quad->triangles().size());
  EXPECT_EQ(0, quad->colors().size());

  // Check what's dirty.
  EXPECT_TRUE(quad->verticesDirty());
  EXPECT_TRUE(quad->trianglesDirty());
  EXPECT_FALSE(quad->colorsDirty());

  // Set clean
  quad->setClean();

  // Check what's dirty.
  EXPECT_FALSE(quad->verticesDirty());
  EXPECT_FALSE(quad->trianglesDirty());
  EXPECT_FALSE(quad->colorsDirty());

  // Set colors and check what's dirty.
  quad->setColor(Color::PURPLE);
  EXPECT_EQ(4, quad->colors().size());
  EXPECT_FALSE(quad->verticesDirty());
  EXPECT_FALSE(quad->trianglesDirty());
  EXPECT_TRUE(quad->colorsDirty());
}

TEST_F(GeometryTest, TestQuadInstanced) {
  auto quad = GeoGen::quad(false);
  quad->setInstancePositions({{0.0f, 0.0f, 0.0f}});
  quad->setInstanceRotations({{1.0f, 0.0f, 0.0f, 0.0f}});
  quad->setInstanceScales({{1.0f, 1.0f, 1.0f}});

  // Check basic stats.
  EXPECT_EQ(4, quad->vertices().size());
  EXPECT_EQ(2, quad->triangles().size());
  EXPECT_EQ(0, quad->colors().size());
  EXPECT_EQ(1, quad->instancePositions().size());
  EXPECT_EQ(1, quad->instanceRotations().size());
  EXPECT_EQ(1, quad->instanceScales().size());
  EXPECT_EQ(0, quad->instanceColors().size());

  // Check what's dirty.
  EXPECT_TRUE(quad->verticesDirty());
  EXPECT_TRUE(quad->trianglesDirty());
  EXPECT_TRUE(quad->instancePositionsDirty());
  EXPECT_TRUE(quad->instanceRotationsDirty());
  EXPECT_TRUE(quad->instanceScalesDirty());
  EXPECT_FALSE(quad->colorsDirty());
  EXPECT_FALSE(quad->instanceColorsDirty());

  // Set clean
  quad->setClean();

  // Check what's dirty.
  EXPECT_FALSE(quad->verticesDirty());
  EXPECT_FALSE(quad->trianglesDirty());
  EXPECT_FALSE(quad->colorsDirty());
  EXPECT_FALSE(quad->instancePositionsDirty());
  EXPECT_FALSE(quad->instanceRotationsDirty());
  EXPECT_FALSE(quad->instanceScalesDirty());
  EXPECT_FALSE(quad->instanceColorsDirty());

  // Set colors and check what's dirty.
  quad->setInstanceColors({Color::PURPLE});
  EXPECT_EQ(1, quad->instanceColors().size());
  EXPECT_FALSE(quad->verticesDirty());
  EXPECT_FALSE(quad->trianglesDirty());
  EXPECT_TRUE(quad->instanceColorsDirty());
}

TEST_F(GeometryTest, TestCurvyFullOpen) {
  // Test a 4-segment open cylinder. This has four segments, each made up of a quad (two triangles)
  // around the x/z +1/-1 axes.
  auto g = GeoGen::curvyMesh({1.0f, 1.0f, 1.0f, true, 4});

  // Check basic stats.
  EXPECT_EQ(10, g->vertices().size());
  EXPECT_EQ(10, g->normals().size());
  EXPECT_EQ(10, g->uvs().size());
  EXPECT_EQ(8, g->triangles().size());
  EXPECT_EQ(0, g->colors().size());

  Vector<HPoint3> verts = {
    {0.0f, -0.5f, 1.0f},
    {0.0f, 0.5f, 1.0f},
    {-1.0f, -0.5f, 0.0f},
    {-1.0f, 0.5f, 0.0f},
    {0.0f, -0.5f, -1.0f},
    {0.0f, 0.5f, -1.0f},
    {1.0f, -0.5f, 0.0f},
    {1.0f, 0.5f, 0.0f},
  };
  Vector<HVector2> uvs = {
    {0.0f, 0.0f},
    {0.0f, 1.0f},
    {0.25f, 0.0f},
    {0.25f, 1.0f},
    {0.5f, 0.0f},
    {0.5f, 1.0f},
    {0.75f, 0.0f},
    {0.75f, 1.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
  };
  Vector<HVector3> normals = {
    {0.0f, 0.0f, 1.0f},
    {-1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, -1.0f},
    {1.0f, 0.0f, 0.0f},
  };

  // Check winding order.
  for (int i = 0; i < g->triangles().size(); ++i) {
    EXPECT_TRUE(facingOutFromOrigin(triVerts(*g, i))) << "Winding order: " << i;
  }

  // First segment.
  EXPECT_THAT(triVerts(*g, 0), equalsVec({verts[0], verts[1], verts[2]}));
  EXPECT_THAT(triUvs(*g, 0), equalsVec({uvs[0], uvs[1], uvs[2]}));
  EXPECT_THAT(triNormals(*g, 0), equalsVec({normals[0], normals[0], normals[1]}));

  EXPECT_THAT(triVerts(*g, 1), equalsVec({verts[1], verts[3], verts[2]}));
  EXPECT_THAT(triUvs(*g, 1), equalsVec({uvs[1], uvs[3], uvs[2]}));
  EXPECT_THAT(triNormals(*g, 1), equalsVec({normals[0], normals[1], normals[1]}));

  // Second segment.
  EXPECT_THAT(triVerts(*g, 2), equalsVec({verts[2], verts[3], verts[4]}));
  EXPECT_THAT(triUvs(*g, 2), equalsVec({uvs[2], uvs[3], uvs[4]}));
  EXPECT_THAT(triNormals(*g, 2), equalsVec({normals[1], normals[1], normals[2]}));

  EXPECT_THAT(triVerts(*g, 3), equalsVec({verts[3], verts[5], verts[4]}));
  EXPECT_THAT(triUvs(*g, 3), equalsVec({uvs[3], uvs[5], uvs[4]}));
  EXPECT_THAT(triNormals(*g, 3), equalsVec({normals[1], normals[2], normals[2]}));

  // Third segment.
  EXPECT_THAT(triVerts(*g, 4), equalsVec({verts[4], verts[5], verts[6]}));
  EXPECT_THAT(triUvs(*g, 4), equalsVec({uvs[4], uvs[5], uvs[6]}));
  EXPECT_THAT(triNormals(*g, 4), equalsVec({normals[2], normals[2], normals[3]}));

  EXPECT_THAT(triVerts(*g, 5), equalsVec({verts[5], verts[7], verts[6]}));
  EXPECT_THAT(triUvs(*g, 5), equalsVec({uvs[5], uvs[7], uvs[6]}));
  EXPECT_THAT(triNormals(*g, 5), equalsVec({normals[2], normals[3], normals[3]}));

  // Fourth segment.
  EXPECT_THAT(triVerts(*g, 6), equalsVec({verts[6], verts[7], verts[0]}));
  EXPECT_THAT(triUvs(*g, 6), equalsVec({uvs[6], uvs[7], uvs[8]}));
  EXPECT_THAT(triNormals(*g, 6), equalsVec({normals[3], normals[3], normals[0]}));

  EXPECT_THAT(triVerts(*g, 7), equalsVec({verts[7], verts[1], verts[0]}));
  EXPECT_THAT(triUvs(*g, 7), equalsVec({uvs[7], uvs[9], uvs[8]}));
  EXPECT_THAT(triNormals(*g, 7), equalsVec({normals[3], normals[0], normals[0]}));
}

TEST_F(GeometryTest, TestCurvyHalfClosed) {
  // Test a 2-segment closed cone. Each segment has a quad (two triangles) and a top/bottom
  // triangle, i.e. 4 total triangles per segment. The cone top is 0.5 and the bottom is 1.5, so the
  // normals are at 45 degrees positive y.
  auto g = GeoGen::curvyMesh({0.5f, 1.5f, 0.5f, false, 2});

  // Check basic stats.
  EXPECT_EQ(14, g->vertices().size());
  EXPECT_EQ(14, g->normals().size());
  EXPECT_EQ(14, g->uvs().size());
  EXPECT_EQ(8, g->triangles().size());
  EXPECT_EQ(0, g->colors().size());

  Vector<HPoint3> verts = {
    {-1.5f, -0.5f, 0.0f},
    {-0.5f, 0.5f, 0.0f},
    {0.0f, -0.5f, -1.5f},
    {0.0f, 0.5f, -0.5f},
    {1.5f, -0.5f, 0.0f},
    {0.5f, 0.5f, 0.0f},
    {0.0f, -0.5f, 0.0f},
    {0.0f, 0.5f, 0.0f},
  };
  Vector<HVector2> uvs = {
    {0.0f, 0.0f},
    {0.0f, 1.0f},
    {0.5f, 0.0f},
    {0.5f, 1.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
  };
  constexpr float irt2 = 0.70710678118f;
  Vector<HVector3> normals = {
    {-irt2, irt2, 0.0f},
    {0.0f, irt2, -irt2},
    {irt2, irt2, 0.0f},
    {0.0f, -1.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
  };

  // Check winding order.
  for (int i = 0; i < g->triangles().size(); ++i) {
    EXPECT_TRUE(facingOutFromOrigin(triVerts(*g, i))) << "Winding order: " << i;
  }

  // First segment.
  EXPECT_THAT(triVerts(*g, 0), equalsVec({verts[0], verts[1], verts[2]}));
  EXPECT_THAT(triUvs(*g, 0), equalsVec({uvs[0], uvs[1], uvs[2]}));
  EXPECT_THAT(triNormals(*g, 0), equalsVec({normals[0], normals[0], normals[1]}));

  EXPECT_THAT(triVerts(*g, 1), equalsVec({verts[1], verts[3], verts[2]}));
  EXPECT_THAT(triUvs(*g, 1), equalsVec({uvs[1], uvs[3], uvs[2]}));
  EXPECT_THAT(triNormals(*g, 1), equalsVec({normals[0], normals[1], normals[1]}));

  EXPECT_THAT(triVerts(*g, 2), equalsVec({verts[6], verts[0], verts[2]}));
  EXPECT_THAT(triUvs(*g, 2), equalsVec({uvs[0], uvs[0], uvs[0]}));
  EXPECT_THAT(triNormals(*g, 2), equalsVec({normals[3], normals[3], normals[3]}));

  EXPECT_THAT(triVerts(*g, 3), equalsVec({verts[7], verts[3], verts[1]}));
  EXPECT_THAT(triUvs(*g, 3), equalsVec({uvs[5], uvs[5], uvs[5]}));
  EXPECT_THAT(triNormals(*g, 3), equalsVec({normals[4], normals[4], normals[4]}));

  // Second segment.
  EXPECT_THAT(triVerts(*g, 4), equalsVec({verts[2], verts[3], verts[4]}));
  EXPECT_THAT(triUvs(*g, 4), equalsVec({uvs[2], uvs[3], uvs[4]}));
  EXPECT_THAT(triNormals(*g, 4), equalsVec({normals[1], normals[1], normals[2]}));

  EXPECT_THAT(triVerts(*g, 5), equalsVec({verts[3], verts[5], verts[4]}));
  EXPECT_THAT(triUvs(*g, 5), equalsVec({uvs[3], uvs[5], uvs[4]}));
  EXPECT_THAT(triNormals(*g, 5), equalsVec({normals[1], normals[2], normals[2]}));

  EXPECT_THAT(triVerts(*g, 6), equalsVec({verts[6], verts[2], verts[4]}));
  EXPECT_THAT(triUvs(*g, 6), equalsVec({uvs[0], uvs[0], uvs[0]}));
  EXPECT_THAT(triNormals(*g, 6), equalsVec({normals[3], normals[3], normals[3]}));

  EXPECT_THAT(triVerts(*g, 7), equalsVec({verts[7], verts[5], verts[3]}));
  EXPECT_THAT(triUvs(*g, 7), equalsVec({uvs[5], uvs[5], uvs[5]}));
  EXPECT_THAT(triNormals(*g, 7), equalsVec({normals[4], normals[4], normals[4]}));
}

TEST_F(GeometryTest, TestCube) {
  auto cube = GeoGen::cubeMesh();
  // Check basic stats.
  EXPECT_EQ(24, cube->vertices().size());
  EXPECT_EQ(12, cube->triangles().size());
  EXPECT_EQ(0, cube->colors().size());
  EXPECT_EQ(24, cube->uvs().size());

  // Check what's dirty.
  EXPECT_TRUE(cube->verticesDirty());
  EXPECT_TRUE(cube->trianglesDirty());
  EXPECT_FALSE(cube->colorsDirty());
  EXPECT_TRUE(cube->uvsDirty());

  // Set clean
  cube->setClean();

  // Check what's dirty.
  EXPECT_FALSE(cube->verticesDirty());
  EXPECT_FALSE(cube->trianglesDirty());
  EXPECT_FALSE(cube->colorsDirty());

  // Set colors and check what's dirty.
  cube->setColor(Color::PURPLE);
  EXPECT_EQ(24, cube->colors().size());
  EXPECT_FALSE(cube->verticesDirty());
  EXPECT_FALSE(cube->trianglesDirty());
  EXPECT_TRUE(cube->colorsDirty());

  // Check winding order.
  for (int i = 0; i < cube->triangles().size(); ++i) {
    EXPECT_TRUE(facingOutFromOrigin(triVerts(*cube, i))) << "Winding order: " << i;
  }
}

TEST_F(GeometryTest, TestSphere) {
  int detail = 8;
  float radius = 1.f;
  auto sphere = GeoGen::sphere(radius, detail);
  EXPECT_EQ(sphere->vertices().size(), (detail + 1) * (detail / 2 + 1));
  EXPECT_EQ(sphere->normals().size(), (detail + 1) * (detail / 2 + 1));

  sphere = GeoGen::sphere(radius, detail, detail / 2);
  EXPECT_EQ(sphere->vertices().size(), (detail + 1) * (detail / 2 + 1));
  EXPECT_EQ(sphere->normals().size(), (detail + 1) * (detail / 2 + 1));

  // Test if normals are correct
  for (int i = 0; i < sphere->vertices().size(); i++) {
    auto &v = sphere->vertices()[i];
    auto &n = sphere->normals()[i];
    std::array<HVector3, 1UL> t = { HVector3{v.x(), v.y(), v.z()} };
    EXPECT_THAT(t, equalsVec({n}));
  }
  radius = 4.5f;
  sphere = GeoGen::sphere(radius, detail);
  for (int i = 0; i < sphere->vertices().size(); i++) {
    auto &v = sphere->vertices()[i];
    auto &n = sphere->normals()[i];
    std::array<HVector3, 1UL> t = { HVector3{v.x(), v.y(), v.z()} };
    EXPECT_THAT(t, equalsVec({4.5f * n}));
  }

  // Check if radius is set correctly
  radius = 1.234f;
  sphere = GeoGen::sphere(radius, detail);
  for (int i = 0; i < sphere->vertices().size(); i++) {
    auto &v = sphere->vertices()[i];
    auto &n = sphere->normals()[i];
    HVector3 vv{v.x(), v.y(), v.z()};
    EXPECT_FLOAT_EQ(vv.l2Norm(), radius);
    EXPECT_FLOAT_EQ(n.l2Norm(), 1.f);
  }

  // Check winding order.
  detail = 16;
  sphere = GeoGen::sphere(radius, detail);
  for (int i = 0; i < sphere->triangles().size(); i++) {
    EXPECT_TRUE(facingOutFromOrigin(triVerts(*sphere, i))) << "Winding order: " << i;
  }
}

TEST_F(GeometryTest, TestSphereSmall) {
  int detail = 4;
  float radius = 1.f;
  auto sphere = GeoGen::sphere(radius, detail);
  Vector<HPoint3> verts = {
    {0.f, 1.f, 0.f},
    {0.f, 1.f, 0.f},
    {0.f, 1.f, 0.f},
    {0.f, 1.f, 0.f},
    {0.f, 1.f, 0.f},
    {1.f, 0.f, 0.f},
    {0.f, 0.f, 1.f},
    {-1.f, 0.f, 0.f},
    {0.f, 0.f, -1.f},
    {1.f, 0.f, 0.f},
    {0.f, -1.f, 0.f},
    {0.f, -1.f, 0.f},
    {0.f, -1.f, 0.f},
    {0.f, -1.f, 0.f},
    {0.f, -1.f, 0.f},
  };
  Vector<HVector2> uvs = {
    {0.0f, 0.0f},
    {0.25f, 0.0f},
    {0.5f, 0.0f},
    {0.75f, 0.0f},
    {1.0f, 0.0f},
    {0.f, 0.5f},
    {0.25f, 0.5f},
    {0.5f, 0.5f},
    {0.75f, 0.5f},
    {1.f, 0.5f},
    {0.f, 1.f},
    {0.25f, 1.f},
    {0.5f, 1.f},
    {0.75f, 1.0f},
    {1.f, 1.f}
  };
  Vector<HVector3> normals = {
    {0.f, 1.f, 0.f},
    {0.f, 1.f, 0.f},
    {0.f, 1.f, 0.f},
    {0.f, 1.f, 0.f},
    {0.f, 1.f, 0.f},
    {1.f, 0.f, 0.f},
    {0.f, 0.f, 1.f},
    {-1.f, 0.f, 0.f},
    {0.f, 0.f, -1.f},
    {1.f, 0.f, 0.f},
    {0.f, -1.f, 0.f},
    {0.f, -1.f, 0.f},
    {0.f, -1.f, 0.f},
    {0.f, -1.f, 0.f},
    {0.f, -1.f, 0.f},
  };
  Vector<MeshIndices> indices = {
    {6, 5, 1},
    {7, 6, 2},
    {8, 7, 3},
    {9, 8, 4},
    {6, 10, 5},
    {7, 11, 6},
    {8, 12, 7},
    {9, 13, 8}
  };

  for (int i = 0; i < sphere->triangles().size(); i++) {
    EXPECT_THAT(triVerts(*sphere, i), equalsVec({verts[indices[i].a], verts[indices[i].b], verts[indices[i].c]}));
    EXPECT_THAT(triUvs(*sphere, i), equalsVec({uvs[indices[i].a], uvs[indices[i].b], uvs[indices[i].c]}));
    EXPECT_THAT(triNormals(*sphere, i), equalsVec({normals[indices[i].a], normals[indices[i].b], normals[indices[i].c]}));
  }
}

TEST_F(GeometryTest, TestBoundingBox) {
  auto cube = GeoGen::cubeMesh();
  auto halfBox = Box3::from({{-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}});
  EXPECT_EQ(cube->boundingBox(), halfBox);
}

TEST_F(GeometryTest, TestRotateGeometryUv) {
  auto quad = GeoGen::quad(false);
  auto rotatedQuad = GeoGen::quad(false);
  GeoGen::rotateCCW(rotatedQuad.get());
  EXPECT_THAT(quad->vertices(), equalsVec(rotatedQuad->vertices()));
  EXPECT_EQ(quad->colors().size(), rotatedQuad->colors().size());
  EXPECT_THAT(quad->normals(), equalsVec(rotatedQuad->normals()));
  EXPECT_THAT(quad->triangles(), equalsVec(rotatedQuad->triangles()));
  EXPECT_THAT(quad->points(), equalsVec(rotatedQuad->points()));

  EXPECT_EQ(quad->uvs().size(), rotatedQuad->uvs().size());
}

}  // namespace c8
