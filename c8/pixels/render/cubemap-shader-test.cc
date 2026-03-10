// Copyright (c) 2022 Niantic Labs, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":cubemap-shader",
    "//c8:hmatrix",
    "//c8/camera:device-infos",
    "//c8/geometry:intrinsics",
    "//c8/io:image-io",
    "//c8/pixels/opengl:gl-framebuffer",
    "//c8/pixels/opengl:offscreen-gl-context",
    "//c8/pixels/opengl:gl-quad",
    "//c8/pixels/opengl:gl-texture",
    "//c8/pixels/opengl:gl-vertex-array",
    "//c8/pixels/render:object8",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xfa3ebe59);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/camera/device-infos.h"
#include "c8/hmatrix.h"
#include "c8/geometry/intrinsics.h"
#include "c8/io/image-io.h"
#include "c8/pixels/opengl/gl-framebuffer.h"
#include "c8/pixels/opengl/gl-quad.h"
#include "c8/pixels/opengl/gl-texture.h"
#include "c8/pixels/opengl/gl-vertex-array.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "c8/pixels/render/cubemap-shader.h"
#include "c8/pixels/render/object8.h"

namespace c8 {

class CubemapShaderTest : public ::testing::Test {};

void fillRGBA8888Pixels(RGBA8888PlanePixels pixels, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  uint8_t *ptr = pixels.pixels();
  for (int r = 0; r < pixels.rows(); ++r) {
    for (int c = 0; c < pixels.cols(); ++c) {
      ptr[0] = r;
      ptr[1] = g;
      ptr[2] = b;
      ptr[3] = a;
      ptr += 4;
    }
  }
}

TEST_F(CubemapShaderTest, TestShader) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  constexpr int TEX_SIZE = 256;

  constexpr int sceneWidth = 480;
  constexpr int sceneHeight = 640;

  GlTextureCubemap cubeTex;
  cubeTex.initialize(GL_RGBA, TEX_SIZE, TEX_SIZE, GL_RGBA, GL_UNSIGNED_BYTE);

  GlRenderbuffer depthRenderBuffer = makeDepthRenderbuffer(TEX_SIZE, TEX_SIZE);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  GlFramebufferObject dest;
  dest.initialize(
    makeLinearRGBA8888Texture2D(sceneWidth, sceneHeight), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // initialize texture data, etc.
  RGBA8888PlanePixelBuffer buffer(TEX_SIZE, TEX_SIZE);

  RGBA8888PlanePixelBuffer outpix(sceneHeight, sceneWidth);

  // red for negative- & positive- X
  fillRGBA8888Pixels(buffer.pixels(), 255, 0, 0, 255);
  cubeTex.bind();
  cubeTex.updateImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, buffer.pixels().pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());
  cubeTex.updateImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, buffer.pixels().pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());
  cubeTex.unbind();

  // blue for positive-Y
  fillRGBA8888Pixels(buffer.pixels(), 0, 0, 255, 255);
  cubeTex.bind();
  cubeTex.updateImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, buffer.pixels().pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());
  cubeTex.unbind();
  // black for nagative-Y
  fillRGBA8888Pixels(buffer.pixels(), 0, 0, 0, 255);
  cubeTex.bind();
  cubeTex.updateImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, buffer.pixels().pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());
  cubeTex.unbind();

  // green for negative- & positive- Z
  fillRGBA8888Pixels(buffer.pixels(), 0, 255, 0, 255);
  cubeTex.bind();
  cubeTex.updateImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, buffer.pixels().pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());
  cubeTex.updateImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, buffer.pixels().pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());
  cubeTex.unbind();

  // shader
  std::unique_ptr<CubemapShader> shader(new CubemapShader());
  shader->initialize();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // camera
  auto k = Intrinsics::rotateCropAndScaleIntrinsics(
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), sceneWidth, sceneHeight);
  std::unique_ptr<Camera> camera = ObGen::perspectiveCamera(k, k.pixelsWidth, k.pixelsHeight);

  // sky cube
  std::unique_ptr<GlVertexArray> skyCube;
  constexpr float vertexData[][3] = {
    {10.0f, 10.0f, 10.0f},
    {-10.0f, 10.0f, 10.0f},
    {10.0f, -10.0f, 10.0f},
    {-10.0f, -10.0f, 10.0f},
    {10.0f, 10.0f, -10.0f},
    {-10.0f, 10.0f, -10.0f},
    {10.0f, -10.0f, -10.0f},
    {-10.0f, -10.0f, -10.0f},
  };

  constexpr uint16_t indexData[] = {
    0, 1, 2, 1, 3, 2, 4, 6, 5, 6, 7, 5,  // z
    0, 2, 4, 2, 6, 4, 1, 5, 3, 5, 7, 3,  // x
    0, 4, 5, 0, 5, 1, 2, 3, 6, 3, 7, 6,  // y
  };

  // create the cube vertex array
  skyCube.reset(new GlVertexArray());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Create the vertex array.
  skyCube->setIndexBuffer(
    GL_TRIANGLES, GL_UNSIGNED_SHORT, sizeof(indexData), indexData, GL_STATIC_DRAW);
  EXPECT_EQ(GL_NO_ERROR, glGetError());
  skyCube->addVertexBuffer(
    GlVertexAttrib::SLOT_0,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    sizeof(vertexData),
    vertexData,
    GL_STATIC_DRAW);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // get camera mvp
  // default camera is looking at the positive z direction
  HMatrix mvp = camera->projection() * camera->world().inv();

  // rendering calls
  dest.bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // bind the shader
  shader->bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glViewport(0, 0, sceneWidth, sceneHeight);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  skyCube->bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());
  glFrontFace(GL_CCW);

  GLboolean restoreDepthMask;
  glGetBooleanv(GL_DEPTH_WRITEMASK, &restoreDepthMask);
  glDepthMask(GL_FALSE);

  GLint mvpLoc = shader->shader()->location("mvp");
  GLint opacityLoc = shader->shader()->location("opacity");
  GLint locSrcTex = shader->shader()->location("colorSampler");

  glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.data().data());
  glUniform1f(opacityLoc, 1.0f);

  glUniform1i(locSrcTex, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex.id());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  skyCube->drawElements();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  glDepthMask(restoreDepthMask);
  skyCube->unbind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // read back data
  auto o = outpix.pixels();
  glReadPixels(0, 0, o.cols(), o.rows(), GL_RGBA, GL_UNSIGNED_BYTE, o.pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  dest.unbind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Default camera is looking at the positive-z direction,
  // therefore the result pixels should have solid green.
  int greenCount = 0;
  uint8_t *ptr = o.pixels();
  for (int r = 0; r < o.rows(); ++r) {
    for (int c = 0; c < o.cols(); ++c) {
      if (ptr[1] == 255) {
        greenCount++;
      }
      ptr += 4;
    }
  }
  EXPECT_EQ(greenCount, sceneWidth * sceneHeight);
}

}  // namespace c8
