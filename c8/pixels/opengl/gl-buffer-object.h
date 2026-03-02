// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// A GlBufferObject provides object semantics for OpenGL buffer objects, such as Pixel Buffers
// Objects and Vertex Buffer Objects. It provides memory management for buffer names, and methods
// for binding and memory mapping.

#pragma once

#include "c8/pixels/opengl/gl.h"

#include <cstddef>

namespace c8 {

class GlBufferObject {
public:
  // Generates the buffer object name, but doesn't create (bind) the buffer object.
  GlBufferObject() noexcept;

  // Create a GlBufferObject object which does not represent a buffer object.
  GlBufferObject(std::nullptr_t) : id_(0) {}

  // Move constructor and assignment.
  GlBufferObject(GlBufferObject &&) noexcept;
  GlBufferObject &operator=(GlBufferObject &&) noexcept;

  // Disallow copying.
  GlBufferObject(const GlBufferObject &) = delete;
  GlBufferObject &operator=(const GlBufferObject &) = delete;

  // Destroys the buffer and deletes the buffer object name.
  ~GlBufferObject() noexcept;

  // Upload data to the buffer, allocating the buffer object with the desired target, size, and
  // usage pattern if needed.
  // For example, when using this as a pixel buffer object to read pixels from a GL framebuffer,
  // call set(GL_PIXEL_PACK_BUFFER, rowBytes * rows, nullptr, GL_DYNAMIC_READ).
  void set(GLenum target, GLsizeiptr byteSize, const GLvoid *data, GLenum usage) noexcept;

  // Bind the buffer object.
  void bind() const noexcept;

  // Unbind the buffer object.
  void unbind() const noexcept;

  // Accessors
  GLuint id() const { return id_; }
  GLenum target() const { return target_; }
  GLsizeiptr size() const { return byteSize_; }
  GLenum usage() const { return usage_; }

#if C8_OPENGL_VERSION_3
  // Map the whole buffer.
  void *mapBuffer(GLenum access) const noexcept;

  // Bind the buffer object and map a buffer range.
  void *mapBufferRange(GLintptr offset, GLsizeiptr length, GLbitfield access) const noexcept;

  // Unmap the buffer and unbind the buffer object.
  GLboolean unmapBuffer() const noexcept;
#endif

private:
  GLuint id_;
  GLenum target_;
  GLsizeiptr byteSize_;
  GLsizeiptr maxByteSize_;
  GLenum usage_;
};

}  // namespace c8
