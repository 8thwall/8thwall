// Copyright (c) 2025 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  data = {
    "//c8/pixels/render/testdata:doty-glb",
  };
  deps = {
    ":glb-io",
    "//c8:string",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xcfa3d6cc);

#include "c8/c8-log.h"
#include "c8/io/glb-io.h"
#include "gtest/gtest.h"

namespace c8 {

const String GLB_PATH = "c8/pixels/render/testdata/doty.glb";

class GlbIoTest : public ::testing::Test {};

TEST_F(GlbIoTest, ParseRaw) {
  auto glb = readGlbRaw(GLB_PATH);
  EXPECT_FALSE(glb.empty());
}

}  // namespace c8
