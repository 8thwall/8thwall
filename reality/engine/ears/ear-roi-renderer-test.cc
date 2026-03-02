// Copyright (c) 2023 Niantic Labs
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":ear-roi-renderer",
    ":ear-types",
    "//c8:string",
    "//c8/io:file-io",
    "//c8/io:image-io",
    "@com_google_googletest//:gtest_main",
  };
  linkstatic = 1;
}
cc_end(0xf7bee81e);

#include <cmath>
#include <cstdio>

#include "c8/io/file-io.h"
#include "c8/io/image-io.h"
#include "c8/stats/scope-timer.h"
#include "c8/string.h"
#include "gtest/gtest.h"
#include "reality/engine/ears/ear-roi-renderer.h"
#include "reality/engine/ears/ear-types.h"

namespace c8 {

class EarRoiRendererTest : public ::testing::Test {};

TEST_F(EarRoiRendererTest, TestViewportForCrop) {
  float w = static_cast<float>(EAR_LANDMARK_DETECTION_INPUT_WIDTH);
  float h = static_cast<float>(EAR_LANDMARK_DETECTION_INPUT_HEIGHT);

  ImageViewport e0 = earViewportForCrop(0);
  EXPECT_FLOAT_EQ(e0.x, 0);
  EXPECT_FLOAT_EQ(e0.y, 0);
  EXPECT_FLOAT_EQ(e0.w, w);
  EXPECT_FLOAT_EQ(e0.h, h);

  ImageViewport e1 = earViewportForCrop(1);
  EXPECT_FLOAT_EQ(e1.x, 0);
  EXPECT_FLOAT_EQ(e1.y, h);
  EXPECT_FLOAT_EQ(e1.w, w);
  EXPECT_FLOAT_EQ(e1.h, h);

  ImageViewport e3 = earViewportForCrop(3);
  EXPECT_FLOAT_EQ(e3.x, w);
  EXPECT_FLOAT_EQ(e3.y, h);
  EXPECT_FLOAT_EQ(e3.w, w);
  EXPECT_FLOAT_EQ(e3.h, h);
}

}  // namespace c8
