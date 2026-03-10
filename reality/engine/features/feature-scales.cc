// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules.h"

cc_library {
  hdrs = {
    "feature-scales.h",
  };
  deps = {
    "//bzl/inliner:rules",
  };
}

#include "reality/engine/features/feature-scales.h"

namespace c8 {}
