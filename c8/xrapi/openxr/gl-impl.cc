#include "c8/xrapi/openxr/gl-impl.h"

#include <exception>

#include "c8/c8-log.h"

namespace c8 {

static c8::GlBuffer *cb_;

GlBuffer *getXrCommandBuffer() { return cb_; }

void setXrCommandBuffer(GlBuffer *glBuffer) { cb_ = glBuffer; }

void checkGLErrorBuffer(GlBuffer *glBuffer, const char *msg) {
  if (true) {
    return;
  }
  if (glBuffer == nullptr) {
    C8Log("[gl-impl] glBuffer is null");
    return;
  }

  GLenum error = glBuffer->runSyncCommand(glGetError);
  if (error != GL_NO_ERROR) {
    C8Log("[gl-impl] GL Error: %s: %d", msg, error);
    std::terminate();
  }
}

}  // namespace c8
