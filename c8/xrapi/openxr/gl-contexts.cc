#include "c8/xrapi/openxr/gl-contexts.h"

#include "c8/vector.h"
#include "c8/xrapi/openxr/openxr.h"

namespace c8 {

Vector<XrGraphicsBindingOpenGLESAndroidKHR> *getCompatibleBindings() {
  static Vector<XrGraphicsBindingOpenGLESAndroidKHR> xrCompatibleBindings;
  return &xrCompatibleBindings;
}

void notifyXrCompatibleGlContext(EGLDisplay display, EGLContext context, EGLConfig config) {
  getCompatibleBindings()->emplace_back(XrGraphicsBindingOpenGLESAndroidKHR{
    .type = XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR,
    .next = nullptr,
    .display = display,
    .config = config,
    .context = context,
  });
}

void *getGlGraphicsBinding() { return static_cast<void *>(getCompatibleBindings()->data()); }

}  // namespace c8
