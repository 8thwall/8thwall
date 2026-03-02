// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "gl-renderbuffer.h",
  };
  deps = {
    ":gl-headers",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xad205aeb);

#include "c8/pixels/opengl/gl-renderbuffer.h"

namespace c8 {

GlRenderbuffer makeDepthRenderbuffer(int width, int height) {
  GlRenderbuffer buf;
#if C8_OPENGL_ES
  buf.initialize(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
#else
  buf.initialize(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
#endif
  return buf;
}

GlRenderbuffer::GlRenderbuffer() noexcept : id_(0), target_(), width_(), height_() {
  glGenRenderbuffers(1, &id_);
}

GlRenderbuffer::~GlRenderbuffer() noexcept {
  if (id_) {
    glDeleteRenderbuffers(1, &id_);
  }
}

void GlRenderbuffer::initialize(
  GLenum target, GLint internalFormat, GLsizei width, GLsizei height) noexcept {
  target_ = target;
  width_ = width;
  height_ = height;

  bind();    // Create the buffer by binding it.
  glRenderbufferStorage(target_, internalFormat, width_, height_);  // init storage.
  unbind();  // Unbind the buffer.
}

GlRenderbuffer::GlRenderbuffer(GlRenderbuffer &&rhs) noexcept {
  memmove(this, &rhs, sizeof(rhs));
  // Reset texture to prevent deletion on destruction.
  rhs.id_ = 0;
}

GlRenderbuffer &GlRenderbuffer::operator=(GlRenderbuffer &&rhs) noexcept {
  if (id_) {
    glDeleteRenderbuffers(1, &id_);
  }
  memmove(this, &rhs, sizeof(rhs));
  // Reset texture to prevent deletion on destruction.
  rhs.id_ = 0;
  return *this;
}

void GlRenderbuffer::bind() const noexcept { glBindRenderbuffer(target_, id_); }

void GlRenderbuffer::unbind() const noexcept { glBindRenderbuffer(target_, 0); }

}  // namespace c8
