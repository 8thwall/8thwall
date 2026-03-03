// Copyright (c) 2023 Niantic Labs, Inc.
// Original Author: Nathan Waters (nathan@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":transform-landmarks",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x21e1f341);

#include "gtest/gtest.h"
#include "reality/engine/deepnets/operations/transform-landmarks.h"
#include "tensorflow/lite/delegates/gpu/common/types.h"

namespace c8 {

class TransformLandmarksTest : public ::testing::Test {};

TEST_F(TransformLandmarksTest, TestAddOpToResolver) {
  tflite::MutableOpResolver resolver;
  resolver.AddCustom(
    "TransformLandmarks",
    RegisterTransformLandmarksV2(),
    /*version=*/2);
}

}  // namespace c8
