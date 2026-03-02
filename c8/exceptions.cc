// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "exceptions.h",
  };
  deps = {
    ":c8-log",
    "//c8/string:format",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xa6ec3ed6);

#include "c8/exceptions.h"

namespace c8 {

// definitions

}  // namespace c8
