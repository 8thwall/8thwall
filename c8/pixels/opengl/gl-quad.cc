// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Christoph Bartschat (christoph@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  name = "gl-quad";
  hdrs = {
    "gl-quad.h",
  };
  deps = {
    ":gl-headers",
    ":gl-vertex-array",
    "//c8:c8-log",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xcd59a18e);

#include "c8/c8-log.h"
#include "c8/pixels/opengl/gl-quad.h"

namespace c8 {

GlVertexArray makeVertexArrayRect() {
  return makeVertexArrayRect({0, 0, 1, 1, 1, 1}, {0, 0, 1, 1, 1, 1});
}

GlVertexArray makeVertexArrayRect(GlSubRect uvSubRect, GlSubRect vertexSubRect) {
  double dl = vertexSubRect.x;
  double du = vertexSubRect.y;
  double dr = vertexSubRect.w + vertexSubRect.x;
  double dd = vertexSubRect.h + vertexSubRect.y;
  double dsw = 2.0 / vertexSubRect.fullW;
  double dsh = 2.0 / vertexSubRect.fullH;

  float vl = static_cast<float>(dl * dsw - 1);
  float vr = static_cast<float>(dr * dsw - 1);
  float vu = static_cast<float>(du * dsh - 1);
  float vd = static_cast<float>(dd * dsh - 1);

  float meshVertices[][3] = {
    {vl, vd, 0.0},  //  -1,  1
    {vl, vu, 0.0},  //  -1, -1
    {vr, vd, 0.0},  //   1,  1
    {vr, vu, 0.0},  //   1, -1
  };

  double sl = uvSubRect.x;
  double su = uvSubRect.y;
  double sr = uvSubRect.w + uvSubRect.x;
  double sd = uvSubRect.h + uvSubRect.y;
  double ssw = 1.0 / uvSubRect.fullW;
  double ssh = 1.0 / uvSubRect.fullH;

  float ul = static_cast<float>(sl * ssw);
  float ur = static_cast<float>(sr * ssw);
  float uu = static_cast<float>(su * ssh);
  float ud = static_cast<float>(sd * ssh);

  float meshUvs[][2] = {
    {ul, ud},  //  0, 1
    {ul, uu},  //  0, 0
    {ur, ud},  //  1, 1
    {ur, uu},  //  1, 0
  };

  const uint8_t meshIndices[][3] = {
    {0, 1, 2},
    {2, 1, 3},
  };

  // Create the vertex array.
  GlVertexArray va;

  // Add the vertices.
  va.addVertexBuffer(
    GlVertexAttrib::SLOT_0,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    sizeof(meshVertices),
    meshVertices,
    GL_STATIC_DRAW);

  // Add the uv texture coordinates.
  va.addVertexBuffer(
    GlVertexAttrib::SLOT_2, 2, GL_FLOAT, GL_FALSE, 0, sizeof(meshUvs), meshUvs, GL_STATIC_DRAW);

  // Set the vertex indices.
  va.setIndexBuffer(
    GL_TRIANGLES, GL_UNSIGNED_BYTE, sizeof(meshIndices), meshIndices, GL_STATIC_DRAW);

  return va;
}

// GlQuad

void GlQuad::initialize() { initialize({0, 0, 1, 1, 1, 1}, {0, 0, 1, 1, 1, 1}); }

void GlQuad::initialize(GlSubRect src, GlSubRect dest) {
  GLint restoreElementBuffer = 0;
  GLint restoreArrayBuffer = 0;
  glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &restoreElementBuffer);
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &restoreArrayBuffer);

  double dl = dest.x;
  double du = dest.y;
  double dr = dest.w + dest.x;
  double dd = dest.h + dest.y;
  double dsw = 2.0 / dest.fullW;
  double dsh = 2.0 / dest.fullH;

  float vl = static_cast<float>(dl * dsw - 1);
  float vr = static_cast<float>(dr * dsw - 1);
  float vu = static_cast<float>(du * dsh - 1);
  float vd = static_cast<float>(dd * dsh - 1);

  float meshVertices[] = {
    // clang-format off
    vl, vd, 0.0,  //  -1,  1
    vl, vu, 0.0,  //  -1, -1
    vr, vd, 0.0,  //   1,  1
    vr, vu, 0.0   //   1, -1
    // clang-format on
  };

  double sl = src.x;
  double su = src.y;
  double sr = src.w + src.x;
  double sd = src.h + src.y;
  double ssw = 1.0 / src.fullW;
  double ssh = 1.0 / src.fullH;

  float ul = static_cast<float>(sl * ssw);
  float ur = static_cast<float>(sr * ssw);
  float uu = static_cast<float>(su * ssh);
  float ud = static_cast<float>(sd * ssh);

  float meshUvs[] = {
    // clang-format off
    ul, ud,  //  0, 1
    ul, uu,  //  0, 0
    ur, ud,  //  1, 1
    ur, uu   //  1, 0
    // clang-format on
  };

  uint16_t meshTriangles[] = {
    // clang-format off
    0, 1, 2,
    2, 1, 3
    // clang-format on
  };

  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(meshVertices), meshVertices, GL_STATIC_DRAW);

  glGenBuffers(1, &uvBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(meshUvs), meshUvs, GL_STATIC_DRAW);

  glGenBuffers(1, &triangleBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(meshTriangles), meshTriangles, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, restoreElementBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, restoreArrayBuffer);
}

void GlQuad::cleanup() {
  glDeleteBuffers(1, &vertexBuffer);
  glDeleteBuffers(1, &uvBuffer);
  glDeleteBuffers(1, &triangleBuffer);
}

}  // namespace c8
