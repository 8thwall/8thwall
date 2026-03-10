// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"
#include "c8/pixels/opengl/gl-version.h"

#if C8_HAS_EGL

#include <cstring>
#include <memory>

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/pixels/opengl/offscreen-gl-context-egl-impl.h"
#include "c8/string.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#if C8_OPENGL_VERSION_3
#include "c8/pixels/opengl/eglext.h"
#endif

namespace c8 {

namespace {

constexpr const char *errorMsgFromCode(EGLint error) {
  switch (error) {
    case EGL_SUCCESS:
      return "EGL_SUCCESS";
    case EGL_NOT_INITIALIZED:
      return "EGL_NOT_INITIALIZED";
    case EGL_BAD_ACCESS:
      return "EGL_BAD_ACCESS";
    case EGL_BAD_ALLOC:
      return "EGL_BAD_ALLOC";
    case EGL_BAD_ATTRIBUTE:
      return "EGL_BAD_ATTRIBUTE";
    case EGL_BAD_CONFIG:
      return "EGL_BAD_CONFIG";
    case EGL_BAD_CONTEXT:
      return "EGL_BAD_CONTEXT";
    case EGL_BAD_CURRENT_SURFACE:
      return "EGL_BAD_CURRENT_SURFACE";
    case EGL_BAD_DISPLAY:
      return "EGL_BAD_DISPLAY";
    case EGL_BAD_MATCH:
      return "EGL_BAD_MATCH";
    case EGL_BAD_NATIVE_PIXMAP:
      return "EGL_BAD_NATIVE_PIXMAP";
    case EGL_BAD_NATIVE_WINDOW:
      return "EGL_BAD_NATIVE_WINDOW";
    case EGL_BAD_PARAMETER:
      return "EGL_BAD_PARAMETER";
    case EGL_BAD_SURFACE:
      return "EGL_BAD_SURFACE";
    case EGL_CONTEXT_LOST:
      return "EGL_CONTEXT_LOST";
    default:
      return "EGL_UNKNOWN_ERROR";
  }
}

String formatEglError(const char *msg) {
  return String(msg) + " (" + errorMsgFromCode(eglGetError()) + ")";
}

}  // namespace

OffscreenGlContextEglImpl::OffscreenGlContextEglImpl(
  int rSize, int gSize, int bSize, int aSize, int depthSize, void *sharedContext)
    : context_(EGL_NO_CONTEXT),
      display_(EGL_NO_DISPLAY),
      sharedContext_(sharedContext),
      surfaceUnused_(EGL_NO_SURFACE) {

#ifdef __EMSCRIPTEN__
  // clang-format off
  EM_ASM(
    if (typeof document === 'undefined') {
      const path = require('path');
      const runfilesDir = process.env.RUNFILES_DIR || path.join(__dirname, '{label}.runfiles');
      const {initDomWindow} = require(path.join(runfilesDir, '_main', 'c8/pixels/opengl/dom-polyfill'));
      initDomWindow({width: 300, height: 150});
    }

    let canvas;
    if (typeof OffscreenCanvas !== 'undefined') {
      canvas = new OffscreenCanvas(300, 150);
    } else {
      canvas = document.createElement("canvas");
    }
    const moduleObject = typeof Module !== 'undefined' ? Module : window.Module;
    Object.assign(moduleObject, {
      canvas : canvas,
    });
  );
  // clang-format on
#endif

#ifdef C8_HAS_X11
  Display *xDisplay = XOpenDisplay(NULL);
  if (!xDisplay) {
    C8_THROW("[offscreen-gl-context-egl-impl] Unable to open XDisplay");
  }
  display_ = eglGetDisplay(xDisplay);
#else  // !C8_HAS_X11
#ifdef C8_USE_ANGLE
  constexpr EGLint displayAttribs[] = {
    EGL_PLATFORM_ANGLE_TYPE_ANGLE,
#ifdef __APPLE__
    EGL_PLATFORM_ANGLE_TYPE_METAL_ANGLE,
#else   // !__APPLE__
    EGL_PLATFORM_ANGLE_TYPE_DEFAULT_ANGLE,
#endif  // __APPLE__
    EGL_NONE};
  display_ = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, nullptr, displayAttribs);
#else
  display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#endif  // C8_USE_ANGLE
#endif  // C8_HAS_X11

  if (!eglInitialize(display_, nullptr, nullptr)) {
    C8_THROW_INVALID_ARGUMENT(
      formatEglError("[offscreen-gl-context-egl-impl] Unable to initialize EGL display"));
  }

  /* Here, the application chooses the configuration it desires.
   * find the best match if possible, otherwise use the very first one
   */
  const EGLint configAttribs[] = {
    EGL_BIND_TO_TEXTURE_RGBA,
    (rSize > 0 && gSize > 0 && bSize > 0 && aSize > 0) ? EGL_TRUE : EGL_DONT_CARE,
    EGL_BIND_TO_TEXTURE_RGB,
    (rSize > 0 && gSize > 0 && bSize > 0 && aSize == 0) ? EGL_TRUE : EGL_DONT_CARE,
    EGL_RED_SIZE,
    rSize,
    EGL_BLUE_SIZE,
    bSize,
    EGL_GREEN_SIZE,
    gSize,
    EGL_ALPHA_SIZE,
    aSize,
    EGL_DEPTH_SIZE,
    depthSize,
    EGL_RENDERABLE_TYPE,
#if C8_OPENGL_VERSION_3
    EGL_OPENGL_ES3_BIT_KHR,
#else
    EGL_OPENGL_ES2_BIT,
#endif
    EGL_SURFACE_TYPE,
    EGL_PBUFFER_BIT,
    EGL_NONE,
  };

