// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
#pragma once

#include "c8/pixels/opengl/gl.h"

#include <cstddef>

namespace c8 {

// Class for managing an OpenGL Renderbuffer.
class GlRenderbuffer {
public:
  // Create the renderbuffer id, but don't allocate the renderbuffer.
  GlRenderbuffer() noexcept;

  // Create a GlRenderbuffer object which does not represent a renderbufer.
  GlRenderbuffer(std::nullptr_t) : id_(0), width_(0), height_(0) {}

  // Delete the renderbuffer id, which will deallocate and destroy the buffer.
  ~GlRenderbuffer() noexcept;

  // Initialize and allocate the renderbuffer, with parameters having the same meaning as
  // glRenderbufferStorage.
  void initialize(
    GLenum target,
    GLint internalFormat,
    GLsizei width,
    GLsizei height) noexcept;

  void bind() const noexcept;
  void unbind() const noexcept;

  // Accessors
  GLuint id() const { return id_; }
  GLenum target() const { return target_; }
  GLsizei width() const { return width_; }
  GLsizei height() const { return height_; }

  // Move constructor and assignment.
  GlRenderbuffer(GlRenderbuffer &&rhs) noexcept;
  GlRenderbuffer &operator=(GlRenderbuffer &&rhs) noexcept;

  // Disallow copying.
  GlRenderbuffer(const GlRenderbuffer &) = delete;
  GlRenderbuffer &operator=(const GlRenderbuffer &) = delete;

private:
  GLuint id_;
  GLenum target_;
  GLsizei width_;
  GLsizei height_;
};

// Return a new GlRenderbuffer with GL_DEPTH_COMPONENT storage.
GlRenderbuffer makeDepthRenderbuffer(int width, int height);

}  // namespace c8
