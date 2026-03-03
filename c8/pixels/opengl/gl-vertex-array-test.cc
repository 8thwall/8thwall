// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":client-gl",
    ":gl-headers",
    ":offscreen-gl-context",
    ":gl-framebuffer",
    ":gl-program",
    ":gl-vertex-array",
    "//c8:scope-exit",
    "//c8/pixels:pixel-buffer",
    "@com_google_googletest//:gtest_main",
    "//c8/io:image-io",
  };
}
cc_end(0x904d026e);

#include "c8/io/image-io.h"
#include "c8/pixels/opengl/client-gl.h"
#include "c8/pixels/opengl/gl-constants.h"
#include "c8/pixels/opengl/gl-framebuffer.h"
#include "c8/pixels/opengl/gl-program.h"
#include "c8/pixels/opengl/gl-version.h"
#include "c8/pixels/opengl/gl-vertex-array.h"
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
const char *const FLIP_Y_VERTEX_CODE =  //
  C8_GLSL_VERSION_LINE
  "in vec3 position;\n"
  "void main() {\n"
  "  gl_Position = vec4(position.x, -position.y, position.z, 1.0);\n"
  "}\n";
const char *const COLOR_INPUT_VERTEX_CODE =  //
  C8_GLSL_VERSION_LINE
  "in vec3 position;\n"
  "in vec3 color;\n"
  "out vec3 rgb;\n"
  "void main() {\n"
  "  gl_Position = vec4(position.x, -position.y, position.z, 1.0);\n"
  "  rgb = color;\n"
  "}\n";
const char *const NOP_FRAGMENT_CODE =  //
  C8_GLSL_VERSION_LINE
  "precision mediump float;\n\n"
  "out vec4 fragmentColor;\n"
  "void main() {\n"
  "  fragmentColor = vec4(0.5, 0.5, 0.5, 1.0);\n"
  "}\n";
const char *const COLOR_FRAGMENT_CODE =  //
  C8_GLSL_VERSION_LINE
  "precision mediump float;\n\n"
  "out vec4 fragmentColor;\n"
  "in vec3 rgb;\n"
  "void main() {\n"
  "  fragmentColor = vec4(rgb, 1.0);\n"
  "}\n";
#else  // OpenGL 2
const char *const FLIP_Y_VERTEX_CODE =  //
  C8_GLSL_VERSION_LINE
  "attribute vec3 position;\n"
  "void main() {\n"
  "  gl_Position = vec4(position.x, -position.y, position.z, 1.0);\n"
  "}\n";
const char *const COLOR_INPUT_VERTEX_CODE =  //
  C8_GLSL_VERSION_LINE
  "attribute vec3 position;\n"
  "attribute vec3 color;\n"
  "varying vec3 rgb;\n"
  "void main() {\n"
  "  gl_Position = vec4(position.x, -position.y, position.z, 1.0);\n"
  "  rgb = color;\n"
  "}\n";
const char *const NOP_FRAGMENT_CODE =  //
  C8_GLSL_VERSION_LINE
  "precision mediump float;\n\n"
  "void main() {\n"
  "  gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0);\n"
  "}\n";
const char *const COLOR_FRAGMENT_CODE =  //
  C8_GLSL_VERSION_LINE
  "precision mediump float;\n\n"
  "varying vec3 rgb;\n"
  "void main() {\n"
  "  gl_FragColor = vec4(rgb, 1.0);\n"
  "}\n";
#endif

std::vector<float> getPixel(ConstRGBA8888PlanePixels pixels, int col, int row) {
  const uint8_t *data = pixels.pixels() + row * pixels.rowBytes() + (col << 2);
  std::vector<float> pixel = {
    data[0] / 255.0f,
    data[1] / 255.0f,
    data[2] / 255.0f,
    data[3] / 255.0f,
  };
  return pixel;
}

MATCHER(FloatEqLowPrecision, "Greater than 0.005 epsilon") {
  constexpr float eps = 0.005;
  return (std::fabs(std::get<0>(arg) - std::get<1>(arg)) <= eps);
}

