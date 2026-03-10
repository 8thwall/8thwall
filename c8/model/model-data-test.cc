// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":model-data",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x09e6d144);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/model/model-data.h"

namespace c8 {

class ModelDataTest : public ::testing::Test {};

TEST_F(ModelDataTest, TestSerializeMesh) {
  ModelData model;
  model.mesh = std::make_unique<MeshData>();
  model.mesh->geometry.points = {
    {0.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
  };
  model.mesh->geometry.normals = {
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f},
  };
  model.mesh->geometry.uvs = {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f},
  };
  model.mesh->geometry.colors = {
    Color::BLACK,
    Color::RED,
    Color::GREEN,
  };
  model.mesh->geometry.triangles = {{0, 1, 2}};

  model.mesh->texture = RGBA8888PlanePixelBuffer(2, 3);
  for (int i = 0; i < 24; ++i) {
    model.mesh->texture.pixels().pixels()[i] = i;
  }

  Vector<uint8_t> serialized;
  serialize(model, &serialized);

  EXPECT_TRUE(hasMesh(serialized.data()));
  EXPECT_FALSE(hasSplatAttributes(serialized.data()));

  auto mesh = meshView(serialized.data());

  EXPECT_EQ(3, mesh.geometry.points.size);
  EXPECT_EQ(3, mesh.geometry.normals.size);
  EXPECT_EQ(3, mesh.geometry.uvs.size);

  EXPECT_EQ(3, mesh.geometry.colors.size);
  EXPECT_EQ(Color::BLACK, mesh.geometry.colors.data[0]);
  EXPECT_EQ(Color::RED, mesh.geometry.colors.data[1]);
  EXPECT_EQ(Color::GREEN, mesh.geometry.colors.data[2]);

  EXPECT_EQ(1, mesh.geometry.triangles.size);
  EXPECT_EQ(0, mesh.geometry.triangles.data[0].a);
  EXPECT_EQ(1, mesh.geometry.triangles.data[0].b);
  EXPECT_EQ(2, mesh.geometry.triangles.data[0].c);

  EXPECT_EQ(2, mesh.texture.rows());
  EXPECT_EQ(3, mesh.texture.cols());
  EXPECT_EQ(12, mesh.texture.rowBytes());
  for (int i = 0; i < 24; ++i) {
    EXPECT_EQ(i, mesh.texture.pixels()[i]);
  }
}

