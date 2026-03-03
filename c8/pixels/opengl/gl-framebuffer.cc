// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Christoph Bartschat (christoph@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "gl-framebuffer.h",
  };
  deps = {
    ":gl-error",
    ":gl-headers",
    ":gl-texture",
    ":gl-renderbuffer",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x72a5ad4a);

#include <utility>

#include "c8/pixels/opengl/gl-error.h"
#include "c8/pixels/opengl/gl-framebuffer.h"

namespace c8 {

void GlFramebufferObject::bind() const noexcept { glBindFramebuffer(target_, framebuffer_); }

void GlFramebufferObject::unbind() const noexcept { glBindFramebuffer(target_, 0); }

// Return a new GlTexture2D with a TEXTURE_2D target, given dimensions and GL_LINEAR min/mag
// filters, and GL_CLAMP_TO_EDGE wrapping.
GlFramebufferObject makeLinearRGBA8888Framebuffer(int width, int height) {
  GlFramebufferObject f;
  f.initialize(makeLinearRGBA8888Texture2D(width, height), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0);
  return f;
}

// Return a new GlTexture2D with a TEXTURE_2D target, given dimensions and GL_NEAREST min/mag
// filters, and GL_CLAMP_TO_EDGE wrapping.
GlFramebufferObject makeNearestRGBA8888Framebuffer(int width, int height) {
  GlFramebufferObject f;
  f.initialize(makeNearestRGBA8888Texture2D(width, height), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0);
  return f;
}

GlFramebufferObject::GlFramebufferObject() noexcept
    : texture_(nullptr), framebuffer_(0), target_(), attachment_(), renderbuffer_(nullptr) {
  glGenFramebuffers(1, &framebuffer_);
}

GlFramebufferObject::~GlFramebufferObject() noexcept {
  if (framebuffer_) {
    glDeleteFramebuffers(1, &framebuffer_);
  }
}

void GlFramebufferObject::initialize(
  GlTexture2D &&texture, GLenum target, GLenum attachment) noexcept {
  texture_ = std::move(texture);
  target_ = target;
  attachment_ = attachment;

  // Bind the framebuffer once to create it.
  bind();

  // Attach the texture to the framebuffer and set target+attachment.
  glFramebufferTexture2D(target_, attachment_, texture_.target(), texture_.id(), 0);

  // Unbind the framebuffer now that it is complete.
  unbind();
}

void GlFramebufferObject::attachRenderbuffer(GlRenderbuffer &&renderbuffer, GLenum attachment) {
  renderbuffer_ = std::move(renderbuffer);
  bind();
  glFramebufferRenderbuffer(target_, attachment, renderbuffer_.target(), renderbuffer_.id());
  unbind();
}

GlFramebufferObject::GlFramebufferObject(GlFramebufferObject &&rhs) noexcept
    : texture_(std::move(rhs.texture_)),
      framebuffer_(std::move(rhs.framebuffer_)),
      target_(std::move(rhs.target_)),
      attachment_(std::move(rhs.attachment_)) {
  // Set the framebuffer to 0, to prevent deleting the framebuffer when the moved object is
  // destroyed.
  rhs.framebuffer_ = 0;
}

GlFramebufferObject &GlFramebufferObject::operator=(GlFramebufferObject &&rhs) noexcept {
  if (framebuffer_) {
    glDeleteFramebuffers(1, &framebuffer_);
  }

  texture_ = std::move(rhs.texture_);
  framebuffer_ = std::move(rhs.framebuffer_);
  target_ = std::move(rhs.target_);
  attachment_ = std::move(rhs.attachment_);
  // Set the framebuffer to 0, to prevent deleting the framebuffer when the moved object is
  // destroyed.
  rhs.framebuffer_ = 0;
  return *this;
}

GlAttachOnlyFramebuffer::GlAttachOnlyFramebuffer() noexcept { glGenFramebuffers(1, &fbId_); }

GlAttachOnlyFramebuffer::~GlAttachOnlyFramebuffer() noexcept { glDeleteFramebuffers(1, &fbId_); }

void GlAttachOnlyFramebuffer::bind() const noexcept { glBindFramebuffer(GL_FRAMEBUFFER, fbId_); }
void GlAttachOnlyFramebuffer::unbind() const noexcept { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void GlAttachOnlyFramebuffer::attachColorTexture2D(
  GLenum colorAttachment, GLenum colorTexTarget, GLuint colorTexId) {
  bind();
  glFramebufferTexture2D(GL_FRAMEBUFFER, colorAttachment, colorTexTarget, colorTexId, 0);
  checkGLError("[GlAttachOnlyFramebuffer] attachColorTexture2D");
  unbind();
}

void GlAttachOnlyFramebuffer::attachRenderBuffer(
  GLenum rbAttachment, GLenum rbTarget, GLuint rbId) {
  bind();
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, rbAttachment, rbTarget, rbId);
  checkGLError("[GlAttachOnlyFramebuffer] attachRenderBuffer");
  unbind();
}

bool GlAttachOnlyFramebuffer::isComplete() const {
  bind();
  GLenum stat = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  unbind();
  return (stat == GL_FRAMEBUFFER_COMPLETE);
}

// GlFrameBuffer
void GlFrameBuffer::initialize(int width, int height) {
  GLint restoreTexture = 0;
  GLint restoreFrameBuffer = 0;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &restoreTexture);
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &restoreFrameBuffer);

  render.initialize();
  glBindTexture(GL_TEXTURE_2D, render.texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

  glGenFramebuffers(1, &frameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render.texture, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, restoreFrameBuffer);
  glBindTexture(GL_TEXTURE_2D, restoreTexture);
}

void GlFrameBuffer::cleanup() {
  render.cleanup();
  glDeleteFramebuffers(1, &frameBuffer);
}

}  // namespace c8
