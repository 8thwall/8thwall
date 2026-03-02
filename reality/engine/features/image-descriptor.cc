// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules.h"

cc_library {
  hdrs = {
    "image-descriptor.h",
  };
  deps = {
    "//bzl/inliner:rules",
  };
}

#include "reality/engine/features/image-descriptor.h"

namespace c8 {}
