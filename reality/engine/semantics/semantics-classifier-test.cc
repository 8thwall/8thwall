// Copyright (c) 2022 Niantic, Inc.
// Original Author: Lynn Dang (lynndang@nianticlabs.com)
//
// Tests the semantic classifier to output the right number of classifications in the right
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
    "//reality/engine/semantics:semantics-classifier",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//reality/engine/semantics/data:semanticsnetportrait",
    "//reality/engine/semantics/testdata:test-input-portrait",
  };
  testonly = 1;
}
cc_end(0xaccaf70c);

#include "c8/io/file-io.h"
#include "c8/io/image-io.h"
#include "c8/pixels/gl-pixels.h"
#include "c8/pixels/gpu-pixels-resizer.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/stats/scope-timer.h"
#include "gtest/gtest.h"
#include "reality/engine/deepnets/multiclass-operations.h"
#include "reality/engine/semantics/semantics-classifier.h"

namespace c8 {

static constexpr char SEMANTICS_PORTRAIT_TFLITE_PATH[] =
  "reality/engine/semantics/data/semantics_mobilenetv3_separable_quarter_fp16.tflite";

static constexpr bool WRITE_IMAGE = false;

class SemanticsClassifierTest : public ::testing::Test {};

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

void maybeWriteSemanticsImage(FloatPixels semantics, const String &path) {
  if (WRITE_IMAGE) {
    RGBA8888PlanePixelBuffer outGray(semantics.rows(), semantics.cols());
    auto d = outGray.pixels();
    floatToRgbaGray(semantics, &d, 0.0f, 1.0f);
    writeImage(d, path);
  }
}

void maybeWriteSemanticsImage(ConstOneChannelFloatPixels semantics, const String &path) {
  if (WRITE_IMAGE) {
    RGBA8888PlanePixelBuffer outGray(semantics.rows(), semantics.cols());
    auto d = outGray.pixels();
    floatToRgbaGray(semantics, &d, 0.0f, 1.0f);
    writeImage(d, path);
  }
}

TEST_F(SemanticsClassifierTest, GenerateSemanticsPortrait) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  SemanticsClassifier classifier(readFile(SEMANTICS_PORTRAIT_TFLITE_PATH));

  const auto width = classifier.getInputWidth();
  const auto height = classifier.getInputHeight();

  // Resize image for semantics.
  auto imBuffer = readImageToRGBA("reality/engine/semantics/testdata/test-input-portrait.png");
  auto pix = imBuffer.pixels();
  auto srcTexture = readImageToLinearTexture(pix);

  GpuPixelsResizer inputGen;
  inputGen.drawNextImage();
  inputGen.draw(srcTexture.tex(), width, height);
  inputGen.read();

  auto image = inputGen.claimImage();

  Vector<FloatPixels> semantics;
  classifier.generateSemantics(image, semantics, {0});  // get the first class

  // Check number of classes and dimensions are correct.
  EXPECT_EQ(semantics.size(), 1) << "Model should only has one output class";
  EXPECT_EQ(semantics[0].rows(), height);
  EXPECT_EQ(semantics[0].cols(), width);

  maybeWriteSemanticsImage(
    semantics[0], "/tmp/semantics-classifier-test-GenerateSemanticsPortrait.jpg");

  auto ratio = getRatio(semantics[0]);
  EXPECT_GT(ratio, 0.04f) << "At least 4 percent of the image needs an output";
}

TEST_F(SemanticsClassifierTest, ObtainSemanticsPortrait) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  SemanticsClassifier classifier(readFile(SEMANTICS_PORTRAIT_TFLITE_PATH));

  const auto width = classifier.getInputWidth();
  const auto height = classifier.getInputHeight();

  // Resize image for semantics.
  auto imBuffer = readImageToRGBA("reality/engine/semantics/testdata/test-input-portrait.png");
  auto pix = imBuffer.pixels();
  auto srcTexture = readImageToLinearTexture(pix);

  GpuPixelsResizer inputGen;
  inputGen.drawNextImage();
  inputGen.draw(srcTexture.tex(), width, height);
  inputGen.read();

  auto image = inputGen.claimImage();

  auto semantics = classifier.obtainSemantics(image);

  maybeWriteSemanticsImage(semantics, "/tmp/semantics-classifier-test-ObtainSemanticsPortrait.jpg");

  auto ratio = getRatio(semantics);
  EXPECT_GT(ratio, 0.04f) << "At least 4 percent of the image needs an output";
}

}  // namespace c8
