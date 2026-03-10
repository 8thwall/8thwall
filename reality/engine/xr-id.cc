// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "xr-id.h",
  };
  deps = {
    "//c8/string:format",
    "//c8:string",
  };
  visibility = {
    "//apps/client/internalqa:__subpackages__",
    "//reality:__subpackages__",
  };
}
cc_end(0x2d24ace2);

#include "reality/engine/xr-id.h"

namespace c8 {}
