// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//visibility:public",
  };
  deps = {
    ":string",
    ":vector",
  };
  hdrs = {
    "hvector.h",
  };
}
cc_end(0x0b095068);

#include "c8/hvector.h"

namespace c8 {

}  // namespace c8
