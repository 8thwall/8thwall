// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "gl-version.h",
  };
  deps = {
    ":gl-headers",
  };
  visibility = {
    "//visibility:public",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x45e3181f);

#include "c8/pixels/opengl/gl-constants.h"
#include "c8/pixels/opengl/gl-version.h"

extern "C" {

const char* c8_glVersion() { return reinterpret_cast<const char *>(glGetString(GL_VERSION)); }

const char *c8_glShadingVersion() {
  return reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
}

}  // extern "C"
