// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// OpenGL macros for  OpenGL versions used in code8.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Run-time methods to query the OpenGL version from the current context.
const char *c8_glVersion();
const char *c8_glShadingVersion();

#ifdef __cplusplus
}
#endif

#if C8_USE_ANGLE

#define C8_OPENGL_ES 1
#define C8_OPENGL_VERSION 3
#define C8_OPENGL_VERSION_2 0
#define C8_OPENGL_VERSION_3 1

#define C8_GL_H <GLES3/gl3.h>
// OpenGL 3.0 extensions are located in GLES2/gl2ext.h according to the standard.
#define C8_GLEXT_H <GLES2/gl2ext.h>

#define C8_HAS_EGL 1
#define C8_HAS_CGL 0
#define C8_HAS_EAGL 0

#define C8_GLSL_VERSION "300 es"
#define C8_GLSL_VERSION_LINE "#version " C8_GLSL_VERSION "\n"

#else  // !C8_USE_ANGLE

#if __APPLE__
#include <TargetConditionals.h>
#endif

#ifdef JAVASCRIPT
#define C8_OPENGL_VERSION 2
#define C8_OPENGL_VERSION_2 1
#define C8_OPENGL_VERSION_3 0
#else
#define C8_OPENGL_VERSION 3
#define C8_OPENGL_VERSION_2 0
#define C8_OPENGL_VERSION_3 1
#endif

#if defined(C8_OPENGL_VERSION_3) && __APPLE__ && !TARGET_OS_IPHONE
#define C8_OPENGL_VERSION_3_CORE_PROFILE 1
#else
#define C8_OPENGL_VERSION_3_CORE_PROFILE 0
#endif

#if (__APPLE__ && TARGET_OS_IPHONE) || defined(ANDROID) || defined(JAVASCRIPT) || defined(LINUX)
#define C8_OPENGL_ES 1
#else
#define C8_OPENGL_ES 0
#endif

#if __APPLE__
#define C8_HAS_EGL 0
#if TARGET_OS_IPHONE
#define C8_HAS_CGL 0
#define C8_HAS_EAGL 1
#else
#define C8_HAS_CGL 1
#define C8_HAS_EAGL 0
#endif
#else
#define C8_HAS_EGL 1
#define C8_HAS_CGL 0
#define C8_HAS_EAGL 0
#endif

#if __APPLE__ && !TARGET_OS_IPHONE
#define C8_GLSL_VERSION "150"
#elif defined(JAVASCRIPT)
#define C8_GLSL_VERSION "100"
#else
#define C8_GLSL_VERSION "300 es"
#endif

#define C8_GLSL_VERSION_LINE "#version " C8_GLSL_VERSION "\n"

#if C8_OPENGL_VERSION_3
#if __APPLE__
#if TARGET_OS_IPHONE
#define C8_GL_H <OpenGLES/ES3/gl.h>
#define C8_GLEXT_H <OpenGLES/ES3/glext.h>
#else
#define C8_GL_H <OpenGL/gl3.h>
#define C8_GLEXT_H <OpenGL/gl3ext.h>
#endif  // !TARGET_OS_IPHONE
#else   // !__APPLE__
#define C8_GL_H <GLES3/gl3.h>
// OpenGL 3.0 extensions are located in GLES2/gl2ext.h according to the standard.
#define C8_GLEXT_H <GLES2/gl2ext.h>
#endif  // !__APPLE__
#endif  // C8_OPENGL_VERSION_3

#if C8_OPENGL_VERSION_2
#if __APPLE__
#if TARGET_OS_IPHONE
#define C8_GL_H <OpenGLES/ES2/gl.h>
#define C8_GLEXT_H <OpenGLES/ES2/glext.h>
#else
#define C8_GL_H <OpenGL/gl.h>
#define C8_GLEXT_H <OpenGL/glext.h>
#endif  // !TARGET_OS_IPHONE
#else   // !__APPLE__
#define C8_GL_H <GLES2/gl2.h>
#define C8_GLEXT_H <GLES2/gl2ext.h>
#endif  // !__APPLE__
#endif  // C8_OPENGL_VERSION_2

#endif  // C8_USE_ANGLE