constexpr float vertexDataTop[][3] = {
  {-1.00f, 1.0f, 0.0f},  // Upper-left corner
  {-1.0f, -1.0f, 0.0f},  // Bottom-left corner
  {1.00f, 1.00f, 0.0f},  // Upper-right corner
};

constexpr float vertexDataFull[][3] = {
  {-1.00f, 1.0f, 0.0f},  // Upper-left corner
  {-1.0f, -1.0f, 0.0f},  // Bottom-left corner
  {1.00f, 1.00f, 0.0f},  // Upper-right corner
  {1.00f, -1.0f, 0.0f},  // Bottom-right corner
};

constexpr float vertexDataBottom[][3] = {
  {-1.0f, -1.0f, 0.0f},  // Bottom-left corner
  {1.00f, 1.00f, 0.0f},  // Upper-right corner
  {1.00f, -1.0f, 0.0f},  // Bottom-right corner
};

constexpr float vertexColorDataTop[][6] = {
  // position // color
  {-1.00f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},  // Upper-left
  {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f},  // Bottom-left
  {1.00f, 1.00f, 0.0f, 0.0f, 0.0f, 1.0f},  // Upper-right
};

constexpr uint16_t indexDataTop[] = {0, 1, 2};
constexpr uint16_t indexDataFull[] = {0, 1, 2, 1, 2, 3};
constexpr uint16_t indexDataBottom[] = {1, 2, 0};
}  // namespace

class GlVertexArrayTest : public ::testing::Test {};

