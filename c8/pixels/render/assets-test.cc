// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":assets",
    "//c8/io:file-io",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//c8/pixels/render/testdata:doty-glb",
    "//bzl/examples/mar/testdata:mar-asset",
  };
}
cc_end(0x2c19e8bb);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/io/file-io.h"
#include "c8/pixels/render/assets.h"

namespace c8 {

const String GLB_PATH = "c8/pixels/render/testdata/doty.glb";
const String MAR_PATH = "bzl/examples/mar/testdata/doty.mar";
const String MAR_TEXTURE_PATH = "bzl/examples/mar/testdata/doty-texture.jpg";

class AssetsTest : public ::testing::Test {};

TEST_F(AssetsTest, TestMar) {
  auto mesh = readMarAndTexture(MAR_PATH, MAR_TEXTURE_PATH);
  EXPECT_NE(nullptr, mesh.get());
  EXPECT_EQ(204593, mesh->geometry().vertices().size());
  EXPECT_EQ(204593, mesh->geometry().uvs().size());
  EXPECT_EQ(365940, mesh->geometry().triangles().size());
  EXPECT_EQ(8192, mesh->material().colorTexture()->rgbaPixels().cols());
  EXPECT_EQ(2048, mesh->material().colorTexture()->rgbaPixels().rows());
}

TEST_F(AssetsTest, TestGlb) {
  auto mesh = glbFileToMesh(GLB_PATH);
  mesh->geometry();

  EXPECT_EQ(78088, mesh->geometry().vertices().size());
  EXPECT_EQ(78088, mesh->geometry().normals().size());
  EXPECT_EQ(78088, mesh->geometry().uvs().size());
  EXPECT_EQ(142681, mesh->geometry().triangles().size());
  EXPECT_EQ(8192, mesh->material().colorTexture()->rgbaPixels().cols());
  EXPECT_EQ(4096, mesh->material().colorTexture()->rgbaPixels().rows());
}

TEST_F(AssetsTest, TestGlbData) {
  auto fileData = readFile(GLB_PATH);
  auto mesh = glbDataToMesh(fileData.data(), fileData.size());

  EXPECT_EQ(78088, mesh->geometry().vertices().size());
  EXPECT_EQ(78088, mesh->geometry().normals().size());
  EXPECT_EQ(78088, mesh->geometry().uvs().size());
  EXPECT_EQ(142681, mesh->geometry().triangles().size());
  EXPECT_EQ(8192, mesh->material().colorTexture()->rgbaPixels().cols());
  EXPECT_EQ(4096, mesh->material().colorTexture()->rgbaPixels().rows());
}

TEST_F(AssetsTest, TestBadGlbData) {
  Vector<uint8_t> glbData{0, 1, 2, 23, 3, 4, 56, 6, 4, 4, 4, 3, 3, 3, 4, 3, 3, 4, 4};
  EXPECT_EQ(nullptr, glbDataToMesh(glbData.data(), glbData.size()));
}


}  // namespace c8
