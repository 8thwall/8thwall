// Copyright (c) 2023 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@nianticlabs.com)
//
// Additional Objc methods to add to the GLFW library at @glfw//:glfw.

#pragma once

// Return a pointer to a CAMetal layer in a GLFW window that can be passed into
// eglGetPlatformDisplay and eglCreateWindowSurface.
void *nc_getMetalLayer(void *nsWindow);

