// Copyright (c) 2023 Niantic Labs
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)
//
// Definitions and functions for renderers

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "renderers.h",
  };
  deps = {
    "//c8:vector",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x795ac510);

#include "reality/engine/render/renderers.h"

namespace c8 {

bool isDeferredGpuReadPixels(const GpuReadPixelsOptions o) {
  return o == GpuReadPixelsOptions::DEFER_READ
    || o == GpuReadPixelsOptions::DEFER_READ_RESTORE_STATE;
}

bool isRestoreStateGpuReadPixels(const GpuReadPixelsOptions o) {
  return o == GpuReadPixelsOptions::DEFER_READ_RESTORE_STATE
    || o == GpuReadPixelsOptions::READ_IMMEDIATELY_RESTORE_STATE;
}

}  // namespace c8
