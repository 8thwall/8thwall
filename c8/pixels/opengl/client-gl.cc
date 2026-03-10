// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "c8/pixels/opengl/client-gl.h"

#include "c8/exceptions.h"
#include "c8/pixels/opengl/gl-version.h"

#if C8_HAS_EGL
#include "c8/pixels/opengl/egl.h"
#elif C8_HAS_CGL
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace c8 {

ClientGlFunctionType *clientGlGetProcAddress(const char *procName) {
#if C8_HAS_EGL
  return eglGetProcAddress(procName);
#elif C8_HAS_CGL
  CFStringRef symbolName =
    CFStringCreateWithCString(kCFAllocatorDefault, procName, kCFStringEncodingASCII);

  auto framework = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengl"));

  ClientGlFunctionType *funcAddress =
    (ClientGlFunctionType *)CFBundleGetFunctionPointerForName(framework, symbolName);

  CFRelease(symbolName);

  return funcAddress;
#else
  C8_THROW("[client-gl@clientGlGetProcAddress] Not yet implemented for this platform.");
#endif
}

}  // namespace c8
