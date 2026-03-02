// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "c8/pixels/opengl/offscreen-gl-context.h"

#include "c8/exceptions.h"
#include "c8/pixels/opengl/gl-version.h"

#if C8_HAS_EGL
#include "c8/pixels/opengl/offscreen-gl-context-egl-impl.h"
#elif C8_HAS_CGL
#include "c8/pixels/opengl/offscreen-gl-context-cgl-impl.h"
#endif

namespace c8 {

OffscreenGlContext OffscreenGlContext::createRGBA8888Context(void *sharedContext) {
#if C8_HAS_EGL
  OffscreenGlContext ctx(new OffscreenGlContextEglImpl(8, 8, 8, 8, 0, sharedContext));
#elif C8_HAS_CGL
  OffscreenGlContext ctx(new OffscreenGlContextCglImpl(8, 8, 8, 8, 0, sharedContext));
#else
  OffscreenGlContext ctx(nullptr);
  C8_THROW("[offscreen-gl-context@createRGBA8888Context] Not yet implemented for this platform.");
#endif
  return ctx;
}

}  // namespace c8
