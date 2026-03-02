// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "codec-api.h",
  };
  visibility = {
    ":codec-protected",
  };
  deps = {
    "//c8/media:media-status",
    "@json//:json",
  };
}
cc_end(0x5657b992);

#include "c8/media/codec/codec-api.h"

namespace c8 {

// definitions

}  // namespace c8
