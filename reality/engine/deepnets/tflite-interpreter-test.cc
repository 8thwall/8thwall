// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":tflite-interpreter",
    "//c8:string",
    "//c8/io:file-io",
    "//c8/io:image-io",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//reality/engine/deepnets/testdata:faces",
    "//third_party/mediapipe/models:face-detection-front",
  };
  linkstatic = 1;
}
cc_end(0x5b093f32);

#include <cmath>
#include <cstdio>

#include "c8/io/file-io.h"
#include "c8/io/image-io.h"
#include "c8/string.h"
#include "gtest/gtest.h"
#include "reality/engine/deepnets/tflite-interpreter.h"

namespace c8 {

class TFLiteInterpreterTest : public ::testing::Test {};

static constexpr char MODEL_PATH[] = "third_party/mediapipe/models/face_detection_front.tflite";
static constexpr char IMAGE_PATH[] = "reality/engine/deepnets/testdata/faces.jpg";

TEST_F(TFLiteInterpreterTest, TestModel) {
  // Load a TFLite interpreter with the tensor flow model.
  TFLiteInterpreter interpreter(readFile(MODEL_PATH), 1);
  auto inputDims = interpreter.getInputTensorDims(0);
  EXPECT_EQ(inputDims.size(), 4);
  EXPECT_EQ(inputDims[0], 1);
  EXPECT_EQ(inputDims[1], 128);
  EXPECT_EQ(inputDims[2], 128);
  EXPECT_EQ(inputDims[3], 3);
  auto out0Dims = interpreter.getOutputTensorDims(0);
  EXPECT_EQ(out0Dims.size(), 3);
  EXPECT_EQ(out0Dims[0], 1);
  EXPECT_EQ(out0Dims[1], 896);
  EXPECT_EQ(out0Dims[2], 16);
  auto out1Dims = interpreter.getOutputTensorDims(1);
  EXPECT_EQ(out1Dims.size(), 3);
  EXPECT_EQ(out1Dims[0], 1);
  EXPECT_EQ(out1Dims[1], 896);
  EXPECT_EQ(out1Dims[2], 1);
}

TEST_F(TFLiteInterpreterTest, TestInvoke) {
  // Read in a (slightly too big) image. The test image is 244x128, we need 128x128.
  auto im = readImageToRGBA(IMAGE_PATH);
  auto pix = im.pixels();

  // Take a hardcoded crop of the test image with a well centered face.
  int cropRowStart = 0;
  int cropColStart = 110;
  int cropRows = 128;
  int cropCols = 128;
  RGBA8888PlanePixels cropPix(
    cropRows,
    cropCols,
    pix.rowBytes(),
    pix.pixels() + cropRowStart * pix.rowBytes() + cropColStart * 4);

  // Load a TFLite interpreter with the tensor flow model.
  TFLiteInterpreter interpreter(readFile(MODEL_PATH), 1);

  // Copy the image pixels to the floating point tensor input, scaling 0:255 to -1:1.
  auto *dst = interpreter->typed_input_tensor<float>(0);
  const float s = 1.0f / 127.5f;
  const uint8_t *src = cropPix.pixels();
  const uint8_t *srcStart = src;
  const uint8_t *srcEnd = srcStart + cropPix.cols() * 4;
  for (int r = 0; r < cropPix.rows(); ++r) {
    src = srcStart;
    while (src != srcEnd) {
      dst[0] = src[0] * s - 1.0f;
      dst[1] = src[1] * s - 1.0f;
      dst[2] = src[2] * s - 1.0f;
      src += 4;
      dst += 3;
    }
    srcStart += cropPix.rowBytes();
    srcEnd += cropPix.rowBytes();
  }

  // Run the tflite model on the input.
  interpreter->Invoke();

  // For this model, the output class is the second tensor output of whether or not a face is
  // present at a number of fixed image locations. Count how many of these locations look like a
  // face.
  auto numBoxes = interpreter->tensor(interpreter->outputs()[1])->dims->data[1];
  int hits = 0;
  auto *classOutput = interpreter->typed_output_tensor<float>(1);
  float thresh = 0.75f;
  float st = -std::log((1.0f - thresh) / thresh);
  for (int i = 0; i < numBoxes; ++i) {
    if (classOutput[i] > st) {
      ++hits;
    }
  }

  // The face is found at a cluster of 10 of these locations, and further processing is needed to
  // get it down to one face.
  EXPECT_EQ(hits, 10);

  // Fill the model input with black pixels (-1 for this model) and re-run.
  dst = interpreter->typed_input_tensor<float>(0);
  std::fill(dst, dst + 128 * 128 * 3, -1.0f);
  interpreter->Invoke();

  hits = 0;
  for (int i = 0; i < numBoxes; ++i) {
    if (classOutput[i] > st) {
      ++hits;
    }
  }

  // We expect no faces in a black image.
  EXPECT_EQ(hits, 0);

  // We also expect no faces in a white image.
  std::fill(dst, dst + 128 * 128 * 3, 1.0f);
  interpreter->Invoke();
  hits = 0;
  for (int i = 0; i < numBoxes; ++i) {
    if (classOutput[i] > st) {
      ++hits;
    }
  }
  EXPECT_EQ(hits, 0);
}

}  // namespace c8
