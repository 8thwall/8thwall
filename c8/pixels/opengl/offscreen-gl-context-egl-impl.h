// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// An EGL implementation of OffscreenGlContext.

#pragma once

#include "c8/pixels/opengl/gl-version.h"

#if C8_HAS_EGL

#include "c8/pixels/opengl/egl.h"
#include "c8/pixels/opengl/offscreen-gl-context-impl.h"

namespace c8 {

class OffscreenGlContextEglImpl : public OffscreenGlContextImpl {
public:
  // Clean-up the context.
  ~OffscreenGlContextEglImpl() override;

  // Disallow move: since OffscreenGlContextEglImpl is referenced through its interface, it
  // should not typically be moved.
  OffscreenGlContextEglImpl(OffscreenGlContextEglImpl &&) = delete;
  OffscreenGlContextEglImpl &operator=(OffscreenGlContextEglImpl &&) = delete;

  // Disallow copying.
  OffscreenGlContextEglImpl(const OffscreenGlContextEglImpl &) = delete;
  OffscreenGlContextEglImpl &operator=(const OffscreenGlContextEglImpl &) = delete;

private:
  // Create the offscreen context.
  // NOTE: In Emscripten the context dimensions are ignored and set to 300x150px.
  OffscreenGlContextEglImpl(
    int rSize, int gSize, int bSize, int aSize, int depthSize, void *sharedContext = EGL_NO_CONTEXT);

  friend class OffscreenGlContext;

  EGLContext context_;
  EGLDisplay display_;
  EGLContext sharedContext_;
  EGLSurface surfaceUnused_;  // Needed for old versions of android that don't support surface-less contexts.
};

}  // namespace c8

#endif  // C8_HAS_EGL
