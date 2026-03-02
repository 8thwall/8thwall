// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  data = {
    "//c8/pixels/render/testdata:doty-glb",
  };
  deps = {
    ":model-data",
    ":model-manager",
    "//c8/io:file-io",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x4d1680ec);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/model/model-data.h"
#include "c8/model/model-manager.h"
#include "c8/io/file-io.h"

namespace c8 {

const String GLB_PATH = "c8/pixels/render/testdata/doty.glb";

class ModelManagerTest : public ::testing::Test {};

TEST_F(ModelManagerTest, TestGlb) {
  auto fileData = readFile(GLB_PATH);

  ModelManager manager;
  manager.loadModel(GLB_PATH, fileData.data(), fileData.size());

  Vector<uint8_t> serialized;
  manager.serializeModel(serialized);

  EXPECT_TRUE(hasMesh(serialized.data()));
  EXPECT_FALSE(hasSplatAttributes(serialized.data()));
  EXPECT_FALSE(hasSplatTexture(serialized.data()));
  EXPECT_FALSE(hasSplatMultiTexture(serialized.data()));

  auto mesh = meshView(serialized.data());

  EXPECT_EQ(78088, mesh.geometry.points.size);
  EXPECT_EQ(78088, mesh.geometry.colors.size);
  EXPECT_EQ(78088, mesh.geometry.normals.size);
  EXPECT_EQ(78088, mesh.geometry.uvs.size);
  EXPECT_EQ(142681, mesh.geometry.triangles.size);

  EXPECT_EQ(8192, mesh.texture.cols());
  EXPECT_EQ(4096, mesh.texture.rows());
}

// TODO: Add splat tests once we have compressed splats.

}  // namespace c8
