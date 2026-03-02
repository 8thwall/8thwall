// Copyright (c) 2024 Niantic, Inc.
// Original Author: Anvith Ekkati (anvith@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "rolling-average.h",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xfaaea85a);

#include "c8/rolling-average.h"

namespace c8 {}  // namespace c8
