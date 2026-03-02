// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Include the right 'egl.h' file for each platform based on header location and version.

#pragma once

#include "c8/pixels/opengl/gl-version.h"

#if __APPLE__  && !C8_USE_ANGLE
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#include <OpenGLES/EAGL.h>
#else
#include <OpenGL/OpenGL.h>
#endif  // !TARGET_OS_IPHONE
#else   // !__APPLE__  || C8_USE_ANGLE
#include <EGL/egl.h>
#endif  // !__APPLE__  && !C8_USE_ANGLE
