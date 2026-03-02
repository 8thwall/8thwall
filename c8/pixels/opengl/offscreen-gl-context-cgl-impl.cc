// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"
#include "c8/pixels/opengl/gl-version.h"

#if C8_HAS_CGL

#include <memory>

#include "c8/exceptions.h"
#include "c8/pixels/opengl/offscreen-gl-context-cgl-impl.h"
#include "c8/string.h"

#if C8_OPENGL_VERSION_3
#include "c8/pixels/opengl/eglext.h"
#endif

namespace c8 {

namespace {

String formatCglError(CGLError error, const char *msg) {
  return String(msg) + " (" + CGLErrorString(error) + ")";
}

}  // namespace

OffscreenGlContextCglImpl::OffscreenGlContextCglImpl(
  int rSize, int gSize, int bSize, int aSize, int depthSize, void *sharedContext)
    : pixelFormat_(nullptr),
      context_(nullptr),
      sharedContext_(reinterpret_cast<CGLContextObj>(sharedContext)) {

  const CGLPixelFormatAttribute pixelFormatAttribs[] = {
    kCGLPFAAccelerated,
    kCGLPFAColorSize,
    static_cast<CGLPixelFormatAttribute>(rSize + gSize + bSize),
    kCGLPFAAlphaSize,
    static_cast<CGLPixelFormatAttribute>(aSize),
    kCGLPFADepthSize,
    static_cast<CGLPixelFormatAttribute>(depthSize),
    kCGLPFAOpenGLProfile,
    static_cast<CGLPixelFormatAttribute>(kCGLOGLPVersion_3_2_Core),
    static_cast<CGLPixelFormatAttribute>(0),  // Zero-terminated.
  };

  CGLError result;
  GLint numPixelFormats;

  result = CGLChoosePixelFormat(pixelFormatAttribs, &pixelFormat_, &numPixelFormats);

  if (pixelFormat_ == nullptr) {
    C8_THROW_INVALID_ARGUMENT(formatCglError(
      result,
      "[offscreen-gl-context-cgl-impl] Unable to find a CGL Pixel Format that matches requested "
      "attributes"));
  }

  // TODO(mc): Eventually add support for context sharing here.
  result = CGLCreateContext(pixelFormat_, sharedContext_, &context_);
  if (result) {
    C8_THROW(
      formatCglError(result, "[offscreen-gl-context-cgl-impl] Unable to create CGL Context"));
  }

  result = CGLSetCurrentContext(context_);
  if (result) {
    C8_THROW(
      formatCglError(result, "[offscreen-gl-context-cgl-impl] Failed call to set CGL Context"));
  }
}

OffscreenGlContextCglImpl::~OffscreenGlContextCglImpl() {
  if (context_) {
    CGLDestroyContext(context_);
    context_ = nullptr;
  }
  if (pixelFormat_) {
    CGLDestroyPixelFormat(pixelFormat_);
    pixelFormat_ = nullptr;
  }
}

}  // namespace c8

#endif  // C8_HAS_CGL
