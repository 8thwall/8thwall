// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "model-data-view.h",
  };
  deps = {
    "//c8:vector",
    "//c8/geometry:mesh-types",
    "//c8/geometry:splat",
    "//c8/pixels:pixels",
  };
}
cc_end(0x6d7ff40b);

#include "c8/model/model-data-view.h"

namespace c8 {

// Empty

}  // namespace c8
