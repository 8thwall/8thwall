// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "staged-ring-buffer.h",
  };
  deps = {
    ":c8-log",
    ":exceptions",
    ":map",
    ":vector",
  };
}
cc_end(0x89229db1);

#include "c8/staged-ring-buffer.h"

namespace c8 {

// definitions.

}
