// Copyright (c) 2022 Niantic Labs, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":image-shader",
    "//c8:hmatrix",
    "//c8/pixels/opengl:gl-framebuffer",
    "//c8/pixels/opengl:offscreen-gl-context",
    "//c8/pixels/opengl:gl-quad",
    "//c8/pixels/opengl:gl-texture",
    "//c8/pixels:test-image",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xb485f497);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/hmatrix.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "c8/pixels/opengl/gl-framebuffer.h"
#include "c8/pixels/opengl/gl-quad.h"
#include "c8/pixels/opengl/gl-texture.h"
#include "c8/pixels/test-image.h"

#include "c8/pixels/render/image-shader.h"

namespace c8 {

class ImageShaderTest : public ::testing::Test {};

TEST_F(ImageShaderTest, TestShader) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  constexpr int WIDTH = 64;
  constexpr int HEIGHT = 64;

  RGBA8888PlanePixelBuffer src(HEIGHT, WIDTH);
  RGBA8888PlanePixelBuffer outpix(HEIGHT, WIDTH);

  GlTexture2D srcTex = makeLinearRGBA8888Texture2D(WIDTH, HEIGHT);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Fill the test image with data on the CPU.
  fillTestImage(src.pixels());

  std::memset(outpix.pixels().pixels(), 0, outpix.pixels().rows() * outpix.pixels().rowBytes());

  // Upload the CPU image to the GPU texture.
  srcTex.bind();
  srcTex.updateImage(src.pixels().pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());
  srcTex.unbind();

  GlFramebufferObject dest;
  dest.initialize(
    makeLinearRGBA8888Texture2D(outpix.pixels().cols(), outpix.pixels().rows()),
    GL_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  GlVertexArray quad = makeVertexArrayRect();

  std::unique_ptr<ImageShader> imageShader;
  imageShader.reset(new ImageShader());
  imageShader->initialize();

  imageShader->bind();
  srcTex.bind();
  dest.bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glViewport(0, 0, WIDTH, HEIGHT);
  glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  quad.bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  GLint mvpLoc = imageShader->shader()->location("mvp");
  GLint opacityLoc = imageShader->shader()->location("opacity");

  GLint locSrcTex = imageShader->shader()->location("colorSampler");

  auto mvp = HMatrixGen::i();
  glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.data().data());
  glUniform1f(opacityLoc, 1.0f);

  glUniform1i(locSrcTex, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, srcTex.id());

  quad.drawElements();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  srcTex.unbind();

  glReadPixels(
    0,
    0,
    outpix.pixels().cols(),
    outpix.pixels().rows(),
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    outpix.pixels().pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());
  dest.unbind();

  // Compare that the src and dst images are the same.
  EXPECT_TRUE(eqImg(outpix.pixels(), src.pixels(), 0));
}

}  // namespace c8
