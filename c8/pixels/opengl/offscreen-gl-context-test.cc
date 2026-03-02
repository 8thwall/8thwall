// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":gl-headers",
    ":offscreen-gl-context",
    ":gl-framebuffer",
    ":gl-program",
    ":gl-quad",
    ":gl-texture",
    "//c8:scope-exit",
    "//c8/pixels:pixel-buffer",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x462d39af);

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
#include "gtest/gtest.h"

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

class OffscreenGlContextTest : public ::testing::Test {};

TEST_F(OffscreenGlContextTest, ContextCalisthenics) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  RGBA8888PlanePixelBuffer test(256, 256);
  RGBA8888PlanePixelBuffer outpix(256, 256);

  GlTexture src;
  GlProgram shader;
  GlQuad quad;

  SCOPE_EXIT([&] {
    src.cleanup();
    shader.cleanup();
    quad.cleanup();
  });

  auto tp = test.pixels();
  fillTestImage(tp);

  quad.initialize();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  GlFramebufferObject dest =
    makeLinearRGBA8888Framebuffer(outpix.pixels().cols(), outpix.pixels().rows());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  src.initialize();
  glBindTexture(GL_TEXTURE_2D, src.texture);
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGBA, tp.cols(), tp.rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tp.pixels());

  EXPECT_EQ(GL_NO_ERROR, glGetError());

  shader.initialize(NOP_VERTEX_CODE, NOP_FRAGMENT_CODE, {"position", "uv"}, {});
  glUseProgram(shader.program);

  EXPECT_EQ(GL_NO_ERROR, glGetError());

  dest.bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glBindBuffer(GL_ARRAY_BUFFER, quad.vertexBuffer);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Core profile OpenGL3 requires a vertex array object, and for OpenGL3 we can also use it.
#if C8_OPENGL_VERSION_3
  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
#endif

  glEnableVertexAttribArray(shader.location("position"));
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glVertexAttribPointer(shader.location("position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glBindBuffer(GL_ARRAY_BUFFER, quad.uvBuffer);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glEnableVertexAttribArray(shader.location("uv"));
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glVertexAttribPointer(shader.location("uv"), 2, GL_FLOAT, GL_FALSE, 0, 0);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glFrontFace(GL_CCW);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.triangleBuffer);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glViewport(0, 0, tp.cols(), tp.rows());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Copy data to byte array
  glActiveTexture(GL_TEXTURE0);
  dest.tex().bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  auto o = outpix.pixels();
  glReadPixels(0, 0, o.cols(), o.rows(), GL_RGBA, GL_UNSIGNED_BYTE, o.pixels());

  EXPECT_EQ(GL_NO_ERROR, glGetError());

  RGBA8888PlanePixels op(tp.rows(), tp.cols(), o.rowBytes(), o.pixels());

  int threshold = 0;

  EXPECT_TRUE(eqImg(tp, op, threshold));

#if C8_OPENGL_VERSION_3
  glDeleteVertexArrays(1, &vao);
#endif

  EXPECT_EQ(GL_NO_ERROR, glGetError());
}

}  // namespace c8
