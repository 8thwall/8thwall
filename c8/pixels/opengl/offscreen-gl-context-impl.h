// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Interface for offscreen GL context implementations.

#pragma once

namespace c8 {

class OffscreenGlContextImpl {
public:
  // Clean-up the context.
  virtual ~OffscreenGlContextImpl() = default;
};

}  // namespace c8