TEST_F(ModelDataTest, TestSerializeSplat) {
  ModelData model;
  model.splatAttributes = std::make_unique<SplatAttributes>();
  model.splatAttributes->positions = {
    {0.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
  };
  model.splatAttributes->rotations = {
    Quaternion::fromPitchYawRollDegrees(0.0f, 0.0f, 90.0f),
    Quaternion::fromPitchYawRollDegrees(0.0f, 90.0f, 0.0f),
    Quaternion::fromPitchYawRollDegrees(90.0f, 0.0f, 0.0f),
  };
  model.splatAttributes->scales = {
    {1.0f, 2.0f, 3.0f},
    {6.0f, 5.0f, 4.0f},
    {7.0f, 8.0f, 9.0f},
  };
  model.splatAttributes->colors = {
    Color::BLACK,
    Color::RED,
    Color::GREEN,
  };

  Vector<uint8_t> serialized;
  serialize(model, &serialized);

  EXPECT_FALSE(hasMesh(serialized.data()));
  EXPECT_TRUE(hasSplatAttributes(serialized.data()));

  auto splat = splatAttributesView(serialized.data());
  EXPECT_EQ(0, splat.header.maxNumPoints);  // We never set this, so it should be 0.
  EXPECT_EQ(0, splat.header.numPoints);     // We never set this, so it should be 0.
  EXPECT_EQ(3, splat.positions.size);
  EXPECT_EQ(3, splat.rotations.size);
  EXPECT_EQ(3, splat.scales.size);
  EXPECT_EQ(3, splat.colors.size);
  EXPECT_EQ(0, splat.shRed.size);
  EXPECT_EQ(0, splat.shGreen.size);
  EXPECT_EQ(0, splat.shBlue.size);
}

TEST_F(ModelDataTest, TestSerializeSplatTextureOnlyIds) {
  ModelData model;
  model.splatTexture = std::make_unique<SplatTexture>();
  model.splatTexture->header = {
    .numPoints = 3,     // numPoints
    .maxNumPoints = 9,  // maxNumPoints
    .shDegree = 2,      // shDegree
    .antialiased = 1,   // antialiased
    .hasSkybox = 1,     // hasSkybox
    .width = 20.0f,     // width
    .height = 30.0f,    // height
    .depth = 40.0f,     // depth
    .centerX = -0.5f,   // centerX
    .centerY = -1.5f,   // centerY
    .centerZ = -2.5f,   // centerZ
  };

  model.splatTexture->sortedIds = {0, 1, 2};

  Vector<uint8_t> serialized;
  serialize(model, &serialized);

  EXPECT_FALSE(hasMesh(serialized.data()));
  EXPECT_FALSE(hasSplatAttributes(serialized.data()));
  EXPECT_TRUE(hasSplatTexture(serialized.data()));

  auto splat = splatTexture(serialized.data());

  EXPECT_EQ(3, splat.header.numPoints);
  EXPECT_EQ(9, splat.header.maxNumPoints);
  EXPECT_EQ(2, splat.header.shDegree);
  EXPECT_EQ(1, splat.header.antialiased);
  EXPECT_EQ(1, splat.header.hasSkybox);
  EXPECT_EQ(20.0f, splat.header.width);
  EXPECT_EQ(30.0f, splat.header.height);
  EXPECT_EQ(40.0f, splat.header.depth);
  EXPECT_EQ(-0.5f, splat.header.centerX);
  EXPECT_EQ(-1.5f, splat.header.centerY);
  EXPECT_EQ(-2.5f, splat.header.centerZ);

  EXPECT_EQ(3, splat.sortedIds.size);
  EXPECT_EQ(0, splat.sortedIds.data[0]);
  EXPECT_EQ(1, splat.sortedIds.data[1]);
  EXPECT_EQ(2, splat.sortedIds.data[2]);

  EXPECT_EQ(0, splat.texture.rows());
  EXPECT_EQ(0, splat.texture.cols());
  EXPECT_EQ(0, splat.texture.rowElements());
  EXPECT_EQ(nullptr, splat.texture.pixels());

  EXPECT_EQ(0, splat.interleavedAttributeData.size);
  EXPECT_EQ(nullptr, splat.interleavedAttributeData.data);

  EXPECT_EQ(0, splat.skybox.px.rows());
  EXPECT_EQ(0, splat.skybox.px.cols());
  EXPECT_EQ(0, splat.skybox.nx.rows());
  EXPECT_EQ(0, splat.skybox.nx.cols());
  EXPECT_EQ(0, splat.skybox.py.rows());
  EXPECT_EQ(0, splat.skybox.py.cols());
  EXPECT_EQ(0, splat.skybox.ny.rows());
  EXPECT_EQ(0, splat.skybox.ny.cols());
  EXPECT_EQ(0, splat.skybox.pz.rows());
  EXPECT_EQ(0, splat.skybox.pz.cols());
  EXPECT_EQ(0, splat.skybox.nz.rows());
  EXPECT_EQ(0, splat.skybox.nz.cols());
}

TEST_F(ModelDataTest, TestSerializeSplatTextureNoIds) {
  ModelData model;
  model.splatTexture = std::make_unique<SplatTexture>();
  model.splatTexture->header = {
    .numPoints = 3,     // numPoints
    .maxNumPoints = 9,  // maxNumPoints
    .shDegree = 2,      // shDegree
    .antialiased = 1,   // antialiased
    .hasSkybox = 0,     // hasSkybox
    .width = 20.0f,     // width
    .height = 30.0f,    // height
    .depth = 40.0f,     // depth
    .centerX = -0.5f,   // centerX
    .centerY = -1.5f,   // centerY
    .centerZ = -2.5f,   // centerZ
  };

  model.splatTexture->texture = RGBA32PlanePixelBuffer(3, 5);
  for (int i = 0; i < 3 * 5 * 4; ++i) {
    model.splatTexture->texture.pixels().pixels()[i] = i;
  }

  Vector<uint8_t> serialized;
  serialize(model, &serialized);

  EXPECT_FALSE(hasMesh(serialized.data()));
  EXPECT_FALSE(hasSplatAttributes(serialized.data()));
  EXPECT_TRUE(hasSplatTexture(serialized.data()));

  auto splat = splatTexture(serialized.data());

  EXPECT_EQ(3, splat.header.numPoints);
  EXPECT_EQ(9, splat.header.maxNumPoints);
  EXPECT_EQ(2, splat.header.shDegree);
  EXPECT_EQ(1, splat.header.antialiased);
  EXPECT_EQ(0, splat.header.hasSkybox);
  EXPECT_EQ(20.0f, splat.header.width);
  EXPECT_EQ(30.0f, splat.header.height);
  EXPECT_EQ(40.0f, splat.header.depth);
  EXPECT_EQ(-0.5f, splat.header.centerX);
  EXPECT_EQ(-1.5f, splat.header.centerY);
  EXPECT_EQ(-2.5f, splat.header.centerZ);

  EXPECT_EQ(0, splat.sortedIds.size);
  EXPECT_EQ(nullptr, splat.sortedIds.data);

  EXPECT_EQ(3, splat.texture.rows());
  EXPECT_EQ(5, splat.texture.cols());
  EXPECT_EQ(20, splat.texture.rowElements());
  for (int i = 0; i < 3 * 4 * 5; ++i) {
    EXPECT_EQ(i, splat.texture.pixels()[i]);
  }

  EXPECT_EQ(3 * 4 * 4 * 4, splat.interleavedAttributeData.size);
  EXPECT_EQ(
    reinterpret_cast<const void *>(splat.texture.pixels()),
    reinterpret_cast<const void *>(splat.interleavedAttributeData.data));

  EXPECT_EQ(0, splat.skybox.px.rows());
  EXPECT_EQ(0, splat.skybox.px.cols());
  EXPECT_EQ(0, splat.skybox.nx.rows());
  EXPECT_EQ(0, splat.skybox.nx.cols());
  EXPECT_EQ(0, splat.skybox.py.rows());
  EXPECT_EQ(0, splat.skybox.py.cols());
  EXPECT_EQ(0, splat.skybox.ny.rows());
  EXPECT_EQ(0, splat.skybox.ny.cols());
  EXPECT_EQ(0, splat.skybox.pz.rows());
  EXPECT_EQ(0, splat.skybox.pz.cols());
  EXPECT_EQ(0, splat.skybox.nz.rows());
  EXPECT_EQ(0, splat.skybox.nz.cols());
}

TEST_F(ModelDataTest, TestSerializeSplatTextureNoSkybox) {
  ModelData model;
  model.splatTexture = std::make_unique<SplatTexture>();
  model.splatTexture->header = {
    .numPoints = 3,     // numPoints
    .maxNumPoints = 9,  // maxNumPoints
    .shDegree = 2,      // shDegree
    .antialiased = 1,   // antialiased
    .hasSkybox = 0,     // hasSkybox
    .width = 20.0f,     // width
    .height = 30.0f,    // height
    .depth = 40.0f,     // depth
    .centerX = -0.5f,   // centerX
    .centerY = -1.5f,   // centerY
    .centerZ = -2.5f,   // centerZ
  };

  model.splatTexture->texture = RGBA32PlanePixelBuffer(3, 5);
  for (int i = 0; i < 3 * 5 * 4; ++i) {
    model.splatTexture->texture.pixels().pixels()[i] = i;
  }

  model.splatTexture->sortedIds = {0, 1, 2};

  Vector<uint8_t> serialized;
  serialize(model, &serialized);

  EXPECT_FALSE(hasMesh(serialized.data()));
  EXPECT_FALSE(hasSplatAttributes(serialized.data()));
  EXPECT_TRUE(hasSplatTexture(serialized.data()));

  auto splat = splatTexture(serialized.data());

  EXPECT_EQ(3, splat.header.numPoints);
  EXPECT_EQ(9, splat.header.maxNumPoints);
  EXPECT_EQ(2, splat.header.shDegree);
  EXPECT_EQ(1, splat.header.antialiased);
  EXPECT_EQ(0, splat.header.hasSkybox);
  EXPECT_EQ(20.0f, splat.header.width);
  EXPECT_EQ(30.0f, splat.header.height);
  EXPECT_EQ(40.0f, splat.header.depth);
  EXPECT_EQ(-0.5f, splat.header.centerX);
  EXPECT_EQ(-1.5f, splat.header.centerY);
  EXPECT_EQ(-2.5f, splat.header.centerZ);

  EXPECT_EQ(3, splat.sortedIds.size);
  EXPECT_EQ(0, splat.sortedIds.data[0]);
  EXPECT_EQ(1, splat.sortedIds.data[1]);
  EXPECT_EQ(2, splat.sortedIds.data[2]);

  EXPECT_EQ(3, splat.texture.rows());
  EXPECT_EQ(5, splat.texture.cols());
  EXPECT_EQ(20, splat.texture.rowElements());
  for (int i = 0; i < 3 * 4 * 5; ++i) {
    EXPECT_EQ(i, splat.texture.pixels()[i]);
  }

  EXPECT_EQ(3 * 4 * 4 * 4, splat.interleavedAttributeData.size);
  EXPECT_EQ(
    reinterpret_cast<const void *>(splat.texture.pixels()),
    reinterpret_cast<const void *>(splat.interleavedAttributeData.data));

  EXPECT_EQ(0, splat.skybox.px.rows());
  EXPECT_EQ(0, splat.skybox.px.cols());
  EXPECT_EQ(0, splat.skybox.nx.rows());
  EXPECT_EQ(0, splat.skybox.nx.cols());
  EXPECT_EQ(0, splat.skybox.py.rows());
  EXPECT_EQ(0, splat.skybox.py.cols());
  EXPECT_EQ(0, splat.skybox.ny.rows());
  EXPECT_EQ(0, splat.skybox.ny.cols());
  EXPECT_EQ(0, splat.skybox.pz.rows());
  EXPECT_EQ(0, splat.skybox.pz.cols());
  EXPECT_EQ(0, splat.skybox.nz.rows());
  EXPECT_EQ(0, splat.skybox.nz.cols());
}

TEST_F(ModelDataTest, TestSerializeSplatTexture) {
  ModelData model;
  model.splatTexture = std::make_unique<SplatTexture>();
  model.splatTexture->header = {
    .numPoints = 3,     // numPoints
    .maxNumPoints = 9,  // maxNumPoints
    .shDegree = 2,      // shDegree
    .antialiased = 1,   // antialiased
    .hasSkybox = 1,     // hasSkybox
    .width = 20.0f,     // width
    .height = 30.0f,    // height
    .depth = 40.0f,     // depth
    .centerX = -0.5f,   // centerX
    .centerY = -1.5f,   // centerY
    .centerZ = -2.5f,   // centerZ
  };

  model.splatTexture->texture = RGBA32PlanePixelBuffer(3, 5);
  for (int i = 0; i < 3 * 5 * 4; ++i) {
    model.splatTexture->texture.pixels().pixels()[i] = i;
  }

  // 4 bytes per pixel * 2x2 pixel images * 6 images = 96 bytes
  model.splatTexture->skybox.data = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,  // px
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,  // nx
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,  // py
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,  // ny
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,  // pz
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95   // nz
  };

  model.splatTexture->sortedIds = {0, 1, 2};

  Vector<uint8_t> serialized;
  serialize(model, &serialized);

  EXPECT_FALSE(hasMesh(serialized.data()));
  EXPECT_FALSE(hasSplatAttributes(serialized.data()));
  EXPECT_TRUE(hasSplatTexture(serialized.data()));

  auto splat = splatTexture(serialized.data());

  EXPECT_EQ(3, splat.header.numPoints);
  EXPECT_EQ(9, splat.header.maxNumPoints);
  EXPECT_EQ(2, splat.header.shDegree);
  EXPECT_EQ(1, splat.header.antialiased);
  EXPECT_EQ(1, splat.header.hasSkybox);
  EXPECT_EQ(20.0f, splat.header.width);
  EXPECT_EQ(30.0f, splat.header.height);
  EXPECT_EQ(40.0f, splat.header.depth);
  EXPECT_EQ(-0.5f, splat.header.centerX);
  EXPECT_EQ(-1.5f, splat.header.centerY);
  EXPECT_EQ(-2.5f, splat.header.centerZ);

  EXPECT_EQ(3, splat.sortedIds.size);
  EXPECT_EQ(0, splat.sortedIds.data[0]);
  EXPECT_EQ(1, splat.sortedIds.data[1]);
  EXPECT_EQ(2, splat.sortedIds.data[2]);

  EXPECT_EQ(3, splat.texture.rows());
  EXPECT_EQ(5, splat.texture.cols());
  EXPECT_EQ(20, splat.texture.rowElements());
  for (int i = 0; i < 3 * 4 * 5; ++i) {
    EXPECT_EQ(i, splat.texture.pixels()[i]);
  }

  EXPECT_EQ(3 * 4 * 4 * 4, splat.interleavedAttributeData.size);
  EXPECT_EQ(
    reinterpret_cast<const void *>(splat.texture.pixels()),
    reinterpret_cast<const void *>(splat.interleavedAttributeData.data));

  EXPECT_EQ(2, splat.skybox.px.rows());
  EXPECT_EQ(2, splat.skybox.px.cols());
  EXPECT_EQ(0, splat.skybox.px.pixels()[0]);
  EXPECT_EQ(15, splat.skybox.px.pixels()[splat.skybox.px.rows() * splat.skybox.px.rowBytes() - 1]);
  EXPECT_EQ(2, splat.skybox.nx.rows());
  EXPECT_EQ(2, splat.skybox.nx.cols());
  EXPECT_EQ(16, splat.skybox.nx.pixels()[0]);
  EXPECT_EQ(31, splat.skybox.nx.pixels()[splat.skybox.nx.rows() * splat.skybox.nx.rowBytes() - 1]);
  EXPECT_EQ(2, splat.skybox.py.rows());
  EXPECT_EQ(2, splat.skybox.py.cols());
  EXPECT_EQ(32, splat.skybox.py.pixels()[0]);
  EXPECT_EQ(47, splat.skybox.py.pixels()[splat.skybox.py.rows() * splat.skybox.py.rowBytes() - 1]);
  EXPECT_EQ(2, splat.skybox.ny.rows());
  EXPECT_EQ(2, splat.skybox.ny.cols());
  EXPECT_EQ(48, splat.skybox.ny.pixels()[0]);
  EXPECT_EQ(63, splat.skybox.ny.pixels()[splat.skybox.ny.rows() * splat.skybox.ny.rowBytes() - 1]);
  EXPECT_EQ(2, splat.skybox.pz.rows());
  EXPECT_EQ(2, splat.skybox.pz.cols());
  EXPECT_EQ(64, splat.skybox.pz.pixels()[0]);
  EXPECT_EQ(79, splat.skybox.pz.pixels()[splat.skybox.pz.rows() * splat.skybox.pz.rowBytes() - 1]);
  EXPECT_EQ(2, splat.skybox.nz.rows());
  EXPECT_EQ(2, splat.skybox.nz.cols());
  EXPECT_EQ(80, splat.skybox.nz.pixels()[0]);
  EXPECT_EQ(95, splat.skybox.nz.pixels()[splat.skybox.nz.rows() * splat.skybox.nz.rowBytes() - 1]);
}

