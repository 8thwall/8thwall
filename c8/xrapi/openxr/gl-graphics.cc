#include "c8/xrapi/openxr/gl-graphics.h"

#include "c8/c8-log.h"
#include "c8/xrapi/openxr/gl-impl.h"
#include "c8/xrapi/openxr/gl-texture-auto-restore.h"

namespace c8 {

GlSwapchain::GlSwapchain(
  GlBuffer *glBuffer,
  GLuint textureId,
  uint32_t width,
  uint32_t height,
  uint32_t count,
  int64_t format,
  XrSwapchainImageOpenGLESKHR *images) {

  glBuffer_ = glBuffer;

  width_ = width;
  height_ = height;

  renderTexture_ = textureId;

  textures_.resize(count);
  eglImages_.resize(count);

  PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR =
    reinterpret_cast<PFNEGLCREATEIMAGEKHRPROC>(eglGetProcAddress("eglCreateImageKHR"));

  if (eglCreateImageKHR == nullptr) {
    C8Log("[gl-graphics] Failed to get eglCreateImageKHR");
    raise(SIGTRAP);
  }

  glEGLImageTargetTexture2DOES_ = reinterpret_cast<PFNGLEGLIMAGETARGETTEXTURE2DOESPROC>(
    eglGetProcAddress("glEGLImageTargetTexture2DOES"));

  if (glEGLImageTargetTexture2DOES_ == nullptr) {
    C8Log("[gl-graphics] Failed to get glEGLImageTargetTexture2DOES");
    raise(SIGTRAP);
  }

  EGLint attributes[] = {EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE};

  for (int i = 0; i < count; i++) {
    textures_[i] = images[i].image;

    eglImages_[i] = glBuffer->runSyncCommand(
      +[](EGLint *_attributes, GLuint _texture, PFNEGLCREATEIMAGEKHRPROC _eglCreateImageKHR)
        -> EGLImage {
        return _eglCreateImageKHR(
          eglGetCurrentDisplay(),
          eglGetCurrentContext(),
          EGL_GL_TEXTURE_2D_KHR,
          reinterpret_cast<EGLClientBuffer>(_texture),
          _attributes);
      },
      attributes,
      textures_[i],
      eglCreateImageKHR);

    checkGLErrorBuffer(glBuffer_, "eglCreateImageKHR");

    if (eglImages_[i] == EGL_NO_IMAGE_KHR) {
      C8Log("[gl-graphics] Failed to create EGL image for texture %d", textures_[i]);
      raise(SIGTRAP);
    }
  }
  checkGLErrorBuffer(glBuffer_, "[gl-graphics] GlSwapchain::GlSwapchain");
}

void GlSwapchain::bind(int index) {
  checkGLErrorBuffer(glBuffer_, "[gl-graphics] bind:start");
  TextureAutoRestore textureAutoRestore(glBuffer_);

  glBuffer_->queueCommand(glBindTexture, GL_TEXTURE_2D, renderTexture_);
  checkGLErrorBuffer(glBuffer_, "[gl-graphics] bind:texture");
  glBuffer_->queueCommand(
    +[](PFNGLEGLIMAGETARGETTEXTURE2DOESPROC _glEGLImageTargetTexture2DOES, EGLImage _eglImage) {
      _glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, _eglImage);
    },
    glEGLImageTargetTexture2DOES_,
    eglImages_[index]);

  checkGLErrorBuffer(glBuffer_, "[gl-graphics] bind:eglImage");

  // This glDisable call is used to make sure the color output isn't overly gamma corrected.
  // This seems to work generally for apps, like solstice.
  // It's possible there might be cases where this affects other rendering operations
  // unintentionally.
  // Refer to https://registry.khronos.org/OpenGL/extensions/EXT/EXT_framebuffer_sRGB.txt
  glBuffer_->queueCommand(glDisable, GL_FRAMEBUFFER_SRGB_EXT);

  checkGLErrorBuffer(glBuffer_, "[gl-graphics] bind:done");
}

void GlSwapchain::updateTextureId(GLuint textureId) { renderTexture_ = textureId; }

void glFinalize(GlBuffer *glBuffer, bool removeDepthStore) {
  checkGLErrorBuffer(glBuffer, "[gl-graphics] glFinalize:start");

  if (removeDepthStore) {
    // Remove unnecessary depth texture store
    // https://developers.meta.com/horizon/documentation/unity/po-advanced-gpu-pipelines
    static const GLenum attachments[] = {GL_DEPTH_ATTACHMENT};
    glBuffer->queueCommand(glInvalidateFramebuffer, GL_FRAMEBUFFER, 1, attachments);
  }

  glBuffer->queueCommand(glEnable, GL_FRAMEBUFFER_SRGB_EXT);

  checkGLErrorBuffer(glBuffer, "[gl-graphics] glFinalize:done");
}

