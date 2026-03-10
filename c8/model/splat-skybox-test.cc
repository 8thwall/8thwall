// Copyright (c) 2024 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":splat-skybox",
    "//c8/geometry:load-splat",
    "//c8/io:image-io",
    "//c8/pixels/opengl:offscreen-gl-context",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//c8/model/models:minion-spz",
  };
}
cc_end(0x53a7ac25);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/geometry/load-splat.h"
#include "c8/io/image-io.h"
#include "c8/model/splat-skybox.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"

namespace c8 {

bool WRITE_IMAGE = false;

class SplatSkyboxTest : public ::testing::Test {};

TEST_F(SplatSkyboxTest, TestMeshGeometry) {
  // Create an offscreen context.
  auto ctx = OffscreenGlContext::createRGBA8888Context();

  auto fullSplat = splatAttributes(loadSplatSpzFilePacked("c8/model/models/minion.spz"), true);

  int faceSize = 512;
  float radius = 2.0f;
  auto bakedSplat = bakeSplatSkybox(fullSplat, radius, faceSize);

  EXPECT_TRUE(bakedSplat.header.hasSkybox);
  EXPECT_LT(bakedSplat.positions.size(), fullSplat.positions.size());
  EXPECT_EQ(bakedSplat.rotations.size(), bakedSplat.positions.size());
  EXPECT_EQ(bakedSplat.scales.size(), bakedSplat.positions.size());
  EXPECT_EQ(bakedSplat.colors.size(), bakedSplat.positions.size());
  EXPECT_EQ(bakedSplat.shRed.size(), bakedSplat.positions.size());
  EXPECT_EQ(bakedSplat.shGreen.size(), bakedSplat.positions.size());
  EXPECT_EQ(bakedSplat.shBlue.size(), bakedSplat.positions.size());

  for (auto pt : bakedSplat.positions) {
    auto r2 = pt.x() * pt.x() + pt.y() * pt.y() + pt.z() * pt.z();
    EXPECT_LE(r2, radius * radius);
  }

  if (WRITE_IMAGE) {
    auto skybox = mergeSkybox(bakedSplat.skybox.skybox());
    writeImage(skybox.pixels(), "/tmp/splat-skybox-test.png");
  }
}

}  // namespace c8
