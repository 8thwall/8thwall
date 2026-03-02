// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":assimp-assets",
    "//c8/io:file-io",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//c8/pixels/render/testdata:doty-glb",
  };
}
cc_end(0xd98b8614);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/io/file-io.h"
#include "c8/pixels/render/assimp-assets.h"

namespace c8 {

const String GLB_PATH = "c8/pixels/render/testdata/doty.glb";

class AssimpAssetsTest : public ::testing::Test {};

TEST_F(AssimpAssetsTest, TestFile) {
  auto mesh = readDiffuseJpgMesh(GLB_PATH);
  EXPECT_NE(nullptr, mesh.get());
  EXPECT_EQ(78088, mesh->geometry().vertices().size());
  EXPECT_EQ(78088, mesh->geometry().uvs().size());
  EXPECT_EQ(142681, mesh->geometry().triangles().size());
  EXPECT_EQ(8192, mesh->material().colorTexture()->rgbaPixels().cols());
  EXPECT_EQ(4096, mesh->material().colorTexture()->rgbaPixels().rows());
}

TEST_F(AssimpAssetsTest, TestData) {
  auto mesh = readDiffuseJpgMeshGlb(readFile(GLB_PATH));
  EXPECT_NE(nullptr, mesh.get());
  EXPECT_EQ(78088, mesh->geometry().vertices().size());
  EXPECT_EQ(78088, mesh->geometry().uvs().size());
  EXPECT_EQ(142681, mesh->geometry().triangles().size());
  EXPECT_EQ(8192, mesh->material().colorTexture()->rgbaPixels().cols());
  EXPECT_EQ(4096, mesh->material().colorTexture()->rgbaPixels().rows());
}

}  // namespace c8
