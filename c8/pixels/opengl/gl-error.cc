// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "gl-error.h",
  };
  deps = {
    ":gl-headers",
    "//c8:c8-log",
    "//c8:string",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x6b96aac6);

#include "c8/pixels/opengl/gl-error.h"

#include "c8/c8-log.h"
#include "c8/pixels/opengl/gl.h"

namespace c8 {
namespace {
constexpr bool ENABLE_GL_ERRORS = false;
}

// Checking for GL errors adds noticeable overhead and should only be used in debugging.
void checkGLError(const String &op) {
  if (!ENABLE_GL_ERRORS) {
    return;
  }
  auto error = glGetError();
  if (error != GL_NO_ERROR) {
    C8Log("%s: glError: 0x%x", op.c_str(), error);
  }
}
}  // namespace c8
