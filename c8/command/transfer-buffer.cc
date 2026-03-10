// Copyright (c) 2024 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@nianticlabs.com)
//
// Transfer buffer for use with the CommandBuffer class.

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"transfer-buffer.h"};
  deps = {};
}
cc_end(0x5aa17181);

#include "c8/command/transfer-buffer.h"

namespace c8 {

void TransferBuffer::release(const void *marker) noexcept {
  // Update the read pointer.
  readMarker_ = reinterpret_cast<const char *>(marker);
}

}  // namespace c8
