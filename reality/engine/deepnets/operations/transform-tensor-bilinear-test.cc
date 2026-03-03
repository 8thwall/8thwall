// Copyright (c) 2023 Niantic Labs, Inc.
// Original Author: Nathan Waters (nathan@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":transform-tensor-bilinear",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x661c1b01);

#include "gtest/gtest.h"
#include "reality/engine/deepnets/operations/transform-tensor-bilinear.h"
#include "tensorflow/lite/delegates/gpu/common/types.h"

namespace c8 {

class TransformTensorBilinearTest : public ::testing::Test {};

TEST_F(TransformTensorBilinearTest, TestAddOpToResolver) {
  tflite::MutableOpResolver resolver;
  resolver.AddCustom(
    "TransformTensorBilinear",
    RegisterTransformTensorBilinearV2(),
    /*version=*/2);
}

}  // namespace c8
