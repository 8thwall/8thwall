// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "self-timing-scope-lock.h",
  };
  deps = {
    ":scope-timer",
    "//c8:string",
  };
}
cc_end(0x3e0e47eb);

#include "c8/stats/self-timing-scope-lock.h"

namespace c8 {}  // namespace c8
