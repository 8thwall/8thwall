// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Standard names for attribute locations in code8.

#pragma once

#include "c8/pixels/opengl/gl.h"

namespace c8 {

// The slots used in the GlProgramObject::initialize() call should match the ones
// in the GlVertexArray::addVertexBuffer
enum class GlVertexAttrib : GLuint {
  SLOT_0 = 0,
  SLOT_1 = 1,
  SLOT_2 = 2,
  SLOT_3 = 3,
  SLOT_4 = 4,
  SLOT_5 = 5,
  SLOT_6 = 6,
  SLOT_7 = 7,
  SLOT_8 = 8,
  SLOT_9 = 9,
  SLOT_10 = 10,
  SLOT_11 = 11,
  SLOT_12 = 12,
  SLOT_13 = 13,
  SLOT_14 = 14,
  SLOT_15 = 15,

  // Ensure this is always 1 greater than the maximum attribute index.
  // Almost all GL implementations support at least 16 attributes (from GL_MAX_VERTEX_ATTRIBS).
  MAX_INDEX = 16,
};

}  // namespace c8
