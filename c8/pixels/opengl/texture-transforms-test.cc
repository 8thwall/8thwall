// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":gl-headers",
    ":offscreen-gl-context",
    ":gl-texture",
    ":gl-framebuffer",
    ":gl-pixel-buffer",
    ":texture-transforms",
    "//c8/pixels:pixel-buffer",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x859614ea);

#include "c8/pixels/opengl/gl-texture.h"

#include "c8/pixels/opengl/gl-pixel-buffer.h"
#include "c8/pixels/opengl/gl-framebuffer.h"
#include "c8/pixels/opengl/gl.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "c8/pixels/opengl/texture-transforms.h"
#include "c8/pixels/pixel-buffer.h"
#include "gtest/gtest.h"

namespace c8 {

namespace {

constexpr int64_t next(int64_t &s) {
  s = (s * 4103861 + 4103881) % 4103887;
  return s;
}

void fillTestImage(RGBA8888PlanePixels tp) {
  int64_t s = 0;
  for (int r = 0; r < tp.rows(); ++r) {
    for (int c = 0; c < tp.cols(); ++c) {
      auto *p = tp.pixels() + r * tp.rowBytes() + c * 4;
      p[0] = next(s) % 255;
      p[1] = next(s) % 255;
      p[2] = next(s) % 255;
      p[3] = 255;
    }
  }
}

bool eqImg(RGBA8888PlanePixels a, RGBA8888PlanePixels b, int thresh) {
  bool passed = true;
  for (int r = 0; r < a.rows(); ++r) {
    for (int c = 0; c < a.cols(); ++c) {
      auto *pa = a.pixels() + r * a.rowBytes() + c * 4;
      auto *pb = b.pixels() + r * b.rowBytes() + c * 4;
      passed &= std::abs(pa[0] - pb[0]) <= thresh;
      passed &= std::abs(pa[1] - pb[1]) <= thresh;
      passed &= std::abs(pa[2] - pb[2]) <= thresh;
    }
  }
  return passed;
}

}  // namespace

class GlTextureTest : public ::testing::Test {};

TEST_F(GlTextureTest, TextureReadWrite) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  // Create the texture copy method.
  auto copyTexture2D = compileCopyTexture2D();

  constexpr int WIDTH = 128;
  constexpr int HEIGHT = 64;

  RGBA8888PlanePixelBuffer src(HEIGHT, WIDTH);

  GlTexture2D srcTex = makeLinearRGBA8888Texture2D(WIDTH, HEIGHT);

  // Fill the test image with data on the CPU.
  fillTestImage(src.pixels());

  // Upload the CPU image to the GPU texture.
  srcTex.bind();
  srcTex.updateImage(src.pixels().pixels());

  // Allocate the destination framebuffer.
  GlFramebufferObject dest;
  dest.initialize(makeLinearRGBA8888Texture2D(WIDTH, HEIGHT), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0);

  // Copy texture from source to destination.
  copyTexture2D(srcTex.tex(), &dest);

  // We are rendering a gray triangle in the upper-left half of the frame, Y-flipped so that the
  // readPixels below would appear as if the image was rendered to a surface.
  RGBA8888PlanePixelBuffer outpix(HEIGHT, WIDTH);
  readFramebufferRGBA8888Pixels(dest, outpix.pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Compare that the src and dst images are the same.
  EXPECT_TRUE(eqImg(outpix.pixels(), src.pixels(), 0));
}

#if C8_OPENGL_VERSION_3
TEST_F(GlTextureTest, TextureReadWriteGlPixelBuffer) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  // Create the texture copy method.
  auto copyTexture2D = compileCopyTexture2D();

  constexpr int WIDTH = 128;
  constexpr int HEIGHT = 64;

  RGBA8888PlanePixelBuffer src(HEIGHT, WIDTH);

  GlTexture2D srcTex = makeLinearRGBA8888Texture2D(WIDTH, HEIGHT);

  // Fill the test image with data on the CPU.
  fillTestImage(src.pixels());

  // Upload the CPU image to the GPU texture.
  srcTex.bind();
  srcTex.updateImage(src.pixels().pixels());

  // Allocate the destination framebuffer.
  GlFramebufferObject dest;
  dest.initialize(makeLinearRGBA8888Texture2D(WIDTH, HEIGHT), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0);

  // Copy texture from source to destination.
  copyTexture2D(srcTex.tex(), &dest);

  // Create a GL Pixel Buffer.
  GlRGBA8888PlanePixelBuffer output(HEIGHT, WIDTH, WIDTH * 4);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  readFramebufferRGBAToPixelBuffer(dest, &output);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Compare that the src and dst images are the same.
  output.bind();
  output.map();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  EXPECT_TRUE(eqImg(output.pixels(), src.pixels(), 0));

  output.unmap();
  output.unbind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());
}
#endif


}  // namespace c8
