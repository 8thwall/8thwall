// Copyright (c) 2022 Niantic Labs, Inc.
// Original Author: Lynn Dang & Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":tflite-model-operations",
    "//c8:string",
    "//c8/io:file-io",
    "//c8/io:image-io",
    "//reality/engine/deepnets:tflite-debug",
    "//third_party/half:half",
    "@com_google_googletest//:gtest_main",
  };
  linkstatic = 1;
}
cc_end(0xc94cecbe);

#include <cmath>
#include <cstdio>

#include "c8/io/file-io.h"
#include "c8/io/image-io.h"
#include "c8/string.h"
#include "gtest/gtest.h"
#include "reality/engine/deepnets/tflite-debug.h"
#include "reality/engine/deepnets/tflite-model-operations.h"
#include "third_party/half/half.hpp"

namespace c8 {

class TFLiteModelOperationsTest : public ::testing::Test {};

TEST_F(TFLiteModelOperationsTest, TestRotateTensorsCCW) {
  // Testing 1 input channel, 1 output channel.
  // inputs = 1, rows = 3, cols = 3, outputs = 1
  float src0[9] = {
    // clang-format off
    0.4, 0.9, -0.8,  // row 1
    0.5, 1.0,  0.0,  // row 2
    0.6, 0.8, -0.9,  // row 3
    // clang-format on
  };

  // inputs = 1, rows = 3, cols = 3, outputs = 1
  float res0[9] = {
    // clang-format off
    -0.8, 0.0, -0.9,  // row 1
     0.9, 1.0,  0.8,  // row 2
     0.4, 0.5,  0.6,  // row 3
    // clang-format on
  };

  half_float::half src0Half[9];
  half_float::half res0Half[9];
  for (int i = 0; i < 9; i++) {
    src0Half[i] = half_float::half_cast<half_float::half>(src0[i]);
    res0Half[i] = half_float::half_cast<half_float::half>(res0[i]);
  }

  rotateSquareTensorFP16(src0Half, 1, 1, 3, false);

  for (int i = 0; i < 9; i++) {
    EXPECT_EQ(src0Half[i], res0Half[i]);
  }

  // Testing 2 input channels, 1 output channel.
  // inputs = 2, rows = 3, cols = 3, outputs = 1
  float src1[18] = {
    // clang-format off
    // input channel 0
    -0.9, 0.0, -0.8,  // row 1
     0.8, 1.0,  0.9,  // row 2
     0.6, 0.5,  0.4,  // row 3
    // input channel 1
    0.1, 0.7, -0.1,  // row 1
    0.2, 0.8,  0.0,  // row 2
    0.3, 0.9, -0.2,  // row 3
    // clang-format on
  };

  // inputs 2, rows = 3, cols = 3, outputs = 1
  float res1[18] = {
    // clang-format off
    // input channel 0
    -0.8, 0.9, 0.4,  // row 1
     0.0, 1.0, 0.5,  // row 2
    -0.9, 0.8, 0.6,  // row 3
    // input channel 1
    -0.1, 0.0, -0.2,  // row 1
     0.7, 0.8,  0.9,  // row 2
     0.1, 0.2,  0.3,  // row 3
    // clang-format on
  };

  half_float::half src1Half[18];
  half_float::half res1Half[18];
  for (int i = 0; i < 18; i++) {
    src1Half[i] = half_float::half_cast<half_float::half>(src1[i]);
    res1Half[i] = half_float::half_cast<half_float::half>(res1[i]);
  }

  rotateSquareTensorFP16(src1Half, 2, 1, 3, false);
  for (int i = 1; i < 18; i++) {
    EXPECT_EQ(src1Half[i], res1Half[i]);
  }

  // Testing 2 input channels, 2 output channels.
  // inputs = 2, rows = 3, cols = 3, outputs = 2
  float src2[36] = {
    // clang-format off
    // input channel 0
    -1.0, 0.5, 0.0, 1.0, -1.0, -1.0,  // row 1
     1.0, 0.5, 1.0, 1.0,  1.0,  0.0,  // row 2
     0.5, 0.5, 0.5, 1.0,  0.5, -1.0,  // row 3
    // input channel 1
    0.5, -1.0, 1.0, 0.0, -1.0, -1.0,  // row 1
    0.5,  1.0, 1.0, 1.0,  0.0,  1.0,  // row 2
    0.5,  0.5, 1.0, 0.5, -1.0,  0.5,  // row 3
    // clang-format on
  };

  // inputs 2, rows = 3, cols = 3, outputs = 2
  float res2[36] = {
    // clang-format off
    // input channel 0
    -1.0, -1.0, 1.0, 0.0, 0.5, -1.0,  // row 1
     0.0,  1.0, 1.0, 1.0, 0.5,  1.0,  // row 2
    -1.0,  0.5, 1.0, 0.5, 0.5,  0.5,  // row 3
    // input channel 1
    -1.0, -1.0, 0.0, 1.0, -1.0, 0.5,  // row 1
     1.0,  0.0, 1.0, 1.0,  1.0, 0.5,  // row 2
     0.5, -1.0, 0.5, 1.0,  0.5, 0.5,  // row 3
    // clang-format on
  };

  half_float::half src2Half[36];
  half_float::half res2Half[36];
  for (int i = 0; i < 36; i++) {
    src2Half[i] = half_float::half_cast<half_float::half>(src2[i]);
    res2Half[i] = half_float::half_cast<half_float::half>(res2[i]);
  }

  rotateSquareTensorFP16(src2Half, 2, 2, 3, false);
  for (int i = 1; i < 36; i++) {
    EXPECT_EQ(src2Half[i], res2Half[i]);
  }
}

TEST_F(TFLiteModelOperationsTest, TestRotateTensorsCW) {
  // Testing 1 input channel, 1 output channel.
  // inputs = 1, rows = 3, cols = 3, outputs = 1
  float src0[9] = {
    // clang-format off
    -1.0, 0.0, -1.0,  // row 1
     0.8, 1.0,  0.9,  // row 2
     0.4, 0.5,  0.6,  // row 3
    // clang-format on
  };

  // inputs = 1, rows = 3, cols = 3, outputs = 1
  float res0[9] = {
    // clang-format off
    0.4, 0.8, -1.0,  // row 1
    0.5, 1.0,  0.0,  // row 2
    0.6, 0.9, -1.0,  // row 3
    // clang-format on
  };

  half_float::half src0Half[9];
  half_float::half res0Half[9];
  for (int i = 0; i < 9; i++) {
    src0Half[i] = half_float::half_cast<half_float::half>(src0[i]);
    res0Half[i] = half_float::half_cast<half_float::half>(res0[i]);
  }

  rotateSquareTensorFP16(src0Half, 1, 1, 3, true);

  for (int i = 0; i < 9; i++) {
    EXPECT_EQ(src0Half[i], res0Half[i]);
  }

  // Testing 2 input channels, 1 output channel.
  // inputs 2, rows = 3, cols = 3, outputs = 1
  float src1[18] = {
    // clang-format off
    // input channel 0
    -1.0, 0.8, 0.6,  // row 1
     0.0, 1.0, 0.5,  // row 2
    -1.0, 0.9, 0.4,  // row 3
    // input channel 1
    -1.0, 0.0, -1.0,  // row 1
     0.2, 1.0,  0.6,  // row 2
     0.7, 0.5,  0.8,  // row 3
    // clang-format on
  };

  // inputs = 2, rows = 3, cols = 3, outputs = 1
  float res1[18] = {
    // clang-format off
    // input channel 0
    -1.0, 0.0, -1.0,  // row 1
     0.9, 1.0,  0.8,  // row 2
     0.4, 0.5,  0.6,  // row 3
    // input channel 1
    0.7, 0.2, -1.0,  // row 1
    0.5, 1.0,  0.0,  // row 2
    0.8, 0.6, -1.0,  // row 3
    // clang-format on
  };

  half_float::half src1Half[18];
  half_float::half res1Half[18];
  for (int i = 0; i < 18; i++) {
    src1Half[i] = half_float::half_cast<half_float::half>(src1[i]);
    res1Half[i] = half_float::half_cast<half_float::half>(res1[i]);
  }

  rotateSquareTensorFP16(src1Half, 2, 1, 3, true);
  for (int i = 1; i < 18; i++) {
    EXPECT_EQ(src1Half[i], res1Half[i]);
  }

  // Testing 2 input channels, 2 output channels.
  // inputs 2, rows = 3, cols = 3, outputs = 2
  float src2[36] = {
    // clang-format off
    // input channel 0
    -1.0, -1.0, 1.0, 0.0, 0.5, -1.0,  // row 1
     0.0,  1.0, 1.0, 1.0, 0.5,  1.0,  // row 2
    -1.0,  0.5, 1.0, 0.5, 0.5,  0.5,  // row 3
    // input channel 1
    -1.0, -1.0, 0.0, 1.0, -1.0, 0.5,  // row 1
     1.0,  0.0, 1.0, 1.0,  1.0, 0.5,  // row 2
     0.5, -1.0, 0.5, 1.0,  0.5, 0.5,  // row 3
    // clang-format on
  };

  // inputs = 2, rows = 3, cols = 3, outputs = 1
  float res2[36] = {
    // clang-format off
    // input channel 0
    -1.0, 0.5, 0.0, 1.0, -1.0, -1.0,  // row 1
     1.0, 0.5, 1.0, 1.0,  1.0,  0.0,  // row 2
     0.5, 0.5, 0.5, 1.0,  0.5, -1.0,  // row 3
    // input channel 1
    0.5, -1.0, 1.0, 0.0, -1.0, -1.0,  // row 1
    0.5,  1.0, 1.0, 1.0,  0.0,  1.0,  // row 2
    0.5,  0.5, 1.0, 0.5, -1.0,  0.5,  // row 3
    // clang-format on
  };

  half_float::half src2Half[36];
  half_float::half res2Half[36];
  for (int i = 0; i < 36; i++) {
    src2Half[i] = half_float::half_cast<half_float::half>(src2[i]);
    res2Half[i] = half_float::half_cast<half_float::half>(res2[i]);
  }

  rotateSquareTensorFP16(src2Half, 2, 2, 3, true);
  for (int i = 1; i < 36; i++) {
    EXPECT_EQ(src2Half[i], res2Half[i]);
  }
}

}  // namespace c8
