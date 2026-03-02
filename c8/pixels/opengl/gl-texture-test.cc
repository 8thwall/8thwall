// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":client-gl",
    ":gl-headers",
    ":offscreen-gl-context",
    ":gl-texture",
    ":gl-quad",
    ":gl-framebuffer",
    ":gl-program",
    "//c8:scope-exit",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:test-image",
    "@com_google_googletest//:gtest_main",

    "//c8/io:image-io",
  };
}
cc_end(0x7e333869);

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
#include "c8/pixels/test-image.h"
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

  GlFramebufferObject dest;

  dest.initialize(
    makeLinearRGBA8888Texture2D(outpix.pixels().cols(), outpix.pixels().rows()),
    GL_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  dest.bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // See if the copy image extension is installed. In prod code, this can be a static check and
  // cached.
  bool copyImageAvailable = false;
#if C8_OPENGL_VERSION_3
  GLint n, i;
  glGetIntegerv(GL_NUM_EXTENSIONS, &n);
  for (i = 0; i < n; i++) {
    copyImageAvailable =
      (strstr((const char *)glGetStringi(GL_EXTENSIONS, i), "GL_EXT_copy_image") != nullptr);
    if (copyImageAvailable) {
      break;
    }
  }
#else
  copyImageAvailable =
    (strstr((const char *)glGetString(GL_EXTENSIONS), "GL_EXT_copy_image") != nullptr);
#endif

#if defined(__APPLE__) && !C8_USE_ANGLE
  // Apple OpenGL is missing the typedef for glCopyImageSubData.
  typedef void(APIENTRYP PFNGLCOPYIMAGESUBDATAEXTPROC)(
    GLuint srcName,
    GLenum srcTarget,
    GLint srcLevel,
    GLint srcX,
    GLint srcY,
    GLint srcZ,
    GLuint dstName,
    GLenum dstTarget,
    GLint dstLevel,
    GLint dstX,
    GLint dstY,
    GLint dstZ,
    GLsizei srcWidth,
    GLsizei srcHeight,
    GLsizei srcDepth);
#endif

  auto glCopyImageSubData = copyImageAvailable
    ? (PFNGLCOPYIMAGESUBDATAEXTPROC)clientGlGetProcAddress("glCopyImageSubDataEXT")
    : nullptr;

  if (glCopyImageSubData) {
    // GPU-copy the src texture directly to the framebuffer texture.
    glCopyImageSubData(
      srcTex.id(),
      srcTex.target(),
      0,
      0,
      0,
      0,
      dest.tex().id(),
      GL_TEXTURE_2D,
      0,
      0,
      0,
      0,
      WIDTH,
      HEIGHT,
      1);
    EXPECT_EQ(GL_NO_ERROR, glGetError());
  } else {
    // Use the render pipeline.
    GlProgramObject program;

    program.initialize(
      NOP_VERTEX_CODE,
      NOP_FRAGMENT_CODE,
      {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
      {});
    EXPECT_EQ(GL_NO_ERROR, glGetError());

    srcTex.bind();
    glUseProgram(program.id());
    EXPECT_EQ(GL_NO_ERROR, glGetError());

    // Create a rect for drawing the texture.
    GlVertexArray rect = makeVertexArrayRect();
    EXPECT_EQ(GL_NO_ERROR, glGetError());

    glFrontFace(GL_CCW);
    EXPECT_EQ(GL_NO_ERROR, glGetError());

    glViewport(0, 0, src.pixels().cols(), src.pixels().rows());
    EXPECT_EQ(GL_NO_ERROR, glGetError());

    // Bind the rect and draw.
    rect.bind();
    rect.drawElements();
    EXPECT_EQ(GL_NO_ERROR, glGetError());
  }

  // We are rendering a gray triangle in the upper-left half of the frame, Y-flipped so that the
  // readPixels below would appear as if the image was rendered to a surface.
  auto px = outpix.pixels();
  glReadPixels(0, 0, px.cols(), px.rows(), GL_RGBA, GL_UNSIGNED_BYTE, px.pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Compare that the src and dst images are the same.
  EXPECT_TRUE(eqImg(px, src.pixels(), 0));
}

TEST_F(GlTextureTest, CubemapTextureReadWrite) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  const int texSize = 256;

  RGBA8888PlanePixelBuffer src(texSize, texSize);
  RGBA8888PlanePixelBuffer outpix(texSize, texSize);

  GlTextureCubemap cubeTex;
  cubeTex.initialize(GL_RGBA, texSize, texSize, GL_RGBA, GL_UNSIGNED_BYTE);

  GlRenderbuffer depthRenderBuffer = makeDepthRenderbuffer(texSize, texSize);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  GlAttachOnlyFramebuffer fb;
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  fb.attachRenderBuffer(GL_DEPTH_ATTACHMENT, depthRenderBuffer.target(), depthRenderBuffer.id());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  for (int f = 0; f < 6; ++f) {
    // Fill the test image with data on the CPU.
    fillTestImage(src.pixels());

    cubeTex.bind();
    cubeTex.updateImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, src.pixels().pixels());
    EXPECT_EQ(GL_NO_ERROR, glGetError());
    cubeTex.unbind();

    fb.attachColorTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + f, cubeTex.id());
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
  }
}

}  // namespace c8
