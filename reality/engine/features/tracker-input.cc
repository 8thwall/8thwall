// Copyright (c) 2026 8th Wall, Inc.
// Original Author: Paris Morgan (parismorgan@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"tracker-input.h"};
  deps = {
    ":frame-point",
    "//c8:vector",
  };
}
cc_end(0x940fb570);

#include "reality/engine/features/tracker-input.h"

namespace c8 {}
