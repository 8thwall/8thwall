// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":gl-headers",
    "//c8:hvector",
    ":offscreen-gl-context",
    ":gl-framebuffer",
    ":gl-program",
    ":gl-vertex-array",
    "//c8:scope-exit",
    "//c8/pixels:pixel-buffer",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xe56bae42);

#include "c8/hvector.h"
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
  R"(
in vec3 position;
void main() {
  gl_Position = vec4(position.x, -position.y, position.z, 1.0);
}
)";

const char *const COLOR_FRAGMENT_CODE =  //
  C8_GLSL_VERSION_LINE
  R"(
precision mediump float;
uniform vec3 color;
out vec4 fragmentColor;
void main() {
  fragmentColor = vec4(color, 1.0);
}
)";

const char *const UNIFORMS_VERTEX_CODE =  //
  C8_GLSL_VERSION_LINE
  R"(
in vec4 position;
void main() {
  gl_Position = position;
}
)";

const char *const UNIFORMS_FRAGMENT_CODE =  //
  C8_GLSL_VERSION_LINE
  R"(
precision mediump float;

struct TestStruct {
  int aInt;
  vec3 aVec3;
};

uniform TestStruct structArray[10];
uniform TestStruct structElement;
uniform float floatArray[10];
uniform float floatPrimitive;

out vec4 fragmentColor;
void main() {
  vec3 result = structArray[9].aVec3 + structElement.aVec3 + vec3(floatArray[9], floatPrimitive, 0.0);
  fragmentColor = vec4(result, 1.0);
}
)";

#else  // OpenGL 2

const char *const FLIP_Y_VERTEX_CODE =  //
  C8_GLSL_VERSION_LINE
  R"(
attribute vec3 position;
void main() {
  gl_Position = vec4(position.x, -position.y, position.z, 1.0);
}
)";

const char *const COLOR_FRAGMENT_CODE =  //
  C8_GLSL_VERSION_LINE
  R"(
precision mediump float;
uniform vec3 color;
void main() {
  gl_FragColor = vec4(color, 1.0);
}
)";

const char *const UNIFORMS_VERTEX_CODE =  //
  C8_GLSL_VERSION_LINE
  R"(
attribute vec4 position;
void main() {
  gl_Position = position;
}
)";

const char *const UNIFORMS_FRAGMENT_CODE =  //
  C8_GLSL_VERSION_LINE
  R"(
precision mediump float;

struct TestStruct {
  int aInt;
  vec3 aVec3;
};

uniform TestStruct structArray[10];
uniform TestStruct structElement;
uniform float floatArray[10];
uniform float floatPrimitive;

void main() {
  vec3 result = structArray[9].aVec3 + structElement.aVec3 + vec3(floatArray[9], floatPrimitive, 0.0);
  gl_FragColor = vec4(result, 1.0);
}
)";

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

class GlProgramTest : public ::testing::Test {};

