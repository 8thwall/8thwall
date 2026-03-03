// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Christoph Bartschat (christoph@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "gl-texture.h",
  };
  deps = {
    ":gl-headers",
    "//c8:c8-log",
    "//c8/pixels/render:texture",
    "//c8:string",
    "//c8/string:format",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xe9902e7a);

#include "c8/c8-log.h"
#include "c8/pixels/opengl/gl-texture.h"
#include "c8/pixels/opengl/glext.h"
#include "c8/string/format.h"

#if C8_OPENGL_VERSION_2
#define GL_RED                            0x1903
#define GL_R32F                           0x822E
#define GL_R32UI                          0x8236
#define GL_RGBA32UI                       0x8D70
#define GL_RED_INTEGER                    0x8D94
#define GL_RGBA_INTEGER                   0x8D99
#endif

namespace c8 {

GlTexture2D makeLinearRGBA8888Texture2D(int width, int height) {
  GlTexture2D tex;
  tex.initialize(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

  tex.bind();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  tex.unbind();

  return tex;
}

GlTexture2D makeNearestRGBA8888Texture2D(int width, int height) {
  GlTexture2D tex;
  tex.initialize(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

  tex.bind();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  tex.unbind();

  return tex;
}

GlTexture2D makeLinearRGBA32Texture2D(int width, int height) {
  GlTexture2D tex;
  tex.initialize(
    GL_TEXTURE_2D, GL_RGBA32UI, width, height, GL_RGBA_INTEGER, GL_UNSIGNED_INT, nullptr);

  tex.bind();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  tex.unbind();

  return tex;
}

GlTexture2D makeNearestR32Texture2D(int width, int height) {
  GlTexture2D tex;
  tex.initialize(
    GL_TEXTURE_2D, GL_R32UI, width, height, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);

  tex.bind();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  tex.unbind();

  return tex;
}

GlTexture2D makeNearestRGBA32Texture2D(int width, int height) {
  GlTexture2D tex;
  tex.initialize(
    GL_TEXTURE_2D, GL_RGBA32UI, width, height, GL_RGBA_INTEGER, GL_UNSIGNED_INT, nullptr);

  tex.bind();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  tex.unbind();

  return tex;
}

GlTexture2D makeLinearR32FTexture2D(int width, int height) {
  GlTexture2D tex;
  tex.initialize(GL_TEXTURE_2D, GL_R32F, width, height, GL_RED, GL_FLOAT, nullptr);

  tex.bind();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  tex.unbind();

  return tex;
}

GlTextureCubemap makeRGBA8888TextureCubemap(int width, int height) {
  GlTextureCubemap tex;
  tex.initialize(GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE);
  return tex;
}

GlTexture wrapRGBA8888Texture(GLuint id, int width, int height) {
  return GlTexture{id, GL_TEXTURE_2D, width, height, GL_RGBA, GL_UNSIGNED_BYTE};
}

#ifdef GL_TEXTURE_EXTERNAL_OES
GlTexture2D makeExternalTexture2D(int width, int height) {
  GlTexture2D tex;
  // The default texure parameters are clamp-to-edge and linear.
  tex.initialize(
    GL_TEXTURE_EXTERNAL_OES, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

  return tex;
}
#else
GlTexture2D makeExternalTexture2D(int width, int height) {
  C8Log("[gl-texture] ERROR: makeExternalTexture2D GL_TEXTURE_EXTERNAL_OES not available.");
  return {};
}
#endif

GlTexture2D::GlTexture2D() noexcept
    : texture_(0), target_(), width_(), height_(), format_(), type_() {
  glGenTextures(1, &texture_);
}

GlTexture2D::~GlTexture2D() noexcept {
  if (texture_) {
    glDeleteTextures(1, &texture_);
  }
}

void GlTexture2D::initialize(
  GLenum target,
  GLint internalFormat,
  GLsizei width,
  GLsizei height,
  GLenum format,
  GLenum type,
  const GLvoid *data) noexcept {
  target_ = target;
  width_ = width;
  height_ = height;
  format_ = format;
  type_ = type;

  // Create the texture by binding it.
  glBindTexture(target_, texture_);

  bool isExternalTextureTarget = false;
#ifdef GL_TEXTURE_EXTERNAL_OES
  if (target_ == GL_TEXTURE_EXTERNAL_OES) {
    isExternalTextureTarget = true;
  }
#endif

  if (!isExternalTextureTarget) {
    // Allocate the texture memory if this is not an external texture.
    glTexImage2D(target_, 0, internalFormat, width_, height_, 0, format_, type_, data);

    // Use exactly 1 mipmap level. Needed to make the texture complete.
#if C8_OPENGL_VERSION_3
    // In OpenGL 3 we can say that there is only one mipmap level.
    glTexParameteri(target_, GL_TEXTURE_MAX_LEVEL, 0);
#else
    // In OpenGL 2 we can only say that the default min filter shouldn't use mipmaps.
    glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#endif
  }

  // Unbind the texture.
  glBindTexture(target_, 0);
}  // namespace c8

GlTexture2D::GlTexture2D(GlTexture2D &&rhs) noexcept {
  memmove(this, &rhs, sizeof(rhs));
  // Reset texture to prevent deletion on destruction.
  rhs.texture_ = 0;
}
GlTexture2D &GlTexture2D::operator=(GlTexture2D &&rhs) noexcept {
  if (texture_) {
    glDeleteTextures(1, &texture_);
  }
  memmove(this, &rhs, sizeof(rhs));
  // Reset texture to prevent deletion on destruction.
  rhs.texture_ = 0;
  return *this;
}

// GlTexture.

void GlTexture::initialize() {
  GLint restoreTexture = 0;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &restoreTexture);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

#if C8_OPENGL_ES || C8_OPENGL_VERSION_2
  // GL_GENERATE_MIPMAP_HINT was removed in OpenGL 3 (non-GLES).
  glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
#endif

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, restoreTexture);
}

void GlTexture::cleanup() { glDeleteTextures(1, &texture); }

TextureTarget GlTexture::textureTarget() const {
  switch (target_) {
    case GL_TEXTURE_2D:
      return TextureTarget::TEXTURE_2D;
      break;

    case GL_TEXTURE_CUBE_MAP:
      return TextureTarget::TEXTURE_CUBE_MAP;
      break;
  }
  return TextureTarget::TEXTURE_2D;
}

void GlTexture::bind() const noexcept { glBindTexture(target_, texture); }
void GlTexture::unbind() const noexcept { glBindTexture(target_, 0); }
void GlTexture::setPixels(const GLvoid *src) {
  glTexSubImage2D(target_, 0, 0, 0, width_, height_, format_, type_, src);
}

void GlTexture2D::bind() const noexcept { glBindTexture(target_, texture_); }

void GlTexture2D::unbind() const noexcept { glBindTexture(target_, 0); }

GlTexture GlTexture2D::tex() const {
  return GlTexture{texture_, target_, width_, height_, format_, type_};
}

// Update the texture 2D image contents.
void GlTexture2D::updateImage(const GLvoid *data) const noexcept {
  glTexSubImage2D(target_, 0, 0, 0, width_, height_, format_, type_, data);
}

String GlTexture::toString() const noexcept {
  return c8::format(
    "(target: %d, width: %d, height: %d, format: %d, type: %d)",
    target_,
    width_,
    height_,
    format_,
    type_);
}

String GlTexture2D::toString() const noexcept {
  return c8::format(
    "(target: %d, width: %d, height: %d, format: %d, type: %d)",
    target_,
    width_,
    height_,
    format_,
    type_);
}

GlTextureCubemap::GlTextureCubemap() noexcept
    : texture_(0), width_(), height_(), format_(), type_() {
  glGenTextures(1, &texture_);
}

GlTextureCubemap::~GlTextureCubemap() noexcept {
  if (texture_) {
    glDeleteTextures(1, &texture_);
  }
}

void GlTextureCubemap::initialize(
  GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type) noexcept {
  width_ = width;
  height_ = height;
  format_ = format;
  type_ = type;

  GLint restoreTexture = 0;
  glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &restoreTexture);

  // Create the texture by binding it.
  glBindTexture(target_, texture_);

  bool isExternalTextureTarget = false;
#ifdef GL_TEXTURE_EXTERNAL_OES
  if (target_ == GL_TEXTURE_EXTERNAL_OES) {
    isExternalTextureTarget = true;
  }
#endif

  if (!isExternalTextureTarget) {
    // Allocate the texture memory for the faces if this is not an external texture.
    for (int i = 0; i < 6; ++i) {
      glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
        0,
        internalFormat,
        width_,
        height_,
        0,
        format_,
        type_,
        nullptr);
    }

    // Use exactly 1 mipmap level. Needed to make the texture complete.
#if C8_OPENGL_VERSION_3
    // In OpenGL 3 we can say that there is only one mipmap level.
    glTexParameteri(target_, GL_TEXTURE_MAX_LEVEL, 0);
#else
    // In OpenGL 2 we can only say that the default min filter shouldn't use mipmaps.
    glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#endif
  }

  glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(target_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(target_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#if !C8_OPENGL_ES
  // GL_TEXTURE_WRAP_R is not defined in OpenGL ES 2.0
  glTexParameteri(target_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
#endif

  // Unbind the texture.
  glBindTexture(target_, restoreTexture);
}  // namespace c8

GlTextureCubemap::GlTextureCubemap(GlTextureCubemap &&rhs) noexcept {
  memmove(this, &rhs, sizeof(rhs));
  // Reset texture to prevent deletion on destruction.
  rhs.texture_ = 0;
}
GlTextureCubemap &GlTextureCubemap::operator=(GlTextureCubemap &&rhs) noexcept {
  if (texture_) {
    glDeleteTextures(1, &texture_);
  }
  memmove(this, &rhs, sizeof(rhs));
  // Reset texture to prevent deletion on destruction.
  rhs.texture_ = 0;
  return *this;
}

void GlTextureCubemap::bind() const noexcept { glBindTexture(target_, texture_); }

void GlTextureCubemap::unbind() const noexcept { glBindTexture(target_, 0); }

GlTexture GlTextureCubemap::tex() const {
  return GlTexture{texture_, target_, width_, height_, format_, type_};
}

// Update the texture 2D image contents.
void GlTextureCubemap::updateImage(GLint target, const GLvoid *data) const noexcept {
  glTexSubImage2D(target, 0, 0, 0, width_, height_, format_, type_, data);
}

String GlTextureCubemap::toString() const noexcept {
  return c8::format(
    "(target: %d, width: %d, height: %d, format: %d, type: %d)",
    target_,
    width_,
    height_,
    format_,
    type_);
}

}  // namespace c8