TEST_F(ModelDataTest, TestSerializeSplatMultiTexture) {
  ModelData model;
  model.splatMultiTexture = std::make_unique<SplatMultiTexture>();
  model.splatMultiTexture->header = {
    .numPoints = 15,   // numPoints
    .shDegree = 2,     // shDegree
    .antialiased = 1,  // antialiased
  };

  model.splatMultiTexture->sortedIds = R32PlanePixelBuffer(3, 5);
  model.splatMultiTexture->positionColor = RGBA32PlanePixelBuffer(3, 5);
  model.splatMultiTexture->rotationScale = RGBA32PlanePixelBuffer(3, 5);
  model.splatMultiTexture->shRG = RGBA32PlanePixelBuffer(3, 5);
  model.splatMultiTexture->shB = RGBA32PlanePixelBuffer(3, 5);
  for (int i = 0; i < 3 * 5 * 1; ++i) {
    model.splatMultiTexture->sortedIds.pixels().pixels()[i] = i;
  }
  for (int i = 0; i < 3 * 5 * 4; ++i) {
    model.splatMultiTexture->positionColor.pixels().pixels()[i] = i + 1;
    model.splatMultiTexture->rotationScale.pixels().pixels()[i] = i + 2;
    model.splatMultiTexture->shRG.pixels().pixels()[i] = i + 3;
    model.splatMultiTexture->shB.pixels().pixels()[i] = i + 4;
  }

  Vector<uint8_t> serialized;
  serialize(model, &serialized);

  EXPECT_FALSE(hasMesh(serialized.data()));
  EXPECT_FALSE(hasSplatAttributes(serialized.data()));
  EXPECT_FALSE(hasSplatTexture(serialized.data()));
  EXPECT_TRUE(hasSplatMultiTexture(serialized.data()));

  auto splat = splatMultiTexture(serialized.data());

  EXPECT_EQ(15, splat.header.numPoints);
  EXPECT_EQ(2, splat.header.shDegree);
  EXPECT_EQ(1, splat.header.antialiased);

  EXPECT_EQ(3, splat.sortedIds.rows());
  EXPECT_EQ(5, splat.sortedIds.cols());
  EXPECT_EQ(5, splat.sortedIds.rowElements());
  for (int i = 0; i < 3 * 5 * 1; ++i) {
    EXPECT_EQ(i, splat.sortedIds.pixels()[i]);
  }

  EXPECT_EQ(3, splat.positionColor.rows());
  EXPECT_EQ(5, splat.positionColor.cols());
  EXPECT_EQ(20, splat.positionColor.rowElements());
  EXPECT_EQ(3, splat.rotationScale.rows());
  EXPECT_EQ(5, splat.rotationScale.cols());
  EXPECT_EQ(20, splat.rotationScale.rowElements());
  EXPECT_EQ(3, splat.shRG.rows());
  EXPECT_EQ(5, splat.shRG.cols());
  EXPECT_EQ(20, splat.shRG.rowElements());
  EXPECT_EQ(3, splat.shB.rows());
  EXPECT_EQ(5, splat.shB.cols());
  EXPECT_EQ(20, splat.shB.rowElements());
  for (int i = 0; i < 3 * 5 * 4; ++i) {
    EXPECT_EQ(i + 1, splat.positionColor.pixels()[i]);
    EXPECT_EQ(i + 2, splat.rotationScale.pixels()[i]);
    EXPECT_EQ(i + 3, splat.shRG.pixels()[i]);
    EXPECT_EQ(i + 4, splat.shB.pixels()[i]);
  }
}

