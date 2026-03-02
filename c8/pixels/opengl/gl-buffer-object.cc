// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "gl-buffer-object.h",
  };
  deps = {
    ":gl-headers",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x015bf574);

#include "c8/pixels/opengl/gl-buffer-object.h"

namespace c8 {

void GlBufferObject::bind() const noexcept { glBindBuffer(target_, id_); }

// Unbind the buffer object.
void GlBufferObject::unbind() const noexcept { glBindBuffer(target_, 0); }

#if C8_OPENGL_VERSION_3
void *GlBufferObject::mapBuffer(GLenum access) const noexcept {
  // Use glMapBufferRange instead of glMapBuffer, since the latter is an extension in ES 3.0.
  return glMapBufferRange(target_, 0, byteSize_, access);
}

// Bind the buffer object and map a buffer range.
void *GlBufferObject::mapBufferRange(GLintptr offset, GLsizeiptr length, GLbitfield access) const
  noexcept {
  return glMapBufferRange(target_, offset, length, access);
}

GLboolean GlBufferObject::unmapBuffer() const noexcept { return glUnmapBuffer(target_); }

#endif

GlBufferObject::GlBufferObject() noexcept
    : id_(0), target_(), byteSize_(), maxByteSize_(), usage_() {
  glGenBuffers(1, &id_);
}

GlBufferObject::GlBufferObject(GlBufferObject &&rhs) noexcept {
  memmove(this, &rhs, sizeof(rhs));
  // Must set the id_ back to zero to prevent destructing the moved object.
  rhs.id_ = 0;
}
GlBufferObject &GlBufferObject::operator=(GlBufferObject &&rhs) noexcept {
  if (id_) {
    glDeleteBuffers(1, &id_);
  }
  memmove(this, &rhs, sizeof(rhs));

  // Must set the id_ back to zero to prevent destructing the moved object.
  rhs.id_ = 0;

  return *this;
}

GlBufferObject::~GlBufferObject() noexcept {
  if (id_) {
    glDeleteBuffers(1, &id_);
  }
}

void GlBufferObject::set(
  GLenum target, GLsizeiptr byteSize, const GLvoid *data, GLenum usage) noexcept {
  bool sameUsage = target_ == target && usage_ == usage;
  target_ = target;
  byteSize_ = byteSize;
  usage_ = usage;

  bind();
  if (byteSize_ > maxByteSize_ || !sameUsage) {
    maxByteSize_ = byteSize_;
    glBufferData(target_, byteSize_, data, usage_);
  } else {
    glBufferSubData(target_, 0, byteSize_, data);
  }
  unbind();
}

}  // namespace c8
