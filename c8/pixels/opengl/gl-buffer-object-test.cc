// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":client-gl",
    ":gl-headers",
    ":offscreen-gl-context",
    ":gl-buffer-object",
    ":gl-framebuffer",
    ":gl-program",
    "//c8:scope-exit",
    "//c8/pixels:pixel-buffer",
    "@com_google_googletest//:gtest_main",

    "//c8/io:image-io",
  };
}
cc_end(0x1443a9d1);

#include "c8/io/image-io.h"
#include "c8/pixels/opengl/client-gl.h"
#include "c8/pixels/opengl/gl-buffer-object.h"
#include "c8/pixels/opengl/gl-framebuffer.h"
#include "c8/pixels/opengl/gl-program.h"
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
const char *const FLIP_Y_VERTEX_CODE =  //
  C8_GLSL_VERSION_LINE
  "in vec3 position;\n"
  "void main() {\n"
  "  gl_Position = vec4(position.x, -position.y, position.z, 1.0);\n"
  "}\n";
const char *const NOP_FRAGMENT_CODE =  //
  C8_GLSL_VERSION_LINE
  "precision mediump float;\n\n"
  "out vec4 fragmentColor;\n"
  "void main() {\n"
  "  fragmentColor = vec4(0.5, 0.5, 0.5, 1.0);\n"
  "}\n";
#else  // OpenGL 2
const char *const FLIP_Y_VERTEX_CODE =  //
  C8_GLSL_VERSION_LINE
  "attribute vec3 position;\n"
  "void main() {\n"
  "  gl_Position = vec4(position.x, -position.y, position.z, 1.0);\n"
  "}\n";
const char *const NOP_FRAGMENT_CODE =  //
  C8_GLSL_VERSION_LINE
  "precision mediump float;\n\n"
  "void main() {\n"
  "  gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0);\n"
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

}  // namespace

class GlBufferObjectTest : public ::testing::Test {};

TEST_F(GlBufferObjectTest, VertexBufferObjectTest) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  RGBA8888PlanePixelBuffer outpix(128, 128);

  GlProgram program;

  SCOPE_EXIT([&] { program.cleanup(); });

  constexpr float vertexData[][3] = {
    {-1.00f, 1.0f, 0.0f},  // Upper-left corner
    {-1.0f, -1.0f, 0.0f},  // Bottom-left corner
    {1.00f, 1.00f, 0.0f},  // Upper-right corner
  };

  constexpr uint16_t indexData[] = {0, 1, 2};

  program.initialize(FLIP_Y_VERTEX_CODE, NOP_FRAGMENT_CODE, {"position"}, {});
  glUseProgram(program.program);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  GlFramebufferObject dest =
    makeLinearRGBA8888Framebuffer(outpix.pixels().cols(), outpix.pixels().rows());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

#if C8_OPENGL_VERSION_2
  auto glGenVertexArrays =
    (PFNGLGENVERTEXARRAYSOESPROC)clientGlGetProcAddress("glGenVertexArraysOES");
  auto glDeleteVertexArrays =
    (PFNGLDELETEVERTEXARRAYSOESPROC)clientGlGetProcAddress("glDeleteVertexArraysOES");
  auto glBindVertexArray =
    (PFNGLBINDVERTEXARRAYOESPROC)clientGlGetProcAddress("glBindVertexArrayOES");
#endif

  GLuint vao;
  glGenVertexArrays(1, &vao);
  EXPECT_EQ(GL_NO_ERROR, glGetError());
  SCOPE_EXIT([&] { glDeleteVertexArrays(1, &vao); });

  glBindVertexArray(vao);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  {
    // Create a Vertex Buffer Object for uploading vertex data from client.
    GlBufferObject vertexBuffer;
    EXPECT_EQ(GL_NO_ERROR, glGetError());

    vertexBuffer.set(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    EXPECT_EQ(GL_NO_ERROR, glGetError());

    // Now do the same for the index buffer.
    GlBufferObject indexBuffer;
    EXPECT_EQ(GL_NO_ERROR, glGetError());

    indexBuffer.set(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);
    EXPECT_EQ(GL_NO_ERROR, glGetError());

    // Bind the VBO to the VAO.
    vertexBuffer.bind();
    EXPECT_EQ(GL_NO_ERROR, glGetError());

    // Set the offset of the vertex attribute.
    glVertexAttribPointer(
      program.location("position"), 3, GL_FLOAT, GL_FALSE, sizeof(vertexData[0]), 0);
    EXPECT_EQ(GL_NO_ERROR, glGetError());

    // Enable the vertex position attribute.
    glEnableVertexAttribArray(program.location("position"));
    EXPECT_EQ(GL_NO_ERROR, glGetError());

    // Bind the IBO to the VAO.
    indexBuffer.bind();

    // If we unbind the VAO, we can delete the buffer objects from the client. The VBO and IBO
    // arrays will still be referenced on the GPU when the VAO is bound again.
    glBindVertexArray(0);
    EXPECT_EQ(GL_NO_ERROR, glGetError());
  }

  // Ensure the VBO and IBO were deleted without error.
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Rebind the VAO.
  glBindVertexArray(vao);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Bind the framebuffer.
  dest.bind();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Clear the framebuffer.
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Set the viewport.
  glViewport(0, 0, outpix.pixels().cols(), outpix.pixels().rows());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Draw the vertices.
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // We are rendering a gray triangle in the upper-left half of the frame, Y-flipped so that the
  // readPixels below would appear as if the image was rendered to a surface.
  auto px = outpix.pixels();
  glReadPixels(0, 0, px.cols(), px.rows(), GL_RGBA, GL_UNSIGNED_BYTE, px.pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Ensure the upper left is gray.
  EXPECT_THAT(getPixel(px, 0, 0), Pointwise(FloatEqLowPrecision(), {0.5f, 0.5f, 0.5f, 1.0f}));

  // Ensure the bottom right is black.
  EXPECT_THAT(getPixel(px, 127, 127), Pointwise(FloatEqLowPrecision(), {0.0f, 0.0f, 0.0f, 1.0f}));

  // Clean-up the Vertex Attrib Array.
  // TODO(mc): Create an objectified version of VAO.
  glDeleteVertexArrays(1, &vao);
  EXPECT_EQ(GL_NO_ERROR, glGetError());
}

}  // namespace c8
