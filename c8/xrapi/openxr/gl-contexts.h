#pragma once

#include "c8/pixels/opengl/gl.h"
#include "c8/pixels/opengl/egl.h"
#include "c8/pixels/opengl/eglext.h"

namespace c8 {

void notifyXrCompatibleGlContext(EGLDisplay display, EGLContext context, EGLConfig config);

void *getGlGraphicsBinding();

}  // namespace c8
