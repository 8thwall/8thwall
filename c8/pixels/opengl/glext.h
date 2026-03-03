// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Include the right 'glext.h' file for each platform based on header location and version.

#pragma once

#include "c8/pixels/opengl/gl-version.h"

#if C8_OPENGL_VERSION_3
#ifdef ANDROID
// Define guard workaround for Android API<21 bug.
// See https://stackoverflow.com/questions/31003863/gles-3-0-including-gl2ext-h
#define __gl2_h_
#endif  // ANDROID
#endif  // C8_OPENGL_VERSION_3

#include C8_GLEXT_H
