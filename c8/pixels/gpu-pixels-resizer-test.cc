// Copyright (c) 2022 Niantic, Inc.
// Original Author: Lynn Dang (lynndang@nianticlabs.com)
//
// Tests gpu resizer to resize images to a specified width and height.

#include "bzl/inliner/rules2.h"

cc_test {
  deps = {
    "//c8/io:image-io",
    "//c8/io:file-io",
    "//c8/pixels:gl-pixels",
    "//c8/pixels:gpu-pixels-resizer",
    "//c8/pixels/opengl:offscreen-gl-context",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//c8/pixels/testdata:frame0",
    "//c8/pixels/testdata:test-input-landscape",
    "//c8/pixels/testdata:test-input-portrait",
  };

  testonly = 1;
}
cc_end(0x1bb86973);

#include "c8/io/file-io.h"
#include "c8/io/image-io.h"
#include "c8/pixels/gl-pixels.h"
#include "c8/pixels/gpu-pixels-resizer.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "c8/stats/scope-timer.h"
#include "gtest/gtest.h"

namespace c8 {

class GPUPixelsResizerTest : public ::testing::Test {};
TEST_F(GPUPixelsResizerTest, ResizeImage) {
  ScopeTimer t("image-generator-test");

  // Expected dimensions.
  const int height = 144;
  const int width = 256;

  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  // Using a correctly sized image, resizing should appear the same.
  auto imBuffer = readImageToRGBA("c8/pixels/testdata/frame_0.jpg");
  auto image = imBuffer.pixels();
  auto srcTexture = readImageToLinearTexture(image);

  GpuPixelsResizer imageGen;
  auto outPixBuffer = imageGen.resizeOnGpu(image, height, width);

  uint8_t *src = image.pixels();
  uint8_t *out = outPixBuffer.pixels().pixels();
  for (int i = 0; i < image.rows(); i++) {
    for (int j = 0; j < image.cols(); j++) {
      EXPECT_EQ(src[0], out[0]);
      EXPECT_EQ(src[1], out[1]);
      EXPECT_EQ(src[2], out[2]);
      EXPECT_EQ(src[3], out[3]);

      src += 4;
      out += 4;
    }
  }

  // Using a landscape image.
  imBuffer = readImageToRGBA("c8/pixels/testdata/test-input-landscape.png");
  image = imBuffer.pixels();
  srcTexture = readImageToLinearTexture(image);
  outPixBuffer = imageGen.resizeOnGpu(image, height, width);

  EXPECT_EQ(outPixBuffer.pixels().rows(), height);
  EXPECT_EQ(outPixBuffer.pixels().cols(), width);

  // Using a portrait image.
  imBuffer = readImageToRGBA("c8/pixels/testdata/test-input-portrait.png");
  image = imBuffer.pixels();
  srcTexture = readImageToLinearTexture(image);
  outPixBuffer = imageGen.resizeOnGpu(image, height, width);

  EXPECT_EQ(outPixBuffer.pixels().rows(), height);
  EXPECT_EQ(outPixBuffer.pixels().cols(), width);
}

TEST_F(GPUPixelsResizerTest, DeferredResizeImagesLandscape) {
  ScopeTimer t("image-generator-test");

  // Expected dimensions.
  const int width = 256;
  const int height = 144;

  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  // Using a correctly sized image, resizing should appear the same.
  auto imBuffer = readImageToRGBA("c8/pixels/testdata/frame_0.jpg");
  auto image = imBuffer.pixels();
  auto srcTexture = readImageToLinearTexture(image);

  GpuPixelsResizer imageGen;
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.drawNextImage();
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.draw(srcTexture.tex(), width, height);
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.read();
  EXPECT_TRUE(imageGen.hasImage());
  auto outPix = imageGen.claimImage();

  uint8_t *src = image.pixels();
  uint8_t *out = outPix.pixels();
  for (int i = 0; i < image.rows(); i++) {
    for (int j = 0; j < image.cols(); j++) {
      EXPECT_EQ(src[0], out[0]);
      EXPECT_EQ(src[1], out[1]);
      EXPECT_EQ(src[2], out[2]);
      EXPECT_EQ(src[3], out[3]);

      src += 4;
      out += 4;
    }
  }

  // Using a landscape image.
  imBuffer = readImageToRGBA("c8/pixels/testdata/test-input-landscape.png");
  image = imBuffer.pixels();
  srcTexture = readImageToLinearTexture(image);
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.drawNextImage();
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.draw(srcTexture.tex(), width, height);
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.read();
  EXPECT_TRUE(imageGen.hasImage());
  outPix = imageGen.claimImage();

  EXPECT_EQ(outPix.rows(), height);
  EXPECT_EQ(outPix.cols(), width);

  // Using a portrait image.
  imBuffer = readImageToRGBA("c8/pixels/testdata/test-input-portrait.png");
  image = imBuffer.pixels();
  srcTexture = readImageToLinearTexture(image);
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.drawNextImage();
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.draw(srcTexture.tex(), width, height);
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.read();
  EXPECT_TRUE(imageGen.hasImage());
  outPix = imageGen.claimImage();

  EXPECT_EQ(outPix.rows(), height);
  EXPECT_EQ(outPix.cols(), width);
}

TEST_F(GPUPixelsResizerTest, DeferredResizeImagesPortrait) {
  ScopeTimer t("image-generator-test");

  // Expected dimensions.
  const int height = 256;
  const int width = 144;

  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  // Using a correctly sized image, resizing should appear the same.
  auto imBuffer = readImageToRGBA("c8/pixels/testdata/frame_0_portrait.jpg");
  auto image = imBuffer.pixels();
  auto srcTexture = readImageToLinearTexture(image);

  GpuPixelsResizer imageGen;
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.drawNextImage();
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.draw(srcTexture.tex(), width, height);
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.read();
  EXPECT_TRUE(imageGen.hasImage());
  auto outPix = imageGen.claimImage();

  uint8_t *src = image.pixels();
  uint8_t *out = outPix.pixels();
  for (int i = 0; i < image.rows(); i++) {
    for (int j = 0; j < image.cols(); j++) {
      EXPECT_EQ(src[0], out[0]);
      EXPECT_EQ(src[1], out[1]);
      EXPECT_EQ(src[2], out[2]);
      EXPECT_EQ(src[3], out[3]);

      src += 4;
      out += 4;
    }
  }

  // Using a portrait image.
  imBuffer = readImageToRGBA("c8/pixels/testdata/test-input-landscape.png");
  image = imBuffer.pixels();
  srcTexture = readImageToLinearTexture(image);
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.drawNextImage();
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.draw(srcTexture.tex(), width, height);
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.read();
  EXPECT_TRUE(imageGen.hasImage());
  outPix = imageGen.claimImage();

  EXPECT_EQ(outPix.rows(), height);
  EXPECT_EQ(outPix.cols(), width);

  // Using a portrait image.
  imBuffer = readImageToRGBA("c8/pixels/testdata/test-input-portrait.png");
  image = imBuffer.pixels();
  srcTexture = readImageToLinearTexture(image);
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.drawNextImage();
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.draw(srcTexture.tex(), width, height);
  EXPECT_FALSE(imageGen.hasImage());

  imageGen.read();
  EXPECT_TRUE(imageGen.hasImage());
  outPix = imageGen.claimImage();

  EXPECT_EQ(outPix.rows(), height);
  EXPECT_EQ(outPix.cols(), width);
}
}  // namespace c8
