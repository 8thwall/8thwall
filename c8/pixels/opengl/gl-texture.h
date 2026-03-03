// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Christoph Bartschat (christoph@8thwall.com)
#pragma once

#include <cstddef>

#include "c8/pixels/opengl/gl.h"
#include "c8/pixels/opengl/glext.h"
#include "c8/pixels/render/texture.h"
#include "c8/string.h"

namespace c8 {

class GlTexture;
class GlTexture2D;
class GlTextureCubemap;

// Return a new GlTexture2D with a TEXTURE_2D target, given dimensions and GL_LINEAR min/mag
// filters, and GL_CLAMP_TO_EDGE wrapping.
GlTexture2D makeLinearRGBA8888Texture2D(int width, int height);

// Return a new GlTexture2D with a TEXTURE_2D target, given dimensions and GL_NEAREST min/mag
// filters, and GL_CLAMP_TO_EDGE wrapping.
GlTexture2D makeNearestRGBA8888Texture2D(int width, int height);

// Return a new GlTexture2D with a TEXTURE_2D target, given dimensions and GL_LINEAR min/mag
// filters, and GL_CLAMP_TO_EDGE wrapping.
GlTexture2D makeLinearRGBA32Texture2D(int width, int height);

// Return a new GlTexture2D with a TEXTURE_2D target, given dimensions and GL_NEAREST min/mag
// filters, and GL_CLAMP_TO_EDGE wrapping.
GlTexture2D makeNearestRGBA32Texture2D(int width, int height);

// Return a new GlTexture2D with a TEXTURE_2D target, given dimensions and GL_NEAREST min/mag
// filters, and GL_CLAMP_TO_EDGE wrapping.
GlTexture2D makeNearestR32Texture2D(int width, int height);

// Return a new GlTexture2D with a TEXTURE_2D target, given dimensions and GL_LINEAR min/mag
// filters, and GL_CLAMP_TO_EDGE wrapping.
GlTexture2D makeLinearR32FTexture2D(int width, int height);

// Return a new GlTextureCubemap with a GL_TEXTURE_CUBE_MAP target, given dimensions for all the
// faces.
GlTextureCubemap makeRGBA8888TextureCubemap(int width, int height);

// Return a new GlTexture2D with a GL_TEXTURE_EXTERNAL_OES target, expected dimensions and
// GL_LINEAR min/mag filter.
GlTexture2D makeExternalTexture2D(int width, int height);

// Return a new GlTexture for the externally specified texture containing rgba8888 data.
GlTexture wrapRGBA8888Texture(GLuint id, int width, int height);

// This is a pointer to a GL texture and its relevant metadata, without any concept of ownerhip over
// that data.
class GlTexture {
public:
  // TODO(nb): make fields private.
  GLuint texture = 0;

  GlTexture() noexcept = default;
  GlTexture(GLuint id, GLenum target, GLsizei width, GLsizei height, GLenum format, GLenum type)
      : texture(id),
        target_(target),
        width_(width),
        height_(height),
        format_(format),
        type_(type) {}

  void bind() const noexcept;
  void unbind() const noexcept;
  void setPixels(const GLvoid *src);

  GLuint id() const { return texture; }
  GLenum target() const { return target_; }
  TextureTarget textureTarget() const;
  GLsizei width() const { return width_; }
  GLsizei height() const { return height_; }
  GLenum format() const { return format_; }
  GLenum type() const { return type_; }

  // TODO(nb): remove these.
  void initialize();
  void cleanup();

  // Simple copying.
  GlTexture(GlTexture &&rhs) noexcept = default;
  GlTexture &operator=(GlTexture &&rhs) noexcept = default;
  GlTexture(const GlTexture &) noexcept = default;
  GlTexture &operator=(const GlTexture &) noexcept = default;

  String toString() const noexcept;

private:
  GLenum target_ = 0;
  GLsizei width_ = 0;
  GLsizei height_ = 0;
  GLenum format_ = 0;
  GLenum type_ = 0;
};

// Class for managing a 2D OpenGL texture.
class GlTexture2D {
public:
  // Create the texture id, but don't allocate the texture.
  GlTexture2D() noexcept;

  // Create a GlTexture2D object which does not represent a texture.
  GlTexture2D(std::nullptr_t) : texture_(0), width_(0), height_(0) {}

  // Delete the texture id, which will deallocate and destroy the buffer.
  ~GlTexture2D() noexcept;

  // Initialize and allocate the texture, with parameters having the same meaning as glTexImage2D.
  // Pass nullptr in for data to allocate the texture but not upload data.
  void initialize(
    GLenum target,
    GLint internalFormat,
    GLsizei width,
    GLsizei height,
    GLenum format,
    GLenum type,
    const GLvoid *data) noexcept;

  void bind() const noexcept;

  void unbind() const noexcept;

  GlTexture tex() const;

  // Update the texture 2D image contents.
  void updateImage(const GLvoid *data) const noexcept;

  // Accessors
  GLuint id() const { return texture_; }
  GLenum target() const { return target_; }
  GLsizei width() const { return width_; }
  GLsizei height() const { return height_; }
  GLenum format() const { return format_; }
  GLenum type() const { return type_; }

  // Move constructor and assignment.
  GlTexture2D(GlTexture2D &&rhs) noexcept;
  GlTexture2D &operator=(GlTexture2D &&rhs) noexcept;

  // Disallow copying.
  GlTexture2D(const GlTexture2D &) = delete;
  GlTexture2D &operator=(const GlTexture2D &) = delete;

  String toString() const noexcept;

private:
  // TODO(nb): replace the below with GlTexture tex_; and refer to e.g. tex.texture_.
  GLuint texture_;
  GLenum target_;
  GLsizei width_;
  GLsizei height_;
  GLenum format_;
  GLenum type_;
};

// Class for managing a cubemap OpenGL texture.
class GlTextureCubemap {
public:
  // Create the texture id, but don't allocate the texture.
  GlTextureCubemap() noexcept;

  // Create a GlTextureCubemap object which does not represent a texture.
  GlTextureCubemap(std::nullptr_t) : texture_(0), width_(0), height_(0) {}

  // Delete the texture id, which will deallocate and destroy the buffer.
  ~GlTextureCubemap() noexcept;

  // Initialize and allocate the texture for all 6 faces with blank data, with parameters having the
  // same meaning as glTexImage2D.
  // Call updateImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, data) etc. to update each face separately.
  void initialize(
    GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type) noexcept;

  void bind() const noexcept;

  void unbind() const noexcept;

  GlTexture tex() const;

  // Update the texture 2D image contents.
  void updateImage(GLint target, const GLvoid *data) const noexcept;

  // Accessors
  GLuint id() const { return texture_; }
  GLenum target() const { return target_; }
  GLsizei width() const { return width_; }
  GLsizei height() const { return height_; }
  GLenum format() const { return format_; }
  GLenum type() const { return type_; }

  // Move constructor and assignment.
  GlTextureCubemap(GlTextureCubemap &&rhs) noexcept;
  GlTextureCubemap &operator=(GlTextureCubemap &&rhs) noexcept;

  // Disallow copying.
  GlTextureCubemap(const GlTextureCubemap &) = delete;
  GlTextureCubemap &operator=(const GlTextureCubemap &) = delete;

  String toString() const noexcept;

private:
  GLuint texture_;
  static constexpr GLenum target_ = GL_TEXTURE_CUBE_MAP;
  GLsizei width_;
  GLsizei height_;
  GLenum format_;
  GLenum type_;
};

}  // namespace c8