  EGLint numConfigs = 0;
  eglChooseConfig(display_, configAttribs, nullptr, 0, &numConfigs);
  std::unique_ptr<EGLConfig[]> configs(new EGLConfig[numConfigs]);
  eglChooseConfig(display_, configAttribs, configs.get(), numConfigs, &numConfigs);

  EGLConfig *config = nullptr;
  for (int i = 0; i < numConfigs; ++i) {
    const EGLConfig &possibleConfig = configs[i];
    EGLint r = 0, g = 0, b = 0, a = 0;
    eglGetConfigAttrib(display_, possibleConfig, EGL_RED_SIZE, &r);
    eglGetConfigAttrib(display_, possibleConfig, EGL_GREEN_SIZE, &g);
    eglGetConfigAttrib(display_, possibleConfig, EGL_BLUE_SIZE, &b);
    eglGetConfigAttrib(display_, possibleConfig, EGL_ALPHA_SIZE, &a);

    if (r == rSize && g == gSize && b == bSize && a == aSize) {
      config = &configs[i];
      break;
    }
  }

  if (!config) {
    C8Log("[offscreen-gl-context-egl-impl] %s", "eglChooseConfig failed; attempting fallback.");
    // Try again with empty configs.
    const EGLint emptyAttribs[] = {EGL_NONE};
    numConfigs = 0;
    eglChooseConfig(display_, emptyAttribs, nullptr, 0, &numConfigs);
    configs.reset(new EGLConfig[numConfigs]);
    eglChooseConfig(display_, emptyAttribs, configs.get(), numConfigs, &numConfigs);

    for (int i = 0; i < numConfigs; ++i) {
      const EGLConfig &possibleConfig = configs[i];
      EGLint r = 0, g = 0, b = 0, a = 0;
      eglGetConfigAttrib(display_, possibleConfig, EGL_RED_SIZE, &r);
      eglGetConfigAttrib(display_, possibleConfig, EGL_GREEN_SIZE, &g);
      eglGetConfigAttrib(display_, possibleConfig, EGL_BLUE_SIZE, &b);
      eglGetConfigAttrib(display_, possibleConfig, EGL_ALPHA_SIZE, &a);

      if (r == rSize && g == gSize && b == bSize && a == aSize) {
        config = &configs[i];
        break;
      }
    }
    if (!config) {
      C8_THROW_INVALID_ARGUMENT(
        formatEglError("[offscreen-gl-context-egl-impl] Unable to find EGL config that matches "
                       "requested attributes"));
    }
  }

#ifndef JAVASCRIPT
  // Ensure we support surfaceless contexts.
  const char *extensions = eglQueryString(display_, EGL_EXTENSIONS);
  if (!strstr(extensions, "EGL_KHR_surfaceless_context")) {
    // NOTE(nb): If this is updated, the capture size in GlAndroidCameraSensorApi19.java also needs
    // to be updated.
    // int captureWidth = ApiLimits.IMAGE_PROCESSING_WIDTH;
    // int captureHeight = ApiLimits.IMAGE_PROCESSING_HEIGHT;
    int surfaceAttributes[] = {EGL_WIDTH, 480, EGL_HEIGHT, 640, EGL_NONE};
    surfaceUnused_ = eglCreatePbufferSurface(display_, *config, surfaceAttributes);
    if (surfaceUnused_ == EGL_NO_SURFACE) {
      C8_THROW(formatEglError("[offscreen-gl-context-egl-impl] Unable to create EGL surface"));
    }
  }
#endif

  // TODO(mc): Eventually add support for context sharing here.
  constexpr EGLint contextAttribs[] = {
    EGL_CONTEXT_CLIENT_VERSION,
    C8_OPENGL_VERSION,
    EGL_NONE,
  };
  context_ = eglCreateContext(display_, *config, sharedContext_, contextAttribs);
  if (context_ == EGL_NO_CONTEXT) {
    C8_THROW(formatEglError("[offscreen-gl-context-egl-impl] Unable to create EGL Context"));
  }

  // Create an OpenGL with no surface, since we are rendering to framebuffers.
  EGLBoolean result = eglMakeCurrent(display_, surfaceUnused_, surfaceUnused_, context_);
  if (result != EGL_TRUE) {
    C8_THROW(formatEglError("[offscreen-gl-context-egl-impl] Failed call to eglMakeCurrent"));
  }
}

OffscreenGlContextEglImpl::~OffscreenGlContextEglImpl() {
  if (display_ != EGL_NO_DISPLAY) {
    eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (context_ != EGL_NO_CONTEXT) {
      eglDestroyContext(display_, context_);
      context_ = EGL_NO_CONTEXT;
    }
    if (surfaceUnused_ != EGL_NO_SURFACE) {
      eglDestroySurface(display_, surfaceUnused_);
      surfaceUnused_ = EGL_NO_SURFACE;
    }
    eglTerminate(display_);
    display_ = EGL_NO_DISPLAY;
  }
}

}  // namespace c8

#endif  // C8_HAS_EGL
