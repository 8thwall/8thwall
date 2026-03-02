// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "image-point.h",
  };
  deps = {
    ":detection-config",
    ":feature-manager",
    ":image-descriptor",
    "//c8:string",
    "//c8/string:format",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x3d251fcc);

#include <sstream>

#include "c8/string.h"
#include "c8/string/format.h"
#include "reality/engine/features/image-point.h"

namespace c8 {
String ImagePointLocation::toString() const noexcept {
  return c8::format(
    "(x: %04.2f, y: %04.2f, scale: %d, angle: %04.2f, response: %04.2f, roi: %d)",
    pt.x,
    pt.y,
    static_cast<int>(scale),
    angle,
    response,
    static_cast<int>(roi));
}
}  // namespace c8