float getFoveaArea(float fixedFoveation, float minFoveaArea = 12.f, float maxFoveaArea = 40.f) {
  return minFoveaArea + (1.0f - fixedFoveation) * (maxFoveaArea - minFoveaArea);
}

void updateTextureFoveation(GlBuffer *glBuffer, int renderTextureId, float fixedFoveation) {
  TextureAutoRestore textureAutoRestore(glBuffer);

  glBuffer->queueCommand(glBindTexture, GL_TEXTURE_2D, renderTextureId);

  // Reference: https://registry.khronos.org/OpenGL/extensions/QCOM/QCOM_texture_foveated.txt
  // Check the bitflags to see if foveation is supported or already applied.

  GLint query;
  glBuffer->runSyncCommand(
    glGetTexParameteriv, GL_TEXTURE_2D, GL_TEXTURE_FOVEATED_FEATURE_QUERY_QCOM, &query);
  if (!((query & GL_FOVEATION_ENABLE_BIT_QCOM)
        && (query & GL_FOVEATION_SCALED_BIN_METHOD_BIT_QCOM))) {
    C8Log("[gl-graphics] Warning: foveation is not supported");
    return;
  }

  // Disable foveation if the fixed foveation is 0.0 (or close to it)
  if (fixedFoveation <= 0.01f) {
    glBuffer->queueCommand(
      glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_FOVEATED_FEATURE_BITS_QCOM, 0);
    return;
  }

  glBuffer->runSyncCommand(
    glGetTexParameteriv, GL_TEXTURE_2D, GL_TEXTURE_FOVEATED_FEATURE_BITS_QCOM, &query);
  if (!((query & GL_FOVEATION_ENABLE_BIT_QCOM)
        && (query & GL_FOVEATION_SCALED_BIN_METHOD_BIT_QCOM))) {

    // Enable foveation on the double wide texture
    glBuffer->queueCommand(
      glTexParameteri,
      GL_TEXTURE_2D,
      GL_TEXTURE_FOVEATED_FEATURE_BITS_QCOM,
      GL_FOVEATION_ENABLE_BIT_QCOM | GL_FOVEATION_SCALED_BIN_METHOD_BIT_QCOM);

    glBuffer->queueCommand(
      glTexParameterf, GL_TEXTURE_2D, GL_TEXTURE_FOVEATED_MIN_PIXEL_DENSITY_QCOM, 0.125f);
  }

  // Used to set foveation parameters on texture
  static PFNGLTEXTUREFOVEATIONPARAMETERSQCOMPROC glTextureFoveationParametersQCOM =
    reinterpret_cast<PFNGLTEXTUREFOVEATIONPARAMETERSQCOMPROC>(
      eglGetProcAddress("glTextureFoveationParametersQCOM"));

  // These parameters were chosen based on some experimentation while comparing to the Quest Browser
  // It's possible these parameters should be different for other headsets

  // The focalX is brought in a bit closer than +/- 0.5, since foveation in the middle of the screen
  // is very noticeable.
  // The focalY is slightly above the center of the image, since the final rendered image seemed to
  // be foveating too much near the top and not enough near the bottom.
  // The gainX and gainY are set to be fairly strong.

  // Left Eye Parameters
  float focalX = -0.4225f;
  float focalY = 0.1f;  // Slightly above center
  float focalPointIdx = 0;
  float gainX = 8.0f * 2;  // Double wide texture
  float gainY = 10.0f;

  // Make the sharp area larger for smaller foveation levels
  float foveaArea = getFoveaArea(fixedFoveation);

  glBuffer->queueCommand(
    glTextureFoveationParametersQCOM,
    renderTextureId,
    0,
    focalPointIdx,
    focalX,
    focalY,
    gainX,
    gainY,
    foveaArea);

  // Right Eye Parameters
  focalX = 0.4225f;
  focalPointIdx = 1;

  glBuffer->queueCommand(
    glTextureFoveationParametersQCOM,
    renderTextureId,
    0,
    focalPointIdx,
    focalX,
    focalY,
    gainX,
    gainY,
    foveaArea);

  checkGLErrorBuffer(glBuffer, "foveation parameters");
}

int64_t selectGlColorFormat(const Vector<int64_t> &formats) {
  for (int64_t format : formats) {
    if (format == GL_SRGB8_ALPHA8) {
      return format;
    }
  }
  C8Log("[gl-graphics] No supported color format found, using %d", formats[0]);
  return formats[0];
}

}  // namespace c8
