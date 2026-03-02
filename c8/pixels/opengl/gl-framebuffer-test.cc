// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":client-gl",
    ":gl-headers",
    ":gl-texture",
    ":gl-quad",
    ":gl-framebuffer",
    ":gl-program",
    ":offscreen-gl-context",
    "//c8:scope-exit",
    "//c8/pixels:pixel-buffer",
    "@com_google_googletest//:gtest_main",
    "//c8/io:image-io",
  };
}
cc_end(0x44cb9b0c);

#include "c8/io/image-io.h"
#include "c8/pixels/opengl/client-gl.h"
#include "c8/pixels/opengl/gl-framebuffer.h"
#include "c8/pixels/opengl/gl-program.h"
#include "c8/pixels/opengl/gl-quad.h"
#include "c8/pixels/opengl/gl-texture.h"
#include "c8/pixels/opengl/gl-version.h"
#include "c8/pixels/opengl/gl.h"
#include "c8/pixels/opengl/glext.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/scope-exit.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::Pointwise;

namespace c8 {

namespace {

#if C8_OPENGL_VERSION_3
const char *const NOP_VERTEX_CODE =  //
  C8_GLSL_VERSION_LINE
  "in vec3 position;\n"
  "in vec2 uv;\n"
  "out vec2 texUv;\n"
  "void main() {\n"
  "  gl_Position = vec4(position, 1.0);\n"
  "  texUv = uv;\n"
  "}\n";
const char *const NOP_FRAGMENT_CODE =  //
  C8_GLSL_VERSION_LINE
  "precision mediump float;\n\n"
  "in vec2 texUv;\n"
  "uniform sampler2D sampler;\n"
  "out vec4 fragmentColor;\n"
  "void main() {\n"
  "  fragmentColor = texture(sampler, texUv);\n"
  "}\n";
#else  // OpenGL 2
const char *const NOP_VERTEX_CODE =  //
  C8_GLSL_VERSION_LINE
  "attribute vec3 position;\n"
  "attribute vec2 uv;\n"
  "varying vec2 texUv;\n"
  "void main() {\n"
  "  gl_Position = vec4(position, 1.0);\n"
  "  texUv = uv;\n"
  "}\n";
const char *const NOP_FRAGMENT_CODE =  //
  C8_GLSL_VERSION_LINE
  "precision mediump float;\n\n"
  "varying vec2 texUv;\n"
  "uniform sampler2D sampler;\n"
  "void main() {\n"
  "  gl_FragColor = texture2D(sampler, texUv);\n"
  "}\n";
#endif

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

  constexpr int WIDTH = 128;
  constexpr int HEIGHT = 64;

  RGBA8888PlanePixelBuffer src(HEIGHT, WIDTH);
  RGBA8888PlanePixelBuffer outpix(HEIGHT, WIDTH);

