// Copyright (c) 2023 Niantic, Inc.
// Original Author: Lynn Dang (lynndang@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":tflite-interpreter",
    "//c8:string",
    "//c8/io:file-io",
    "//c8/io:image-io",
    "//c8/pixels:gl-pixels",
    "//c8/pixels:gpu-pixels-resizer",
    "//c8/pixels/opengl:offscreen-gl-context",
    "//reality/engine/deepnets/testdata:embedded-semantics",
    "//reality/engine/deepnets:tflite-multiclassifier",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//reality/engine/deepnets/testdata:office",
  };
  testonly = 1;
}
cc_end(0xfda2e1c9);

#include <cmath>
#include <cstdio>

#include "c8/io/file-io.h"
#include "c8/io/image-io.h"
#include "c8/pixels/gl-pixels.h"
#include "c8/pixels/gpu-pixels-resizer.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "c8/string.h"
#include "gtest/gtest.h"
#include "reality/engine/deepnets/testdata/embedded-semantics.h"
#include "reality/engine/deepnets/tflite-multiclassifier.h"

namespace c8 {

class TFLiteMultiClassTest : public ::testing::Test {};

static constexpr char IMAGE_9x16_PATH[] = "reality/engine/deepnets/testdata/office_w360_h640.png";

float getRatio(ConstFloatPixels pixels, float threshold = 0.6f) {
  auto width = pixels.cols();
  auto height = pixels.rows();
  const float *ptr = pixels.pixels();
  int passThreshold = 0;
  for (int i = 0; i < width * height; ++i) {
    if (ptr[i] > threshold) {
      passThreshold++;
    }
  }
  return static_cast<float>(passThreshold) / (width * height);
}

TEST_F(TFLiteMultiClassTest, TestInvoke) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  const Vector<uint8_t> tfliteFile(
    embeddedSemanticsFp32TfliteData,
    embeddedSemanticsFp32TfliteData + embeddedSemanticsFp32TfliteSize);
  constexpr int cacheSize = 8000000;
  TfLiteMultiClassifier classifier(tfliteFile, cacheSize, 256, 144, 20);

  const auto width = classifier.getInputWidth();
  const auto height = classifier.getInputHeight();

  // Resize image for semantics.
  auto imBuffer = readImageToRGBA(IMAGE_9x16_PATH);
  auto pix = imBuffer.pixels();
  auto srcTexture = readImageToLinearTexture(pix);

  GpuPixelsResizer inputGen;
  inputGen.drawNextImage();
  inputGen.draw(srcTexture.tex(), width, height);
  inputGen.read();

  auto image = inputGen.claimImage();

  Vector<FloatPixels> semantics;
  classifier.processOutput(image);
  classifier.generateOutput(image, semantics, {1});  // get the ground

  // Check number of classes and dimensions are correct.
  EXPECT_EQ(semantics.size(), 1) << "Model should only has one output class";
  EXPECT_EQ(semantics[0].rows(), height);
  EXPECT_EQ(semantics[0].cols(), width);

  auto ratio = getRatio(semantics[0]);
  EXPECT_GT(ratio, 0.04f) << "At least 4 percent of the image needs an output";
}

TEST_F(TFLiteMultiClassTest, TestSingleOutput) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  const Vector<uint8_t> tfliteFile(
    embeddedSemanticsFp32TfliteData,
    embeddedSemanticsFp32TfliteData + embeddedSemanticsFp32TfliteSize);
  constexpr int cacheSize = 8000000;
  TfLiteMultiClassifier classifier(tfliteFile, cacheSize, 256, 144, 20);

  const auto width = classifier.getInputWidth();
  const auto height = classifier.getInputHeight();

  // Resize image for semantics.
  auto imBuffer = readImageToRGBA(IMAGE_9x16_PATH);
  auto pix = imBuffer.pixels();
  auto srcTexture = readImageToLinearTexture(pix);

  GpuPixelsResizer inputGen;
  inputGen.drawNextImage();
  inputGen.draw(srcTexture.tex(), width, height);
  inputGen.read();

  auto image = inputGen.claimImage();

  Vector<FloatPixels> semantics;

  classifier.processOutput(image);
  for (int i = 0; i < 20; i++) {
    auto semantics = classifier.obtainSingleOutput(image, i);

    EXPECT_EQ(semantics.rows(), height);
    EXPECT_EQ(semantics.cols(), width);
  }
}

TEST_F(TFLiteMultiClassTest, TestOutTensorToClass) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  const Vector<uint8_t> tfliteFile(
    embeddedSemanticsFp32TfliteData,
    embeddedSemanticsFp32TfliteData + embeddedSemanticsFp32TfliteSize);
  constexpr int cacheSize = 8000000;

  // Using a dummy number of classes to specifically test outTensorToClassImage.
  TfLiteMultiClassifier classifier(tfliteFile, cacheSize, 256, 144, 20);

  float src[80] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                   1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                   1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                   1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  OneChannelFloatPixelBuffer destBuffer = OneChannelFloatPixelBuffer(2, 2);

  float *srcPtr = src;
  for (int j = 1; j <= 4; j++) {
    classifier.outTensorToClassImage(srcPtr, destBuffer.pixels());
    for (int i = 0; i < 4; i++) {
      EXPECT_EQ(j, destBuffer.pixels().pixels()[i]);
    }
    srcPtr++;
  }
}

}  // namespace c8
