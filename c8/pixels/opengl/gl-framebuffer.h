// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Christoph Bartschat (christoph@8thwall.com)
#pragma once

#include <cstddef>

#include "c8/pixels/opengl/gl-renderbuffer.h"
#include "c8/pixels/opengl/gl-texture.h"
#include "c8/pixels/opengl/gl.h"

namespace c8 {

// Class for managing an OpenGL Framebuffer Object.
class GlFramebufferObject {
public:
  // Constructing a GlFramebufferObject gens the texture id but doesn't initialize the framebuffer.
  GlFramebufferObject() noexcept;

  // Create a GlFramebufferObject object which does not represent a framebuffer.
  GlFramebufferObject(std::nullptr_t)
      : texture_(nullptr), framebuffer_(0), target_(0), attachment_(0), renderbuffer_(nullptr) {}

  // Deleting the GlFramebufferObject will deallocate and destroy the buffer.
  ~GlFramebufferObject() noexcept;

  // Initialize and complete the Framebuffer. Pass an initialized GlTexture2D by move, which will be
  // owned by the GlFramebufferObject and bound to the Framebuffer. The target can be
  // GL_FRAMEBUFFER, GL_READ_FRAMEBUFFER or GL_DRAW_FRAMEBUFFER. The attachment point can be
  // GL_COLOR_ATTACHMENTi, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT or
  // GL_DEPTH_STENCIL_ATTACHMENT.
  void initialize(GlTexture2D &&texture, GLenum target, GLenum attachment) noexcept;

  // Attach the renderbuffer to this framebuffer at the specified attachment. Must be one of
  // GL_COLOR_ATTACHMENTi, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT or
  // GL_DEPTH_STENCIL_ATTACHMENT.
  void attachRenderbuffer(GlRenderbuffer &&renderbuffer, GLenum attachment);

  void bind() const noexcept;

  void unbind() const noexcept;

  // Accessors.
  const GlTexture2D &tex() const { return texture_; }
  GLuint id() const { return framebuffer_; }
  GLenum target() const { return target_; }
  GLenum attachment() const { return attachment_; }

  // Move constructor and assignment.
  GlFramebufferObject(GlFramebufferObject &&rhs) noexcept;
  GlFramebufferObject &operator=(GlFramebufferObject &&rhs) noexcept;

  // Disallow copying.
  GlFramebufferObject(const GlFramebufferObject &) = delete;
  GlFramebufferObject &operator=(const GlFramebufferObject &) = delete;

private:
  GlTexture2D texture_;
  GLuint framebuffer_;
  GLenum target_;
  GLenum attachment_;
  GlRenderbuffer renderbuffer_;
};

// Return a new GlTexture2D with a TEXTURE_2D target, given dimensions and GL_LINEAR min/mag
// filters, and GL_CLAMP_TO_EDGE wrapping.
GlFramebufferObject makeLinearRGBA8888Framebuffer(int width, int height);

// Return a new GlTexture2D with a TEXTURE_2D target, given dimensions and GL_NEAREST min/mag
// filters, and GL_CLAMP_TO_EDGE wrapping.
GlFramebufferObject makeNearestRGBA8888Framebuffer(int width, int height);

// Class for attach-only OpenGL Framebuffer.
// This class does not own or manage the textures or render buffers attached to it.
class GlAttachOnlyFramebuffer {
public:
  // Constructing a GlAttachOnlyFramebuffer gens the framebuffer id.
  GlAttachOnlyFramebuffer() noexcept;

  // Create a GlAttachOnlyFramebuffer object which does not represent a framebuffer.
  GlAttachOnlyFramebuffer(std::nullptr_t) : fbId_(0) {}

  // Deleting the GlAttachOnlyFramebuffer will deallocate and destroy the framebuffer.
  ~GlAttachOnlyFramebuffer() noexcept;

  void bind() const noexcept;
  void unbind() const noexcept;

  void attachColorTexture2D(GLenum colorAttachment, GLenum colorTexTarget, GLuint colorTexId);

  void attachRenderBuffer(GLenum rbAttachment, GLenum rbTarget, GLuint rbId);

  bool isComplete() const;

  // Default move constructors.
  GlAttachOnlyFramebuffer(GlAttachOnlyFramebuffer &&) = default;
  GlAttachOnlyFramebuffer &operator=(GlAttachOnlyFramebuffer &&) = default;

  // Disallow copying.
  GlAttachOnlyFramebuffer(const GlAttachOnlyFramebuffer &) = delete;
  GlAttachOnlyFramebuffer &operator=(const GlAttachOnlyFramebuffer &) = delete;

private:
  GLuint fbId_ = 0;
};

// This struct is harder to use than the class version above. Erik would like to deprecate and
// remove this class in favor of GlFramebufferObject.
struct GlFrameBuffer {
  GlTexture render;
  GLuint frameBuffer;

  void initialize(int width, int height);
  void cleanup();
};

}  // namespace c8
