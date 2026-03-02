// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)
//
// Tests the semantic operations to output the right number of classifications in the right
// dimensions. Additionally checks with a test image that it scores the right classifications.

#include "bzl/inliner/rules2.h"

cc_test {
  deps = {
    "//c8/io:image-io",
    "//c8/io:file-io",
    "//c8/pixels:gl-pixels",
    "//c8/pixels:gpu-pixels-resizer",
    "//c8/pixels/opengl:offscreen-gl-context",
    "//reality/engine/deepnets:multiclass-operations",
    "@com_google_googletest//:gtest_main",
  };

  testonly = 1;
}
cc_end(0xc94df2c8);

#include "c8/io/file-io.h"
#include "c8/io/image-io.h"
#include "c8/pixels/gl-pixels.h"
#include "c8/pixels/gpu-pixels-resizer.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "c8/stats/scope-timer.h"
#include "gtest/gtest.h"
#include "reality/engine/deepnets/multiclass-operations.h"

namespace c8 {

class MultiClassOperationTest : public ::testing::Test {};
TEST_F(MultiClassOperationTest, MultiClassBinaryMap) {
  // Generate semantics and corresponding class map from the correct dimension.
  ScopeTimer t("unit test");

  // rows = 3, cols = 3, row elements = 3
  float src0[9] = {
    // clang-format off
    0.1, 0.2, 0.8,  // row 1
    0.5, 0.6, 0.7,  // row 2
    0.9, 0.3, 0.3,  // row 3
    // clang-format on
  };
  // rows = 3, cols = 3, row elements = 3
  float src1[9] = {
    // clang-format off
    0.8, 0.9, 0.3,  // row 1
    0.5, 0.5, 0.3,  // row 2
    0.1, 0.8, 0.2,  // row 3
    // clang-format on
  };

  uint8_t class0Pix[36] = {
    // clang-format off
    0, 0, 0, 0,  0, 0, 0, 0,  255, 255, 255, 255,  // row 1
    255, 255, 255, 255,  255, 255, 255, 255,  255, 255, 255, 255,  // row 2
    255, 255, 255, 255,  0, 0, 0, 0,  0, 0, 0, 0,  // row 3
    // clang-format on
  };

  FloatPixels semPix0(3, 3, 3, src0);
  FloatPixels semPix1(3, 3, 3, src1);

  Vector<FloatPixels> semantics;
  semantics.push_back(semPix0);
  semantics.push_back(semPix1);

  const int classId = 0;
  const float threshold = 0.4;  // Threshold to count whether the semantic score is high enough.

  // compare the results
  auto buffer = RGBA8888PlanePixelBuffer(3, 3);
  auto outPix = buffer.pixels();

  multiClassBinaryMap(semantics, outPix, classId, threshold);

  RGBA8888PlanePixels expected(3, 3, 12, class0Pix);

  auto pix = outPix.pixels();
  auto expPix = expected.pixels();
  for (int r = 0; r < 3; ++r) {
    for (int c = 0; c < 3; ++c) {
      EXPECT_EQ(pix[0], expPix[0]);
      EXPECT_EQ(pix[1], expPix[1]);
      EXPECT_EQ(pix[2], expPix[2]);

      pix += 4;
      expPix += 4;
    }
  }
}

}  // namespace c8
