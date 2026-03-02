// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Christoph Bartschat (christoph@8thwall.com)
#pragma once

#include "c8/pixels/opengl/gl.h"

#include "c8/pixels/opengl/gl-vertex-array.h"

namespace c8 {

struct GlSubRect {
  int x;
  int y;
  int w;
  int h;
  int fullW;
  int fullH;
};

// Create a vertex array rectangle, consisisting of two uv-mapped triangles that fully cover the
// clip area and texture area in a no-op vertex shader.
GlVertexArray makeVertexArrayRect();

// Create a vertex array rectangle, consisisting of two uv-mapped triangles.
//
// Parameters:
//   uvSubRect - specifies a sub-window for the uv texture coordinates
//   uvVertexRect - specifies a sub-window, relative to (fullW, fullH) for the vertices.
GlVertexArray makeVertexArrayRect(GlSubRect uvSubRect, GlSubRect vertexSubRect);

// This struct is deprecated. Use the makeVertexArrayRect methods above.
struct GlQuad {
  GLuint vertexBuffer = 0;
  GLuint uvBuffer = 0;
  GLuint triangleBuffer = 0;

  void initialize();
  void initialize(GlSubRect src, GlSubRect dest);
  void cleanup();
};

}  // namespace c8