TEST_F(GlVertexArrayTest, VertexBufferObjectTest) {
  GlProgram program;
  RGBA8888PlanePixelBuffer outpix(128, 128);
  auto px = outpix.pixels();
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  SCOPE_EXIT([&] { program.cleanup(); });

  program.initialize(FLIP_Y_VERTEX_CODE, NOP_FRAGMENT_CODE, {"position"}, {});
  glUseProgram(program.program);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  GlFramebufferObject dest =
    makeLinearRGBA8888Framebuffer(outpix.pixels().cols(), outpix.pixels().rows());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Bind the framebuffer.
  dest.bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Set the viewport.
  glViewport(0, 0, outpix.pixels().cols(), outpix.pixels().rows());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Create the vertex array.
  GlVertexArray vao;
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Case 1: Draw to the top left.

  // Add the index buffer object.
  vao.setIndexBuffer(
    GL_TRIANGLES, GL_UNSIGNED_SHORT, sizeof(indexDataTop), indexDataTop, GL_DYNAMIC_DRAW);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Add a vertex buffer object.
  vao.addVertexBuffer(
    GlVertexAttrib::SLOT_0,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    sizeof(vertexDataTop),
    vertexDataTop,
    GL_DYNAMIC_DRAW);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Bind the VAO.
  vao.bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Clear the framebuffer.
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the vertices.
  vao.drawElements();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // We are rendering a gray triangle in the upper-left half of the frame, Y-flipped so that the
  // readPixels below would appear as if the image was rendered to a surface.
  glReadPixels(0, 0, px.cols(), px.rows(), GL_RGBA, GL_UNSIGNED_BYTE, px.pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Ensure the upper left is gray.
  EXPECT_THAT(getPixel(px, 0, 0), Pointwise(FloatEqLowPrecision(), {0.5f, 0.5f, 0.5f, 1.0f}));

  // Ensure the bottom right is black.
  EXPECT_THAT(getPixel(px, 127, 127), Pointwise(FloatEqLowPrecision(), {0.0f, 0.0f, 0.0f, 1.0f}));

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Case 2: Draw to the full quad. This allocates new data in the buffers because there are
  // elements that were added.

  // Add the index buffer object.
  vao.setIndexBuffer(
    GL_TRIANGLES, GL_UNSIGNED_SHORT, sizeof(indexDataFull), indexDataFull, GL_DYNAMIC_DRAW);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Add a vertex buffer object.
  vao.addVertexBuffer(
    GlVertexAttrib::SLOT_0,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    sizeof(vertexDataFull),
    vertexDataFull,
    GL_DYNAMIC_DRAW);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Bind the VAO.
  vao.bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Clear the framebuffer.
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the vertices.
  vao.drawElements();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // We are rendering a gray triangle in the upper-left half of the frame, Y-flipped so that the
  // readPixels below would appear as if the image was rendered to a surface.
  glReadPixels(0, 0, px.cols(), px.rows(), GL_RGBA, GL_UNSIGNED_BYTE, px.pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Ensure the upper left is gray.
  EXPECT_THAT(getPixel(px, 0, 0), Pointwise(FloatEqLowPrecision(), {0.5f, 0.5f, 0.5f, 1.0f}));

  // Ensure the bottom right is gray.
  EXPECT_THAT(getPixel(px, 127, 127), Pointwise(FloatEqLowPrecision(), {0.5f, 0.5f, 0.5f, 1.0f}));

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Case 3: Draw to the lower right. This reuses the existing buffer data.

  // Add the index buffer object.
  vao.setIndexBuffer(
    GL_TRIANGLES, GL_UNSIGNED_SHORT, sizeof(indexDataBottom), indexDataBottom, GL_DYNAMIC_DRAW);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Add a vertex buffer object.
  vao.addVertexBuffer(
    GlVertexAttrib::SLOT_0,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    sizeof(vertexDataBottom),
    vertexDataBottom,
    GL_DYNAMIC_DRAW);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Bind the VAO.
  vao.bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Clear the framebuffer.
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the vertices.
  vao.drawElements();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // We are rendering a gray triangle in the upper-left half of the frame, Y-flipped so that the
  // readPixels below would appear as if the image was rendered to a surface.
  glReadPixels(0, 0, px.cols(), px.rows(), GL_RGBA, GL_UNSIGNED_BYTE, px.pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Ensure the upper left is black.
  EXPECT_THAT(getPixel(px, 0, 0), Pointwise(FloatEqLowPrecision(), {0.0f, 0.0f, 0.0f, 1.0f}));

  // Ensure the bottom right is gray.
  EXPECT_THAT(getPixel(px, 127, 127), Pointwise(FloatEqLowPrecision(), {0.5f, 0.5f, 0.5f, 1.0f}));
}

TEST_F(GlVertexArrayTest, StringVertexAttributeFlow) {
  GlProgram program;
  RGBA8888PlanePixelBuffer outpix(128, 128);
  auto px = outpix.pixels();
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  SCOPE_EXIT([&] { program.cleanup(); });

  const std::vector<std::string> attributes = {"position"};

  program.initialize(FLIP_Y_VERTEX_CODE, NOP_FRAGMENT_CODE, attributes, {});
  glUseProgram(program.program);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  GlFramebufferObject dest =
    makeLinearRGBA8888Framebuffer(outpix.pixels().cols(), outpix.pixels().rows());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Bind the framebuffer.
  dest.bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Set the viewport.
  glViewport(0, 0, outpix.pixels().cols(), outpix.pixels().rows());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Create the vertex array.
  GlVertexArray vao;
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Case 1: Draw to the top left.

  // Initialize the vertex array object with the provided attributes.
  vao.initialize(attributes);

  // Add the index buffer object.
  vao.setIndexBuffer(
    GL_TRIANGLES, GL_UNSIGNED_SHORT, sizeof(indexDataTop), indexDataTop, GL_DYNAMIC_DRAW);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Add a vertex buffer object.
  vao.addVertexBuffer(
    attributes[0],  // "position"
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    sizeof(vertexDataTop),
    vertexDataTop,
    GL_DYNAMIC_DRAW);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Bind the VAO.
  vao.bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Clear the framebuffer.
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the vertices.
  vao.drawElements();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // We are rendering a gray triangle in the upper-left half of the frame, Y-flipped so that the
  // readPixels below would appear as if the image was rendered to a surface.
  glReadPixels(0, 0, px.cols(), px.rows(), GL_RGBA, GL_UNSIGNED_BYTE, px.pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Ensure the upper left is gray.
  EXPECT_THAT(getPixel(px, 0, 0), Pointwise(FloatEqLowPrecision(), {0.5f, 0.5f, 0.5f, 1.0f}));

  // Ensure the bottom right is black.
  EXPECT_THAT(getPixel(px, 127, 127), Pointwise(FloatEqLowPrecision(), {0.0f, 0.0f, 0.0f, 1.0f}));
}

TEST_F(GlVertexArrayTest, InterleavedBufferFlow) {
  RGBA8888PlanePixelBuffer outpix(128, 128);
  auto px = outpix.pixels();
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  GlProgramObject programObject;

  const Vector<String> attributes = {"position", "color"};

  programObject.initialize(COLOR_INPUT_VERTEX_CODE, COLOR_FRAGMENT_CODE, attributes, {});
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  glUseProgram(programObject.id());

  GlFramebufferObject dest =
    makeLinearRGBA8888Framebuffer(outpix.pixels().cols(), outpix.pixels().rows());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Bind the framebuffer.
  dest.bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Set the viewport.
  glViewport(0, 0, outpix.pixels().cols(), outpix.pixels().rows());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Create the vertex array.
  GlVertexArray vao;
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Case 1: Draw to the top left.

  // Initialize the vertex array object with the provided attributes.
  vao.initialize(attributes);

  // Add the index buffer object.
  vao.setIndexBuffer(
    GL_TRIANGLES, GL_UNSIGNED_SHORT, sizeof(indexDataTop), indexDataTop, GL_DYNAMIC_DRAW);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Add an interleaved vertex buffer object.
  const int interleavedIdx = 0;
  const int interleavedBufferSize = sizeof(vertexColorDataTop);
  vao.setInterleavedBuffer(
    interleavedIdx, interleavedBufferSize, vertexColorDataTop, GL_DYNAMIC_DRAW);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Set the vertex attribute pointers.
  const int posDims = 3;
  const int colorDims = 3;
  const int interleavedStride = sizeof(vertexColorDataTop[0]);
  EXPECT_EQ(
    (posDims + colorDims) * sizeof(float), interleavedStride);

  vao.setInterleavedAttribute(
    interleavedIdx,
    "position",
    posDims,
    GL_FLOAT,
    GL_FALSE,
    interleavedStride,
    0);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  vao.setInterleavedAttribute(
    interleavedIdx,
    "color",
    colorDims,
    GL_FLOAT,
    GL_FALSE,
    interleavedStride,
    posDims * sizeof(float));
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Bind the VAO.
  vao.bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Clear the framebuffer.
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw the vertices.
  vao.drawElements();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // We are rendering a gray triangle in the upper-left half of the frame, Y-flipped so that the
  // readPixels below would appear as if the image was rendered to a surface.
  glReadPixels(0, 0, px.cols(), px.rows(), GL_RGBA, GL_UNSIGNED_BYTE, px.pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Ensure the upper left is nearly red.
  EXPECT_THAT(getPixel(px, 0, 0), Pointwise(FloatEqLowPrecision(), {0.992156863f, 0.00392156886f, 0.00392156886f, 1.0f}));

  // Ensure the bottom left is nearly green.
  EXPECT_THAT(getPixel(px, 0, 126), Pointwise(FloatEqLowPrecision(), {0.00392156886f, 0.992156863f, 0.00392156886f, 1.0f}));

  // Ensure the upper right is nearly blue.
  EXPECT_THAT(getPixel(px, 126, 0), Pointwise(FloatEqLowPrecision(), {0.00392156886f, 0.00392156886f, 0.992156863f, 1.0f}));

  // Ensure the bottom right is black.
  EXPECT_THAT(getPixel(px, 127, 127), Pointwise(FloatEqLowPrecision(), {0.0f, 0.0f, 0.0f, 1.0f}));
}
}  // namespace c8