TEST_F(ModelDataTest, TestSerializPointCloud) {
  ModelData model;
  model.pointCloud = std::make_unique<PointCloudGeometry>();
  model.pointCloud->points = {
    {0.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
  };
  model.pointCloud->normals = {
    {3.0f, 0.0f, 1.0f},
    {2.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 1.0f},
  };
  model.pointCloud->colors = {
    Color::BLACK,
    Color::RED,
    Color::GREEN,
  };

  Vector<uint8_t> serialized;
  serialize(model, &serialized);

  EXPECT_FALSE(hasMesh(serialized.data()));
  EXPECT_FALSE(hasSplatAttributes(serialized.data()));
  EXPECT_FALSE(hasSplatTexture(serialized.data()));
  EXPECT_FALSE(hasSplatMultiTexture(serialized.data()));
  EXPECT_TRUE(hasPointCloud(serialized.data()));

  auto pc = pointCloudView(serialized.data());

  EXPECT_EQ(3, pc.points.size);
  EXPECT_EQ(3, pc.colors.size);
  EXPECT_EQ(3, pc.normals.size);

  for (int i = 0; i < 3; ++i) {
    EXPECT_EQ((i == 1) * 1.0f, pc.points.data[i].x());
    EXPECT_EQ((i == 2) * 1.0f, pc.points.data[i].y());
    EXPECT_EQ((i == 0) * 1.0f, pc.points.data[i].z());

    EXPECT_EQ(3.0f - i, pc.normals.data[i].x());
    EXPECT_EQ(0.0f, pc.normals.data[i].y());
    EXPECT_EQ(1.0f, pc.normals.data[i].z());
  }

  EXPECT_EQ(Color::BLACK, pc.colors.data[0]);
  EXPECT_EQ(Color::RED, pc.colors.data[1]);
  EXPECT_EQ(Color::GREEN, pc.colors.data[2]);
}

}  // namespace c8
