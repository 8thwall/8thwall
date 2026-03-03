// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// An offscreen GL context and surface for offscreen rendering.

#pragma once

#include <memory>

#include "c8/pixels/opengl/offscreen-gl-context-impl.h"

namespace c8 {

class OffscreenGlContext : public OffscreenGlContextImpl {
public:
  // Create a RGBA8888 Context, without depth or stencil buffers.
  // NOTE: If you need some other type of context, create another factory method here.
  static OffscreenGlContext createRGBA8888Context(void *sharedContext = nullptr);

  // Clean-up the context.
  ~OffscreenGlContext() override = default;

  // Default move constructors.
  OffscreenGlContext(OffscreenGlContext &&) = default;
  OffscreenGlContext &operator=(OffscreenGlContext &&) = default;

  // Disallow copying.
  OffscreenGlContext(const OffscreenGlContext &) = delete;
  OffscreenGlContext &operator=(const OffscreenGlContext &) = delete;

private:
  OffscreenGlContext(OffscreenGlContextImpl *impl) : impl_(impl) {}

  std::unique_ptr<OffscreenGlContextImpl> impl_;
};

}  // namespace c8
