// Copyright (c) 2024 Niantic, Inc.
// Original Author: Lynn Dang (lynndang@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "xrweb.h",
  };
  deps = {
    "//c8:string",
  };
}
cc_end(0x08637cca);

#include "reality/app/xr/js/xrweb.h"
namespace c8 {

XrwebEnvironment &xrwebEnvironment() {
  static XrwebEnvironment e;
  return e;
}

}  // namespace c8
