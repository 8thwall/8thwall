// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//visibility:public",
  };
  hdrs = {"strcat.h"}, deps = {"//c8:string"};
}
cc_end(0x3f144cfb);

#include "c8/string/strcat.h"

namespace c8 {

// definitions

}  // namespace c8
