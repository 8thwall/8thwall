// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Nathan Waters (nathan@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":load-mesh",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//c8:c8-log",
    "//bzl/examples/draco/testdata:draco-asset",
    "//bzl/examples/glb/testdata:glb-asset",
    "//bzl/examples/mar/testdata:mar-asset",
  };
}
cc_end(0x5c1b2481);

#include "c8/c8-log.h"
#include "c8/geometry/load-mesh.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace c8 {

class LoadMeshTest : public ::testing::Test {};

using testing::Eq;
using testing::Pointwise;

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsHPoint(const HPoint3 &vec) { return Pointwise(AreWithin(0.0001), vec.data()); }

TEST_F(LoadMeshTest, TestBinaryDracoMesh) {
  MeshGeometry mesh = dracoFileToMesh("bzl/examples/draco/testdata/bunny.drc", false).value();

  EXPECT_EQ(mesh.points.size(), 34834);
  EXPECT_EQ(mesh.triangles.size(), 69451);
  // The bunny file does not have color, so we default to gray dummy colors
  EXPECT_EQ(mesh.colors.size(), 34834);
  for (auto &color : mesh.colors) {
    EXPECT_EQ(color.r(), 128);
    EXPECT_EQ(color.g(), 128);
    EXPECT_EQ(color.b(), 128);
  }

  EXPECT_THAT(mesh.triangles[0].a, 16461);
  EXPECT_THAT(mesh.triangles[0].b, 1);
  EXPECT_THAT(mesh.triangles[0].c, 2);
  EXPECT_THAT(mesh.triangles[1].a, 1);
  EXPECT_THAT(mesh.triangles[1].b, 3);
  EXPECT_THAT(mesh.triangles[1].c, 2);
  EXPECT_THAT(mesh.triangles.back().a, 18);
  EXPECT_THAT(mesh.triangles.back().b, 14);
  EXPECT_THAT(mesh.triangles.back().c, 23);

  EXPECT_THAT(mesh.points[0].data(), equalsHPoint({-0.090556f, 0.144390f, 0.027556f}));
  EXPECT_THAT(mesh.points[1].data(), equalsHPoint({-0.053672f, 0.033862f, 0.020771f}));
  EXPECT_THAT(mesh.points[2].data(), equalsHPoint({-0.053358f, 0.033206f, 0.019630f}));
  EXPECT_THAT(mesh.points.back().data(), equalsHPoint({-0.0864597f, 0.107753f, 0.0163893f}));
}

TEST_F(LoadMeshTest, TestBadDracoData) {
  Vector<uint8_t> dracoData{0, 1, 2, 23, 3, 4, 56, 6, 4, 4, 4, 3, 3, 3, 4, 3, 3, 4, 4};
  EXPECT_FALSE(dracoDataToMesh(&dracoData).has_value());
}

TEST_F(LoadMeshTest, TestMarFile) {
  auto mesh = marFileToMesh("bzl/examples/mar/testdata/doty.mar").value();
  EXPECT_EQ(mesh.points.size(), 204593);
  EXPECT_EQ(mesh.triangles.size(), 365940);
  EXPECT_EQ(mesh.normals.size(), 204593);
  EXPECT_TRUE(mesh.colors.empty());
  EXPECT_EQ(mesh.uvs.size(), 204593);
}

TEST_F(LoadMeshTest, TestGlbData) {
  tinygltf::Model model;
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  EXPECT_TRUE(
    loader.LoadBinaryFromFile(&model, &err, &warn, "bzl/examples/glb/testdata/catan.glb"));

  auto meshGeo = tinyGLTFModelToMesh(model);

  EXPECT_EQ(49300, meshGeo.points.size());
  EXPECT_EQ(49300, meshGeo.normals.size());
  EXPECT_EQ(49300, meshGeo.uvs.size());
  EXPECT_EQ(90468, meshGeo.triangles.size());
}

TEST_F(LoadMeshTest, TestGlbDataUnsignedShort) {
  tinygltf::Model model;
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  EXPECT_TRUE(
    loader.LoadBinaryFromFile(&model, &err, &warn, "bzl/examples/glb/testdata/arm_template.glb"));

  auto meshGeo = tinyGLTFModelToMesh(model);

  EXPECT_EQ(80, meshGeo.points.size());
  EXPECT_EQ(80, meshGeo.normals.size());
  EXPECT_EQ(80, meshGeo.uvs.size());
  EXPECT_EQ(128, meshGeo.triangles.size());
}
}  // namespace c8