TEST_F(GlProgramTest, ProgramObjectTest) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  RGBA8888PlanePixelBuffer outpix(128, 128);

  GlProgramObject program;
  program.initialize(
    FLIP_Y_VERTEX_CODE, COLOR_FRAGMENT_CODE, {{"position", GlVertexAttrib::SLOT_0}}, {{"color"}});
  GLint colorUniform = program.location("color");
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  EXPECT_NE(-1, colorUniform);

  GlFramebufferObject dest;
  {
    GlTexture2D destTex;
    destTex.initialize(
      GL_TEXTURE_2D,
      GL_RGBA,
      outpix.pixels().cols(),
      outpix.pixels().rows(),
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      nullptr);
    dest.initialize(std::move(destTex), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0);
  }

  constexpr float vertexData[][3] = {
    {-1.00f, 1.0f, 0.0f},  // Upper-left corner
    {-1.0f, -1.0f, 0.0f},  // Bottom-left corner
    {1.00f, 1.00f, 0.0f},  // Upper-right corner
  };

  constexpr uint16_t indexData[] = {0, 1, 2};

  // Create the vertex array.
  GlVertexArray vao;
  vao.setIndexBuffer(GL_TRIANGLES, GL_UNSIGNED_SHORT, sizeof(indexData), indexData, GL_STATIC_DRAW);
  vao.addVertexBuffer(
    GlVertexAttrib::SLOT_0,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    sizeof(vertexData),
    vertexData,
    GL_STATIC_DRAW);

  // Bind the VAO and framebuffer.
  vao.bind();
  dest.bind();

  // Clear the framebuffer.
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Set the viewport.
  glViewport(0, 0, outpix.pixels().cols(), outpix.pixels().rows());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Set the active shader program.
  glUseProgram(program.id());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Set the triangle color to green
  glUniform3f(colorUniform, 0.0f, 0.8f, 0.0f);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Draw the vertices.
  vao.drawElements();
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // We are rendering a green triangle in the upper-left half of the frame, Y-flipped so that the
  // readPixels below would appear as if the image was rendered to a surface.
  auto px = outpix.pixels();
  glReadPixels(0, 0, px.cols(), px.rows(), GL_RGBA, GL_UNSIGNED_BYTE, px.pixels());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Ensure the upper left is gray.
  EXPECT_THAT(getPixel(px, 0, 0), Pointwise(FloatEqLowPrecision(), {0.0f, 0.8f, 0.0f, 1.0f}));

  // Ensure the bottom right is black.
  EXPECT_THAT(getPixel(px, 127, 127), Pointwise(FloatEqLowPrecision(), {0.0f, 0.0f, 0.0f, 1.0f}));
}

struct TestStruct {
  int aInt;
  HVector3 aVec3;
};

TEST_F(GlProgramTest, UniformLocationsTest) {
  // Set up data.
  float floatPrimitive = 50.f;
  TestStruct structElement{99, {100.1f, 100.1f, 100.1f}};
  Vector<TestStruct> structArray;
  Vector<float> floatArray;
  for (int i = 0; i < 10; ++i) {
    structArray.push_back({i, {i + .1f, i + .1f, i + .1f}});
    floatArray.push_back(i + .1f);
  }

  // Initialize shader.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  GlProgramObject program;
  program.initialize(
    UNIFORMS_VERTEX_CODE,
    UNIFORMS_FRAGMENT_CODE,
    {{"position", GlVertexAttrib::SLOT_0}},
    {{"floatPrimitive"},
     {"structElement", {"aInt", "aVec3"}, 0},
     {"structArray", {"aInt", "aVec3"}, 10},
     {"floatArray", {}, 10}});

  // Set the active shader program.
  glUseProgram(program.id());
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Primitive.
  {
    auto loc = program.location("floatPrimitive");
    EXPECT_NE(-1, loc);
    glUniform1f(loc, floatPrimitive);
    EXPECT_EQ(GL_NO_ERROR, glGetError());
  }

  // Struct.
  {
    {
      auto loc = program.location("structElement", "aInt");
      EXPECT_NE(-1, loc);
      glUniform1i(loc, structElement.aInt);
      EXPECT_EQ(GL_NO_ERROR, glGetError());
    }
    {
      auto loc = program.location("structElement", "aVec3");
      EXPECT_NE(-1, loc);
      glUniform3fv(loc, 1, structElement.aVec3.data().data());
      EXPECT_EQ(GL_NO_ERROR, glGetError());
    }
  }

  // Struct array.
  for (int i = 0; i < structArray.size(); ++i) {
    {
      auto loc = program.location("structArray", i, "aInt");
      EXPECT_NE(-1, loc);
      glUniform1i(loc, structArray[i].aInt);
      EXPECT_EQ(GL_NO_ERROR, glGetError());
    }
    {
      auto loc = program.location("structArray", i, "aVec3");
      EXPECT_NE(-1, loc);
      glUniform3fv(loc, 1, structArray[i].aVec3.data().data());
      EXPECT_EQ(GL_NO_ERROR, glGetError());
    }
  }

  // Array.
  for (int i = 0; i < floatArray.size(); ++i) {
    auto loc = program.location("floatArray", i);
    EXPECT_NE(-1, loc);
    glUniform1f(loc, floatArray[i]);
    EXPECT_EQ(GL_NO_ERROR, glGetError());
  }
}

}  // namespace c8
