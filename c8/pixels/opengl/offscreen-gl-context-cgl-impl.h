// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// An EGL implementation of OffscreenGlContext.

#pragma once

#include "c8/pixels/opengl/gl-version.h"

#if C8_HAS_CGL

#include "c8/pixels/opengl/egl.h"
#include "c8/pixels/opengl/offscreen-gl-context-impl.h"

namespace c8 {

class OffscreenGlContextCglImpl : public OffscreenGlContextImpl {
public:
  // Clean-up the context.
  ~OffscreenGlContextCglImpl() override;

  // Disallow move: since OffscreenGlContextCglImpl is referenced through its interface, it
  // should not typically be moved.
  OffscreenGlContextCglImpl(OffscreenGlContextCglImpl &&) = delete;
  OffscreenGlContextCglImpl &operator=(OffscreenGlContextCglImpl &&) = delete;

  // Disallow copying.
  OffscreenGlContextCglImpl(const OffscreenGlContextCglImpl &) = delete;
  OffscreenGlContextCglImpl &operator=(const OffscreenGlContextCglImpl &) = delete;

private:
  // Create the offscreen context.
  // NOTE: In Emscripten the context dimensions are ignored and set to 300x150px.
  OffscreenGlContextCglImpl(
    int rSize, int gSize, int bSize, int aSize, int depthSize, void *sharedContext = nullptr);

  friend class OffscreenGlContext;

  CGLPixelFormatObj pixelFormat_;
  CGLContextObj context_;
  CGLContextObj sharedContext_;
};

}  // namespace c8

#endif  // C8_HAS_CGL
