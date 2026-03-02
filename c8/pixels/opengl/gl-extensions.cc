// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"gl-extensions.h"},
  visibility = {
    "//visibility:public",
  };
  deps = {
    ":gl-headers",
  };
}
cc_end(0x13746573);

#include "c8/pixels/opengl/gl-extensions.h"

#include "c8/pixels/opengl/gl.h"

namespace c8 {

bool hasGlExtension(const char *extensionName) {
#if C8_OPENGL_VERSION_3
  GLint n, i;
  glGetIntegerv(GL_NUM_EXTENSIONS, &n);
  for (i = 0; i < n; i++) {
    bool extensionAvailable =
      (strstr((const char *)glGetStringi(GL_EXTENSIONS, i), extensionName) != nullptr);
    if (extensionAvailable) {
      return true;
    }
  }
  return false;
#else
  return (strstr((const char *)glGetString(GL_EXTENSIONS), extensionName) != nullptr);
#endif
}

}  // namespace c8
