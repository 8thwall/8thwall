// Copyright (c) 2023 Niantic, Inc.
// Original Author: Lynn Dang (lynndang@nianticlabs.com)
//
// Tests the selfie segmentation to output the right number of classifications in the right
// dimensions. Additionally checks with a test image that it scores the right classifications.

#include "bzl/inliner/rules2.h"

cc_test {
  deps = {
    "//c8/io:image-io",
    "//c8/io:file-io",
    "//c8/pixels:gl-pixels",
    "//c8/pixels:gpu-pixels-resizer",
    "//c8/pixels/opengl:offscreen-gl-context",
    "//reality/engine/selfie-segmentation:selfie-segmentation",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//reality/engine/selfie-segmentation/data:selfiemulticlass",
    "//reality/engine/testdata:realface",
  };
  testonly = 1;
}
cc_end(0xb4c5e9e9);

#include "c8/io/file-io.h"
#include "c8/io/image-io.h"
#include "c8/pixels/gl-pixels.h"
#include "c8/pixels/gpu-pixels-resizer.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/stats/scope-timer.h"
#include "gtest/gtest.h"
#include "reality/engine/selfie-segmentation/selfie-segmentation.h"

namespace c8 {

static constexpr char SELFIE_SEGMENTAION_TFLITE_PATH[] =
  "reality/engine/selfie-segmentation/data/selfie_multiclass_256x256.tflite";
static constexpr char IMAGE_PATH[] =
  "reality/engine/testdata/real_test_face.jpg";

static constexpr bool WRITE_IMAGE = false;

class SelfieSegmentationTest : public ::testing::Test {};

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

void maybeWriteSegmentationImage(FloatPixels segmentation, const String &path) {
  if (WRITE_IMAGE) {
    RGBA8888PlanePixelBuffer outGray(segmentation.rows(), segmentation.cols());
    auto d = outGray.pixels();
    floatToRgbaGray(segmentation, &d, 0.0f, 1.0f);
    writeImage(d, path);
  }
}

void maybeWriteSegmentationImage(ConstOneChannelFloatPixels segmentation, const String &path) {
  if (WRITE_IMAGE) {
    RGBA8888PlanePixelBuffer outGray(segmentation.rows(), segmentation.cols());
    auto d = outGray.pixels();
    floatToRgbaGray(segmentation, &d, 0.0f, 1.0f);
    writeImage(d, path);
  }
}

void maybeWriteInputImage(ConstRGBA8888PlanePixels inputImage, const String &path) {
  if (WRITE_IMAGE) {
    writeImage(inputImage, path);
  }
}

TEST_F(SelfieSegmentationTest, GenerateSegmentation) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  SelfieSegmentation classifier(readFile(SELFIE_SEGMENTAION_TFLITE_PATH));

  const auto width = classifier.getInputWidth();
  const auto height = classifier.getInputHeight();

  // Resize image for semantics.
  auto imBuffer = readImageToRGBA(IMAGE_PATH);
  auto pix = imBuffer.pixels();
  auto srcTexture = readImageToLinearTexture(pix);

  GpuPixelsResizer inputGen;
  inputGen.drawNextImage();
  inputGen.draw(srcTexture.tex(), width, height);
  inputGen.read();

  auto image = inputGen.claimImage();

  maybeWriteInputImage(image, "/tmp/segmentation-input.jpg");

  Vector<FloatPixels> segmentations;
  classifier.generateSegmentation(image, segmentations, {0, 1, 2, 3, 4, 5});

  // Check number of classes and dimensions are correct.
  EXPECT_EQ(segmentations.size(), 6) << "Model has 6 output options";
  EXPECT_EQ(segmentations[1].rows(), height);
  EXPECT_EQ(segmentations[1].cols(), width);

  maybeWriteSegmentationImage(
    segmentations[1], "/tmp/semantics-classifier-test-GenerateSemanticsPortrait.jpg");

  auto ratio = getRatio(segmentations[1]);
  EXPECT_GT(ratio, 0.02f) << "At least 2 percent of the image needs an output";
}

}  // namespace c8
