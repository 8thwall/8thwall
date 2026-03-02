// Copyright (c) 2023 Niantic Labs, Inc.
// Original Author: Nathan Waters (nathan@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":landmarks-to-transform-matrix",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xc82751a8);

#include "gtest/gtest.h"
#include "reality/engine/deepnets/operations/landmarks-to-transform-matrix.h"
#include "tensorflow/lite/delegates/gpu/common/types.h"

namespace c8 {

class LandmarksToTransformMatrixTest : public ::testing::Test {};

TEST_F(LandmarksToTransformMatrixTest, TestAddOpToResolver) {
  tflite::MutableOpResolver resolver;
  resolver.AddCustom(
    "Landmarks2TransformMatrix",
    RegisterLandmarksToTransformMatrixV2(),
    /*version=*/2);
}

}  // namespace c8