  GlTexture2D srcTex;
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  srcTex.initialize(GL_TEXTURE_2D, GL_RGBA, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Fill the test image with data on the CPU.
  fillTestImage(src.pixels());

  // Upload the CPU image to the GPU texture.
  srcTex.bind();
  srcTex.updateImage(src.pixels().pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // The destination GlFramebufferObject that we are testing.
  GlFramebufferObject dest;

  {
    GlTexture2D destTex;
    destTex.initialize(GL_TEXTURE_2D, GL_RGBA, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    EXPECT_EQ(GL_NO_ERROR, glGetError());

    auto destTexId = destTex.id();
    EXPECT_NE(0, destTex.id());

    // Initialize the destination framebuffer with the texture. After this, the destTex is moved and
    // owned by the GlFramebufferObject.
    dest.initialize(std::move(destTex), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0);
    EXPECT_EQ(GL_NO_ERROR, glGetError());

    // The moved destTex now has an empty id.
    EXPECT_EQ(0, destTex.id());

    // The framebuffer has the destTex id.
    EXPECT_EQ(destTexId, dest.tex().id());

    EXPECT_EQ(GL_TEXTURE_2D, dest.tex().target());
    EXPECT_EQ(GL_COLOR_ATTACHMENT0, dest.attachment());
    EXPECT_EQ(GL_FRAMEBUFFER, dest.target());
  }

  GlProgram program;
  GlQuad quad;
  SCOPE_EXIT([&program, &quad] {
    program.cleanup();
    quad.cleanup();
  });
  program.initialize(NOP_VERTEX_CODE, NOP_FRAGMENT_CODE, {"position", "uv"}, {});
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  quad.initialize();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  srcTex.bind();
  glUseProgram(program.program);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glBindBuffer(GL_ARRAY_BUFFER, quad.vertexBuffer);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

#if C8_OPENGL_VERSION_3
  // For the purposes of keeping this test basic, only use VertexArrayObjects when they are required
  // (OpenGL3) and not when they are extensions (OpenGL2 and WebGL).
  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
#endif

  glEnableVertexAttribArray(program.location("position"));
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glVertexAttribPointer(program.location("position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glBindBuffer(GL_ARRAY_BUFFER, quad.uvBuffer);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glEnableVertexAttribArray(program.location("uv"));
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glVertexAttribPointer(program.location("uv"), 2, GL_FLOAT, GL_FALSE, 0, 0);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glFrontFace(GL_CCW);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.triangleBuffer);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Bind the GlFramebufferObject for rendering.
  dest.bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glViewport(0, 0, src.pixels().cols(), src.pixels().rows());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // We are rendering a gray triangle in the upper-left half of the frame, Y-flipped so that the
  // readPixels below would appear as if the image was rendered to a surface.
  auto px = outpix.pixels();
  glReadPixels(0, 0, px.cols(), px.rows(), GL_RGBA, GL_UNSIGNED_BYTE, px.pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Unbind the GlFramebufferObject.
  dest.unbind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Compare that the src and dst images are the same.
  EXPECT_TRUE(eqImg(px, src.pixels(), 0));
}

TEST_F(GlTextureTest, AttachOnlyFramebuffer) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  constexpr int WIDTH = 128;
  constexpr int HEIGHT = 64;

  RGBA8888PlanePixelBuffer src(HEIGHT, WIDTH);
  RGBA8888PlanePixelBuffer outpix(HEIGHT, WIDTH);

  GlTexture2D srcTex;
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  srcTex.initialize(GL_TEXTURE_2D, GL_RGBA, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Fill the test image with data on the CPU.
  fillTestImage(src.pixels());

  // Upload the CPU image to the GPU texture.
  srcTex.bind();
  srcTex.updateImage(src.pixels().pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  GlRenderbuffer depthRenderBuffer = makeDepthRenderbuffer(WIDTH, HEIGHT);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  GlAttachOnlyFramebuffer fb;
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  fb.attachColorTexture2D(GL_COLOR_ATTACHMENT0, srcTex.target(), srcTex.id());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  fb.attachRenderBuffer(GL_DEPTH_ATTACHMENT, depthRenderBuffer.target(), depthRenderBuffer.id());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  EXPECT_TRUE(fb.isComplete());

  fb.bind();
  auto px = outpix.pixels();
  glReadPixels(0, 0, px.cols(), px.rows(), GL_RGBA, GL_UNSIGNED_BYTE, px.pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());
  fb.unbind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Compare that the src and dst images are the same.
  EXPECT_TRUE(eqImg(px, src.pixels(), 0));

  // Test if the textures are still valid after attached-to framebuffer is destroyed.
  std::unique_ptr<GlAttachOnlyFramebuffer> fbPtr(new GlAttachOnlyFramebuffer());
  fbPtr->attachColorTexture2D(GL_COLOR_ATTACHMENT0, srcTex.target(), srcTex.id());
  EXPECT_EQ(GL_NO_ERROR, glGetError());
  fbPtr->attachRenderBuffer(
    GL_DEPTH_ATTACHMENT, depthRenderBuffer.target(), depthRenderBuffer.id());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  EXPECT_TRUE(fb.isComplete());
  EXPECT_TRUE(fbPtr->isComplete());

  fbPtr.reset();
  EXPECT_TRUE(fb.isComplete());
  EXPECT_NE(0, srcTex.id());
  EXPECT_NE(0, depthRenderBuffer.id());
}

}  // namespace c8
