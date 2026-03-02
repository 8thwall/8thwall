// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// A GlPixelBuffer is a pixel buffer that is managed by OpenGL. It can be mapped into client memory
// and then accesed like a normal CPU PixelBuffer. Under the hood it is a pixel buffer object,
// combined with an 8th Wall Pixels header.

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "gl-pixel-buffer.h",
  };
  deps = {
    ":gl-headers",
    ":gl-buffer-object",
    "//c8/pixels:pixels",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x76154f10);

#include "c8/pixels/opengl/gl-pixel-buffer.h"

namespace c8 {

// definitions.

}  // namespace c8
