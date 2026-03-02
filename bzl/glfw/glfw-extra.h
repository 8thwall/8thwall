// Copyright (c) 2023 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@nianticlabs.com)
//
// Additional methods to add to the GLFW library at @glfw//:glfw.

#pragma once

#include <GLFW/glfw3.h>

#ifdef __cplusplus
extern "C" {
#endif

// Return a pointer to a GLFW window's native platform window (or layer) that can be passed into
// eglGetPlatformDisplay and eglCreateWindowSurface.
void *nc_getGlfwNativeWindow(GLFWwindow *window);

#ifdef __cplusplus
}
#endif
