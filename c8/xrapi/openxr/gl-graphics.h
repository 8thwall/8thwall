#pragma once

#include "c8/pixels/opengl/egl.h"
#include "c8/pixels/opengl/eglext.h"
#include "c8/pixels/opengl/gl.h"
#include "c8/pixels/opengl/glext.h"
#include "c8/vector.h"
#include "c8/xrapi/openxr/gl-impl.h"
#include "c8/xrapi/openxr/openxr.h"

namespace c8 {
class GlSwapchain {
public:
  GlSwapchain(
    GlBuffer *glBuffer,
    GLuint textureId,
    uint32_t width,
    uint32_t height,
    uint32_t count,
    int64_t format,
    XrSwapchainImageOpenGLESKHR *images);

  void bind(int index);

  void updateTextureId(GLuint textureId);

private:
  Vector<GLuint> textures_;
  Vector<EGLImage> eglImages_;
  GLuint renderTexture_ = 0;
  uint32_t width_;
  uint32_t height_;
  PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES_ = nullptr;

  GlBuffer *glBuffer_;
};

void updateTextureFoveation(GlBuffer *glBuffer, int renderTextureId, float fixedFoveation);
void glFinalize(GlBuffer *glBuffer, bool removeDepthStore);
int64_t selectGlColorFormat(const Vector<int64_t> &formats);
int64_t selectGlDepthFormat(const Vector<int64_t> &formats);

}  // namespace c8
